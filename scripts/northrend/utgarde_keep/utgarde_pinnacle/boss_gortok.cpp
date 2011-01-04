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
SDName: Boss_Gortok
SD%Complete: 90%
SDComment:
SDAuthor: MaxXx2021 Aka Mioka.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO               = -1575015,
    SAY_SLAY_1              = -1575016,
    SAY_SLAY_2              = -1575017,
    SAY_DEATH               = -1575018,

    //Others Spells

    SPELL_ORB_VISUAL        = 48044,
    SPELL_ORB_CHANNEL       = 48048,
    SPELL_FREEZE_ANIM       = 16245,
    SPELL_WAKEUP_GORTOK     = 47670,
    SPELL_AWAKEN_SUBBOSS    = 47669,

    //Gortok Spell's
    SPELL_IMPALE            = 48261,
    SPELL_IMPALE_H          = 59268,

    SPELL_WITHERING_ROAR    = 48256,
    SPELL_WITHERING_ROAR_H  = 59267,

    SPELL_ARCING_SMASH      = 48260,

    //FurlBorg Spell's
    SPELL_CHAIN_LIGHTING_N  = 48140,
    SPELL_CHAIN_LIGHTING_H  = 59273,
    SPELL_CRAZED            = 48139,
    SPELL_TERRIFYING_ROAR   = 48144,

    //Worgen Spells
    SPELL_MORTAL_WOUND_N    = 48137,
    SPELL_MORTAL_WOUND_H    = 59265,
    SPELL_ENRAGE_1          = 48138,
    SPELL_ENRAGE_2          = 48142,

    //Rhino Spells
    SPELL_GORE_N            = 48130,
    SPELL_GORE_H            = 59264,
    SPELL_GRIEVOUS_WOUND_N  = 48105,
    SPELL_GRIEVOUS_WOUND_H  = 59263,
    SPELL_STOMP             = 48131,

    //Jormungar Spells
    SPELL_ACID_SPIT         = 48132,
    SPELL_ACID_SPLATTER_N   = 48136,
    SPELL_ACID_SPLATTER_H   = 59272,
    SPELL_POISON_BREATH_N   = 48133,
    SPELL_POISON_BREATH_H   = 59271,

    POINT_ID_ORB            = 1
};

#define ORB_SPAWN_X             238.61f
#define ORB_SPAWN_Y             -460.71f
#define ORB_MOVE_X              279.11f
#define ORB_MOVE_Y              -452.01f
#define ORB_Z                   109.57f

struct MANGOS_DLL_DECL npc_gortok_orbAI : public ScriptedAI
{
    npc_gortok_orbAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiStepTimer;
    uint32 m_uiStep;
    bool m_bTimeToAct;
    uint32 m_uiBossEntry;
    uint8 m_uiBossCount;

    void Reset()
    {
        m_uiStepTimer = 4000;
        m_uiStep = 1;
        m_bTimeToAct = true;
        m_uiBossEntry = urand(NPC_WORGEN, NPC_RHINO);
        m_uiBossCount = 0;
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetDisplayId(16925);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_NON_ATTACKABLE);
        m_creature->CastSpell(m_creature, SPELL_ORB_VISUAL, true);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_ORB)
            DoAction();
    }

    void DoAction()
    {
        m_uiStepTimer = 2000;
        m_uiStep = 3;
        m_bTimeToAct = true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_bTimeToAct)
            if (m_uiStepTimer <= uiDiff)
            {
                switch (m_uiStep)
                {
                    case 1:
                        m_creature->SendMonsterMoveWithSpeed(ORB_SPAWN_X, ORB_SPAWN_Y, ORB_Z+4.0f, 4000);
                        m_creature->GetMap()->CreatureRelocation(m_creature, ORB_SPAWN_X, ORB_SPAWN_Y, ORB_Z+4.0f, 0);
                        m_uiStepTimer = 4000;
                        break;
                    case 2:
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_ORB, ORB_MOVE_X, ORB_MOVE_Y, ORB_Z+2.0f);
                        m_bTimeToAct = false;
                        break;
                    case 3:
                        if (Creature* pBoss = m_pInstance->instance->GetCreature(m_pInstance->GetData64(m_uiBossEntry)))
                        {
                            pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_WAKEUP_GORTOK);
                            if (pTempSpell)
                            {
                                pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                                pTempSpell->EffectImplicitTargetB[0] = 0;
                                pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
                                pTempSpell->EffectImplicitTargetB[1] = 0;
                                pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
                                pTempSpell->EffectImplicitTargetB[2] = 0;
                                m_creature->CastSpell(pBoss, pTempSpell, true);
                            }
                        }
                        m_uiStepTimer = 10000;
                        break;
                    case 4:
                        if (Creature* pBoss = m_pInstance->instance->GetCreature(m_pInstance->GetData64(m_uiBossEntry)))
                        {
                            pBoss->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                            pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pBoss->SetInCombatWithZone();
                        }
                        ++m_uiBossCount;
                        ++m_uiBossEntry;
                        if (m_uiBossEntry > NPC_RHINO)
                            m_uiBossEntry = NPC_WORGEN;
                        if (m_uiBossCount == 4)
                            m_uiBossEntry = NPC_GORTOK;
                        if (m_uiBossCount == 5)
                            m_creature->ForcedDespawn();
                        m_bTimeToAct = false;
                        break;
                }
                ++m_uiStep;
            }
            else
                m_uiStepTimer -= uiDiff;
    }
};

