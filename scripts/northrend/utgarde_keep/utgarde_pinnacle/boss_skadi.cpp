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
SDName: Boss_Skadi
SD%Complete: 95%
SDComment: Стартует от тригера 4991. Нужно мастштабировать эффекты дыма...
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "WorldPacket.h"
#include "utgarde_pinnacle.h"
#include "Vehicle.h"

enum
{
    SAY_AGGRO                       = -1575019,
    SAY_DRAKEBREATH_1               = -1575020,
    SAY_DRAKEBREATH_2               = -1575021,
    SAY_DRAKEBREATH_3               = -1575022,
    SAY_DRAKE_HARPOON_1             = -1575023,
    SAY_DRAKE_HARPOON_2             = -1575024,
    SAY_KILL_1                      = -1575025,
    SAY_KILL_2                      = -1575026,
    SAY_KILL_3                      = -1575027,
    SAY_DEATH                       = -1575028,
    SAY_DRAKE_DEATH                 = -1575029,
    EMOTE_HARPOON_RANGE             = -1575030,

    SPELL_CRUSH                     = 50234,
    SPELL_CRUSH_H                   = 59330,

    SPELL_WHIRLWIND                 = 50228,
    SPELL_WHIRLWIND_H               = 59322,

    SPELL_POISONED_SPEAR            = 50255,
    SPELL_POISONED_SPEAR_H          = 59331,

    // casted with base of creature 22515 (World Trigger), so we must make sure
    // to use the close one by the door leading further in to instance.
    SPELL_SUMMON_GAUNTLET_MOBS      = 48630,   // tick every 30 sec
    SPELL_SUMMON_GAUNTLET_MOBS_H    = 59275,   // tick every 25 sec

    SPELL_GAUNTLET_PERIODIC         = 47546,   // what is this? Unknown use/effect, but probably related

    //56790?
    SPELL_LAUNCH_HARPOON            = 51368,   //48642 - dont shoot, into grauf strange... // this spell hit drake to reduce HP (force triggered from 48641)
    SPELL_SUMMON_HARPOON            = 56789,   //Summon GameObject When Harpooner dies!
    SPELL_GRAUF_BREATH_L            = 47590,   //Yesssss... Mother Fucker... I find it.... :P
    SPELL_GRAUF_BREATH_R            = 47592,   //Burn Right triggers...
    SPELL_GRAUF_BREATH_L_H          = 47563,
    SPELL_GRAUF_BREATH_R_H          = 47593,
    SPELL_TRIGGER_BREATH_FIRE_N     = 47579,
    SPELL_TRIGGER_BREATH_FIRE_H     = 60020,
    SPELL_BREATH_UNK                = 47594,    //What is it?
};

//477.629f, -484.716f, 104.738f, 4.73f world trigger
//476.803f, -517.157f, 104.723f move summon
//477.391f, -490.158f, 104.736f summon first wave
//481.257f, -507.116f, 104.723f warrior
//478.695f, -511.605f, 104.723f witch
//482.158f, -515.095f, 104.723f warrior
/*######
## boss_skadi
######*/

struct MANGOS_DLL_DECL boss_skadiAI : public ScriptedAI
{
    boss_skadiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsFireAttack;
    bool m_bIsStartEvent;
    bool m_bIsSkadiMove;
    bool m_bIsStop;
    bool m_bIsSecondPhase;
    bool m_bIsGraufFlameEvent;

    uint32 m_uiIntroCount;
    uint32 m_uiIntroTimer;
    uint32 m_uiMoveTimer;
    uint32 m_uiStopTimer;
    uint32 m_uiWpCount;
    uint32 m_uiJumpTimer;

    uint32 uiCrushTimer;
    uint32 uiPoisonedSpearTimer;
    uint32 uiWhirlwindTimer;

    uint8 m_uiFireStuck;
    uint8 uiWing;

