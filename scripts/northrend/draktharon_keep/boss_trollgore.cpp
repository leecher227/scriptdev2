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
    SPELL_INFECTED_WOUND            = 49637,
    SPELL_CORPSE_EXPLODE_N          = 49555,                // implemented in core in spell Consume script effect
    SPELL_CORPSE_EXPLODE_H          = 59807,                //
    SPELL_CONSUME_N                 = 49380,
    SPELL_CONSUME_H                 = 59803,
    SPELL_CONSUME_BUFF_N            = 49381,
    SPELL_CONSUME_BUFF_H            = 59805,

    SPELL_INVADER_TAUNT             = 49405,

    SPELL_INVADER_A                 = 49456, 
    SPELL_INVADER_B                 = 49457,
    SPELL_INVADER_C                 = 49458,

    NPC_FLY_BAT                     = 27724,                //vehicle :p
    NPC_INVADER                     = 27709,
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

    void Reset()
    {
        m_uiConsumeTimer = urand(10000, 15000);
        m_uiCrushTimer = urand(7000, 10000);
        m_uiInfectedWoundTimer = urand(5000, 8000);
        m_uiWaveTimer = 2000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TROLLGORE, FAIL);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
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

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TROLLGORE, DONE);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) 
    {
        if (pDoneBy->GetTypeId() != TYPEID_PLAYER)
            uiDamage = uiDamage / 3;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCrushTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CRUSH);
            m_uiCrushTimer = urand(5000, 10000);
        }
        else
            m_uiCrushTimer -= uiDiff;

        if (m_uiInfectedWoundTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_INFECTED_WOUND);
            m_uiInfectedWoundTimer = urand(10000, 11000);
        }
        else
            m_uiInfectedWoundTimer -= uiDiff;

        if (m_uiWaveTimer < uiDiff)
        {
            for (uint8 i=0; i<3; ++i)
                m_creature->SummonCreature(NPC_INVADER, InvaderSummon[i].x, InvaderSummon[i].y, InvaderSummon[i].z, InvaderSummon[i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
            m_uiWaveTimer = 30000;
        }
        else
            m_uiWaveTimer -= uiDiff;

        if (m_uiConsumeTimer < uiDiff)
        {
            DoScriptText(SAY_CONSUME, m_creature);
            m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CONSUME_N : SPELL_CONSUME_H, true);
            m_uiConsumeTimer = urand(10000, 15000);
        }
        else
            m_uiConsumeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_drakkari_invader
######*/

struct MANGOS_DLL_DECL npc_drakkari_invaderAI : public ScriptedAI
{
    npc_drakkari_invaderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiMoveTimer;
    uint64 m_uiVehicleGUID;
    bool m_bIsMovePhase;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bIsMovePhase = false;
        m_uiMoveTimer = 1000;
        m_uiVehicleGUID = 0;
        VehicleInit();
    }

    void VehicleInit()
    {
        if (Creature* pTemp = m_creature->SummonCreature(NPC_FLY_BAT, m_creature->GetPositionX(),  m_creature->GetPositionY(),  m_creature->GetPositionZ(),  m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20000))
        {
            pTemp->AddSplineFlag(SPLINEFLAG_FLYING);
            pTemp->SetSpeedRate(MOVE_WALK, 3.0f, true);
            m_uiVehicleGUID = pTemp->GetGUID();
            MoveBat(pTemp);  
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        
        if (pSpell->Id == (m_bIsRegularMode ? SPELL_CORPSE_EXPLODE_N : SPELL_CORPSE_EXPLODE_H))
        {
            if (m_pInstance)
            {
                if (Creature* pTrollGore = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_TROLLGORE)))
                    DoScriptText(SAY_EXPLODE, pTrollGore);
            }
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId)
    {
        if (uiType == POINT_MOTION_TYPE && uiPointId == 1)
        {
            m_creature->ExitVehicle();
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            if (m_pInstance)
            {
                if (Creature* pTrollgore = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_TROLLGORE)))
                    ScriptedAI::AttackStart(pTrollgore);
            }
            m_bIsMovePhase = true;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_creature->GetHealth() < uiDamage)
        {
            uiDamage = 0;
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE))
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
                m_creature->getHostileRefManager().clearReferences();
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
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_TROLLGORE) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (m_bIsMovePhase)
        {
            if (m_uiMoveTimer < uiDiff)
            {
                if (Creature* pSummoned = m_creature->GetMap()->GetCreature(m_uiVehicleGUID))
                    pSummoned->GetMotionMaster()->MovePoint(0, -192.944f, -657.083f, 62.696f);
                m_bIsMovePhase = false;
            }
            else
                m_uiMoveTimer -= uiDiff;
        }

        if (!m_creature->GetVehicle() && m_creature->GetPositionZ() > 40.0f && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_uiVehicleGUID))
            {
                if (pTemp->CreateVehicleKit(40))
                {
                    if (VehicleKit* pKit = pTemp->GetVehicleKit())
                        m_creature->EnterVehicle(pKit, 0);
                }
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_trollgore(Creature* pCreature)
{
    return new boss_trollgoreAI(pCreature);
}

CreatureAI* GetAI_npc_drakkari_invader(Creature* pCreature)
{
    return new npc_drakkari_invaderAI(pCreature);
}

void AddSC_boss_trollgore()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_trollgore";
    pNewScript->GetAI = &GetAI_boss_trollgore;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_drakkari_invader";
    pNewScript->GetAI = &GetAI_npc_drakkari_invader;
    pNewScript->RegisterSelf();
}
