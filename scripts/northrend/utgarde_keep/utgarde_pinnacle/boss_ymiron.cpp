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
SDName: Boss_Ymiron
SD%Complete: 90%
SDComment: Нужны Визуальные эффекты появления призраков и Проверить таймеры (Брал от балды :D). 
SDComment: Evenger Spirit если у них спеллы?
SDAuthor: MaxXx2021 Aka Mioka.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO                   = -1575031,
    SAY_SUMMON_BJORN            = -1575032,
    SAY_SUMMON_HALDOR           = -1575033,
    SAY_SUMMON_RANULF           = -1575034,
    SAY_SUMMON_TORGYN           = -1575035,
    SAY_SLAY_1                  = -1575036,
    SAY_SLAY_2                  = -1575037,
    SAY_SLAY_3                  = -1575038,
    SAY_SLAY_4                  = -1575039,
    SAY_DEATH                   = -1575040,

    SPELL_BANE_N                              = 48294,
    SPELL_BANE_H                              = 59301,
    SPELL_DARK_SLASH                          = 48292,
    SPELL_FETID_ROT_N                         = 48291,
    SPELL_FETID_ROT_H                         = 59300,
    SPELL_SCREAMS_OF_THE_DEAD                 = 51750,
    SPELL_SPIRIT_BURST_N                      = 48529,
    SPELL_SPIRIT_BURST_H                      = 59305,
    SPELL_SPIRIT_STRIKE_N                     = 48423,
    SPELL_SPIRIT_STRIKE_H                     = 59304,
    SPELL_ANCESTORS_VENGEANCE                 = 16939,

    SPELL_SUMMON_AVENGING_SPIRIT01            = 48586,
    SPELL_SUMMON_AVENGING_SPIRIT02            = 48587,
    SPELL_SUMMON_AVENGING_SPIRIT03            = 48588,
    SPELL_SUMMON_AVENGING_SPIRIT04            = 48589,
    SPELL_SUMMON_SPIRIT_FOUNT                 = 48386,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON            = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT            = 48307,
    SPELL_LIGHT_BEAM                          = 50236, //may be not right. Need find right spell. And remove Comment in script!

    SPELL_SPIRIT_FOUNT_N                      = 48380,
    SPELL_SPIRIT_FOUNT_H                      = 59320,

    NPC_BJORN_VISUAL                   = 27304,
    NPC_HALDOR_VISUAL                  = 27310,
    NPC_RANULF_VISUAL                  = 27311,
    NPC_TORGYN_VISUAL                  = 27312,
    NPC_SPIRIT_FOUNT                   = 27339,
    NPC_AVENGING_SPIRIT                = 27386,

    POINT_ID_YMIRON_RANULF             = 1,
    POINT_ID_YMIRON_HALDOR             = 2,
    POINT_ID_YMIRON_BJORN              = 3,
    POINT_ID_YMIRON_TORGYN             = 4
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations YmironPoints[]=
{
    {402.577f, -335.175f, 104.757f},  // RANULF
    {403.175f, -314.411f, 104.755f},  // HALDOR
    {381.753f, -314.570f, 104.757f},  // BJORN
    {381.044f, -335.059f, 104.754f},  // TORGYN
};

/*######
## boss_ymiron
######*/

struct MANGOS_DLL_DECL boss_ymironAI : public ScriptedAI
{
    boss_ymironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Creature* Ghost;

    uint32 m_uiBaneTimer;
    uint32 m_uiDarkSlashTimer;
    uint32 m_uiFetidRotTimer;
    uint32 m_uiAncestorsVengeanceTimer;
    uint32 m_uiStepTimer;

    uint32 m_uiRanulfAbbilityTimer;
    uint32 m_uiHaldorAbbilityTimer;
    uint32 m_uiBjornAbbilityTimer;
    uint32 m_uiTorgynAbbilityTimer;

    uint8 m_uiGhostNo;
    uint8 m_uiStep;

    bool m_bIsFirstGhostActive;
    bool m_bIsSecondGhostActive;
    bool m_bIsThirdGhostActive;
    bool m_bIsFourGhostActive;
    bool m_bIsGhostEvent;

    bool m_bIsRanulfActive;
    bool m_bIsHaldorActive;
    bool m_bIsBjornActive;
    bool m_bIsTorgynActive;