    void Reset()
    {
       m_bIsStartEvent = false;
       m_bIsFireAttack = false;
       m_bIsGraufFlameEvent = false;
       m_bIsSkadiMove = false;
       m_bIsStop = false;
       m_bIsSecondPhase = false;
    
       m_uiIntroCount = 0;
       m_uiIntroTimer = 100;
       m_uiMoveTimer = 0;
       m_uiWpCount = 0;
       m_uiStopTimer = 0;
       m_uiFireStuck = 0;
       m_uiJumpTimer = 5000;

       uiWing = urand(1, 2);
       uiCrushTimer = 8000;
       uiPoisonedSpearTimer = 10000;
       uiWhirlwindTimer = 17000;

       if(m_pInstance)
          m_pInstance->SetData(TYPE_SKADI, NOT_STARTED);

       SummonTrigger(false);
       m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

       if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
       {
          if(Creature* Grauf = ((Creature*)vGrauf))
             Grauf->SetActiveObjectState(false);
          m_creature->ExitVehicle();
          m_creature->GetMotionMaster()->MoveTargetedHome();
          vGrauf->RemoveFromWorld();
       }

       if(Creature* pGrauf = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
       {
          pGrauf->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
          pGrauf->SetVisibility(VISIBILITY_ON);
       }
    }

    void SummonFirstWave()
    {
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 477.391f, -490.158f, 104.736f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 481.257f, -507.116f, 104.723f); 
        if(Creature* pSummon = m_creature->SummonCreature(NPC_SHAMAN, 477.391f, -490.158f, 104.736f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 478.695f, -511.605f, 104.723f);
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 477.391f, -490.158f, 104.736f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 482.158f, -515.095f, 104.723f);

        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 447.093f, -510.072f, 104.688f);
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 445.653f, -520.059f, 104.775f);

        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 426.504f, -510.525f, 104.840f);
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 426.603f, -519.086f, 104.882f);

        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 408.139f, -509.427f, 105.070f);
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 407.233f, -517.863f, 104.892f);

        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 385.399f, -507.578f, 104.756f);
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 485.957f, -515.855f, 104.723f);

        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 367.741f, -507.643f, 104.723f);
        if(Creature* pSummon = m_creature->SummonCreature(NPC_WARRIOR, 476.803f, -517.157f, 104.723f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
           pSummon->GetMotionMaster()->MovePoint(0, 367.295f, -517.381f, 104.77523f);
    }

    void JustReachedHome()
    {
        m_creature->SetActiveObjectState(false);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bIsStartEvent)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_SKADI) == SPECIAL)
            {
                m_pInstance->SetData(TYPE_SKADI, IN_PROGRESS);
                m_bIsStartEvent = true;
                m_creature->AddThreat(pWho);
                m_creature->SetInCombatWith(pWho);
                m_creature->SetInCombatWithZone();
            }

            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
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
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_creature->SetActiveObjectState(false);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, DONE);
    }

    void MoveGrauf(float X, float Y, float Z, uint32 Time)
    {
        if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
        {
           m_uiMoveTimer = Time;
           vGrauf->GetMap()->CreatureRelocation(vGrauf, X, Y, Z, vGrauf->GetAngle(X, Y));
           vGrauf->SendMonsterMove(X, Y, Z, SPLINETYPE_NORMAL, vGrauf->GetSplineFlags(), Time);
           m_creature->Relocate(X, Y, Z, m_creature->GetAngle(vGrauf));
        }
    }

    void NextWp()
    {
        m_uiWpCount++;
        if(m_uiWpCount > 15)
           m_uiWpCount = 0;
        switch(m_uiWpCount)
        {
           case 0: 
		 if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
	          MoveGrauf(vGrauf->GetPositionX(), vGrauf->GetPositionY(), 137.451f, 3000); 
             break;
           case 1: MoveGrauf(344.994f, -548.188f, 128.451f, 4000); break;
           case 2: MoveGrauf(365.189f, -549.410f, 112.231f, 3000); break;
           case 3: MoveGrauf(403.306f, -551.190f, 112.231f, 4000); break;
           case 4: MoveGrauf(446.291f, -551.226f, 112.231f, 3000); break;
           case 5: MoveGrauf(484.186f, -553.961f, 114.544f, 3000); break;

           case 6: MoveGrauf(521.438f, -542.188f, 120.972f, 3000); break;
           case 7: MoveGrauf(510.603f, -530.988f, 120.586f, 3000); DoScriptText(EMOTE_HARPOON_RANGE, m_creature); m_bIsStop = true; m_uiStopTimer = 10000; break;
 
           case 8: MoveGrauf(479.678f, -513.855f, 116.717f, 5000); m_bIsFireAttack = true; break;
           case 9: MoveGrauf(467.336f, -513.589f, 116.717f, 2000); break;
           case 10: MoveGrauf(452.770f, -513.343f, 116.717f, 2000); break;
           case 11: MoveGrauf(435.336f, -513.114f, 116.717f, 2000); break;
           case 12: MoveGrauf(414.336f, -512.582f, 116.717f, 2000); break;
           case 13: MoveGrauf(394.769f, -512.589f, 116.717f, 2000); break;
           case 14: MoveGrauf(370.437f, -511.580f, 116.717f, 2000); break;
           case 15: MoveGrauf(343.9314f, -512.052f, 116.717f, 2000); m_bIsFireAttack = false; break;
        }
    }

    void DoSkadiBreathText(uint8 uiText, uint8 uiSpell)
    {
         switch(uiText)
         {
            case 1: DoScriptText(SAY_DRAKEBREATH_1, m_creature); break;
            case 2: DoScriptText(SAY_DRAKEBREATH_2, m_creature); break;
            case 3: DoScriptText(SAY_DRAKEBREATH_3, m_creature); break;
         }
         switch(uiSpell)
         {
            case 1: 
               if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
                  vGrauf->CastSpell(vGrauf, SPELL_GRAUF_BREATH_L, false); 
               break;
            case 2: 
               if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
                  vGrauf->CastSpell(vGrauf, SPELL_GRAUF_BREATH_R, false); 
               break;
         }
    }

    void BurnTrigger(uint8 uiStyle)
    {
        std::list<Creature*> pCreatures;
        GetCreatureListWithEntryInGrid(pCreatures, m_creature, NPC_FLAME_TRIGGER, 20.0f);

        if (pCreatures.empty())
            return;

        if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
        {
           for (std::list<Creature*>::iterator iter = pCreatures.begin(); iter != pCreatures.end(); ++iter)
           {
              float angle = m_creature->GetAngle(m_creature);
              float Tangle = m_creature->GetAngle((*iter));

              //if(Tangle > angle - 45.0f && Tangle < angle + 45.0f)
                 //(*iter)->CastSpell((*iter), m_bIsRegularMode ? SPELL_TRIGGER_BREATH_FIRE_N : SPELL_TRIGGER_BREATH_FIRE_H, false);

                switch(uiStyle)
                {
                   case 1:
                      if((*iter)->GetPositionY() > -512.0f)
                        (*iter)->CastSpell((*iter), m_bIsRegularMode ? SPELL_TRIGGER_BREATH_FIRE_N : SPELL_TRIGGER_BREATH_FIRE_H, false);
                      break;
                   case 2:
                      if((*iter)->GetPositionY() < -512.0f)
                        (*iter)->CastSpell((*iter), m_bIsRegularMode ? SPELL_TRIGGER_BREATH_FIRE_N : SPELL_TRIGGER_BREATH_FIRE_H, false);
                      break;
                }
           }
        }
    }
    
    void SummonTrigger(bool Active)
    {
         if(Active)
         {
            if(Creature* pTrigger = m_creature->SummonCreature(NPC_WORLD_TRIGGER, 477.629f, -484.716f, 104.738f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 27000))
            {
               pTrigger->CastSpell(pTrigger, m_bIsRegularMode ? SPELL_SUMMON_GAUNTLET_MOBS : SPELL_SUMMON_GAUNTLET_MOBS_H, false);
               m_pInstance->SetData64(NPC_WORLD_TRIGGER, pTrigger->GetGUID());
            }
         }
            else
         {
            if(Creature* pTrigger = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_WORLD_TRIGGER)))
               pTrigger->RemoveFromWorld();
         }

    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if(m_bIsStartEvent)
            {
                if (m_uiIntroTimer < uiDiff)
                {
                    m_uiIntroTimer = 1000;

                    switch(m_uiIntroCount)
                    {
                        case 0:
                            DoScriptText(SAY_AGGRO, m_creature);
                            SummonTrigger(true);
                            m_uiIntroTimer = 3000;
                            m_uiIntroCount++;
                            break;
                        case 1:
                            SummonFirstWave();
                            if(Creature* pGrauf = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
                            {
                               if(Vehicle* vGrauf = m_creature->SummonVehicle(NPC_GRAUF, pGrauf->GetPositionX(), pGrauf->GetPositionY(), pGrauf->GetPositionZ(), pGrauf->GetOrientation())) 
                               {
                                 m_creature->SetActiveObjectState(true);
                                 if(Creature* Grauf = ((Creature*)vGrauf))
                                   Grauf->SetActiveObjectState(true);
                                 m_creature->EnterVehicle(vGrauf, 0, true);
                               }
                               pGrauf->SetVisibility(VISIBILITY_OFF);
                            }
                            m_uiIntroTimer = 4500;
                            m_uiIntroCount++;
                            break;
                        case 2:
                            if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
                            {
                               vGrauf->SetSpeedRate(MOVE_WALK , 2.5f);
                               vGrauf->SetSpeedRate(MOVE_RUN , 2.5f);
                               vGrauf->AddSplineFlag(SPLINEFLAG_FLYING);
                               vGrauf->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
                               vGrauf->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                               vGrauf->CastSpell(vGrauf, 59553, true);
                               m_bIsSkadiMove = true;
                               MoveGrauf(vGrauf->GetPositionX(), vGrauf->GetPositionY(), 137.451f, 4000);
                            }
                            m_uiIntroTimer = 6000;
                            m_uiIntroCount++;
                            break;
                    }
                }
                else
                    m_uiIntroTimer -= uiDiff;
            }

            if(m_bIsSkadiMove && !m_bIsStop)
            {
               if(m_uiMoveTimer <= uiDiff)
               {
                  NextWp();
               }else m_uiMoveTimer -= uiDiff;
            }

            if(m_bIsStop)
            {
               if(m_uiStopTimer <= uiDiff)
               {
                  m_bIsStop = false;
                  m_bIsSkadiMove = true;
                  NextWp();
               }else m_uiStopTimer -= uiDiff;
            }

            if(m_bIsSecondPhase)
            {
               if(m_uiJumpTimer <= uiDiff)
               {
                  SummonTrigger(false);
                  m_bIsSecondPhase = false;
                  m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                  m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                  if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                      m_creature->AI()->AttackStart(pTarget);
               }else m_uiJumpTimer -= uiDiff;
            }

            if(m_bIsFireAttack)
            {
               Map *map = m_creature->GetMap();
               if (map->IsDungeon() && !m_bIsGraufFlameEvent)
               {
                  Map::PlayerList const &PlayerList = map->GetPlayers();

                  if (PlayerList.isEmpty())
                     return;

                  for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                  {
                     if(m_creature->GetDistance2d(i->getSource()) < 20.0f)
                     {
                        if(i->getSource()->GetPositionY() > -512.0f)
                           uiWing = 1;
                        else uiWing = 2;
                        m_bIsSkadiMove = false;
                        m_bIsGraufFlameEvent = true;
                        m_uiWpCount++;
                        m_uiMoveTimer = 500;
                        DoSkadiBreathText(urand(1,3), urand(1,2));
                     }
                  }
               }

               if(m_bIsGraufFlameEvent)
               {
                  if(m_uiMoveTimer <= uiDiff)
                  {
                     BurnTrigger(uiWing);
                     if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
                     {
                       float angle = vGrauf->GetAngle(vGrauf);

                       float X = vGrauf->GetPositionX() - 3.0f * cos(angle);
                       float Y = vGrauf->GetPositionY() - 3.0f * sin(angle);

                       MoveGrauf(X, Y, 113.717f, 500);
                     }

                     m_uiFireStuck++;
                     if(m_uiFireStuck > 9)
                     {
                        m_bIsFireAttack = false;
                        m_uiFireStuck = 0;
                        m_bIsSkadiMove = true;
                        m_bIsGraufFlameEvent = false;
                        if(Vehicle* vGrauf = m_creature->GetMap()->GetVehicle(m_creature->GetVehicleGUID()))
                        {
                           vGrauf->RemoveAllAuras();
                        }
                     }
                     m_uiMoveTimer = 500;
                  }else m_uiMoveTimer -= uiDiff;
               }
            }

            return;
        }
          else
        {
            if (uiCrushTimer <= uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CRUSH : SPELL_CRUSH_H);
                uiCrushTimer = 8000;
            } else uiCrushTimer -= uiDiff;

            if (uiPoisonedSpearTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                   DoCast(pTarget, m_bIsRegularMode ? SPELL_POISONED_SPEAR : SPELL_POISONED_SPEAR_H);
                uiPoisonedSpearTimer = 10000;
            } else uiPoisonedSpearTimer -= uiDiff;

            if (uiWhirlwindTimer <= uiDiff)
            {
               if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_WHIRLWIND : SPELL_WHIRLWIND_H);
               uiWhirlwindTimer = 20000;
            } else uiWhirlwindTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

