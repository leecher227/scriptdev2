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
SDName: boss_kologarn
SD%Complete:
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells
    SAY_AGGRO           = -1603150,
    SAY_SHOCKWEAVE      = -1603151,
    SAY_GRAB            = -1603152,
    SAY_LEFT_ARM_LOST   = -1603153,
    SAY_RIGHT_ARM_LOST  = -1603154,
    SAY_SLAY1           = -1603155,
    SAY_SLAY2           = -1603156,
    SAY_BERSERK         = -1603157,
    SAY_DEATH           = -1603158,
    EMOTE_RIGHT_ARM     = -1603355,
    EMOTE_LEFT_ARM      = -1603356,
    EMOTE_STONE_GRIP    = -1603357,

    //Kologarn
    SPELL_OVERHEAD_SMASH            = 63356,
    SPELL_OVERHEAD_SMASH_H          = 64003,
    SPELL_ONE_ARMED_SMASH           = 63573,
    SPELL_ONE_ARMED_SMASH_H         = 64006,
    SPELL_STONE_SHOUT               = 63716,
    SPELL_STONE_SHOUT_H             = 64005,
    SPELL_PETRIFYING_BREATH         = 62030,
    SPELL_PETRIFYING_BREATH_H       = 63980,
    SPELL_FOCUSED_EYEBEAM_MOB       = 63676,
    SPELL_FOCUSED_EYEBEAM_RIGHT     = 63702,
    SPELL_FOCUSED_EYEBEAM_KOLOGARN  = 63348,
    SPELL_FOCUSED_EYEBEAM_DMG       = 63347,
    SPELL_FOCUSED_EYEBEAM_DMG_H     = 63977,
    SPELL_FOCUSED_EYEBEAM           = 63346,
    SPELL_FOCUSED_EYEBEAM_H         = 63976,

    SPELL_ENRAGE                    = 26662,
    //Left Arm
    SPELL_ARM_SWEEP                 = 63766,
    SPELL_ARM_SWEEP_H               = 63983,
    //Right Arm
    SPELL_STONE_GRIP                = 64290,
    SPELL_STONE_GRIP_H              = 64292,
    //both arms
    SPELL_ARM_VISUAL                = 64753,
    //Rubble
    SPELL_RUMBLE                    = 63818,
    SPELL_STONE_NOVA                = 63978,
    //NPC ids
    MOB_RUBBLE                      = 33768,
    NPC_ARM_SWEEP_TARGET            = 33661,
    NPC_FOCUSED_EYEBEAM             = 33632,
    NPC_FOCUSED_EYEBEAM_RIGHT       = 33802,

    POS_X                           = 1784,
    POS_LEFT_Y                      = -45,
    POS_RIGHT_Y                     = -4,
    POS_Z                           = 449,

    MODEL_ID_INVISIBLE              = 11686,

    ACHIEV_RUBBLE_AND_ROLL          = 2959,
    ACHIEV_RUBBLE_AND_ROLL_H        = 2960,
    ACHIEV_WITH_OPEN_ARMS           = 2951,
    ACHIEV_WITH_OPEN_ARMS_H         = 2952,
    ACHIEV_DISARMED                 = 2953,
    ACHIEV_DISARMED_H               = 2954,
    ACHIEV_IF_LOOKS_COULD_KILL      = 2955,
    ACHIEV_IF_LOOKS_COULD_KILL_H    = 2956,
};

#define DEAD_X  1797.15f
#define DEAD_Y  -24.4f
#define DEAD_Z  448.275f

// Rubble
struct MANGOS_DLL_DECL mob_ulduar_rubbleAI : public ScriptedAI
{
    mob_ulduar_rubbleAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiStone_Nova_Timer;

    void Reset()
    {
        m_uiStone_Nova_Timer = urand(8000, 12000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_KOLOGARN) != IN_PROGRESS)
        {
            m_creature->ForcedDespawn();
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStone_Nova_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_RUMBLE : SPELL_STONE_NOVA);
            m_uiStone_Nova_Timer = urand(7000, 9000);
        }
        else
            m_uiStone_Nova_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ulduar_rubble(Creature* pCreature)
{
    return new mob_ulduar_rubbleAI(pCreature);
}

// Left Arm
struct MANGOS_DLL_DECL boss_left_armAI : public ScriptedAI
{
    boss_left_armAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArmSweep_Timer;

