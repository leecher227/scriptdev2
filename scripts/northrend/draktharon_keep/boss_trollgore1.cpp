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
SDName: Boss_Trollgore
SD%Complete: 90%
SDComment: Timers
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"
#include "Vehicle.h"

enum
{
    SAY_AGGRO                       = -1600000,
    SAY_CONSUME                     = -1600001,
    SAY_DEATH                       = -1600002,
    SAY_EXPLODE                     = -1600003,
    SAY_KILL                        = -1600004,

    SPELL_CRUSH                     = 49639,
    SPELL_INFECTED_WOUND            = 49367,
    SPELL_CORPSE_EXPLODE_N          = 49555,
    SPELL_CORPSE_EXPLODE_H          = 59087,
    SPELL_CONSUME_N                 = 49380,
    SPELL_CONSUME_H                 = 59803,
    SPELL_CONSUME_BUFF_N            = 49381,
    SPELL_CONSUME_BUFF_H            = 59805,

    SPELL_CORPSE_EXPLODE_PROC_N     = 49618,
    SPELL_CORPSE_EXPLODE_PROC_H     = 59809,
    SPELL_CORPSE_EXPLODE_BONE       = 51270,

    SPELL_INVADER_TAUNT             = 49405,

    SPELL_INVADER_A                 = 49456, 
    SPELL_INVADER_B                 = 49457,
    SPELL_INVADER_C                 = 49458,

    NPC_FLY_BAT                     = 27724, //vehicle :p
    NPC_INVADER_A                   = 27709,
    NPC_INVADER_B                   = 27753,
    NPC_INVADER_C                   = 27754,
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

struct Locations InvaderSummon[]=
{
    {-212.266f, -654.887f, 54.294f, 3.12f}, //
    {-213.251f, -666.072f, 53.357f, 3.01f}, //
    {-213.076f, -679.635f, 53.070f, 2.96f}, //
};

/*######
## boss_trollgore
######*/

struct MANGOS_DLL_DECL boss_trollgoreAI : public ScriptedAI
{
    boss_trollgoreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiConsumeTimer;
    uint32 m_uiCrushTimer;
    uint32 m_uiInfectedWoundTimer;
    uint32 m_uiWaveTimer;
    uint32 m_uiCorpseExplodeTimer;

    void Reset()
    {
        m_uiCorpseExplodeTimer = urand(10000, 15000);
        m_uiConsumeTimer = urand(5000, 8000);
        m_uiCrushTimer = urand(7000, 10000);
        m_uiWaveTimer = 6000;

        if(m_pInstance)
           m_pInstance->SetData(TYPE_TROLLGORE, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if(m_pInstance)
           m_pInstance->SetData(TYPE_TROLLGORE, IN_PROGRESS);
 
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetCharmerOrOwnerPlayerOrPlayerItself())
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(m_pInstance)
           m_pInstance->SetData(TYPE_TROLLGORE, DONE);
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        if(done_by->GetTypeId() != TYPEID_PLAYER)
           damage = damage / 3;
    }