/*######
## boss_grauf
######*/

struct MANGOS_DLL_DECL boss_graufAI : public ScriptedAI
{
    boss_graufAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiSpeechPauseTimer;
    bool m_bIsSpeechPause;

    void Reset()
    {
       uiSpeechPauseTimer = 3000;
       m_bIsSpeechPause = false;
       m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LAUNCH_HARPOON)
        {
            pCaster->DealDamage(m_creature, m_creature->GetMaxHealth() / 6,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            if(Creature* pSkadi = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SKADI)))
            {
               if(!m_bIsSpeechPause && m_creature->isAlive())
               {
                  switch(urand(0, 1))
                  {
                     case 0: DoScriptText(SAY_DRAKE_HARPOON_1, pSkadi); uiSpeechPauseTimer = 8000; m_bIsSpeechPause = true; break;
                     case 1: DoScriptText(SAY_DRAKE_HARPOON_2, pSkadi); uiSpeechPauseTimer = 6000; m_bIsSpeechPause = true; break;
                  }
               }
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->SetActiveObjectState(false);
        if(Creature* pSkadi = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SKADI)))
        {
           DoScriptText(SAY_DRAKE_DEATH, pSkadi);
           pSkadi->AddSplineFlag(SPLINEFLAG_FLYING);
           pSkadi->GetMap()->CreatureRelocation(pSkadi, 481.508f, -513.599f, 104.722f, pSkadi->GetAngle(481.508f, -513.599f));
           pSkadi->SendMonsterMove(481.508f, -513.599f, 104.722f, SPLINETYPE_NORMAL, pSkadi->GetSplineFlags(), 4000);
           ((boss_skadiAI*)pSkadi->AI())->m_bIsSecondPhase = true;
        }

        if(Creature* pGrauf = m_creature->SummonCreature(NPC_GRAUF, m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5000))
        {
          pGrauf->AddSplineFlag(SPLINEFLAG_FLYING);
          pGrauf->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
          pGrauf->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
          pGrauf->SetStandState(UNIT_STAND_STATE_DEAD);
          pGrauf->GetMap()->CreatureRelocation(pGrauf, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, 0);
          pGrauf->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, SPLINETYPE_NORMAL , pGrauf->GetSplineFlags(), 6000);

        }
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bIsSpeechPause)
        {
           if(uiSpeechPauseTimer <= uiDiff)
           {
              m_bIsSpeechPause = false;
           } else uiSpeechPauseTimer -= uiDiff;
        }
              
        return;
    }
};