    void Reset()
    {
       m_uiRanulfAbbilityTimer = urand(7000, 10000); 
       m_uiHaldorAbbilityTimer = urand(7000, 10000); 
       m_uiBjornAbbilityTimer = urand(7000, 10000); 
       m_uiTorgynAbbilityTimer = urand(7000, 10000); 
       m_uiBaneTimer = urand(3000,7000);
       m_uiDarkSlashTimer = urand(7000,13000);
       m_uiFetidRotTimer = urand(8000,15000);
       m_uiAncestorsVengeanceTimer = m_bIsRegularMode ? 60000 : 45000;
       m_uiGhostNo = urand(0,1);
       m_uiStepTimer = 400;
       m_uiStep = 1;

       m_bIsFirstGhostActive = false;
       m_bIsSecondGhostActive = false;
       m_bIsThirdGhostActive = false;
       m_bIsFourGhostActive = false;
       m_bIsGhostEvent = false;

       m_bIsRanulfActive = false;
       m_bIsHaldorActive = false;
       m_bIsBjornActive = false;
       m_bIsTorgynActive = false;

       Ghost = NULL;
       ResetGhost();
    }

    void ResetGhost()
    {
       if(!m_pInstance) return;

       if(Creature* pBjorn = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BJORN)))
       {
          pBjorn->DeleteThreatList(); 
          pBjorn->CombatStop(true);
          pBjorn->SetDisplayId(11686);
          pBjorn->RemoveAllAuras();
       }

       if(Creature* pHaldor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_HALDOR)))
       {
          pHaldor->DeleteThreatList();
          pHaldor->CombatStop(true);
          pHaldor->SetDisplayId(11686);
          pHaldor->RemoveAllAuras();
       }

       if(Creature* pTorgyn = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_TORGYN)))
       {
          pTorgyn->DeleteThreatList();
          pTorgyn->CombatStop(true);
          pTorgyn->SetDisplayId(11686);
          pTorgyn->RemoveAllAuras();
       }

       if(Creature* pRanulf = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RANULF)))
       {
          pRanulf->DeleteThreatList();
          pRanulf->CombatStop(true);
          pRanulf->SetDisplayId(11686);
          pRanulf->RemoveAllAuras();
       }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        ResetGhost();
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_DARK_SLASH)
          if(target->GetTypeId() == TYPEID_PLAYER)
            m_creature->DealDamage(target, target->GetHealth() / 2,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void AttackStart(Unit* pWho)
    {
        if(m_bIsGhostEvent)
           return;
 
         ScriptedAI::AttackStart(pWho);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if(!m_pInstance) return;

        if (uiPointId == POINT_ID_YMIRON_RANULF)
        {
            if(Creature* pRanulf = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RANULF)))
            {
               m_creature->SetFacingToObject(pRanulf);
               pRanulf->SetDisplayId(28087);
               //pRanulf->CastSpell(pRanulf, SPELL_LIGHT_BEAM, true);
               DoCast(m_creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT, true);
               DoScriptText(SAY_SUMMON_RANULF, m_creature);
               m_bIsRanulfActive = true;
               Ghost = pRanulf;
            }
            m_uiStepTimer = 200;
            m_uiStep++;
            return;
        }

        if (uiPointId == POINT_ID_YMIRON_HALDOR)
        {
            if(Creature* pHaldor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_HALDOR)))
            {
               m_creature->SetFacingToObject(pHaldor);
               pHaldor->SetDisplayId(28086);
               //pHaldor->CastSpell(pHaldor, SPELL_LIGHT_BEAM, true);
               DoCast(m_creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT, true);
               DoScriptText(SAY_SUMMON_HALDOR, m_creature);
               m_bIsHaldorActive = true;
               Ghost = pHaldor;
            }
            m_uiStepTimer = 200;
            m_uiStep++;
            return;
        }

        if (uiPointId == POINT_ID_YMIRON_BJORN)
        {
            if(Creature* pBjorn = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BJORN)))
            {
               m_creature->SetFacingToObject(pBjorn);
               pBjorn->SetDisplayId(28085);
               //pBjorn->CastSpell(pBjorn, SPELL_LIGHT_BEAM, true);
               DoCast(m_creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT, true);
               DoScriptText(SAY_SUMMON_BJORN, m_creature);
               m_bIsBjornActive = true;
               Ghost = pBjorn;
            }
            m_uiStepTimer = 200;
            m_uiStep++;
            return;
        }

        if (uiPointId == POINT_ID_YMIRON_TORGYN)
        {
            if(Creature* pTorgyn = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_TORGYN)))
            {
               m_creature->SetFacingToObject(pTorgyn);
               pTorgyn->SetDisplayId(28088);
               //pTorgyn->CastSpell(pTorgyn, SPELL_LIGHT_BEAM, true);
               DoCast(m_creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT, true);
               DoScriptText(SAY_SUMMON_TORGYN, m_creature);
               m_bIsTorgynActive = true;
               Ghost = pTorgyn;
            }
            m_uiStepTimer = 200;
            m_uiStep++;
            return;
        }
    }

    void StartMove()
    {
        switch(m_uiGhostNo)
        {
           case 0:
              m_creature->GetMotionMaster()->MovePoint(POINT_ID_YMIRON_RANULF, YmironPoints[m_uiGhostNo].x, YmironPoints[m_uiGhostNo].y, YmironPoints[m_uiGhostNo].z);
              break;
           case 1:
              m_creature->GetMotionMaster()->MovePoint(POINT_ID_YMIRON_HALDOR, YmironPoints[m_uiGhostNo].x, YmironPoints[m_uiGhostNo].y, YmironPoints[m_uiGhostNo].z);
              break;
           case 2:
              m_creature->GetMotionMaster()->MovePoint(POINT_ID_YMIRON_BJORN, YmironPoints[m_uiGhostNo].x, YmironPoints[m_uiGhostNo].y, YmironPoints[m_uiGhostNo].z);
              break;
           case 3:
              m_creature->GetMotionMaster()->MovePoint(POINT_ID_YMIRON_TORGYN, YmironPoints[m_uiGhostNo].x, YmironPoints[m_uiGhostNo].y, YmironPoints[m_uiGhostNo].z);
              break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_SPIRIT_SUMMONER)
        {
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
          if(!m_bIsGhostEvent) return;

          if(m_uiStepTimer <= uiDiff)
          {
             switch(m_uiStep)
             {
                case 1:
                   StartMove();
                   m_uiStep++;
                   break;
                case 3:
                   if(Ghost)
                      Ghost->CastSpell(Ghost, 50142, true);
                   m_uiStepTimer = 2800;
                   m_uiStep++;
                   break;
                case 4:
                   if(Ghost)
                      Ghost->CastSpell(Ghost, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                   m_uiStepTimer = 3000;
                   m_uiStep++;
                   break;
                case 5:
                   m_bIsGhostEvent = false;
                   if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                      m_creature->GetMotionMaster()->MoveChase(pTarget);
                   m_uiStep++;
                   break;      
             }
          } else m_uiStepTimer -= uiDiff;

          return;
        }
          else
        {
          if (m_uiBaneTimer <= uiDiff)
          {
              DoCast(m_creature, m_bIsRegularMode ? SPELL_BANE_N : SPELL_BANE_H);
              m_uiBaneTimer = urand(20000,25000);
          } else m_uiBaneTimer -= uiDiff;

          if (m_uiDarkSlashTimer <= uiDiff)
          {
              DoCast(m_creature->getVictim(), SPELL_DARK_SLASH);
              m_uiDarkSlashTimer = m_bIsRegularMode ? urand(30000,35000) : urand(20000,25000);
          } else m_uiDarkSlashTimer -= uiDiff;

          if (m_uiFetidRotTimer <= uiDiff)
          {
              DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FETID_ROT_N : SPELL_FETID_ROT_H);
              m_uiFetidRotTimer = urand(10000,15000);
          } else m_uiFetidRotTimer -= uiDiff;

          if (m_uiAncestorsVengeanceTimer <= uiDiff)
          {
              DoCast(m_creature, SPELL_ANCESTORS_VENGEANCE);
              m_uiAncestorsVengeanceTimer = m_bIsRegularMode ? urand(60000,65000) : urand(45000,50000);
          } else m_uiAncestorsVengeanceTimer -= uiDiff;

          DoMeleeAttackIfReady();

          if(!m_pInstance) return;

          if(m_bIsRanulfActive)
          {
             if(m_uiRanulfAbbilityTimer <= uiDiff)
             {
                if(Creature* pRanulf = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_RANULF)))
                   pRanulf->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_BURST_N : SPELL_SPIRIT_BURST_H, true);
                m_uiRanulfAbbilityTimer = 10000;
             } else m_uiRanulfAbbilityTimer -= uiDiff;
          }

          if(m_bIsHaldorActive)
          {
             if(m_uiHaldorAbbilityTimer <= uiDiff)
             {
                if(Creature* pHaldor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_HALDOR)))
                   pHaldor->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_STRIKE_N : SPELL_SPIRIT_STRIKE_H, true);
                m_uiHaldorAbbilityTimer = urand(6000, 10000);
             } else m_uiHaldorAbbilityTimer -= uiDiff;
          }

          if(m_bIsBjornActive)
          {
             if(m_uiBjornAbbilityTimer <= uiDiff)
             {
                m_bIsBjornActive = false;
                if(Creature* pBjorn = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_BJORN)))
                   if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                     pBjorn->CastSpell(pTarget, SPELL_SUMMON_SPIRIT_FOUNT, true); 
             } else m_uiBjornAbbilityTimer -= uiDiff;
          }

          if(m_bIsTorgynActive)
          {
             if(m_uiTorgynAbbilityTimer <= uiDiff)
             {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_SUMMON_AVENGING_SPIRIT01, true);
                m_creature->CastSpell(m_creature->getVictim(), SPELL_SUMMON_AVENGING_SPIRIT02, true);
                m_creature->CastSpell(m_creature->getVictim(), SPELL_SUMMON_AVENGING_SPIRIT03, true);
                m_creature->CastSpell(m_creature->getVictim(), SPELL_SUMMON_AVENGING_SPIRIT04, true);
                m_uiTorgynAbbilityTimer = m_bIsRegularMode ? urand(12000, 17000) : urand(10000, 13000);
             } else m_uiTorgynAbbilityTimer -= uiDiff;
          }

          if(Creature* WorldTrigger = GetClosestCreatureWithEntry(m_creature, NPC_YMIRON_TRIGGER, 70.0f))
            if(m_creature->GetDistance2d(WorldTrigger) > 39.0f)
               m_creature->AI()->EnterEvadeMode();

          if(m_creature->GetHealthPercent() < 80.0f && !m_bIsFirstGhostActive)
          {
                m_bIsFirstGhostActive = true;
                YmironSettings();
          }

          if(m_creature->GetHealthPercent() < 60.0f && !m_bIsSecondGhostActive)
          {
                m_bIsSecondGhostActive = true;
                YmironSettings();
          }

          if(m_creature->GetHealthPercent() < 40.0f && !m_bIsThirdGhostActive)
          {
                m_bIsThirdGhostActive = true;
                YmironSettings();
          }

          if(m_creature->GetHealthPercent() < 20.0f && !m_bIsFourGhostActive)
          {
                m_bIsFourGhostActive = true;
                YmironSettings();
          }
        }
    }
 
    void YmironSettings()
    {
         m_uiGhostNo++;
         if(m_uiGhostNo > 3)
            m_uiGhostNo = 0;
         m_uiStepTimer = 200;
         m_uiStep = 1;
         m_bIsGhostEvent = true;
         m_creature->InterruptNonMeleeSpells(true);
         DoCast(m_creature, SPELL_SCREAMS_OF_THE_DEAD, true);
         m_creature->GetMotionMaster()->Clear(false);
         m_creature->GetMotionMaster()->MoveIdle();
         m_creature->StopMoving();
         m_creature->AttackStop();
         m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
    }
};

