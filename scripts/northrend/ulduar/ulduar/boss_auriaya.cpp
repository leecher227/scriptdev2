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
SDName: boss_auriaya
SD%Complete:
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //Yells
    SAY_AGGRO       = -1603070,
    SAY_SLAY1       = -1603071,
    SAY_SLAY2       = -1603072,
    SAY_BERSERK     = -1603073,
    SAY_DEATH       = -1603074,
    EMOTE_SCREECH   = -1603358,
    EMOTE_DEFENDER  = -1603359,

    //Auriaya
    SPELL_BERSERK               = 47008,
    SPELL_GUARDIAN_SWARM        = 64396,
    SPELL_SENTINEL_BLAST        = 64389,
    SPELL_SENTINEL_BLAST_H      = 64678,
    SPELL_SONIC_SCREECH         = 64422,
    SPELL_SONIC_SCREECH_H       = 64688,
    SPELL_TERRIFYING_SCREECH    = 64386,

    //Feral Defender
    SPELL_FEIGN_DEATH           = 57685,
    SPELL_FERAL_ESSENCE         = 64455,
    SPELL_FERAL_POUNCE          = 64478,
    SPELL_FERAL_POUNCE_H        = 64669,
    SPELL_FERAL_RUSH            = 64496,
    SPELL_FERAL_RUSH_H          = 64674,

    //Sanctum Sentry
    SPELL_RIP_FLESH             = 64375,
    SPELL_RIP_FLESH_H           = 64667,
    SPELL_SAVAGE_POUNCE         = 64666,
    SPELL_SAVAGE_POUNCE_H       = 64374,
    SPELL_STRENGHT_OF_PACK      = 64369,
    //Seeping Feral Essence
    AURA_VOID_ZONE              = 64458,
    AURA_VOID_ZONE_H            = 64676,
    //NPC ids
    MOB_VOID_ZONE               = 34098,
    MOB_FERAL_DEFENDER          = 34035,
    MOB_SWARMING_GUARDIAN       = 34034,

    MODEL_ID_INVISIBLE          = 11686,

    ACHIEV_CRAZY_CAT_LADY       = 3006,
    ACHIEV_CRAZY_CAT_LADY_H     = 3007,

    ACHIEV_NINE_LIVES           = 3076,
    ACHIEV_NINE_LIVES_H         = 3077,
};

bool m_bCrazyCatLady;
bool m_bNineLives;

// Seeping Feral Essence
struct MANGOS_DLL_DECL mob_seeping_feral_essenceAI : public ScriptedAI
{
    mob_seeping_feral_essenceAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        SetCombatMovement(false);
        m_creature->setFaction(14);
        m_creature->SetDisplayId(MODEL_ID_INVISIBLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature, m_bIsRegularMode ? AURA_VOID_ZONE : AURA_VOID_ZONE_H);
        m_creature->SetRespawnDelay(DAY);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_AURIAYA) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_seeping_feral_essence(Creature* pCreature)
{
    return new mob_seeping_feral_essenceAI(pCreature);
}

// Sanctum Sentry
struct MANGOS_DLL_DECL mob_sanctum_sentryAI : public ScriptedAI
{
    mob_sanctum_sentryAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiRipFlesh_Timer;
    uint32 m_uiSavagePounce_Timer;

    bool m_bSavagePounce;
    uint32 m_uiSavagePounceEnd_Timer;
    uint64 m_uiSavagePounceTargetGUID;