    void Reset()
    {
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->ExitVehicle();
        m_uiArmSweep_Timer = 10000;
    }

    void JustRespawned()
    {
        DoCast(m_creature, SPELL_ARM_VISUAL);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            if (Creature* pRightArm = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIGHT_ARM)))
            {
                if (pRightArm->isAlive())
                    pRightArm->SetInCombatWithZone();
            }
            if (Creature* pKologarn = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KOLOGARN)))
            {
                if (pKologarn->isAlive())
                {
                    m_creature->EnterVehicle(pKologarn->GetVehicleKit(), 0);
                    pKologarn->SetInCombatWithZone();
                }
            }
        }
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KOLOGARN)))
        {
            DoScriptText(SAY_LEFT_ARM_LOST, pTemp);
            if (pTemp->isAlive())
                pTemp->DealDamage(pTemp, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArmSweep_Timer < diff)
        {
            if (m_pInstance)
            {
                if (Creature* pKologarn = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KOLOGARN)))
                    DoScriptText(SAY_SHOCKWEAVE, pKologarn);
            }

            if (Creature* pArmSweepTarget = m_creature->SummonCreature(NPC_ARM_SWEEP_TARGET, m_creature->getVictim()->GetPositionX(), m_creature->getVictim()->GetPositionY(), m_creature->getVictim()->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 500))
            {
                pArmSweepTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pArmSweepTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ARM_SWEEP : SPELL_ARM_SWEEP_H);
            }
            m_uiArmSweep_Timer = 10000;
        }
        else
            m_uiArmSweep_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_left_arm(Creature* pCreature)
{
    return new boss_left_armAI(pCreature);
}

// Right Arm
struct MANGOS_DLL_DECL boss_right_armAI : public ScriptedAI
{
    boss_right_armAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiStone_Grip_Timer;
    uint32 m_uiFreeDamage;
    uint32 m_uiMaxDamage;
    uint64 m_uiGripTargetGUID[3];
    uint8 m_uiMaxTargets;
    bool m_bStoneGrip;

    void Reset()
    {
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->ExitVehicle();
        m_uiStone_Grip_Timer = 16000;
        m_uiMaxTargets = m_bIsRegularMode ? 1 : 3;
        for (uint8 i = 0; i < m_uiMaxTargets; ++i)
            m_uiGripTargetGUID[i] = 0;
        m_uiFreeDamage = 0;
        m_bStoneGrip = false;
        m_uiMaxDamage = m_bIsRegularMode ? 100000 : 480000;
    }

    void JustRespawned()
    {
        DoCast(m_creature, SPELL_ARM_VISUAL);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            if (Creature* pLeftArm = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEFT_ARM)))
            {
                if (pLeftArm->isAlive())
                    pLeftArm->SetInCombatWithZone();
            }
            if (Creature* pKologarn = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KOLOGARN)))
            {
                if (pKologarn->isAlive())
                {
                    m_creature->EnterVehicle(pKologarn->GetVehicleKit(), 1);
                    pKologarn->SetInCombatWithZone();
                }
            }
        }
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->SetInCombatWithZone();
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (m_bStoneGrip)
            m_uiFreeDamage += uiDamage;
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_creature->GetMap()->GetCreature( m_pInstance->GetData64(NPC_KOLOGARN)))
        {
            DoScriptText(SAY_RIGHT_ARM_LOST, pTemp);
            if (pTemp->isAlive())
                pTemp->DealDamage(pTemp, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        for (uint8 i = 0; i < m_uiMaxTargets; ++i)
        {
            if (Unit* pVictim = m_creature->GetMap()->GetUnit(m_uiGripTargetGUID[i]))
            {
                pVictim->RemoveAurasDueToSpell(SPELL_STONE_GRIP);
                pVictim->RemoveAurasDueToSpell(SPELL_STONE_GRIP_H);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bStoneGrip)
        {
            if (m_uiFreeDamage > m_uiMaxDamage)
            {
                for (uint8 i = 0; i < m_uiMaxTargets; ++i)
                {
                    if (Unit* pVictim = m_creature->GetMap()->GetUnit(m_uiGripTargetGUID[i]))
                    {
                        pVictim->RemoveAurasDueToSpell(SPELL_STONE_GRIP);
                        pVictim->RemoveAurasDueToSpell(SPELL_STONE_GRIP_H);
                        pVictim->ExitVehicle();
                        pVictim->NearTeleportTo(POS_X, POS_RIGHT_Y, POS_Z, 0);
                    }
                    m_uiGripTargetGUID[i] = 0;
                }
                m_bStoneGrip = false;
            }
        }

        if (m_uiStone_Grip_Timer < diff)
        {
            if (Creature* pKologarn = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KOLOGARN)))
            {
                DoScriptText(SAY_GRAB, pKologarn);
                DoScriptText(EMOTE_STONE_GRIP, pKologarn);

                for (uint8 i = 0; i < m_uiMaxTargets; ++i)
                {
                    Unit* pTarget = NULL;
                    for (uint8 j = 0; j < 25; ++j)
                    {
                        pTarget = pKologarn->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
                        if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && !pTarget->GetVehicle())
                            break;
                    }
                    if (pTarget)
                    {
                        pTarget->EnterVehicle(m_creature->GetVehicleKit(), i);
                        m_uiGripTargetGUID[i] = pTarget->GetGUID();
                        pTarget->CastSpell(pTarget, m_bIsRegularMode ? SPELL_STONE_GRIP : SPELL_STONE_GRIP_H, true);
                    }
                }
            }
            m_uiFreeDamage = 0;
            m_bStoneGrip = true;
            m_uiStone_Grip_Timer = 16000;
        }
        else
            m_uiStone_Grip_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_right_arm(Creature* pCreature)
{
    return new boss_right_armAI(pCreature);
}