struct MANGOS_DLL_DECL npc_bjorn_sphereAI : public ScriptedAI
{
    npc_bjorn_sphereAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	
    void Reset()
    {
      if(!m_pInstance) return;

      if(Creature* pYmiron = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_YMIRON)))
      {
         m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
         m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
         m_creature->setFaction(pYmiron->getFaction());
         m_creature->SetDisplayId(11686);
         m_creature->SetLevel(80);
         m_creature->SetSpeedRate(MOVE_RUN, 0.4f);
         m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_FOUNT_N : SPELL_SPIRIT_FOUNT_H, false);
         if(Unit* pTarget = pYmiron->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            m_creature->GetMotionMaster()->MoveChase(pTarget);
      }
    }
  
    void UpdateAI(const uint32 uiDiff)
    {
         if(!m_creature->HasAura(m_bIsRegularMode ? SPELL_SPIRIT_FOUNT_N : SPELL_SPIRIT_FOUNT_H))
            m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_FOUNT_N : SPELL_SPIRIT_FOUNT_H, false);

         if(!m_creature->SelectHostileTarget() || !m_creature->getVictim()) 
            return;
    }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI(pCreature);
}

CreatureAI* GetAI_npc_bjorn_sphere(Creature* pCreature)
{
    return new npc_bjorn_sphereAI(pCreature);
}

void AddSC_boss_ymiron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ymiron";
    newscript->GetAI = &GetAI_boss_ymiron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bjorn_sphere";
    newscript->GetAI = &GetAI_npc_bjorn_sphere;
    newscript->RegisterSelf();
}
