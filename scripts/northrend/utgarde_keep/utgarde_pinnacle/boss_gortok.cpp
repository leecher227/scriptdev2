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
SDName: Boss_Gortok
SD%Complete: 90%
SDComment: Нужно проверить таймера кастов, взяты с тринити.
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
    SPELL_WAKEUP_GORTOK     = 47670, //need spell script Target on 26687, and huck on other critter.

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

    POINT_ID_WORGEN             = 1,
    POINT_ID_FURLBORG           = 2,
    POINT_ID_JORMUNGAR          = 3,
    POINT_ID_RHINO              = 4,
    POINT_ID_GORTOK             = 5
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

struct Locations MoveLocs[]=
{
    {0, 0, 0},
    {261.6f, -449.3f, 109.5f},
    {263.3f, -454.0f, 109.5f},
    {291.5f, -450.4f, 109.5f},
    {291.5f, -454.0f, 109.5f},
    {310.0f, -453.4f, 109.5f},
    {238.6f, -460.7f, 109.5f}
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
    uint32 uiStepTimer;
    uint32 uiStep;

    uint8 uiAddCount;

    void Reset()
    {
        uiArcingSmashTimer = 15000;
        uiImpaleTimer = 12000;
        uiWhiteringRoarTimer = 10000;

        uiAddCount = 0;
        uiStep = 0;
        uiStepTimer = 100;

        m_creature->GetMotionMaster()->MoveTargetedHome();

        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
            {
               m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
               m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
               m_creature->SetStandState(UNIT_STAND_STATE_STAND);
               DoCast(m_creature, SPELL_FREEZE_ANIM);
            }

            m_pInstance->SetData(TYPE_GORTOK, NOT_STARTED);

            Creature* pTemp;
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_WORGEN))) && !pTemp->isAlive())
                pTemp->Respawn();
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RHINO))) && !pTemp->isAlive())
                pTemp->Respawn();
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_JORMUNGAR))) && !pTemp->isAlive())
                pTemp->Respawn();
            if ((pTemp = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_FURLBORG))) && !pTemp->isAlive())
                pTemp->Respawn();

            if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_STASIS_GENERATOR)))
            {
                pGo->SetGoState(GO_STATE_READY);
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

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

    void JustReachedHome()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }

    void StartEvent()
    {
        uiAddCount++;
        uiStep = 1;
        uiStepTimer = 100;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
          if(uiStepTimer <= uiDiff)
          {
            switch(uiStep)
            {
              case 1:
                 if(Creature* pOrb = m_creature->SummonCreature(NPC_STASIS_CONTROLLER, MoveLocs[6].x, MoveLocs[6].y, MoveLocs[6].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 10000))
                 {
                    pOrb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pOrb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
                    m_pInstance->SetData64(NPC_STASIS_CONTROLLER, pOrb->GetGUID());
                    pOrb->CastSpell(pOrb,SPELL_ORB_VISUAL,true);
                 }
                 uiStepTimer = 4000;
                 uiStep++;
                 break;
              case 2:
                 if(Creature* pOrb = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_STASIS_CONTROLLER)))
                    pOrb->GetMotionMaster()->MovePoint(uiAddCount, MoveLocs[uiAddCount].x, MoveLocs[uiAddCount].y, MoveLocs[uiAddCount].z);
                 uiStep++;
                 break;
            } 
          } else uiStepTimer -= uiDiff;

          return;
        }
          else
        {
          if (uiArcingSmashTimer <= uiDiff)
          {
              DoCast(m_creature, SPELL_ARCING_SMASH);
              uiArcingSmashTimer = urand(13000,17000);
          } else uiArcingSmashTimer -= uiDiff;

          if (uiImpaleTimer <= uiDiff)
          {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_IMPALE);
            uiImpaleTimer = urand(8000,12000);
          } else uiImpaleTimer -= uiDiff;

          if (uiWhiteringRoarTimer <= uiDiff)
          {
              DoCast(m_creature, SPELL_WITHERING_ROAR);
              uiWhiteringRoarTimer = urand(8000,12000);
          } else uiWhiteringRoarTimer -= uiDiff;

          DoMeleeAttackIfReady();
        }
    }
};

struct MANGOS_DLL_DECL npc_furlborgAI : public ScriptedAI
{
    npc_furlborgAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiChainLightingTimer;
    uint32 uiCrazedTimer;
    uint32 uiTerrifyingRoarTimer;

    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uiChainLightingTimer = 5000;
        uiCrazedTimer = 10000;
        uiTerrifyingRoarTimer = 15000;

        m_creature->GetMotionMaster()->MoveTargetedHome();

        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
        {
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
           m_creature->SetStandState(UNIT_STAND_STATE_STAND);
           DoCast(m_creature, SPELL_FREEZE_ANIM);
        }

