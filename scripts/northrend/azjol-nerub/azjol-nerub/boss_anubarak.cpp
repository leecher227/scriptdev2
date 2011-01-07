/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Anubarak
SD%Complete: 100%
SDComment:
SDAuthor: MaxXx2021
SDCategory: Azjol'NerubstrInstData
EndScriptData */


#include "precompiled.h"
#include "azjol-nerub.h"

/*######
## boss_anubarak
######*/

enum
{
    SPELL_CARRION_BEETLES            = 53520,
    SPELL_LEECHING_SWARM             = 53467,
    SPELL_IMPALE_GROUND              = 53455,
    SPELL_SUMMON_IMPALE              = 53456, //summon impale in random position
    SPELL_IMPALE_N                   = 53454,
    SPELL_IMPALE_H                   = 59446,
    SPELL_POUND_N                    = 53472,
    SPELL_POUND_H                    = 59433,
    SPELL_SUBMERGE                   = 53421,

    NPC_GUARDIAN                     = 29216,
    NPC_VENOMANCER                   = 29217,
    NPC_DATTER                       = 29213,
    NPC_IMPALE_TARGET                = 29184,

    SAY_INTRO                       = -1601014,
    SAY_AGGRO                       = -1601015,
    SAY_KILL_1                      = -1601016,
    SAY_KILL_2                      = -1601017,
    SAY_KILL_3                      = -1601018,
    SAY_SUBMERGE_1                  = -1601019,
    SAY_SUBMERGE_2                  = -1601020,
    SAY_LOCUST_1                    = -1601021,
    SAY_LOCUST_2                    = -1601022,
    SAY_LOCUST_3                    = -1601023,
    SAY_DEATH                       = -1601024
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations SpawnPoint[2] =
{
    {550.7f, 282.8f, 224.3f},
    {551.1f, 229.4f, 224.3f}
};

struct MANGOS_DLL_DECL boss_anubarakAI : public ScriptedAI
{
    boss_anubarakAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance *pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint8 m_uiSubmergedCount;
    uint8 m_uiSummonCount;

    uint32 m_uiStepTimer;
    uint32 m_uiStep;

    uint32 m_uiCarrionBeetlesTimer;
    uint32 m_uiLeechingSwarmTimer;
    uint32 m_uiPoundTimer;
    uint32 m_uiSummonTimer;

    void Reset()
    {
        m_uiCarrionBeetlesTimer = 8000;
        m_uiLeechingSwarmTimer = 20000;
        m_uiPoundTimer = 15000;

        m_uiPhase = 0;
        m_uiStepTimer = 100;
        m_uiStep = 1;
        m_uiSubmergedCount = 0;
        m_uiSummonCount = 1;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);

        DespawnAll();

        if (pInstance)
            pInstance->SetData(TYPE_ANUBARAK, NOT_STARTED);
    }

    void DespawnAll()
    {
        std::list<Creature*> m_pGuard;
        GetCreatureListWithEntryInGrid(m_pGuard, m_creature, NPC_GUARDIAN, DEFAULT_VISIBILITY_INSTANCE);

        if (!m_pGuard.empty())
            for (std::list<Creature*>::iterator itr = m_pGuard.begin(); itr != m_pGuard.end(); ++itr)
                (*itr)->ForcedDespawn();

        std::list<Creature*> m_pVen;
        GetCreatureListWithEntryInGrid(m_pVen, m_creature, NPC_VENOMANCER, DEFAULT_VISIBILITY_INSTANCE);

        if (!m_pVen.empty())
            for (std::list<Creature*>::iterator iter = m_pVen.begin(); iter != m_pVen.end(); ++iter)
                (*iter)->ForcedDespawn();

        std::list<Creature*> m_pDat;
        GetCreatureListWithEntryInGrid(m_pDat, m_creature, NPC_DATTER, DEFAULT_VISIBILITY_INSTANCE);

        if (!m_pDat.empty())
            for (std::list<Creature*>::iterator iter = m_pDat.begin(); iter != m_pDat.end(); ++iter)
                (*iter)->ForcedDespawn();
    }

    void EnterCombat(Unit *pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (pInstance)
            pInstance->SetData(TYPE_ANUBARAK, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_uiPhase == 0 && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 15.0f))
            m_uiPhase = 1;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Intro()
    {
        switch(m_uiStep)
        {
            case 1:
                DoScriptText(SAY_INTRO, m_creature);
                m_uiStepTimer = 23000;
                m_uiStep++;
                break;
            case 2:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetInCombatWithZone();
                m_uiPhase = 2;
                break;
        }
    }