/*######
## npc_skadi_breath_trigger
######*/

struct MANGOS_DLL_DECL npc_skadi_breath_triggerAI : public ScriptedAI
{
    npc_skadi_breath_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if(pSpell->Id == SPELL_GRAUF_BREATH_L_H)
        {
            DoCast(m_creature, SPELL_TRIGGER_BREATH_FIRE_N);
        }

        if(pSpell->Id == SPELL_GRAUF_BREATH_R_H)
        {
            DoCast(m_creature, SPELL_TRIGGER_BREATH_FIRE_N);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
       pSummoned->SetDisplayId(11686);   
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        damage = 0;
    }

    void AttackStart(Unit* who)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
    }
};

/*######
## npc_skadi_summon
######*/

struct MANGOS_DLL_DECL npc_skadi_summonAI : public ScriptedAI
{
    npc_skadi_summonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
   
    bool m_bIsSummonTriggered;
    
    uint8 uiPoint;

    uint32 m_uiMoveTimer;

    void Reset()
    {
        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        uiPoint = 0;
        m_uiMoveTimer = 0;
        m_bIsSummonTriggered = false;
        if(m_creature->GetPositionY() > -487.0f) 
        {
           uiPoint = 1;
           m_uiMoveTimer = 100;
           m_bIsSummonTriggered = true;
        }
    }