        if(m_pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS)
        {
           if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
             if(pGortok->isAlive())
               ((boss_gortokAI*)pGortok->AI())->Reset();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiChainLightingTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTING_N : SPELL_CHAIN_LIGHTING_H);
            uiChainLightingTimer = 5000 + rand()%5000;
        } else uiChainLightingTimer -=  uiDiff;

        if (uiCrazedTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_CRAZED);
            uiCrazedTimer = 8000 + rand()%4000;
        } else uiCrazedTimer -=  uiDiff;

        if (uiTerrifyingRoarTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_TERRIFYING_ROAR);
            uiTerrifyingRoarTimer = 10000 + rand()%10000;
        } else uiTerrifyingRoarTimer -=  uiDiff;

        DoMeleeAttackIfReady();
    }

    void AttackStart(Unit* who)
    {
        if(!who)
           return;

        if(m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
          if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
            ((boss_gortokAI*)pGortok->AI())->StartEvent();
        }
    }

    void JustReachedHome()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }
};

struct MANGOS_DLL_DECL npc_worgenAI : public ScriptedAI
{
    npc_worgenAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiMortalWoundTimer;
    uint32 uiEnrage1Timer;
    uint32 uiEnrage2Timer;

    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uint32 uiMortalWoundTimer = 5000;
        uint32 uiEnrage1Timer = 15000;
        uint32 uiEnrage2Timer = 10000;

        m_creature->GetMotionMaster()->MoveTargetedHome();

        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
        {
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
           m_creature->SetStandState(UNIT_STAND_STATE_STAND);
           DoCast(m_creature, SPELL_FREEZE_ANIM);
        }

        if(m_pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS)
        {
           if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
             if(pGortok->isAlive())
               ((boss_gortokAI*)pGortok->AI())->Reset();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiMortalWoundTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MORTAL_WOUND_N : SPELL_MORTAL_WOUND_H);
            uiMortalWoundTimer = 3000 + rand()%4000;
        } else uiMortalWoundTimer -= uiDiff;

        if (uiEnrage1Timer <= uiDiff)
        {
            DoCast(m_creature, SPELL_ENRAGE_1);
            uiEnrage1Timer = 15000;
        } else uiEnrage1Timer -= uiDiff;

        if (uiEnrage2Timer <= uiDiff)
        {
            DoCast(m_creature, SPELL_ENRAGE_2);
            uiEnrage2Timer = 10000;
        } else uiEnrage2Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
          if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
            ((boss_gortokAI*)pGortok->AI())->StartEvent();
        }
    }

    void JustReachedHome()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }
};

struct MANGOS_DLL_DECL npc_rhinoAI : public ScriptedAI
{
    npc_rhinoAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiStompTimer;
    uint32 uiGoreTimer;
    uint32 uiGrievousWoundTimer;

    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uiStompTimer = 10000;
        uiGoreTimer = 15000;
        uiGrievousWoundTimer = 20000;

        m_creature->GetMotionMaster()->MoveTargetedHome();

        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
        {
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
           m_creature->SetStandState(UNIT_STAND_STATE_STAND);
           DoCast(m_creature, SPELL_FREEZE_ANIM);
        }

        if(m_pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS)
        {
           if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
             if(pGortok->isAlive())
               ((boss_gortokAI*)pGortok->AI())->Reset();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiStompTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_STOMP);
            uiStompTimer = 8000 + rand()%4000;
        } else uiStompTimer -= uiDiff;

        if (uiGoreTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_GORE_N : SPELL_GORE_H);
            uiGoreTimer = 13000 + rand()%4000;
        } else uiGoreTimer -= uiDiff;

        if (uiGrievousWoundTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_GRIEVOUS_WOUND_N : SPELL_GRIEVOUS_WOUND_H);
            uiGrievousWoundTimer = 18000 + rand()%4000;
        } else uiGrievousWoundTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
          if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
            ((boss_gortokAI*)pGortok->AI())->StartEvent();
        }
    }

    void JustReachedHome()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }
};

struct MANGOS_DLL_DECL npc_jormungarAI : public ScriptedAI
{
    npc_jormungarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 uiAcidSpitTimer;
    uint32 uiAcidSplatterTimer;
    uint32 uiPoisonBreathTimer;

    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        uiAcidSpitTimer = 3000;
        uiAcidSplatterTimer = 12000;
        uiPoisonBreathTimer = 10000;

        m_creature->GetMotionMaster()->MoveTargetedHome();

        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
        {
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
           m_creature->SetStandState(UNIT_STAND_STATE_STAND);
           DoCast(m_creature, SPELL_FREEZE_ANIM);
        }