    void SubmergedPhase()
    {
        m_creature->InterruptNonMeleeSpells(false);
        m_uiSubmergedCount++;
        m_uiSummonCount = 1;

        m_uiStepTimer = 40000;
        m_uiSummonTimer = 10000;

        m_creature->CastSpell(m_creature, SPELL_SUMMON_IMPALE, true);
        m_creature->CastSpell(m_creature, SPELL_SUBMERGE, false);


        switch(urand(0, 1))
        {
            case 0: DoScriptText(SAY_SUBMERGE_1, m_creature); break;
            case 1: DoScriptText(SAY_SUBMERGE_2, m_creature); break;
        }
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);

        m_uiPhase = 3;
    }

    void NormalPhase()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
        m_creature->RemoveAurasDueToSpell(SPELL_SUMMON_IMPALE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
        m_creature->CastSpell(m_creature, 50142, true);
        m_uiPhase = 2;
    }

    void JustSummoned(Creature* pSummoned)
    { 
        pSummoned->SetRespawnDelay(DAY);
        if (pSummoned->GetEntry() != NPC_IMPALE_TARGET)
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            case 1:
                if (m_uiStepTimer <= uiDiff)
                    Intro();
                else
                   m_uiStepTimer -= uiDiff;
                break;
            case 2:
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if (m_creature->GetHealthPercent() < 100-25*(m_uiSubmergedCount+1))
                    SubmergedPhase();

                if (m_creature->GetDistance2d(551.018f, 248.278f) > 50.0f)
                    m_creature->AI()->EnterEvadeMode();

                if (m_uiLeechingSwarmTimer <= uiDiff)
                {
                    switch(urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_LOCUST_1, m_creature); break;
                        case 1: DoScriptText(SAY_LOCUST_2, m_creature); break;
                        case 2: DoScriptText(SAY_LOCUST_3, m_creature); break;
                    }
                    DoCast(m_creature, SPELL_LEECHING_SWARM);
                    m_uiLeechingSwarmTimer = 19000;
                }
                else
                    m_uiLeechingSwarmTimer -= uiDiff;

                if (m_uiCarrionBeetlesTimer <= uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature->getVictim(), SPELL_CARRION_BEETLES);
                    m_uiCarrionBeetlesTimer = 25000;
                }
                else
                    m_uiCarrionBeetlesTimer -= uiDiff;

                if (m_uiPoundTimer <= uiDiff)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_POUND_N : SPELL_POUND_H);
                    m_uiPoundTimer = 16500;
                }
                else
                    m_uiPoundTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            case 3:
                if (m_uiStepTimer <= uiDiff)
                    NormalPhase();
                else
                    m_uiStepTimer -= uiDiff;

                if (m_uiSummonTimer < uiDiff)
                {
                    switch(m_uiSummonCount)
                    {
                        case 1:
                            for (uint8 i = 0; i < 2; ++i)
                                m_creature->SummonCreature(NPC_GUARDIAN, SpawnPoint[i].x, SpawnPoint[i].y, SpawnPoint[i].z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000);
                            break;
                        case 2:
                            for (uint8 i = 0; i < 2; ++i)
                                m_creature->SummonCreature(NPC_VENOMANCER, SpawnPoint[i].x, SpawnPoint[i].y, SpawnPoint[i].z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000);
                            break;
                        case 3:
                            for (uint8 i = 0; i < 2; ++i)
                                m_creature->SummonCreature(NPC_DATTER, SpawnPoint[i].x, SpawnPoint[i].y, SpawnPoint[i].z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000);
                            break;
                        case 4:
                            break;
                    }
                    if (m_uiSummonCount < m_uiSubmergedCount)
                        m_uiSummonCount++;
                    else
                        m_uiSummonCount = 4;
                    m_uiSummonTimer = 10000;
                }
                else
                    m_uiSummonTimer -= uiDiff;
                break;
        }
    }

    void JustDied(Unit *pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (pInstance)
            pInstance->SetData(TYPE_ANUBARAK, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

};

CreatureAI* GetAI_boss_anubarak(Creature *pCreature)
{
    return new boss_anubarakAI (pCreature);
}

struct MANGOS_DLL_DECL npc_anubarak_impaleAI : public ScriptedAI
{
    npc_anubarak_impaleAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance *pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiImpaleTimer;

    void Reset()
    { 
        m_creature->CastSpell(m_creature, SPELL_IMPALE_GROUND, false);
        m_uiImpaleTimer = 2000;
        m_creature->ForcedDespawn(3000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->HasAura(SPELL_IMPALE_GROUND))
        {
            if (m_uiImpaleTimer < uiDiff)
            {
                m_creature->RemoveAurasDueToSpell(SPELL_IMPALE_GROUND);
                m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_IMPALE_N : SPELL_IMPALE_H, false);
            }
            else
                m_uiImpaleTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_anubarak_impale(Creature *pCreature)
{
    return new npc_anubarak_impaleAI (pCreature);
}

void AddSC_boss_anubarak()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_anubarak";
    newscript->GetAI = &GetAI_boss_anubarak;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_anubarak_impale";
    newscript->GetAI = &GetAI_npc_anubarak_impale;
    newscript->RegisterSelf();
}
