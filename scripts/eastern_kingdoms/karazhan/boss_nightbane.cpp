/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Nightbane
SD%Complete: 0
SDComment: Place holder
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    SPELL_BELLOWING_ROAR        = 36922,
    SPELL_CHARRED_EARTH         = 30129,                    //Also 30209 (Target Charred Earth) triggers this
    SPELL_DISTRACTING_ASH       = 30130,
    SPELL_SMOLDERING_BREATH     = 30210,
    SPELL_TAIL_SWEEP            = 25653,
    SPELL_RAIN_OF_BONES         = 37098,
    SPELL_SMOKING_BLAST         = 37057,
    SPELL_FIREBALL_BARRAGE      = 30282,
    SPELL_CLEAVE                = 30131,

    NPC_RESTLESS_SKELETON       = 17261,

    POINT_ID_AIR                = 1,
    POINT_ID_LAND               = 2
};

struct Locations
{
    float x, y, z;
};

static Locations Flight[]=
{
	{-11171.26f, -1879.95f, 104.0f},
    {-11154.85f, -1894.43f, 91.47f}
};

struct MANGOS_DLL_DECL boss_nightbaneAI : public ScriptedAI
{
    boss_nightbaneAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    // Ground State
    uint32 Bellowing_Timer;
    uint32 Charred_Timer;
    uint32 Distracting_Timer;
    uint32 Smoldering_Timer;
    uint32 Cleave_Timer;
    uint32 Tail_Timer;

    // Air State
    uint32 Fly_Timer;
    uint32 Rain_Timer;
    uint32 Summon_Timer;
    uint32 Smoking_Timer;
    uint32 Fireball_Timer;

    uint32 Phase;
    Locations Summon_Loc;
    bool Flying;
    bool Landed;

    void Reset()
    {
        // Ground State
        Bellowing_Timer = urand(45000, 60000);
        Charred_Timer = urand(20000, 40000);
        Distracting_Timer = urand(40000, 70000);
        Smoldering_Timer = 30000;
        Cleave_Timer = urand(5000, 10000);
        Tail_Timer = 5000;
        Summon_Loc.x = Flight[1].x;
        Summon_Loc.y = Flight[1].y;
        Summon_Loc.z = Flight[1].z;
        Phase = 1;

        Flying = false;
        Landed = false;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NIGHTBANE, NOT_STARTED);

        m_creature->SetHover(false);
        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NIGHTBANE, DONE);
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NIGHTBANE, IN_PROGRESS);

        m_creature->SetInCombatWithZone();
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_AIR)
        {
            Rain_Timer = 2000;
            Fireball_Timer = 5000;
            Smoking_Timer = 999999;
            Summon_Timer = 999999;
            Fly_Timer = 999999;
            m_creature->SetHover(true);
            return;
        }

        if (uiPointId == POINT_ID_LAND)
        {
            Bellowing_Timer = urand(45000, 60000);
            Charred_Timer = urand(20000, 40000);
            Distracting_Timer = urand(40000, 70000);
            Smoldering_Timer = 30000;
            Cleave_Timer = urand(5000, 10000);
            Tail_Timer = 5000;
            Flying = false;
            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
            m_creature->SetHover(false);
            DoResetThreat();
            Landed = true;
            return;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE) 
            return;

        if ((m_creature->GetHealthPercent() <= 75 && Phase == 1) ||
            (m_creature->GetHealthPercent() <= 50 && Phase == 2) ||
            (m_creature->GetHealthPercent() <= 25 && Phase == 3))
        {
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
            Flying = true;
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_AIR, Flight[0].x, Flight[0].y, Flight[0].z);
            Phase++;
        }

        if (Landed)
        {
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            Landed = false;
        }

        if (!Flying)
        {
            if (Bellowing_Timer < diff)
            {
                DoCast(m_creature, SPELL_BELLOWING_ROAR);
                Bellowing_Timer = urand(45000, 60000);
            }
            else
                Bellowing_Timer -= diff;

            if (Distracting_Timer < diff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_DISTRACTING_ASH);
                Distracting_Timer = urand(40000, 70000);
            }
            else
                Distracting_Timer -= diff;

            if (Charred_Timer < diff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CHARRED_EARTH);
                Charred_Timer = urand(20000, 40000);
            }
            else
                Charred_Timer -= diff;

            if (Cleave_Timer < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                Cleave_Timer = 10000;
            }
            else
                Cleave_Timer -= diff;

            if (Smoldering_Timer <diff)
            {
                DoCast(m_creature->getVictim(), SPELL_SMOLDERING_BREATH);
                Smoldering_Timer = 30000;
            }
            else
                Smoldering_Timer -= diff;

            if (Tail_Timer <diff)
            {
                DoCast(m_creature, SPELL_TAIL_SWEEP);
                Tail_Timer = 5000;
            }
            else
                Tail_Timer -= diff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (Rain_Timer < diff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    DoCast(pTarget, SPELL_RAIN_OF_BONES);
                    Summon_Loc.x = pTarget->GetPositionX();
                    Summon_Loc.y = pTarget->GetPositionY();
                    Summon_Loc.z = pTarget->GetPositionZ();
                    Summon_Timer = 5000;
                    Smoking_Timer = 12000;
                    Rain_Timer = 999999;
                }
            }
            else
                Rain_Timer -= diff;

            if (Summon_Timer < diff)
            {
                for (uint8 i = 0; i < 5; ++i)
                {
                    Creature* pSummoned = m_creature->SummonCreature(NPC_RESTLESS_SKELETON, Summon_Loc.x-5+urand(0, 10), Summon_Loc.y-5+urand(0, 10), Summon_Loc.z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 900000);
                    pSummoned->SetInCombatWithZone();
                }
                Summon_Timer = 999999;
            }
            else
                Summon_Timer -= diff;

            if (Smoking_Timer < diff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_SMOKING_BLAST);
                Fly_Timer = 16000;
                Smoking_Timer = 999999;
            }
            else
                Smoking_Timer -= diff;

            if (Fireball_Timer < diff)
            {
                ThreatList const& lThreatList = m_creature->getThreatManager().getThreatList();
                for (ThreatList::const_iterator i = lThreatList.begin(); i != lThreatList.end(); ++i)
                {
                    if (Unit* pUnit = Unit::GetUnit(*m_creature, (*i)->getUnitGuid()))
                        if (pUnit->isAlive() && !m_creature->IsWithinDistInMap(pUnit, 40, true))
                            DoCast(pUnit, SPELL_FIREBALL_BARRAGE);
                }
                Fireball_Timer = 1000;
            }
            else
                Fireball_Timer -= diff;

            if (Fly_Timer < diff)
            {
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, Flight[1].x, Flight[1].y, Flight[1].z);
                Fly_Timer = 9999999;
            }
            else
                Fly_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_boss_nightbane(Creature *pCreature)
{
    return new boss_nightbaneAI (pCreature);
}

void AddSC_boss_nightbane()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_nightbane";
    newscript->GetAI = &GetAI_boss_nightbane;
    newscript->RegisterSelf();
}