    void JustDied(Unit* pKiller)
    {
        if(m_creature->GetEntry() == NPC_HARPOONER)
           if(Creature* pSummon = m_creature->SummonCreature(NPC_FLAME_TRIGGER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 4.73f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 2000))
           {
              pSummon->SetDisplayId(11686);
              pSummon->CastSpell(pSummon, SPELL_SUMMON_HARPOON, false);
           }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_creature->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->canFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    AttackStart(pWho);
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho, 0.0f);
                }
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        m_creature->GetMotionMaster()->MovementExpired(false);
        m_bIsSummonTriggered = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
           if(m_bIsSummonTriggered)
           {
              if(m_uiMoveTimer <= uiDiff)
              {
                 switch(uiPoint)
                 {
                    case 1:
                      m_creature->GetMotionMaster()->MovementExpired(false);
                      if(Creature* pTrigger = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_WORLD_TRIGGER)))
                         m_creature->GetMotionMaster()->MovePoint(0, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ());
                      m_uiMoveTimer = 1500;
                      uiPoint++;
                      break;
                    case 2:
                      m_creature->GetMotionMaster()->MovementExpired(false);
                      m_creature->GetMotionMaster()->MovePoint(0, 476.803f, -517.157f, 104.723f);
                      m_uiMoveTimer = 3000;
                      uiPoint++;
                      break;
                    case 3:
                      m_creature->SetInCombatWithZone();
                      uiPoint++;
                      break;
                 }
              } else m_uiMoveTimer -= uiDiff;
           }

           return;
        }
           else
        {
           DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI(pCreature);
}