    void Reset()
    {
        m_uiRipFlesh_Timer = 13000;
        m_uiSavagePounce_Timer = urand(5000, 10000);
        m_bSavagePounce = false;
        m_uiSavagePounceEnd_Timer = 1000;
        m_uiSavagePounceTargetGUID = 0;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_AURIAYA)))
            {
                if (pTemp->isAlive())
                    pTemp->SetInCombatWithZone();
            }
        }
        DoCast(m_creature, SPELL_STRENGHT_OF_PACK);
    }

    void JustDied(Unit* pKiller)
    {
        m_bCrazyCatLady = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            // Follow Auriaya
            if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
            {
                if (Creature* pAuriaya = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_AURIAYA)))
                {
                    if (pAuriaya->isAlive())
                    {
                        m_creature->SetSpeedRate(MOVE_WALK, pAuriaya->GetSpeedRate(MOVE_WALK), true);
                        m_creature->GetMotionMaster()->MoveFollow(pAuriaya, 5.0f, m_creature->GetAngle(pAuriaya));
                    }
                }
            }
            return;
        }

        if (m_bSavagePounce)
        {
            if (m_uiSavagePounceEnd_Timer < diff)
            {
                if (Unit* pSavagePounceTarget = m_creature->GetMap()->GetUnit(m_uiSavagePounceTargetGUID))
                    m_creature->AddThreat(pSavagePounceTarget, -10000000.0f);
                m_bSavagePounce = false;
            }
            else
                m_uiSavagePounceEnd_Timer -= diff;

            return;
        }

        if (m_uiRipFlesh_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_RIP_FLESH : SPELL_RIP_FLESH_H);
            m_uiRipFlesh_Timer = urand(13000, 17000);
        }
        else
            m_uiRipFlesh_Timer -= diff;

        if (m_uiSavagePounce_Timer < diff)
        {
            Unit* pTarget = NULL;
            for (uint8 i=0; i<26; ++i)
            {
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (pTarget && m_creature->IsWithinLOSInMap(pTarget) && m_creature->GetDistance2d(pTarget) >= 8 && m_creature->GetDistance2d(pTarget)<=25)
                    break;
                if (i>=25)
                    pTarget = NULL;
            }
            if (pTarget)
            {
                m_creature->AddThreat(pTarget, 10000000.0f);
                m_uiSavagePounceTargetGUID = pTarget->GetGUID();
                DoCast(pTarget, m_bIsRegularMode ? SPELL_SAVAGE_POUNCE : SPELL_SAVAGE_POUNCE_H);
                m_uiSavagePounceEnd_Timer = 1000;
                m_bSavagePounce = true;
            }
            m_uiSavagePounce_Timer = urand(5000, 10000);
            return;
        }
        else
            m_uiSavagePounce_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sanctum_sentry(Creature* pCreature)
{
    return new mob_sanctum_sentryAI(pCreature);
}

// Feral Defender
struct MANGOS_DLL_DECL mob_feral_defenderAI : public ScriptedAI
{
    mob_feral_defenderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFeralPounce_Timer;
    uint32 m_uiFeralPounceEnd_Timer;
    uint64 m_uiFeralPounceTargetGUID;
    uint32 m_uiFeralRush_Timer;
    uint32 m_uiFeralRushEnd_Timer;
    uint32 m_uiFeralRushNext_Timer;
    uint64 m_uiFeralRushTargetGUID;
    uint32 m_uiRevive_Timer;

    bool m_bFeralPounce;
    bool m_bFeralRush;
    bool m_bIsDead;
    bool m_bHasAura;