// Kologarn
struct MANGOS_DLL_DECL boss_kologarnAI : public ScriptedAI
{
    boss_kologarnAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSpell_Timer;
    uint32 m_uiCheck_Timer;
    uint32 m_uiEyebeam_Timer;
    uint32 m_uiRespawnRightTimer;
    uint32 m_uiRespawnLeftTimer;
    uint32 m_uiEnrageTimer;

    bool m_bIsRightDead;
    bool m_bIsLeftDead;

    uint32 m_uiRubbleNo;
    uint32 m_uiDisarmedTimer;
    bool m_bOpenArms;
    bool m_bIfLooksCouldKill;

    void Reset()
    {
        m_creature->SetStandState(UNIT_STAND_STATE_SUBMERGED);
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_KOLOGARN) == DONE)
            {
                m_creature->SetDeathState(JUST_DIED);
                m_creature->GetMap()->CreatureRelocation(m_creature, DEAD_X, DEAD_Y, DEAD_Z, M_PI);
                m_creature->SendMonsterMove(DEAD_X, DEAD_Y, DEAD_Z, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 0);
                m_creature->SetCorpseDelay(WEEK);
                return;
            }
            else
            {
                if (Creature* pLeftArm = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEFT_ARM)))
                {
                    if (pLeftArm->isDead())
                        pLeftArm->Respawn();
                }
                if (Creature* pRightArm = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIGHT_ARM)))
                {
                    if (pRightArm->isDead())
                        pRightArm->Respawn();
                }
            }
        }

        m_uiSpell_Timer = urand(15000, 20000);
        m_uiCheck_Timer = 5000;
        m_uiEnrageTimer = 600000;
        m_uiEyebeam_Timer   = 10000 + urand(1000, 5000);
        m_uiRespawnLeftTimer = 30000;
        m_uiRespawnRightTimer = 30000;
        m_bIsRightDead = false;
        m_bIsLeftDead = false;

        m_uiRubbleNo = 0;
        m_bOpenArms = true;
        m_uiDisarmedTimer = 0;
        m_bIfLooksCouldKill = true;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KOLOGARN, FAIL);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_KOLOGARN, IN_PROGRESS);

            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEFT_ARM)))
            {
                if (pTemp->isAlive())
                    pTemp->SetInCombatWithZone();
            }
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIGHT_ARM)))
            {
                if (pTemp->isAlive())
                    pTemp->SetInCombatWithZone();
            }
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_SLAY1, m_creature);
        else
            DoScriptText(SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            // hacky way to complete achievements; use only if you have this function
            // Rubble and Roll
            if (m_uiRubbleNo >= 25)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_RUBBLE_AND_ROLL : ACHIEV_RUBBLE_AND_ROLL_H);

            // With Open Arms
            if (m_bOpenArms)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_WITH_OPEN_ARMS : ACHIEV_WITH_OPEN_ARMS_H);

            // Disarmed
            if (m_bIsRightDead && m_bIsLeftDead && m_uiDisarmedTimer <= 12000)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_DISARMED : ACHIEV_DISARMED_H);

            // If Looks Could Kill
            if (m_bIfLooksCouldKill)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_IF_LOOKS_COULD_KILL : ACHIEV_IF_LOOKS_COULD_KILL_H);

            // Death yell
            m_pInstance->SetData(TYPE_KOLOGARN, DONE);

            // Destroy arms
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEFT_ARM)))
            {
                if (pTemp->isAlive())
                    pTemp->DealDamage(pTemp, pTemp->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIGHT_ARM)))
            {
                if (pTemp->isAlive())
                    pTemp->DealDamage(pTemp, pTemp->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        m_creature->SetDeathState(JUST_DIED);
        m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+20.3f, M_PI);
        m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+20.3f, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 0);
        m_creature->SetCorpseDelay(WEEK);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                m_uiSpell_Timer = urand(15000, 20000);
                if (!m_bIsRightDead && !m_bIsLeftDead)
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_OVERHEAD_SMASH : SPELL_OVERHEAD_SMASH_H);
                else if (m_bIsRightDead && m_bIsLeftDead)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_STONE_SHOUT : SPELL_STONE_SHOUT_H);
                    m_uiSpell_Timer = 3000;
                }
                else
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ONE_ARMED_SMASH : SPELL_ONE_ARMED_SMASH_H);
            }
        }
        else
            m_uiSpell_Timer -= uiDiff;   

        if (m_uiEyebeam_Timer < uiDiff)
        {
            Unit* pTarget = NULL;
            for (uint8 i=0; i<25; ++i)
            {
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (pTarget && m_creature->GetDistance2d(pTarget) > ATTACK_DISTANCE)
                    break;
            }
            if (!pTarget)
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget)
            {
                if (Creature* pTemp = m_creature->SummonCreature(NPC_FOCUSED_EYEBEAM, pTarget->GetPositionX(), pTarget->GetPositionY()+4.0f, pTarget->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    pTemp->CastSpell(m_creature, SPELL_FOCUSED_EYEBEAM_MOB, true);
                    pTemp->AddThreat(pTarget, 100000.0f);
                }
                if (Creature* pTemp = m_creature->SummonCreature(NPC_FOCUSED_EYEBEAM_RIGHT, pTarget->GetPositionX(), pTarget->GetPositionY()-4.0f, pTarget->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    pTemp->CastSpell(m_creature, SPELL_FOCUSED_EYEBEAM_RIGHT, true);
                    pTemp->AddThreat(pTarget, 100000.0f);
                }
            }
            m_uiEyebeam_Timer = urand(20000, 25000);
        }
        else
            m_uiEyebeam_Timer -= uiDiff;

        // respawn arms
        if (m_bIsLeftDead)
        {
            if (m_uiRespawnLeftTimer < uiDiff)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEFT_ARM)))
                {
                    pTemp->Respawn();
                    m_bIsLeftDead   = false;
                    DoScriptText(EMOTE_LEFT_ARM, m_creature);
                }
            }
            else
                m_uiRespawnLeftTimer -= uiDiff;
        }

        if (m_bIsRightDead)
        {
            if (m_uiRespawnRightTimer < uiDiff)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIGHT_ARM)))
                {
                    pTemp->Respawn();
                    m_bIsRightDead  = false;
                    DoScriptText(EMOTE_RIGHT_ARM, m_creature);
                }
            }
            else
                m_uiRespawnRightTimer -= uiDiff; 
        }

        // check if arms are dead and if there is no player in melee range
        if (m_uiCheck_Timer < uiDiff)
        {
            if (Creature* pLeftArm = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEFT_ARM)))
            {
                if (pLeftArm->isDead() && !m_bIsLeftDead)
                {
                    m_bOpenArms = false;

                    if (m_bIsRightDead)
                    {
                        m_uiSpell_Timer = 0;
                        m_uiDisarmedTimer = 0;
                    }

                    for (uint8 i = 0; i < 5; i ++)
                    {
                        if (Creature* pRubble = m_creature->SummonCreature(MOB_RUBBLE, POS_X-3+irand(0, 6), POS_LEFT_Y-5+irand(0, 10), POS_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                        {
                            pRubble->SetInCombatWithZone();
                            m_uiRubbleNo += 1;
                        }
                    }
                    m_bIsLeftDead = true;
                    m_uiRespawnLeftTimer = 30000;
                }
            }
            if (Creature* pRightArm = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIGHT_ARM)))
            {
                if (pRightArm->isDead() && !m_bIsRightDead)
                {
                    m_bOpenArms = false;

                    if (m_bIsLeftDead)
                    {
                        m_uiSpell_Timer = 0;
                        m_uiDisarmedTimer = 0;
                    }

                    for (uint8 i = 0; i < 5; i ++)
                    {
                        if (Creature* pRubble = m_creature->SummonCreature(MOB_RUBBLE, POS_X-3+irand(0, 6), POS_RIGHT_Y-5+irand(0, 10), POS_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                        {
                            pRubble->SetInCombatWithZone();
                            m_uiRubbleNo += 1;
                        }
                    }
                    m_bIsRightDead = true;
                    m_uiRespawnRightTimer = 30000;
                }
            }

            //Petrifying breath
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                if (m_creature->GetDistance2d(m_creature->getVictim()) > ATTACK_DISTANCE)
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_PETRIFYING_BREATH : SPELL_PETRIFYING_BREATH_H);
            }

            m_uiCheck_Timer = 1000;
        }
        else
            m_uiCheck_Timer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            DoScriptText(SAY_BERSERK, m_creature);
            m_creature->CastStop();
            DoCast(m_creature, SPELL_ENRAGE);
            m_uiEnrageTimer = 30000;
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (!m_bIsRightDead || !m_bIsLeftDead)
            DoMeleeAttackIfReady();
        else
            m_uiDisarmedTimer += uiDiff;

        m_creature->SetUInt32Value(UNIT_FIELD_TARGET, 0);
    }
};