/*######
## boss_gortok
######*/

struct MANGOS_DLL_DECL boss_gortokAI : public ScriptedAI
{
    boss_gortokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiArcingSmashTimer;
    uint32 uiImpaleTimer;
    uint32 uiWhiteringRoarTimer;

    void Reset()
    {
        uiArcingSmashTimer = 15000;
        uiImpaleTimer = 12000;
        uiWhiteringRoarTimer = 10000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCast(m_creature, SPELL_FREEZE_ANIM);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_GORTOK, NOT_STARTED);

            Creature* pTemp;
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_WORGEN))) && pTemp->isDead())
                pTemp->Respawn();
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RHINO))) && pTemp->isDead())
                pTemp->Respawn();
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_JORMUNGAR))) && pTemp->isDead())
                pTemp->Respawn();
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_FURLBORG))) && pTemp->isDead())
                pTemp->Respawn();

            if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_STASIS_GENERATOR)))
            {
                pGo->SetGoState(GO_STATE_READY);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
            }

            if (Creature* pController = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STASIS_CONTROLLER)))
                pController->ForcedDespawn();
        }
    }

    void JustReachedHome()
    {
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void AttackStart(Unit* who)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(m_pInstance)
           m_pInstance->SetData(TYPE_GORTOK, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiArcingSmashTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_ARCING_SMASH);
            uiArcingSmashTimer = urand(13000, 17000);
        }
        else
            uiArcingSmashTimer -= uiDiff;

        if (uiImpaleTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_IMPALE);
            uiImpaleTimer = urand(8000, 12000);
        }
        else
            uiImpaleTimer -= uiDiff;

        if (uiWhiteringRoarTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_WITHERING_ROAR);
            uiWhiteringRoarTimer = urand(8000, 12000);
        }
        else
            uiWhiteringRoarTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_furlborgAI : public ScriptedAI
{
    npc_furlborgAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiChainLightingTimer;
    uint32 uiCrazedTimer;
    uint32 uiTerrifyingRoarTimer;

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uiChainLightingTimer = 5000;
        uiCrazedTimer = 10000;
        uiTerrifyingRoarTimer = 15000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCast(m_creature, SPELL_FREEZE_ANIM);

        if (m_pInstance)
            if (Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
                if (pGortok->isAlive())
                    ((boss_gortokAI*)pGortok->AI())->Reset();
    }

    void JustReachedHome()
    {
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* who)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            if (Creature* pController = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STASIS_CONTROLLER)))
                ((npc_gortok_orbAI*)pController->AI())->DoAction();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiChainLightingTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTING_N : SPELL_CHAIN_LIGHTING_H);
            uiChainLightingTimer = 5000 + rand()%5000;
        }
        else
            uiChainLightingTimer -=  uiDiff;

        if (uiCrazedTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_CRAZED);
            uiCrazedTimer = 8000 + rand()%4000;
        }
        else
            uiCrazedTimer -=  uiDiff;

        if (uiTerrifyingRoarTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_TERRIFYING_ROAR);
            uiTerrifyingRoarTimer = 10000 + rand()%10000;
        }
        else
            uiTerrifyingRoarTimer -=  uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_worgenAI : public ScriptedAI
{
    npc_worgenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiMortalWoundTimer;
    uint32 uiEnrage1Timer;
    uint32 uiEnrage2Timer;

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uint32 uiMortalWoundTimer = 5000;
        uint32 uiEnrage1Timer = 15000;
        uint32 uiEnrage2Timer = 10000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCast(m_creature, SPELL_FREEZE_ANIM);

        if (m_pInstance)
            if (Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
                if (pGortok->isAlive())
                    ((boss_gortokAI*)pGortok->AI())->Reset();
    }

    void JustReachedHome()
    {
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* who)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            if (Creature* pController = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STASIS_CONTROLLER)))
                ((npc_gortok_orbAI*)pController->AI())->DoAction();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiMortalWoundTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MORTAL_WOUND_N : SPELL_MORTAL_WOUND_H);
            uiMortalWoundTimer = 3000 + rand()%4000;
        }
        else
            uiMortalWoundTimer -= uiDiff;

        if (uiEnrage1Timer <= uiDiff)
        {
            DoCast(m_creature, SPELL_ENRAGE_1);
            uiEnrage1Timer = 15000;
        }
        else
            uiEnrage1Timer -= uiDiff;

        if (uiEnrage2Timer <= uiDiff)
        {
            DoCast(m_creature, SPELL_ENRAGE_2);
            uiEnrage2Timer = 10000;
        }
        else
            uiEnrage2Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_rhinoAI : public ScriptedAI
{
    npc_rhinoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiStompTimer;
    uint32 uiGoreTimer;
    uint32 uiGrievousWoundTimer;

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uiStompTimer = 10000;
        uiGoreTimer = 15000;
        uiGrievousWoundTimer = 20000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCast(m_creature, SPELL_FREEZE_ANIM);

        if (m_pInstance)
            if (Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
                if (pGortok->isAlive())
                    ((boss_gortokAI*)pGortok->AI())->Reset();
    }

    void JustReachedHome()
    {
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* who)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            if (Creature* pController = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STASIS_CONTROLLER)))
                ((npc_gortok_orbAI*)pController->AI())->DoAction();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiStompTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_STOMP);
            uiStompTimer = 8000 + rand()%4000;
        }
        else
            uiStompTimer -= uiDiff;

        if (uiGoreTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_GORE_N : SPELL_GORE_H);
            uiGoreTimer = 13000 + rand()%4000;
        }
        else
            uiGoreTimer -= uiDiff;

        if (uiGrievousWoundTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_GRIEVOUS_WOUND_N : SPELL_GRIEVOUS_WOUND_H);
            uiGrievousWoundTimer = 18000 + rand()%4000;
        }
        else
            uiGrievousWoundTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_jormungarAI : public ScriptedAI
{
    npc_jormungarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiAcidSpitTimer;
    uint32 uiAcidSplatterTimer;
    uint32 uiPoisonBreathTimer;

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uiAcidSpitTimer = 3000;
        uiAcidSplatterTimer = 12000;
        uiPoisonBreathTimer = 10000;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCast(m_creature, SPELL_FREEZE_ANIM);

        if (m_pInstance)
            if (Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
                if (pGortok->isAlive())
                    ((boss_gortokAI*)pGortok->AI())->Reset();
    }

    void JustReachedHome()
    {
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void AttackStart(Unit* who)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            if (Creature* pController = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STASIS_CONTROLLER)))
                ((npc_gortok_orbAI*)pController->AI())->DoAction();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiAcidSpitTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ACID_SPIT);
            uiAcidSpitTimer = 2000 + rand()%2000;
        }
        else
            uiAcidSpitTimer -= uiDiff;

        if (uiAcidSplatterTimer <= uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ACID_SPLATTER_N : SPELL_ACID_SPLATTER_H);
            uiAcidSplatterTimer = 10000 + rand()%4000;
        }
        else
            uiAcidSplatterTimer -= uiDiff;

        if (uiPoisonBreathTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_POISON_BREATH_N : SPELL_POISON_BREATH_H);
            uiPoisonBreathTimer = 8000 + rand()%4000;
        }
        else
            uiPoisonBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool GOUse_gortok_generator(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
    {
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        pGo->SetGoState(GO_STATE_ACTIVE);
        if (Creature* pController = pGo->SummonCreature(NPC_STASIS_CONTROLLER, ORB_SPAWN_X, ORB_SPAWN_Y, ORB_Z, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
            m_pInstance->SetData64(NPC_STASIS_CONTROLLER, pController->GetGUID());
        m_pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
    }
    return true;
}

CreatureAI* GetAI_boss_gortok(Creature* pCreature)
{
    return new boss_gortokAI(pCreature);
}

CreatureAI* GetAI_npc_worgen(Creature* pCreature)
{
    return new npc_worgenAI(pCreature);
}

CreatureAI* GetAI_npc_furlborg(Creature* pCreature)
{
    return new npc_furlborgAI(pCreature);
}

CreatureAI* GetAI_npc_rhino(Creature* pCreature)
{
    return new npc_rhinoAI(pCreature);
}

CreatureAI* GetAI_npc_jormungar(Creature* pCreature)
{
    return new npc_jormungarAI(pCreature);
}

CreatureAI* GetAI_npc_gortok_orb(Creature* pCreature)
{
    return new npc_gortok_orbAI(pCreature);
}

void AddSC_boss_gortok()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_gortok";
    newscript->GetAI = &GetAI_boss_gortok;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_worgen";
    newscript->GetAI = &GetAI_npc_worgen;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_furlborg";
    newscript->GetAI = &GetAI_npc_furlborg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rhino";
    newscript->GetAI = &GetAI_npc_rhino;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_jormungar";
    newscript->GetAI = &GetAI_npc_jormungar;
    newscript->RegisterSelf();
	
    newscript = new Script;
    newscript->Name = "npc_gortok_orb";
    newscript->GetAI = &GetAI_npc_gortok_orb;
    newscript->RegisterSelf();
	
    newscript = new Script;
    newscript->Name = "go_gortok_generator";
    newscript->pGOUse=&GOUse_gortok_generator;
    newscript->RegisterSelf();
}