    void Reset()
    {
        m_uiFeralPounce_Timer = urand(8000, 10000);
        m_uiFeralPounceEnd_Timer = 1000;
        m_uiFeralPounceTargetGUID = 0;
        m_uiFeralRush_Timer = 13000;
        m_uiFeralRushEnd_Timer = 6000;
        m_uiFeralRushNext_Timer = 600;
        m_uiFeralRushTargetGUID = 0;
        m_uiRevive_Timer = 30000;
        m_bFeralPounce = false;
        m_bFeralRush = false;
        m_bIsDead = false;
        m_bHasAura = false;

        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void Aggro(Unit* pWho)
    {
        DoCast(m_creature, SPELL_FERAL_ESSENCE);
    }

    void JustDied(Unit* pKiller)
    {
        m_bNineLives = true;
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (!m_bIsDead && uiDamage > m_creature->GetHealth())
        {
            m_creature->SummonCreature(MOB_VOID_ZONE, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (m_creature->HasAura(SPELL_FERAL_ESSENCE))
            {
                m_creature->SetHealth(0);
                m_creature->GetMotionMaster()->Clear();
                m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                m_uiRevive_Timer = 30000;
                m_bIsDead = true;
                uiDamage = 0;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_AURIAYA) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bIsDead)
        {
            if (m_uiRevive_Timer < diff)
            {
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (m_creature->HasAura(SPELL_FERAL_ESSENCE))
                {
                    // remove 1 stack of the aura
                    if (SpellAuraHolder* strenght = m_creature->GetSpellAuraHolder(SPELL_FERAL_ESSENCE))
                    {
                        if (strenght->ModStackAmount(-1))
                            m_creature->RemoveAurasDueToSpell(SPELL_FERAL_ESSENCE);
                    }
                }
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->GetMotionMaster()->MoveChase(pTarget);

                DoResetThreat();
                m_uiFeralPounce_Timer = urand(8000, 10000);
                m_uiFeralPounceEnd_Timer = 1000;
                m_uiFeralPounceTargetGUID = 0;
                m_uiFeralRush_Timer = 13000;
                m_uiFeralRushEnd_Timer = 6000;
                m_uiFeralRushNext_Timer = 600;
                m_uiFeralRushTargetGUID = 0;
                m_uiRevive_Timer = 30000;
                m_bFeralPounce = false;
                m_bFeralRush = false;
                m_bIsDead = false;
            }
            else
                m_uiRevive_Timer -= diff;

            return;
        }

        // hacky way of stacking aura, needs fixing
        if (SpellAuraHolder* pAura = m_creature->GetSpellAuraHolder(SPELL_FERAL_ESSENCE))
        {
            if (pAura->GetStackAmount() < 9 && !m_bHasAura)
            {
                m_bHasAura = true;
                pAura->SetStackAmount(9);
            }
        }

        if (m_bFeralPounce)
        {
            if (m_uiFeralPounceEnd_Timer < diff)
            {
                if (Unit* pFeralPounceTarget = m_creature->GetMap()->GetUnit(m_uiFeralPounceTargetGUID))
                    m_creature->AddThreat(pFeralPounceTarget, -10000000.0f);
                m_uiFeralPounceTargetGUID = 0;
                m_bFeralPounce = false;
            }
            else
                m_uiFeralPounceEnd_Timer -= diff;

            return;
        }

        if (m_bFeralRush)
        {
            if (m_uiFeralRushNext_Timer < diff)
            {
                if (Unit* pFeralRushTarget = m_creature->GetMap()->GetUnit(m_uiFeralRushTargetGUID))
                    m_creature->AddThreat(pFeralRushTarget, -10000000.0f);
                for (uint8 i=0; i<25; ++i)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (m_uiFeralRushTargetGUID == pTarget->GetGUID())
                            continue;
                        else
                        {
                            m_creature->AddThreat(pTarget, 10000000.0f);
                            m_uiFeralRushTargetGUID = pTarget->GetGUID();
                            DoCast(pTarget, m_bIsRegularMode ? SPELL_FERAL_RUSH : SPELL_FERAL_RUSH_H);
                            break;
                        }
                    }
                }
                m_uiFeralRushNext_Timer = m_bIsRegularMode ? 1200 : 600;
            }
            else
                m_uiFeralRushNext_Timer -= diff;

            if (m_uiFeralRushEnd_Timer < diff)
            {
                if (Unit* pFeralRushTarget = m_creature->GetMap()->GetUnit(m_uiFeralRushTargetGUID))
                    m_creature->AddThreat(pFeralRushTarget, -10000000.0f);
                m_uiFeralRushTargetGUID = 0;
                m_bFeralRush = false;
            }
            else
                m_uiFeralRushEnd_Timer -= diff;

            return;
        }

        if (m_uiFeralPounce_Timer < diff)
        {
            Unit* pTarget = NULL;
            for (uint8 i=0; i<26; ++i)
            {
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (pTarget && m_creature->IsWithinLOSInMap(pTarget) && m_creature->GetDistance2d(pTarget) >= 5 && m_creature->GetDistance2d(pTarget)<=45)
                    break;
                if (i>=25)
                    pTarget = NULL;
            }
            if (pTarget)
            {
                m_creature->AddThreat(pTarget, 10000000.0f);
                m_uiFeralPounceTargetGUID = pTarget->GetGUID();
                DoCast(pTarget, m_bIsRegularMode ? SPELL_FERAL_POUNCE : SPELL_FERAL_POUNCE_H);
                m_uiFeralPounceEnd_Timer = 1000;
                m_bFeralPounce = true;
            }
            m_uiFeralPounce_Timer = urand(9000, 11000);
            return;
        }
        else
            m_uiFeralPounce_Timer -= diff;

        if (m_uiFeralRush_Timer < diff)
        {
            m_uiFeralRush_Timer = urand(7000, 10000);
            m_uiFeralRushEnd_Timer = 6000;
            m_uiFeralRushNext_Timer = 0;
            m_bFeralRush = true;
            return;
        }
        else
            m_uiFeralRush_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_feral_defender(Creature* pCreature)
{
    return new mob_feral_defenderAI(pCreature);
}

// Auriaya
struct MANGOS_DLL_DECL boss_auriayaAI : public ScriptedAI
{
    boss_auriayaAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEnrage_Timer;
    uint32 m_uiSwarm_Timer;
    uint32 m_uiSonicScreech_Timer;
    uint32 m_uiSentinelBlast_Timer;
    uint32 m_uiTerrifyingScreech_Timer;
    uint32 m_uiSummon_Timer;
    uint8 m_uiSwarmcount;

    bool m_bHasBerserk;
    bool m_bIsDefender;

    void Reset()
    {
        m_uiEnrage_Timer = 600000;
        m_uiSentinelBlast_Timer = 120000;
        m_uiTerrifyingScreech_Timer = urand(30000, 45000);
        m_uiSonicScreech_Timer = urand(15000, 20000);
        m_uiSwarm_Timer         = urand(50000, 55000);

        m_uiSummon_Timer        = urand(60000, 70000);
        m_uiSwarmcount          = 10;
        m_bHasBerserk           = false;
        m_bIsDefender           = false;

        std::list<Creature*> lSentrys;
        GetCreatureListWithEntryInGrid(lSentrys, m_creature, NPC_SANCTUM_SENTRY, 200.0f);
        if (!lSentrys.empty())
        {
            for (std::list<Creature*>::iterator iter = lSentrys.begin(); iter != lSentrys.end(); ++iter)
            {
                if ((*iter) && (*iter)->isDead())
                    (*iter)->Respawn();
            }
        }

        // achievments
        m_bCrazyCatLady = true;
        m_bNineLives    = false;
    }

    void JustDied(Unit* pKiller)
    {
        //death yell
        DoScriptText(SAY_DEATH, m_creature);

        std::list<Creature*> lSentrys;
        GetCreatureListWithEntryInGrid(lSentrys, m_creature, NPC_SANCTUM_SENTRY, 200.0f);
        if (!lSentrys.empty())
        {
            for (std::list<Creature*>::iterator iter = lSentrys.begin(); iter != lSentrys.end(); ++iter)
            {
                if ((*iter))
                    (*iter)->ForcedDespawn();
            }
        }
        std::list<Creature*> lGuards;
        GetCreatureListWithEntryInGrid(lGuards, m_creature, MOB_SWARMING_GUARDIAN, 200.0f);
        if (!lGuards.empty())
        {
            for (std::list<Creature*>::iterator iter = lGuards.begin(); iter != lGuards.end(); ++iter)
            {
                if ((*iter))
                    (*iter)->ForcedDespawn();
            }
        }

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_AURIAYA, DONE);

            // hacky way to complete achievements; use only if you have this function
            if (m_bCrazyCatLady)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_CRAZY_CAT_LADY : ACHIEV_CRAZY_CAT_LADY_H);

            if (m_bNineLives)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_NINE_LIVES : ACHIEV_NINE_LIVES_H);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, IN_PROGRESS);

        std::list<Creature*> lSentrys;
        GetCreatureListWithEntryInGrid(lSentrys, m_creature, NPC_SANCTUM_SENTRY, 200.0f);
        if (!lSentrys.empty())
        {
            for (std::list<Creature*>::iterator iter = lSentrys.begin(); iter != lSentrys.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->SetInCombatWithZone();
            }
        }

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_SLAY1, m_creature);
        else
            DoScriptText(SAY_SLAY2, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiTerrifyingScreech_Timer < uiDiff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoScriptText(EMOTE_SCREECH, m_creature);
                DoCast(m_creature, SPELL_TERRIFYING_SCREECH);
                m_uiTerrifyingScreech_Timer = urand(30000, 45000);
                m_uiSentinelBlast_Timer = 3000;
            }
        }
        else
            m_uiTerrifyingScreech_Timer -= uiDiff;

        if (m_uiSentinelBlast_Timer < uiDiff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_SENTINEL_BLAST : SPELL_SENTINEL_BLAST_H);
                m_uiSentinelBlast_Timer = 120000;
            }
        }
        else
            m_uiSentinelBlast_Timer -= uiDiff;

        if (m_uiSonicScreech_Timer < uiDiff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SONIC_SCREECH : SPELL_SONIC_SCREECH_H);
                m_uiSonicScreech_Timer = urand(28000, 32000);
            }
        }
        else
            m_uiSonicScreech_Timer -= uiDiff;

        // summon swarm, spell needs core fix
        if (m_uiSwarm_Timer < uiDiff)
        {
            for (uint8 i = 0; i < 15; ++i)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (pTarget)
                {
                    float angle = (float)rand()*360/RAND_MAX + 1;
                    float homeX = pTarget->GetPositionX() + 8*cos(angle*(M_PI/180));
                    float homeY = pTarget->GetPositionY() + 8*sin(angle*(M_PI/180));
                    if (Creature* pSwarm = m_creature->SummonCreature(MOB_SWARMING_GUARDIAN, homeX, homeY, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                        pSwarm->SetInCombatWithZone();
                }
            }
            m_uiSwarm_Timer = urand(33000, 37000);
        }
        else
            m_uiSwarm_Timer -= uiDiff;

        if (!m_bIsDefender)
        {
            if (m_uiSummon_Timer < uiDiff)
            {
                if (Creature* pDefender = DoSpawnCreature(MOB_FERAL_DEFENDER, frand(-3.0f, 3.0f), frand(-3.0f, 3.0f), 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 40000))
                {
                    pDefender->SetInCombatWithZone();
                    DoScriptText(EMOTE_DEFENDER, m_creature);
                    m_bIsDefender = true;
                }
            }
            else
                m_uiSummon_Timer -= uiDiff;
        }

        if (!m_bHasBerserk)
        {
            if (m_uiEnrage_Timer < uiDiff)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_creature->CastStop();
                DoCast(m_creature, SPELL_BERSERK);
                m_bHasBerserk = true;
            }
            else
                m_uiEnrage_Timer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriayaAI(pCreature);
}

void AddSC_boss_auriaya()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_auriaya";
    NewScript->GetAI = GetAI_boss_auriaya;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_seeping_feral_essence";
    NewScript->GetAI = &GetAI_mob_seeping_feral_essence;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_sanctum_sentry";
    NewScript->GetAI = &GetAI_mob_sanctum_sentry;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_feral_defender";
    NewScript->GetAI = &GetAI_mob_feral_defender;
    NewScript->RegisterSelf();
}