CreatureAI* GetAI_boss_grauf(Creature* pCreature)
{
    return new boss_graufAI(pCreature);
}

CreatureAI* GetAI_npc_skadi_breath_trigger(Creature* pCreature)
{
    return new npc_skadi_breath_triggerAI(pCreature);
}

CreatureAI* GetAI_npc_skadi_summon(Creature* pCreature)
{
    return new npc_skadi_summonAI(pCreature);
}

bool AreaTrigger_at_skadi(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SKADI) == NOT_STARTED)
            pInstance->SetData(TYPE_SKADI, SPECIAL);
    }

    return false;
}

bool GOHello_skadi_harpoon(Player *pPlayer, GameObject *pGo)
{
    if(pPlayer)
       if(Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_HARPOON, 1)) 
       {
           pPlayer->SendNewItem(pItem, 1, true, false);
           pGo->RemoveFromWorld();
       }

    return true;
}

bool GOHello_skadi_harpoon_launcher(Player *pPlayer, GameObject *pGo)
{
    if(Creature* WorldTrigger = GetClosestCreatureWithEntry(pGo, NPC_HARPOON_TRIGGER, 10.0f))
    {
       ScriptedInstance *m_pInstance = (ScriptedInstance*)pGo->GetInstanceData();

       if(m_pInstance)
       {
          if(Creature* pSkadi = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SKADI)))
            if(pSkadi->isAlive())
               if(Vehicle* pGrauf = pGo->GetMap()->GetVehicle(pSkadi->GetVehicleGUID()))
                  if(pGrauf->isAlive() && WorldTrigger->GetDistance2d(pGrauf) < 32.0f)
                  {
                     WorldTrigger->CastSpell(pGrauf, SPELL_LAUNCH_HARPOON, true);
                  } 
                     else 
                  {
                     if(pPlayer)
                        if(Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_HARPOON, 1)) 
                        {
                           pPlayer->SendNewItem(pItem, 1, true, false);
                        }
                  }
       }
    }

    return true;
}

void AddSC_boss_skadi()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_skadi";
    newscript->GetAI = &GetAI_boss_skadi;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_grauf";
    newscript->GetAI = &GetAI_boss_grauf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skadi_breath_trigger";
    newscript->GetAI = &GetAI_npc_skadi_breath_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skadi_summon";
    newscript->GetAI = &GetAI_npc_skadi_summon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_skadi";
    newscript->pAreaTrigger = &AreaTrigger_at_skadi;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_skadi_harpoon";
    newscript->pGOHello=&GOHello_skadi_harpoon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_skadi_harpoon_launcher";
    newscript->pGOHello=&GOHello_skadi_harpoon_launcher;
    newscript->RegisterSelf();
}