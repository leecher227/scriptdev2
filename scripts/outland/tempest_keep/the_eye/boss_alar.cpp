/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/   

/* ScriptData
SDName: boss_alar
SD%Complete: 95 (missing proper enrage spell and only partial support of Dive bomb in core)
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "the_eye.h"

#define SPELL_FLAME_BUFFET            34121 // Flame Buffet - every 1,5 secs in phase 1 if there is no victim in melee range and after Dive Bomb in phase 2 with same conditions
#define SPELL_FLAME_QUILLS            34229 // Randomly after changing position in phase after watching tonns of movies, set probability 20%
#define SPELL_REBIRTH                 34342 // Rebirth - beginning of second phase (after loose all health in phase 1)
#define SPELL_REBIRTH_2               35369 // Rebirth (another, without healing to full HP) - after Dive Bomb in phase 2
#define SPELL_MELT_ARMOR              35410 // Melt Armor - every 60 sec in phase 2
#define SPELL_CHARGE                  35412 // Charge - 30 sec cooldown
#define SPELL_DIVE_BOMB_VISUAL        35367 // Bosskillers says 30 sec cooldown, wowwiki says 30 sec colldown, DBM and BigWigs addons says ~47 sec
#define SPELL_DIVE_BOMB               35181 // after watching tonns of movies, set cooldown to 40+rand()%5.
#define SPELL_ENRAGE                  27680 // 10 minutes after phase 2 starts (id is wrong, but proper id is unknown)

#define CREATURE_EMBER_OF_ALAR        19551 // Al'ar summons one Ember of Al'ar every position change in phase 1 and two after Dive Bomb. Also in phase 2 when Ember of Al'ar dies, boss loose 3% health.
#define SPELL_EMBER_BLAST             34133 // When Ember of Al'ar dies, it casts Ember Blast

#define CREATURE_FLAME_PATCH_ALAR     20602 // Flame Patch - every 30 sec in phase 2
#define SPELL_FLAME_PATCH             35380 //

float waypoint[6][3] = 
{
    {340.15f, 58.65f, 17.71f},
    {388.09f, 31.54f, 20.18f},
    {388.18f, -32.85f, 20.18f},
    {340.29f, -60.19f, 17.72f},
    {331.0f, 0.01f, 39.0f},
    {331.0f, 0.01f, -2.39f}
};

bool Phase1;

struct MANGOS_DLL_DECL boss_alarAI : public ScriptedAI
{
    boss_alarAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        DefaultSize = m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
        DefaultModel = m_creature->GetUInt32Value(UNIT_FIELD_DISPLAYID);
        DefaultMoveSpeedRate = m_creature->GetSpeedRate(MOVE_RUN);
        Reset();
    }
    
    ScriptedInstance *pInstance;
    
    uint32 Platforms_Move_Timer;
    uint32 FlameBuffet_Timer;
    uint32 FlameQuillsDuration_Timer;
    uint32 DiveBomb_Timer;
    uint32 DiveBombCastDelay_Timer;
    uint32 CorpseDisappear_Timer;
    uint32 MeltArmor_Timer;
    uint32 Charge_Timer;
    uint32 FlamePatch_Timer;
    uint32 Enrage_Timer;
    uint32 ChargeDelay_Timer;

    float DefaultSize;
    float DefaultMoveSpeedRate;
    uint32 DefaultModel;

    bool Moving;
    bool Charge;
    bool FlameQuills;
    bool FakeDeath;
    bool Phase2_begin;
    bool FlameBuffetAfterDiveBomb;
    int8 cur_wp;
    float Charge_target_threat;
    Unit* Charge_target;

    void Reset()
    {
        Platforms_Move_Timer = 30000+rand()%5000;
        FlameQuills = false;
        cur_wp = 0;
        FlameBuffet_Timer = 1500;
        CorpseDisappear_Timer = 9999999;
        Enrage_Timer = 600000;
        Moving = false;
        Phase1 = true;
        FakeDeath = false;
        Phase2_begin = false;
        FlameBuffetAfterDiveBomb = false;
        Charge_target = NULL;

        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, DefaultSize);
        m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, DefaultModel);
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
        m_creature->SetSpeedRate(MOVE_RUN, DefaultMoveSpeedRate);
        m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
        SetCombatMovement(false);
        m_creature->SetActiveObjectState(true);
        m_creature->GetMotionMaster()->Clear();

        if(pInstance)
            pInstance->SetData(TYPE_ALAR, NOT_STARTED);
        
    }
    
    void Aggro (Unit *who)
    {
        if(pInstance)
            pInstance->SetData(TYPE_ALAR, IN_PROGRESS);

        CreaturePointMove(0, waypoint[0][0], waypoint[0][1], waypoint[0][2]);
    }

    void JustDied(Unit *victim)
    {
        if(pInstance)
            pInstance->SetData(TYPE_ALAR, DONE);
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if ((damage < m_creature->GetHealth()) || (!Phase1))
            return;

        damage = 0;

        m_creature->SetHealth(0);
        m_creature->RemoveAllAuras();
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        FakeDeath = true;
        Moving = false;
        CorpseDisappear_Timer = 4000;
    }

    void CreaturePointMove(uint32 id, float X, float Y, float Z)
    {
        Moving = true;
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
        m_creature->GetMotionMaster()->MovePoint(id, X, Y, Z);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        Moving = false;
        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
        switch (id)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                Platforms_Move_Timer = 30000+rand()%5000;
                break;
            case 4:
                DoCast(m_creature, SPELL_FLAME_QUILLS);
                FlameQuillsDuration_Timer = 10000;
                FlameQuills = true;
                break;
            case 5:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetVisibility(VISIBILITY_ON);
                DoCast(m_creature, SPELL_REBIRTH);
                Phase2_begin = true;
                FakeDeath = false;
                break;
            case 6:
                m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, DefaultSize/4.0f);
                DoCast(m_creature->getVictim(), SPELL_DIVE_BOMB_VISUAL);
                DiveBombCastDelay_Timer = 4000;
                break;
            case 7:
                m_creature->SetSpeedRate(MOVE_RUN, DefaultMoveSpeedRate);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, DefaultModel);
                m_creature->SetVisibility(VISIBILITY_OFF);  // I know, that this sequence of commands looks stupid, but without it visual effect of Rebirth 
                m_creature->SetVisibility(VISIBILITY_ON);   // after Dive Bomb looks not perfect (Al'ar appears, and than again disappears and cast Rebirth).
                DoCast(m_creature, SPELL_REBIRTH_2);
                DiveBombCastDelay_Timer = 9999999;
                FlameBuffetAfterDiveBomb = true;
                DiveBomb_Timer = 40000+rand()%5000;
                FlameBuffet_Timer = 5000;
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim() || Moving)
            return;

        if (Charge)
        {
            if (ChargeDelay_Timer < diff)
            {
                if (Charge_target)
                {
                    m_creature->getThreatManager().modifyThreatPercent(Charge_target, -100);
                    m_creature->getThreatManager().addThreat(Charge_target, Charge_target_threat);
                }
                Charge = false;
                m_creature->SetSpeedRate(MOVE_RUN, DefaultMoveSpeedRate);
            }else ChargeDelay_Timer -= diff;
            return;
        }
        
        if (!Phase1)
            if (Enrage_Timer < diff)
            {
                DoCast(m_creature, SPELL_ENRAGE);
                Enrage_Timer = 600000;
            }else Enrage_Timer -= diff;

        if (CorpseDisappear_Timer < diff)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            CreaturePointMove(5, waypoint[5][0], waypoint[5][1], waypoint[5][2]);
            CorpseDisappear_Timer = 9999999;
        }else CorpseDisappear_Timer -= diff;

        if (FakeDeath)
            return;

        if (Phase2_begin)
        {
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            MeltArmor_Timer = 60000;
            Charge_Timer = 7000;
            DiveBomb_Timer = 40000+rand()%5000;
            DiveBombCastDelay_Timer = 9999999;
            FlameBuffet_Timer = 9999999;
            FlamePatch_Timer = 30000;
            Phase2_begin = false;
            Phase1 = false;
        }

        if (Phase1)
        {
            if (FlameQuills) {
                if (FlameQuillsDuration_Timer < diff)
                {
                    switch(rand()%2)
                    {
                    case 0:
                        cur_wp = 0;
                        break;
                    case 1:
                        cur_wp = 3;
                        break;
                    }
                    CreaturePointMove(cur_wp, waypoint[cur_wp][0], waypoint[cur_wp][1], waypoint[cur_wp][2]);
                    FlameQuills = false;
                }else FlameQuillsDuration_Timer -= diff;
                return;
            }
            if (Platforms_Move_Timer < diff)
            {
                if (Creature* Summoned = m_creature->SummonCreature(CREATURE_EMBER_OF_ALAR, waypoint[cur_wp][0], waypoint[cur_wp][1], waypoint[cur_wp][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        Summoned->AI()->AttackStart(pTarget);
                if (!urand(0, 3))
                {
                    cur_wp = 4;
                    FlameQuills = true;
                }
                else
                {
                    cur_wp++;
                    if (cur_wp == 4)
                        cur_wp = 0;
                }
                CreaturePointMove(cur_wp, waypoint[cur_wp][0], waypoint[cur_wp][1], waypoint[cur_wp][2]);
            }else Platforms_Move_Timer -= diff;

            if (FlameBuffet_Timer < diff)
            {
                ThreatList const& lThreatList = m_creature->getThreatManager().getThreatList();
                if(!lThreatList.empty())
                {
                    bool InMeleeRange = false;
                    for (ThreatList::const_iterator i = lThreatList.begin(); i != lThreatList.end(); ++i) 
                    {
                        if (Unit* pUnit = m_creature->GetMap()->GetUnit((*i)->getUnitGuid()))
                            if (m_creature->IsWithinDistInMap(pUnit, 5))
                            {                        
                                InMeleeRange = true;
                                m_creature->getThreatManager().addThreat(pUnit, 1.0f);
                                break;
                            }
                            else
                                m_creature->getThreatManager().modifyThreatPercent(pUnit, -100);
                    }
                    if (!InMeleeRange)
                        DoCast(m_creature, SPELL_FLAME_BUFFET);
                }
                FlameBuffet_Timer = 1500;
            }else FlameBuffet_Timer -= diff;
        } else
        {
            if (FlameBuffetAfterDiveBomb)
            {
                if (FlameBuffet_Timer < diff)
                {
                    if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 5))
                    {                        
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                        FlameBuffetAfterDiveBomb = false;
                        FlameBuffet_Timer = 9999999;
                    } else
                    {
                        DoCast(m_creature, SPELL_FLAME_BUFFET);
                        FlameBuffet_Timer = 1500;
                    }
                } else FlameBuffet_Timer -= diff;
            } else
            {
                if (MeltArmor_Timer < diff)
                {
                    DoCast(m_creature->getVictim(), SPELL_MELT_ARMOR);
                    MeltArmor_Timer = 60000;
                }else MeltArmor_Timer -= diff;

                if (Charge_Timer < diff)
                {
                    if (Charge_target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    {
                        m_creature->SetInFront(Charge_target);
                        Charge_target_threat = m_creature->getThreatManager().getThreat(Charge_target);
                        m_creature->getThreatManager().addThreat(Charge_target, 100000000.0f);
                        m_creature->SetSpeedRate(MOVE_RUN, DefaultMoveSpeedRate*5.0f);
                        DoCast(Charge_target, SPELL_CHARGE);
                        ChargeDelay_Timer = 2000;
                        Charge = true;
                    }
                    Charge_Timer = 30000;
                }else Charge_Timer -= diff;
            }

            if (DiveBomb_Timer < diff)
            {
                CreaturePointMove(6, waypoint[4][0], waypoint[4][1], waypoint[4][2]);
                DiveBomb_Timer = 40000+rand()%5000;
            }else DiveBomb_Timer -= diff;

            if (DiveBombCastDelay_Timer < diff)
            {
                m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetDisplayId(11686);
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (target)
                    DoCast(target, SPELL_DIVE_BOMB);
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, DefaultSize);
                m_creature->RemoveAurasDueToSpell(SPELL_DIVE_BOMB_VISUAL);
                m_creature->SetSpeedRate(MOVE_RUN, DefaultMoveSpeedRate*2.0f);
                if (target)
                {
                    CreaturePointMove(7, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                    for (int8 i=1; i<=2; i++)
                        if (Creature* Summoned = m_creature->SummonCreature(CREATURE_EMBER_OF_ALAR, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                            if (Unit* target1 = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                Summoned->AI()->AttackStart(target1);
                }
                DiveBombCastDelay_Timer = 9999999;
            }else DiveBombCastDelay_Timer -= diff;

            if (FlamePatch_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (Creature* Summoned = m_creature->SummonCreature(CREATURE_FLAME_PATCH_ALAR, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 120000))
                    {
                        Summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        Summoned->SetFloatValue(OBJECT_FIELD_SCALE_X, Summoned->GetFloatValue(OBJECT_FIELD_SCALE_X)*2.9f);
                        Summoned->SetDisplayId(11686);
                        Summoned->setFaction(m_creature->getFaction());
                        Summoned->SetLevel(m_creature->getLevel());
                        Summoned->CastSpell(Summoned, SPELL_FLAME_PATCH, false);
                    }
                }
                FlamePatch_Timer = 30000;
            }else FlamePatch_Timer -= diff;

        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_alar(Creature *_Creature)
{
    return new boss_alarAI (_Creature);
}

struct MANGOS_DLL_DECL mob_ember_of_alarAI : public ScriptedAI
{
    mob_ember_of_alarAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    ScriptedInstance *pInstance;

    bool Die;

    void Reset()
    {
        Die = false;
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if (damage < m_creature->GetHealth())
            return;

        damage = 0;
        DoCast(m_creature, SPELL_EMBER_BLAST);
        if (pInstance && !Phase1)
            if (Unit* Alar = m_creature->GetMap()->GetUnit(pInstance->GetData64(DATA_ALAR)))
            {
                int AlarHealth = Alar->GetHealth() - Alar->GetMaxHealth()*0.03;
                if (AlarHealth > 0)
                    Alar->SetHealth(AlarHealth);
                else
                    Alar->SetHealth(1);
            }
        Die = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (Die)
        {
            m_creature->SetHealth(0);
            m_creature->SetDeathState(JUST_DIED);
        }

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_ember_of_alar(Creature *_Creature)
{
    return new mob_ember_of_alarAI (_Creature);
}

struct MANGOS_DLL_DECL mob_flame_patch_alarAI : public ScriptedAI
{
    mob_flame_patch_alarAI(Creature *c) : ScriptedAI(c) {}

    void Reset() {}

    void AttackStart(Unit* who) {}

    void MoveInLineOfSight(Unit* who) {}

    void UpdateAI(const uint32 diff) {}

};

CreatureAI* GetAI_mob_flame_patch_alar(Creature *_Creature)
{
    return new mob_flame_patch_alarAI (_Creature);
}

void AddSC_boss_alar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_alar";
    newscript->GetAI = &GetAI_boss_alar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ember_of_alar";
    newscript->GetAI = &GetAI_mob_ember_of_alar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_flame_patch_alar";
    newscript->GetAI = &GetAI_mob_flame_patch_alar;
    newscript->RegisterSelf();
}