        if(m_pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS)
        {
           if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
             if(pGortok->isAlive())
               ((boss_gortokAI*)pGortok->AI())->Reset();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (uiAcidSpitTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ACID_SPIT);
            uiAcidSpitTimer = 2000 + rand()%2000;
        } else uiAcidSpitTimer -= uiDiff;

        if (uiAcidSplatterTimer <= uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ACID_SPLATTER_N : SPELL_ACID_SPLATTER_H);
            uiAcidSplatterTimer = 10000 + rand()%4000;
        } else uiAcidSplatterTimer -= uiDiff;

        if (uiPoisonBreathTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_POISON_BREATH_N : SPELL_POISON_BREATH_H);
            uiPoisonBreathTimer = 8000 + rand()%4000;
        } else uiPoisonBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
          if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
            ((boss_gortokAI*)pGortok->AI())->StartEvent();
        }
    }

    void JustReachedHome()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoCast(m_creature, SPELL_FREEZE_ANIM);
    }
};

struct MANGOS_DLL_DECL npc_gortok_orbAI : public ScriptedAI
{
    npc_gortok_orbAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint32 uiStepTimer;
    uint32 uiStep;

    void Reset()
    {
        uiStepTimer = 5000;
        uiStep = 0;
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetDisplayId(11686);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if(!m_pInstance) return;

        if (uiPointId == POINT_ID_WORGEN)
        {
            if(Creature* pWorgen = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_WORGEN)))
            {
              m_creature->CastSpell(m_creature, SPELL_WAKEUP_GORTOK, true);
              uiStepTimer = 5000;
              uiStep = 1;
            }
            return;
        }

        if (uiPointId == POINT_ID_FURLBORG)
        {
            if(Creature* pFurlborg = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_FURLBORG)))
            {
              m_creature->CastSpell(m_creature, SPELL_WAKEUP_GORTOK, true);
              uiStepTimer = 5000;
              uiStep = 2;
            }
            return;
        }

        if (uiPointId == POINT_ID_JORMUNGAR)
        {
            if(Creature* pJormungar = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_JORMUNGAR)))
            {
              m_creature->CastSpell(m_creature, SPELL_WAKEUP_GORTOK, true);
              uiStepTimer = 5000;
              uiStep = 3;
            }
            return;
        }

        if (uiPointId == POINT_ID_RHINO)
        {
            if(Creature* pRhino = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RHINO)))
            {
              m_creature->CastSpell(m_creature, SPELL_WAKEUP_GORTOK, true);
              uiStepTimer = 5000;
              uiStep = 4;
            }
            return;
        }

        if (uiPointId == POINT_ID_GORTOK)
        {
            if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
            {
              m_creature->CastSpell(pGortok, SPELL_WAKEUP_GORTOK, true);
              uiStepTimer = 5000;
              uiStep = 5;
            }
            return;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
      if(!m_pInstance) return;

      if(uiStepTimer <= uiDiff)
      {
        switch(uiStep)
        {
          case 1:
             if(Creature* pWorgen = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_WORGEN)))
             { 
               pWorgen->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
               pWorgen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
               pWorgen->SetInCombatWithZone();
             }
             m_creature->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
             m_creature->ForcedDespawn();
             break;
          case 2:
             if(Creature* pFurlborg = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_FURLBORG)))
             {
               pFurlborg->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
               pFurlborg->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
               pFurlborg->SetInCombatWithZone();
             }
             m_creature->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
             m_creature->ForcedDespawn();
             break;
          case 3:
             if(Creature* pJormungar = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_JORMUNGAR)))
             {
               pJormungar->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
               pJormungar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
               pJormungar->SetInCombatWithZone();
             }
             m_creature->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
             m_creature->ForcedDespawn();
             break;
          case 4:
             if(Creature* pRhino = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RHINO)))
             {
               pRhino->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
               pRhino->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
               pRhino->SetInCombatWithZone();
             }
             m_creature->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
             m_creature->ForcedDespawn();
             break;
          case 5:
             if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
             {
               pGortok->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
               pGortok->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
               pGortok->SetInCombatWithZone();
             }
             m_creature->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
             m_creature->ForcedDespawn();
             break;
        }
      } else uiStepTimer -= uiDiff;

        return;
    }
};

bool GOHello_gortok_generator(Player *pPlayer, GameObject *pGO)
{
    ScriptedInstance *m_pInstance = (ScriptedInstance*)pGO->GetInstanceData();

    if(m_pInstance)
    {
       if(Creature* pGortok = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GORTOK)))
         if(pGortok->isAlive())
         {
            pGO->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
            pGO->SetGoState(GO_STATE_ACTIVE);
           ((boss_gortokAI*)pGortok->AI())->StartEvent();
         }
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
    newscript->pGOHello=&GOHello_gortok_generator;
    newscript->RegisterSelf();
}