CreatureAI* GetAI_boss_kologarn(Creature* pCreature)
{
    return new boss_kologarnAI(pCreature);
}

// Focused Eyebeam
struct MANGOS_DLL_DECL mob_focused_eyebeamAI : public ScriptedAI
{
    mob_focused_eyebeamAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiEntry = m_creature->GetEntry();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEntry;

    uint32 m_uiStartMove_Timer;
    uint32 m_uiStartDmg_Timer;

    void Reset()
    {
        SetCombatMovement(false);
        m_creature->SetDisplayId(MODEL_ID_INVISIBLE);
        m_uiStartMove_Timer = 1300;
        m_uiStartDmg_Timer = 2000;
        m_creature->ForcedDespawn(10000);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if ((pSpell->Id == SPELL_FOCUSED_EYEBEAM || pSpell->Id == SPELL_FOCUSED_EYEBEAM_H) && pTarget->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_pInstance)
            {
                if (Creature* pKologarn = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KOLOGARN)))
                    ((boss_kologarnAI*)pKologarn->AI())->m_bIfLooksCouldKill = false;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_KOLOGARN) != IN_PROGRESS)
        {
            m_creature->ForcedDespawn();
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStartMove_Timer < diff)
        {
            float fDy = -4.0f;
            if (m_uiEntry == NPC_FOCUSED_EYEBEAM_RIGHT)
                fDy = 4.0f;
            m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY()+fDy, m_creature->GetPositionZ());
            m_uiStartMove_Timer = 10000;
        }
        else
            m_uiStartMove_Timer -= diff;

        if (m_uiStartDmg_Timer < diff)
        {
            if (m_uiEntry == NPC_FOCUSED_EYEBEAM_RIGHT)
            {
                m_creature->ForcedDespawn();
                return;
            }
            DoCast(m_creature, SPELL_FOCUSED_EYEBEAM_KOLOGARN, true);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_FOCUSED_EYEBEAM_DMG : SPELL_FOCUSED_EYEBEAM_DMG_H, true);
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            m_uiStartDmg_Timer = 10000;
        }
        else
            m_uiStartDmg_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_focused_eyebeam(Creature* pCreature)
{
    return new mob_focused_eyebeamAI(pCreature);
}

void AddSC_boss_kologarn()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_kologarn";
    NewScript->GetAI = GetAI_boss_kologarn;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_ulduar_rubble";
    NewScript->GetAI = &GetAI_mob_ulduar_rubble;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_focused_eyebeam";
    NewScript->GetAI = &GetAI_mob_focused_eyebeam;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_left_arm";
    NewScript->GetAI = &GetAI_boss_left_arm;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_right_arm";
    NewScript->GetAI = &GetAI_boss_right_arm;
    NewScript->RegisterSelf();
}