    void SummonWaves()
    {
        m_creature->SummonCreature(NPC_INVADER_A,InvaderSummon[0].x,InvaderSummon[0].y,InvaderSummon[0].z,InvaderSummon[0].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_INVADER_B,InvaderSummon[1].x,InvaderSummon[1].y,InvaderSummon[1].z,InvaderSummon[1].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
        m_creature->SummonCreature(NPC_INVADER_C,InvaderSummon[2].x,InvaderSummon[2].y,InvaderSummon[2].z,InvaderSummon[2].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if(pSpell->Id == (m_bIsRegularMode ? SPELL_CONSUME_N : SPELL_CONSUME_H))
        {
           m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_CONSUME_BUFF_N : SPELL_CONSUME_BUFF_H, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCrushTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CRUSH);
            m_uiCrushTimer = urand(5000, 10000);
        }else m_uiCrushTimer -= uiDiff;

        if (m_uiWaveTimer < uiDiff)
        {
            SummonWaves();
            m_uiWaveTimer = 30000;
        }else m_uiWaveTimer -= uiDiff;

        if (m_uiConsumeTimer < uiDiff)
        {
            DoScriptText(SAY_CONSUME, m_creature);
            m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CONSUME_N : SPELL_CONSUME_H, true);
            m_uiConsumeTimer = urand(10000, 15000);
        }else m_uiConsumeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_drakkari_invaider
######*/

struct MANGOS_DLL_DECL npc_drakkari_invaiderAI : public ScriptedAI
{
    npc_drakkari_invaiderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBoomTimer;
    uint32 m_uiMoveTimer;

    uint64 m_uiVehicleGUID;

    bool m_bIsSpellHit;
    bool m_bIsMovePhase;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiBoomTimer = 7000;
        m_bIsSpellHit = false;
        m_bIsMovePhase = false;
        m_uiMoveTimer = 1000;
        m_uiVehicleGUID = 0;
        VehicleInit();
    }

    void VehicleInit()
    {
       if(Creature* pTemp = m_creature->SummonCreature(NPC_FLY_BAT, m_creature->GetPositionX(),  m_creature->GetPositionY(),  m_creature->GetPositionZ(),  m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000))
       {
          pTemp->AddSplineFlag(SPLINEFLAG_FLYING);
          pTemp->SetSpeedRate(MOVE_WALK, 3.0f, true);
          m_uiVehicleGUID = pTemp->GetGUID();
          MoveBat(pTemp);  
       }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if(pSpell->Id == (m_bIsRegularMode ? SPELL_CORPSE_EXPLODE_N : SPELL_CORPSE_EXPLODE_H))
        {
           if(m_pInstance)
              if(Creature* pTrollGore = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_TROLLGORE)))
                 DoScriptText(SAY_EXPLODE, pTrollGore);
           m_bIsSpellHit = true;
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

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId)
    {
        if (uiType == POINT_MOTION_TYPE && uiPointId == 1)
        {
           m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           if(m_creature->GetVehicle())
              m_creature->ExitVehicle();
           if(m_pInstance)
              if(Creature* pTrollGore = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_TROLLGORE)))
                 ScriptedAI::AttackStart(pTrollGore);
           m_bIsMovePhase = true;
        }
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
       if(m_creature->GetHealth() < uiDamage)
       {
           uiDamage = 0;
           if(!m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE))
           {
              m_creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
              m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
              m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
              m_creature->RemoveAllAuras();
              m_creature->InterruptNonMeleeSpells(false);
              m_creature->SetHealth(1);
              m_creature->GetMotionMaster()->Clear(false);
              m_creature->GetMotionMaster()->MoveIdle();
              m_creature->AttackStop();
              m_creature->StopMoving();
           }
       }
    }

    void MoveBat(Creature* pBat)
    {
       float fDis = 44.0f;
       float fAng = pBat->GetOrientation();
       float X = pBat->GetPositionX()+fDis*cos(fAng);
       float Y = pBat->GetPositionY()+fDis*sin(fAng);
       float Z = 26.534f;
       pBat->GetMotionMaster()->MovePoint(1, X, Y, Z);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_pInstance)
           return;

        if(m_pInstance->GetData(TYPE_TROLLGORE) != IN_PROGRESS)
           m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if(m_bIsSpellHit)
        {
           if(m_uiBoomTimer < uiDiff)
               m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
           else m_uiBoomTimer -= uiDiff;
        }

        if(m_bIsMovePhase)
        {
           if(m_uiMoveTimer < uiDiff)
           {
              if(Creature* pSummoned = m_creature->GetMap()->GetCreature(m_uiVehicleGUID))
                 pSummoned->GetMotionMaster()->MovePoint(0, -192.944f, -657.083f, 62.696f);
              m_bIsMovePhase = false;
           } else m_uiMoveTimer -= uiDiff;
        }

        if(!m_creature->GetVehicle() && m_creature->GetPositionZ() > 40.0f && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        {
           if(Creature* pTemp = m_creature->GetMap()->GetCreature(m_uiVehicleGUID))
           {
              if(pTemp->CreateVehicleKit(40))
                 if(VehicleKit* pKit = pTemp->GetVehicleKit())
                    m_creature->EnterVehicle(pKit, 0);
           }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if(!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
           DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_trollgore(Creature* pCreature)
{
    return new boss_trollgoreAI(pCreature);
}

CreatureAI* GetAI_npc_drakkari_invaider(Creature* pCreature)
{
    return new npc_drakkari_invaiderAI(pCreature);
}

void AddSC_boss_trollgore()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_trollgore";
    newscript->GetAI = &GetAI_boss_trollgore;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_drakkari_invader";
    newscript->GetAI = &GetAI_npc_drakkari_invaider;
    newscript->RegisterSelf();
}
