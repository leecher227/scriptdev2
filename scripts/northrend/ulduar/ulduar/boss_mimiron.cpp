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
SDName: boss_mimiron
SD%Complete: 
SDComment: hack, hack, hack :P But this is cool. Very Offlike. Need Jump Core Support!
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"
#include "Vehicle.h"

enum
{
    //yells
    SAY_AGGRO           = -1603241,
    SAY_HARD_MODE       = -1603242,
    SAY_BERSERK         = -1603243,
    SAY_TANK_ACTIVE     = -1603244,
    SAY_TANK_SLAY1      = -1603245,
    SAY_TANK_SLAY2      = -1603246,
    SAY_TANK_DEATH      = -1603247,
    SAY_TORSO_ACTIVE    = -1603248,
    SAY_TORSO_SLAY1     = -1603249,
    SAY_TORSO_SLAY2     = -1603250,
    SAY_TORSO_DEATH     = -1603251,
    SAY_HEAD_ACTIVE     = -1603252,
    SAY_HEAD_SLAY1      = -1603253,
    SAY_HEAD_SLAY2      = -1603254,
    SAY_HEAD_DEATH      = -1603255,
    SAY_ROBOT_ACTIVE    = -1603256,
    SAY_ROBOT_SLAY1     = -1603257,
    SAY_ROBOT_SLAY2     = -1603258,
    SAY_ROBOT_DEATH     = -1603259,

    //Computer
    SAY_COMPUTER_START_TIMER  = -1606241,
    SAY_COMPUTER_10MIN        = -1606242,
    SAY_COMPUTER_09MIN        = -1606243, 
    SAY_COMPUTER_08MIN        = -1606244,
    SAY_COMPUTER_07MIN        = -1606245,
    SAY_COMPUTER_06MIN        = -1606246,
    SAY_COMPUTER_05MIN        = -1606247,
    SAY_COMPUTER_04MIN        = -1606248,
    SAY_COMPUTER_03MIN        = -1606249,
    SAY_COMPUTER_02MIN        = -1606250,
    SAY_COMPUTER_01MIN        = -1606251,
    SAY_COMPUTER_00MIN        = -1606252,
    SAY_COMPUTER_ENDTIMER     = -1606253,

    EMOTE_PLASMA_BLAST      = -1603371,

    SPELL_JET_PACK          = 63341, // used by mimiron to change seats
    SPELL_SELF_REPAIR       = 64383,
    NPC_COMPUTER            = 34143,

    // hard mode spells
    SPELL_SELF_DESTRUCTION  = 64613,    // visual aura
    SPELL_SELF_DESTRUCT     = 64610,    // damage aura
    SPELL_EMERGENCY_MODE_AURA   = 65101,
    NPC_MIMIRON_INFERNO     = 33370,    // used to cast the self destruct

    SPELL_FLAMES            = 64561,    // may be the fires spells
    SPELL_FLAMES_SUMMON     = 64563,    // 64567
    SPELL_FLAMES_SPREAD     = 64562,
    NPC_FLAME               = 34121,
    NPC_FLAME_INITIAL       = 34363,

    //spells
    //leviathan
    SPELL_PROXIMITY_MINES   = 65347, // also in phase 4
    SPELL_MINE_SUMMON       = 65347,
    MOB_PROXIMITY_MINE      = 34362,
    SPELL_EXPLOSION         = 66351,
    SPELL_EXPLOSION_H       = 63009,
    SPELL_NAPALM_SHELL      = 63666,
    SPELL_NAPALM_SHELL_H    = 65026,
    SPELL_PLASMA_BLAST      = 62997,
    SPELL_PLASMA_BLAST_H    = 64529,
    SPELL_SHOCK_BLAST       = 63631, // also in phase 4
    SPELL_FLAME_SUPRESSANT  = 64570, // hard mode
    LEVIATHAN_TURRET        = 34071,

    //vx001
    SPELL_RAPID_BURST_L_N   = 63387,
    SPELL_RAPID_BURST_L_H   = 64531,
    SPELL_RAPID_BURST_R_N   = 64019,
    SPELL_RAPID_BURST_R_H   = 64532,
    SPELL_LASER_TRIGG       = 63274, //spell script
    SPELL_LASER_PREPEAD     = 63414, //cast before laser damage
    SPELL_ROCKET_DMG        = 64064,
    SPELL_ROCKET_MISSILE    = 63036,
    SPELL_TORSO_DISABLED    = 64120,
    SPELL_HEAT_WAVE         = 63677,
    SPELL_HEAT_WAVE_H       = 64533,
    SPELL_HAND_PULSE_L      = 64348, // only in phase 4
    SPELL_HAND_PULSE_L_H    = 64536,
    SPELL_HAND_PULSE_R      = 64352, // only in phase 4
    SPELL_HAND_PULSE_R_H    = 64537,
    SPELL_FLAME_SUPRESS     = 65192,    // used by robot in melee range
    NPC_MIMIRON_FOCUS       = 33835,    //33369
    NPC_ROCKET_STRIKE       = 34047,


    // frostbomb
    SPELL_FROST_BOMB_EXPL   = 64626,
    SPELL_FROST_BOMB_AURA   = 64624,    // before explode
    SPELL_FROST_BOMB_VISUAL = 64625,    // bomb grows
    SPELL_FROST_BOMB_SUMMON = 64627,    // summon the frostbomb

    //aerial unit
    SPELL_PLASMA_BALL       = 63689, // also in phase 4
    SPELL_PLASMA_BALL_H     = 64535, // also in phase 4
    MOB_ASSALT_BOT          = 34057,
    MOB_BOMB_BOT            = 33836,
    MOB_BOMB_BOT_321        = 33346,
    MOB_BOMB_BOT_500        = 34192,
    MOB_JUNK_BOT            = 33855,
    SPELL_TELEPORT_BOT      = 64446,
    SPELL_MAGNETIC_FIELD    = 64668,
    SPELL_MAGNETIC_CORE     = 64436, // increase dmg taken by 50%, used by magnetic core
    SPELL_MAGNITIC_ITEM     = 64444,
    MOB_MAGNETIC_CORE       = 34068,
    SPELL_MAGNETIC_EFFECT   = 64438,
    ITEM_MAGNETIC_CORE      = 46029,
    SPELL_BOMB_BOT_SUMMON   = 63811,
    SPELL_BOMB_BOT          = 63767,
    SPELL_BOMB_BOT_EXPLODE  = 63801,

    //hard mode
    // summons fires
    SPELL_EMERGENCY_MODE    = 64582,
    MOB_FROST_BOMB          = 34149,
    MOB_EMERGENCY_FIRE_BOT  = 34147,
    SPELL_DEAFENING_SIREN   = 64616,
    SPELL_WATER_SPRAY       = 64619,

    SPELL_MIMIRONS_INFERNO  = 62910,  // maybe used by rocket
    SPELL_MIMIRONS_INFERNO2 = 62909,  // maybe hard mode
    SPELL_BERSERK           = 26662,

    ACHIEV_FIREFIGHTER      = 3180,
    ACHIEV_FIREFIGHTER_H    = 3189,
};

enum MimironPhase
{
    PHASE_IDLE      = 0,
    PHASE_INTRO     = 1,
    PHASE_LEVIATHAN = 2,
    PHASE_TRANS_1   = 3,
    PHASE_VX001     = 4,
    PHASE_TRANS_2   = 5,
    PHASE_AERIAL    = 6,
    PHASE_TRANS_3   = 7,
    PHASE_ROBOT     = 8,
    PHASE_OUTRO     = 9,
};

#define CENTER_X            2744.732f
#define CENTER_Y            2569.479f
#define CENTER_Z            364.312f 

const float PosTankHome[2]= {2794.86f, 2597.83f};
struct LocationsXY
{
    float x, y;
    uint32 id;
};
static LocationsXY SummonLoc[]=
{
    {2753.665f, 2584.712f},
    {2754.150f, 2554.445f},
    {2726.966f, 2569.032f},
    {2759.085f, 2594.249f},
    {2759.977f, 2544.345f},
    {2715.542f, 2569.160f},
    {2765.070f, 2604.337f},
    {2765.676f, 2534.558f},
    {2703.810f, 2569.132f},
};


struct MANGOS_DLL_DECL leviathan_turretAI : public ScriptedAI
{   
    leviathan_turretAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
        pCreature->AddSplineFlag(SPLINEFLAG_FLYING);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPlasmaBlastTimer;
    uint32 m_uiNapalmShellTimer;
    uint32 m_uiCheckTimer;

    bool m_bIsMoved;

    void Reset()
    {
        m_uiPlasmaBlastTimer = 20000;
        m_uiNapalmShellTimer = urand(7000, 15000);
        m_uiCheckTimer = 10;
        m_bIsMoved = false;
    }

    void VehicleHack()
    {
        if (m_pInstance)
        {
            if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
            {
                if (!m_bIsMoved)
                    m_creature->GetMap()->CreatureRelocation(m_creature, pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ()+6.0f, m_creature->GetOrientation());
                m_creature->SendMonsterMove(pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ()+6.0f, SPLINETYPE_NORMAL , SPLINEFLAG_DONE, 0);
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
         m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), CENTER_Z, m_creature->GetOrientation());
         m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), CENTER_Z, SPLINETYPE_NORMAL , m_creature->GetSplineFlags(), 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiCheckTimer < uiDiff)
        {
            VehicleHack();
            m_uiCheckTimer= 10;
        }
        else
            m_uiCheckTimer -= uiDiff;

        if (m_uiPlasmaBlastTimer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                {
                    DoScriptText(EMOTE_PLASMA_BLAST, m_creature);
                    if (Unit* pTarget = pTank->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_PLASMA_BLAST : SPELL_PLASMA_BLAST_H);
                }
            }
            m_uiPlasmaBlastTimer = 30000;
        }
        else
            m_uiPlasmaBlastTimer -= uiDiff;

        if (m_uiNapalmShellTimer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                {
                    if (Unit* pTarget = pTank->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_NAPALM_SHELL : SPELL_NAPALM_SHELL_H);
                        m_uiNapalmShellTimer = urand(7000, 20000);
                    }
                }
            }
        }
        else
            m_uiNapalmShellTimer -= uiDiff;
    }
};

// Leviathan Mk script
struct MANGOS_DLL_DECL boss_leviathan_mkAI : public ScriptedAI
{   
    boss_leviathan_mkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    uint32 m_uiMinesTimer;
    uint32 m_uiShockBlastTimer;
    uint32 m_uiRepairTimer;

    bool m_bHasSuppresed;
    uint32 m_uiSupressTimer;
    uint32 m_uiSetFireTimer;


    void Reset()
    {
        m_bHasSuppresed         = false;
        m_uiSetFireTimer        = 10000;
        m_uiSupressTimer        = 10000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_uiMinesTimer          = 5000;
        m_uiShockBlastTimer     = 30000;
        m_uiRepairTimer = 15000;
        SetCombatMovement(true);
        if (Creature* pGun = GetClosestCreatureWithEntry(m_creature, LEVIATHAN_TURRET, 80.0f))
        {
            if (pGun->isDead())
                pGun->Respawn();
        }
        if (m_pInstance)
        {
            if (Creature* pMimiron = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_MIMIRON)))
                pMimiron->AI()->EnterEvadeMode();
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* pWho)
    {
       if(Creature* pGun = GetClosestCreatureWithEntry(m_creature, LEVIATHAN_TURRET, 80.0f))
          pGun->SetInCombatWithZone();
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
         if(uiDamage > m_creature->GetHealth())
         {
             uiDamage = 0;
             if(m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
             {
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_1);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetHealth(1);
                m_creature->InterruptNonMeleeSpells(true);
                if(Creature* pGun = GetClosestCreatureWithEntry(m_creature, LEVIATHAN_TURRET, 80.0f))
                   pGun->DealDamage(pGun, pGun->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->RemoveAllAuras();
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->StopMoving();
                m_creature->AttackStop();
                SetCombatMovement(false);
             }
             if(m_pInstance && m_pInstance->GetData(TYPE_LEVIATHAN_MK) == NOT_STARTED && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
             {
                m_pInstance->SetData(TYPE_LEVIATHAN_MK, IN_PROGRESS);
                if(m_pInstance->GetData(TYPE_LEVIATHAN_MK) == IN_PROGRESS && m_pInstance->GetData(TYPE_VX001) == IN_PROGRESS && m_pInstance->GetData(TYPE_AERIAL_UNIT) == IN_PROGRESS)
                {
                   m_pInstance->SetData(TYPE_LEVIATHAN_MK, DONE);
                   m_pInstance->SetData(TYPE_VX001, DONE);
                   m_pInstance->SetData(TYPE_AERIAL_UNIT, DONE);
                   m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_OUTRO);
                }
                else
                {
                   m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                   m_creature->SetHealth(1);
                   m_creature->InterruptNonMeleeSpells(true);
                   m_creature->RemoveAllAuras();
                   m_creature->AttackStop();
                   DoCast(m_creature, SPELL_SELF_REPAIR);
                }
             }
        }
    }

    void SuppressFires()
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, DEFAULT_VISIBILITY_INSTANCE);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, DEFAULT_VISIBILITY_INSTANCE);
        if (!lFires.empty())
        {
            for(std::list<Creature*>::iterator iter = lFires.begin(); iter != lFires.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->ForcedDespawn();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_LEVIATHAN_MK) == IN_PROGRESS)
        {
           if(m_uiRepairTimer < uiDiff)
           {
               m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
               m_pInstance->SetData(TYPE_LEVIATHAN_MK, NOT_STARTED);
               if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                  AttackStart(pTarget);
               m_uiRepairTimer = 15000;
           } else m_uiRepairTimer -= uiDiff;
           return;
        }

        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
           return;

        if(m_uiMinesTimer < uiDiff)
        {
           for(uint8 i = 0; i < urand(8, 10); i++)
               m_creature->CastSpell(m_creature, SPELL_PROXIMITY_MINES, true);
           m_uiMinesTimer = 30000;
        }
         else m_uiMinesTimer -= uiDiff;

                 // shock blast
        if(m_uiShockBlastTimer < uiDiff)
        {
           DoCast(m_creature, SPELL_SHOCK_BLAST);
           m_uiShockBlastTimer = 50000;
        }
         else m_uiShockBlastTimer -= uiDiff;

        if(m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_LEVIATHAN)
        {
            if(m_uiSetFireTimer < uiDiff)
            {
                // start again 3 fires
                for(uint8 i = 0; i < 4; i++)
                {
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                       pTarget->CastSpell(pTarget, SPELL_FLAMES_SUMMON, true);
                }
                m_uiSetFireTimer = urand(10000, 30000);
            }
            else m_uiSetFireTimer -= uiDiff;

            if(!m_bHasSuppresed)
            {
               if(m_creature->GetHealthPercent() < 50.0f)
               {
                   m_creature->InterruptNonMeleeSpells(true);
                   DoCast(m_creature, SPELL_FLAME_SUPRESSANT);
                   m_bHasSuppresed = true;
                   m_uiSupressTimer = 2000;
                   m_uiSetFireTimer = 10000;
               }
            }
            else
            {
               if(m_uiSupressTimer < uiDiff)
               {
                   SuppressFires();
                   m_uiSupressTimer = 600000;
               }
               else m_uiSupressTimer -= uiDiff;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// VX001 script
struct MANGOS_DLL_DECL boss_vx001AI : public ScriptedAI
{   
    boss_vx001AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->AddSplineFlag(SPLINEFLAG_FLYING);
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance* m_pInstance;

    bool m_bStartAttack;
    uint32 m_uiAttackStartTimer;

    uint32 m_uiRapidBurstTimer;
    uint32 m_uiLaserBarrageTimer;
    uint32 m_uiRocketStrikeTimer;
    uint32 m_uiHeatWaveTimer;
    uint32 m_uiHandPulseTimer;

    uint32 m_uiFlameSuppressTimer;
    uint32 m_uiFrostBombTimer;
    uint32 m_uiSpreadFiresTimer;
    uint32 m_uiMoveTimer;
    uint32 m_uiEndBarrageTimer;
    uint32 m_uiStartBarrageTimer;
    uint32 m_uiProcTimer;
    uint32 m_uiRepairTimer;
    uint64 m_uiTargetGUID;
	
    bool m_bIsBarrage;

    void Reset()
    {
        m_uiRapidBurstTimer     = 1000;
        m_uiLaserBarrageTimer   = 60000;
        m_uiRocketStrikeTimer   = 25000;
        m_uiHeatWaveTimer       = 20000;
        m_uiHandPulseTimer      = 1000;
        m_uiRepairTimer = 15000;

        m_uiFlameSuppressTimer  = urand(10000, 15000);
        m_uiFrostBombTimer      = urand(25000, 30000);
        m_uiSpreadFiresTimer    = urand(1000, 5000);

        m_bIsBarrage = false;
        m_uiTargetGUID = 0;
        m_uiMoveTimer = 2000;
        m_uiEndBarrageTimer = 100;
        m_uiStartBarrageTimer = 100;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (m_pInstance)
        {
            if (Creature* pMimiron = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_MIMIRON)))
            {
                if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_VX001)
                {
                    if (urand(0, 1))
                        DoScriptText(SAY_TORSO_SLAY1, pMimiron);
                    else
                        DoScriptText(SAY_TORSO_SLAY2, pMimiron);
                }
                if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
                {
                    if (urand(0, 1))
                        DoScriptText(SAY_ROBOT_SLAY1, pMimiron);
                    else
                        DoScriptText(SAY_ROBOT_SLAY2, pMimiron);
                }
            }
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) || m_bIsBarrage)
            return;

        ScriptedAI::AttackStart(pWho);
    }
 
    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
         if (uiDamage > m_creature->GetHealth())
         {
             uiDamage = 0;
             if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_VX001)
             {
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_2);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetHealth(1);
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->RemoveAllAuras();
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->StopMoving();
                m_creature->AttackStop();
                DoCast(m_creature, SPELL_TORSO_DISABLED);
             }
             if(m_pInstance && m_pInstance->GetData(TYPE_VX001) == NOT_STARTED && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
             {
                m_pInstance->SetData(TYPE_VX001, IN_PROGRESS);
                if(m_pInstance->GetData(TYPE_LEVIATHAN_MK) == IN_PROGRESS && m_pInstance->GetData(TYPE_VX001) == IN_PROGRESS && m_pInstance->GetData(TYPE_AERIAL_UNIT) == IN_PROGRESS)
                {
                   m_pInstance->SetData(TYPE_LEVIATHAN_MK, DONE);
                   m_pInstance->SetData(TYPE_VX001, DONE);
                   m_pInstance->SetData(TYPE_AERIAL_UNIT, DONE);
                   m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_OUTRO);
                }
                else
                {
                   m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                   m_creature->SetHealth(1);
                   m_creature->InterruptNonMeleeSpells(true);
                   m_creature->RemoveAllAuras();
                   m_creature->AttackStop();
                   DoCast(m_creature, SPELL_SELF_REPAIR);
                }
             }
        }
    }

    void SuppressFires()
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, 10.0f);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, 10.0f);
        if (!lFires.empty())
        {
            for(std::list<Creature*>::iterator iter = lFires.begin(); iter != lFires.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->ForcedDespawn();
            }
        }
    }

    Creature* SelectRandomFire()
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, DEFAULT_VISIBILITY_INSTANCE);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, DEFAULT_VISIBILITY_INSTANCE);

        //This should not appear!
        if (lFires.empty()){
            m_uiFrostBombTimer = 5000;
            return NULL;
        }

        std::list<Creature* >::iterator iter = lFires.begin();
        advance(iter, urand(0, lFires.size()-1));

        if((*iter)->isAlive())
            return *iter;
        else
        {
            m_uiFrostBombTimer = 500;
            return NULL;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_VX001) == IN_PROGRESS)
        {
           if(m_uiRepairTimer < uiDiff)
           {
               m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
               m_pInstance->SetData(TYPE_VX001, NOT_STARTED);
               if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                  AttackStart(pTarget);
               m_uiRepairTimer = 15000;
           } else m_uiRepairTimer -= uiDiff;
           return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(!m_bIsBarrage)
        {
           if(m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
           {
              if(m_uiFrostBombTimer < uiDiff)
              {
                  if(Creature* pFire = SelectRandomFire())
                      m_creature->SummonCreature(MOB_FROST_BOMB, pFire->GetPositionX(), pFire->GetPositionY(), pFire->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 10000);
                  m_uiFrostBombTimer = urand(50000, 60000);
                  m_uiSpreadFiresTimer = urand(15000, 20000);
              } else m_uiFrostBombTimer -= uiDiff;

              if(m_uiSpreadFiresTimer < uiDiff)
              {
                  // start again 3 fires
                  for(uint8 i = 0; i < 4; i++)
                  {
                      if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                       pTarget->CastSpell(pTarget, SPELL_FLAMES_SUMMON, true);
                  }
                  m_uiSpreadFiresTimer = urand(10000, 30000);
              } else m_uiSpreadFiresTimer -= uiDiff;
           }

           if(m_uiRocketStrikeTimer < uiDiff)
           {
              if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
              {
                 m_creature->CastSpell(pTarget, SPELL_ROCKET_MISSILE, true);
                 if(Creature* pRocket = m_creature->SummonCreature(NPC_ROCKET_STRIKE, pTarget->GetPositionX(), pTarget->GetPositionY(), CENTER_Z, 3.16f, TEMPSUMMON_TIMED_DESPAWN, 15000))
                 {
                    pRocket->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pRocket->SetDisplayId(11686);
                    pRocket->CastSpell(pRocket, SPELL_ROCKET_DMG, false);
                 }
              }
              m_uiRocketStrikeTimer = urand(15000, 25000);
           }
            else m_uiRocketStrikeTimer -= uiDiff;

           if(m_uiLaserBarrageTimer < uiDiff)
           {
              float fAng = urand(0, 359.0f);
              if(Creature* pTarget = m_creature->SummonCreature(NPC_MIMIRON_FOCUS, CENTER_X-40.76f*cos(fAng), CENTER_Y-40.76f*sin(fAng), CENTER_Z, 3.16f, TEMPSUMMON_TIMED_DESPAWN, 15000))
              {
                  m_uiStartBarrageTimer = 3800;
                  m_uiEndBarrageTimer = 14000;
                  m_uiTargetGUID = pTarget->GetGUID();
                  m_creature->SetInCombatWith(pTarget);
                  m_creature->AddThreat(pTarget, 10000000.0f);
                  m_creature->SetOrientation(m_creature->GetAngle(pTarget));
                  m_creature->InterruptNonMeleeSpells(false);
                  DoCast(pTarget, SPELL_LASER_PREPEAD);
                  m_bIsBarrage = true;
              }
              m_uiLaserBarrageTimer = 60000;
           }
            else m_uiLaserBarrageTimer -= uiDiff;

           switch(m_pInstance->GetData(TYPE_MIMIRON_PHASE))
           {
              case PHASE_VX001:
                  if(m_uiRapidBurstTimer < uiDiff)
                  {
                      if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                      {
                          switch(urand(0,1))
                          {
                             case 0: DoCast(pTarget, m_bIsRegularMode ? SPELL_RAPID_BURST_L_N : SPELL_RAPID_BURST_L_H); break;
                             case 1: DoCast(pTarget, m_bIsRegularMode ? SPELL_RAPID_BURST_R_N : SPELL_RAPID_BURST_R_H); break;
                          }
                      }
                      m_uiRapidBurstTimer = 750;
                  }
                  else m_uiRapidBurstTimer -= uiDiff;

                  if(m_uiHeatWaveTimer < uiDiff)
                  {
                      m_creature->InterruptNonMeleeSpells(false);
                      DoCast(m_creature, m_bIsRegularMode ? SPELL_HEAT_WAVE : SPELL_HEAT_WAVE_H);
                      m_uiHeatWaveTimer = urand(10000, 15000);
                  }
                  else m_uiHeatWaveTimer -= uiDiff;

                  if(m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                  {
                     if(m_uiFlameSuppressTimer < uiDiff)
                     {
                        DoCast(m_creature, SPELL_FLAME_SUPRESS);
                        SuppressFires();
                        m_uiFlameSuppressTimer = urand(30000, 76000);
                     } else m_uiFlameSuppressTimer -= uiDiff;
                  }
                  break;
              case PHASE_ROBOT:
                  if(m_uiHandPulseTimer < uiDiff)
                  {
                      if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                      {
                          switch(urand(0,1))
                          {
                             case 0: DoCast(pTarget, m_bIsRegularMode ? SPELL_HAND_PULSE_L : SPELL_HAND_PULSE_L_H); break;
                             case 1: DoCast(pTarget, m_bIsRegularMode ? SPELL_HAND_PULSE_R : SPELL_HAND_PULSE_R_H); break;
                          }
                      }
                      m_uiHandPulseTimer = 1000;
                  }
                  else m_uiHandPulseTimer -= uiDiff;
                  break;
           }
        }
        else
        { 
           if(m_uiStartBarrageTimer < uiDiff)
           {
               m_creature->InterruptNonMeleeSpells(false);
               if(Creature* pTarget = m_pInstance->instance->GetCreature(m_uiTargetGUID))
                  m_creature->CastSpell(pTarget, SPELL_LASER_TRIGG, false);
               m_uiStartBarrageTimer = 60000;
           }
            else m_uiStartBarrageTimer -= uiDiff;

           if(m_uiEndBarrageTimer < uiDiff)
           {
               m_bIsBarrage = false;  
           }
             else m_uiEndBarrageTimer -= uiDiff;
        }
     }
};

// Aerial command unit script
struct MANGOS_DLL_DECL boss_aerial_command_unitAI : public ScriptedAI
{   
    boss_aerial_command_unitAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->AddSplineFlag(SPLINEFLAG_FLYING);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 m_uiPlasmaBallTimer;
    uint32 m_uiSummonWavesTimer;
    uint32 m_uiMoveTimer;
    uint32 m_uiVehicleTimer;
    uint32 m_uiRepairTimer;
    uint32 m_uiSpreadFiresTimer;

    bool m_bIsHasMagnetic;

    void Reset()
    {
        m_uiSpreadFiresTimer    = urand(1000, 5000);
        m_uiPlasmaBallTimer     = 3000;
        m_uiSummonWavesTimer    = 10000;
        m_uiVehicleTimer = 10;
        m_uiMoveTimer           = 5000;
        m_bIsHasMagnetic = false;
        m_uiRepairTimer = 15000;

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit* pWho)
    {
        if(m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
         if(uiDamage > m_creature->GetHealth())
         {
             uiDamage = 0;
             if(m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL && !m_creature->HasAura(SPELL_MAGNETIC_CORE))
             {
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                m_creature->RemoveAurasDueToSpell(SPELL_MAGNETIC_EFFECT);
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_TRANS_3);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetHealth(1);
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->RemoveAllAuras();
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->StopMoving();
                m_creature->AttackStop();
             }
             if(m_pInstance && m_pInstance->GetData(TYPE_AERIAL_UNIT) == NOT_STARTED && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
             {
                m_pInstance->SetData(TYPE_AERIAL_UNIT, IN_PROGRESS);
                if(m_pInstance->GetData(TYPE_LEVIATHAN_MK) == IN_PROGRESS && m_pInstance->GetData(TYPE_VX001) == IN_PROGRESS && m_pInstance->GetData(TYPE_AERIAL_UNIT) == IN_PROGRESS)
                {
                   m_pInstance->SetData(TYPE_LEVIATHAN_MK, DONE);
                   m_pInstance->SetData(TYPE_VX001, DONE);
                   m_pInstance->SetData(TYPE_AERIAL_UNIT, DONE);
                   m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_OUTRO);
                }
                else
                {
                   m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                   m_creature->SetHealth(1);
                   m_creature->InterruptNonMeleeSpells(true);
                   m_creature->RemoveAllAuras();
                   m_creature->AttackStop();
                   DoCast(m_creature, SPELL_SELF_REPAIR);
                }
             }
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (m_pInstance)
        {
            if (Creature* pMimiron = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_MIMIRON)))
            {
                if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
                {
                    if (urand(0, 1))
                        DoScriptText(SAY_HEAD_SLAY1, pMimiron);
                    else
                        DoScriptText(SAY_HEAD_SLAY2, pMimiron);
                }
                if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_ROBOT)
                {
                    if (urand(0,1))
                        DoScriptText(SAY_ROBOT_SLAY1, pMimiron);
                    else
                        DoScriptText(SAY_ROBOT_SLAY2, pMimiron);
                }
            }
        }
    }

    void JustSummoned(Creature* pSummon)
    {
        pSummon->SetInCombatWithZone();
    }

    void VehicleHack()
    {
        if (m_pInstance)
        {
            if (Creature* pMimiron = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_MIMIRON)))
            {
                pMimiron->GetMap()->CreatureRelocation(pMimiron, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+1.0f, m_creature->GetOrientation());
                pMimiron->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+1.0f, SPLINETYPE_NORMAL , SPLINEFLAG_DONE, 10);
            }
        }
    }

    void MagneticCast()
    {    
         m_creature->GetMotionMaster()->Clear(false);
         m_creature->GetMotionMaster()->MovementExpired(false);
         m_creature->StopMoving();
         m_creature->InterruptNonMeleeSpells(true);
         m_creature->CastSpell(m_creature, SPELL_MAGNETIC_CORE, true);
         m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
         m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
         m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), CENTER_Z, m_creature->GetOrientation());
         m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), CENTER_Z, SPLINETYPE_NORMAL , m_creature->GetSplineFlags(), 1000);
         m_creature->CastSpell(m_creature, SPELL_MAGNETIC_EFFECT, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_AERIAL_UNIT) == IN_PROGRESS)
        {
            if(m_uiRepairTimer < uiDiff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_pInstance->SetData(TYPE_AERIAL_UNIT, NOT_STARTED);
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                    AttackStart(pTarget);
                m_uiRepairTimer = 15000;
            }
            else
               m_uiRepairTimer -= uiDiff;

            return;
        }

        if (m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_TRANS_3 || m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
        {
           if(m_uiVehicleTimer < uiDiff)
           { 
               VehicleHack();
               m_uiVehicleTimer = 10;
           }
           else
               m_uiVehicleTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->HasAura(SPELL_MAGNETIC_CORE))
            return;
        else
        {
            if (m_creature->HasAura(SPELL_MAGNETIC_EFFECT))
            {
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                m_creature->RemoveAurasDueToSpell(SPELL_MAGNETIC_EFFECT);
                m_uiMoveTimer = 500;
            }
        }

        if (m_uiPlasmaBallTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_PLASMA_BALL : SPELL_PLASMA_BALL_H);
            m_uiPlasmaBallTimer = urand(3000, 5000);
        }
        else
            m_uiPlasmaBallTimer -= uiDiff;

        switch (m_pInstance->GetData(TYPE_MIMIRON_PHASE))
        {
            case PHASE_AERIAL:
               if(m_uiMoveTimer < uiDiff)
               {
                  float Dis = urand(5, 15);
                  float fAng = urand(0, 359);
                  float X = m_creature->GetPositionX()-Dis*cos(fAng);
                  float Y = m_creature->GetPositionY()-Dis*sin(fAng);
                  float Z = CENTER_Z+9.5f;
                  m_creature->GetMotionMaster()->MovementExpired(false);
                  if(m_creature->GetDistance2d(CENTER_X, CENTER_Y) > 30.0f)
                     m_creature->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, Z);
                  else m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
           
                  m_uiMoveTimer = 10000;
            
               } else m_uiMoveTimer -= uiDiff;

               if(m_uiSummonWavesTimer < uiDiff)
               {
                   uint32 m_uiCreatureEntry;
                   // summon emergency boots
                   if(m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                   {
                       switch(urand(0, 6))
                       {
                         case 0:
                            m_uiCreatureEntry = MOB_BOMB_BOT;
                            break;
                         case 1:
                            m_uiCreatureEntry = MOB_BOMB_BOT_500;
                            break;
                         case 2:
                             m_uiCreatureEntry = MOB_JUNK_BOT;
                             break;
                         case 3:
                         case 4:
                             m_uiCreatureEntry = MOB_EMERGENCY_FIRE_BOT;
                             break;
                         case 5:
                         case 6:
                             m_uiCreatureEntry = MOB_ASSALT_BOT;
                             break;
                       }
                   }
                   else
                   {
                       switch(urand(0, 4))
                       {
                         case 0:
                            m_uiCreatureEntry = MOB_BOMB_BOT;
                            break;
                         case 1:
                            m_uiCreatureEntry = MOB_JUNK_BOT;
                            break;
                         case 2:
                         case 3:
                            m_uiCreatureEntry = MOB_ASSALT_BOT;
                            break;
                         case 4:
                            m_uiCreatureEntry = MOB_BOMB_BOT_500;
                            break;
                       }
                   }
                   uint8 m_uiSummonLoc = urand(0, 8);
                   if(m_uiCreatureEntry != 0)
                     if(Creature* pSummon =  m_creature->SummonCreature(m_uiCreatureEntry, SummonLoc[m_uiSummonLoc].x, SummonLoc[m_uiSummonLoc].y, CENTER_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 60000))
                       pSummon->CastSpell(pSummon, SPELL_TELEPORT_BOT, false);

                   m_uiSummonWavesTimer = urand (10000, 15000);
               }
               else m_uiSummonWavesTimer -= uiDiff; 

               if(m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
               {
                   if(m_uiSpreadFiresTimer < uiDiff)
                   {
                       // start again 3 fires
                       for(uint8 i = 0; i < 4; i++)
                       {
                           if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                               pTarget->CastSpell(pTarget, SPELL_FLAMES_SUMMON, true);
                       }
                       m_uiSpreadFiresTimer = urand(10000, 30000);
                   }
                   else m_uiSpreadFiresTimer -= uiDiff;
               }
               break;
        }
    }
};

struct MANGOS_DLL_DECL boss_mimironAI : public ScriptedAI
{   
    boss_mimironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *m_pInstance;

    bool m_bIsHardMode;

    uint64 m_uiTankGUID;
    uint64 m_uiTorsoGUID;
    uint64 m_uiHeadGUID;
  
    uint32 m_uiStepTimer;
    uint32 m_uiStep;
    uint32 m_uiProcTimer;

    bool m_bIsProc;

    void Reset()
    {

        m_uiTankGUID            = 0;
        m_uiTorsoGUID           = 0;
        m_uiHeadGUID            = 0;
        m_uiStep                = 1;
        m_uiStepTimer           = 4000;
        m_uiProcTimer           = 100;
        m_bIsProc = false;
        m_bIsHardMode = false;

        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if(Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
           pTorso->RemoveFromWorld();
        if(Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
           pHead->RemoveFromWorld();
        if(Creature* pComp = GetClosestCreatureWithEntry(m_creature, NPC_COMPUTER, 80.0f))
           pComp->RemoveFromWorld();
        if(m_pInstance)
        {
            m_pInstance->SetData(TYPE_MIMIRON, NOT_STARTED);
            m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_IDLE);
            m_pInstance->SetData(TYPE_MIMIRON_HARD, NOT_STARTED);
            m_pInstance->SetData(TYPE_LEVIATHAN_MK, NOT_STARTED);
            m_pInstance->SetData(TYPE_VX001, NOT_STARTED); 
            m_pInstance->SetData(TYPE_AERIAL_UNIT, NOT_STARTED);
            if(GameObject* pButton = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MIMIRON_BUTTON)))
               pButton->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        }
    }

    void AttackStart(Unit* pWho)
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_pInstance && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 15.0f) && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_IDLE)
        {
            m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
            m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_INTRO);
            if(GameObject* pButton = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MIMIRON_BUTTON)))
               pButton->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JumpTo(float X, float Y, float Z, float Vertical, uint32 Time)
    {
       m_creature->CastSpell(m_creature, SPELL_JET_PACK, true);
       m_creature->GetMap()->CreatureRelocation(m_creature, X, Y, Z, m_creature->GetAngle(X,Y));
       m_creature->SendMonsterMoveJump(X, Y, Z, Vertical, SPLINEFLAG_TRAJECTORY, Time);
    }

    void TorsoJump(Creature* torso, float X, float Y, float Z, float Vertical, uint32 Time)
    {
       torso->GetMap()->CreatureRelocation(torso, X, Y, Z, torso->GetOrientation());
       torso->SendMonsterMoveJump(X, Y, Z, Vertical, SPLINEFLAG_TRAJECTORY, Time);
    }

    void TankPosition()
    {
        if (m_pInstance)
        {
            if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
            {
                m_creature->GetMap()->CreatureRelocation(m_creature, pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ(), m_creature->GetOrientation());
                m_creature->SendMonsterMove(pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ(), SPLINETYPE_NORMAL , SPLINEFLAG_DONE, 1);
            }
        }
    }

    void Intro()
    {
        switch(m_uiStep)
        {
            case 1:
                m_creature->SetInCombatWithZone();
                if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                {
                    DoScriptText(SAY_HARD_MODE, m_creature);
                    m_uiStepTimer = 12000;
                    m_uiStep = 4;
                }
                else
                {
                    DoScriptText(SAY_AGGRO, m_creature);
                    JumpNextStep(10000);
                }
                break;
            case 2:
                JumpTo(2742.561f, 2567.766f, 369.717f, 6.0f, 2000);
                JumpNextStep(3000);
                break;
            case 3:
                m_creature->RemoveAurasDueToSpell(SPELL_JET_PACK);
                DoScriptText(SAY_TANK_ACTIVE, m_creature);
                JumpNextStep(14000);
                break;
            case 4:
                if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                        JumpTo(pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ(), 8.0f, 3000);
                }
                else
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                        JumpTo(pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ(), 5.0f, 3000);
                }
                JumpNextStep(3000);
                break;
            case 5:
                m_creature->SetVisibility(VISIBILITY_OFF);
                if (m_pInstance)
                {
                    m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_LEVIATHAN);
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                    {
                        pTank->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pTank->SetInCombatWithZone();
                        if (m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                            pTank->CastSpell(pTank, SPELL_EMERGENCY_MODE_AURA, false);
                    }
               }
               m_uiStep = 1;
               m_uiStepTimer = 1000;
               break;
         }
         return;
    }

    void Trans01()
    {
        switch(m_uiStep)
        {
            case 1:
                if (m_pInstance)
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                        pTank->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, CENTER_Z);
                    JumpNextStep(5000);
                }
                break;
            case 2:
                if (m_pInstance)
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                    {
                        pTank->GetMotionMaster()->MovementExpired(false);
                        pTank->GetMotionMaster()->MovePoint(0, 2795.638f, 2599.115f, 364.313f);
                    }
                }
                JumpNextStep(10000);
                break;
            case 3:
                TankPosition();
                m_creature->SetVisibility(VISIBILITY_ON);
                DoScriptText(SAY_TANK_DEATH, m_creature);
                m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
                if (m_pInstance)
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                        JumpTo(pTank->GetPositionX(), pTank->GetPositionY(), pTank->GetPositionZ()+6.0f, 8.0f, 3000);
                }
                JumpNextStep(3000);
                break;
            case 4:
                if (GameObject* pLift = GetClosestGameObjectWithEntry(m_creature, GO_MIMIRON_ELEVATOR, DEFAULT_VISIBILITY_INSTANCE))
                    pLift->SetGoState(GO_STATE_READY);
                m_creature->RemoveAurasDueToSpell(SPELL_JET_PACK);
                JumpNextStep(12000);
                break;
            case 5:
                m_creature->SummonCreature(NPC_VX001, CENTER_X, CENTER_Y, CENTER_Z, 3.16f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 190000);
                JumpNextStep(3000);
                break;
            case 6:
                if (GameObject* pLift = GetClosestGameObjectWithEntry(m_creature, GO_MIMIRON_ELEVATOR, DEFAULT_VISIBILITY_INSTANCE))
                  pLift->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                JumpNextStep(6000);
                break;
            case 7:
                JumpTo(2743.245f, 2569.410f, 372.155f, 8.0f, 4000);
                JumpNextStep(5000);
                break;
            case 8:
                m_creature->RemoveAurasDueToSpell(SPELL_JET_PACK);
                DoScriptText(SAY_TORSO_ACTIVE, m_creature);
                JumpNextStep(8000);
                break;
            case 9:
                JumpTo(CENTER_X, CENTER_Y, CENTER_Z+2.0f, 4.0f, 2000);
                JumpNextStep(2000);
                break;
            case 10:
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                    pTorso->CastSpell(pTorso, 50142, false);
                JumpNextStep(3000);
                break;
            case 11:
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                    pTorso->CastSpell(pTorso, 52970, true);
                m_creature->RemoveAurasDueToSpell(SPELL_JET_PACK);
                m_creature->SetVisibility(VISIBILITY_OFF);
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_VX001);
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                {
                    pTorso->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pTorso->SetInCombatWithZone();
                    if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                        pTorso->CastSpell(pTorso, SPELL_EMERGENCY_MODE_AURA, false);
                }
                m_uiStep = 1;
                m_uiStepTimer = 100;
                break;
        }
    }

    void Trans02()
    {
         switch(m_uiStep)
         {
            case 1:
               m_creature->SetVisibility(VISIBILITY_ON);
               JumpNextStep(1000);
               break;
            case 2:
               JumpTo(CENTER_X, CENTER_Y, 372.155f, 8.0f, 2000);
               JumpNextStep(3000);
               break;
            case 3:
               DoScriptText(SAY_TORSO_DEATH, m_creature);
               JumpNextStep(9000);
               break;
            case 4:
               m_creature->SummonCreature(NPC_AERIAL_UNIT, CENTER_X, CENTER_Y, 390.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 190000);
               JumpNextStep(3000);
               break;
            case 5:
               if(Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
               {
                  pHead->GetMap()->CreatureRelocation(pHead, CENTER_X, CENTER_Y, 376.550f, pHead->GetOrientation());
                  pHead->SendMonsterMove(CENTER_X, CENTER_Y, 376.550f, SPLINETYPE_NORMAL , pHead->GetSplineFlags(), 3000);
                  //pHead->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, 376.550f);
               }
               JumpNextStep(4000);
               break;
            case 6:
               if(Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
                  pHead->GetMotionMaster()->MovePoint(0, 2740.884f, 2556.056f, 376.550f);
               JumpNextStep(4000);
               break;
            case 7:
               JumpTo(2740.727f, 2554.341f, 378.850f, 5.0f, 3000);
               JumpNextStep(5000);
               break;
            case 8:
               DoScriptText(SAY_HEAD_ACTIVE, m_creature);
               JumpNextStep(8000);
               break;
            case 9:
               JumpTo(2740.737f, 2556.015f, 376.461f, 4.0f, 2000);
               JumpNextStep(2000);
               break;
            case 10:
               m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_AERIAL);
               if(Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
               {
                  pHead->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                  pHead->SetInCombatWithZone();
                  if(m_pInstance->GetData(TYPE_MIMIRON_HARD) == IN_PROGRESS)
                     pHead->CastSpell(pHead, SPELL_EMERGENCY_MODE_AURA, false);
               }
               m_uiStep = 1;
               m_uiStepTimer = 1500;
               break;
         }
    }

    void Trans03()
    {    
        switch(m_uiStep)
        {
            case 1:
                if(Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
                {
                    pHead->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
                    pHead->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                    pHead->SetSpeedRate(MOVE_RUN, 3.0f, true);
                    pHead->SetSpeedRate(MOVE_WALK, 3.0f, true);
                    pHead->GetMap()->CreatureRelocation(pHead, pHead->GetPositionX(), pHead->GetPositionY(), CENTER_Z+14.0f, pHead->GetOrientation());
                    pHead->SendMonsterMove(pHead->GetPositionX(), pHead->GetPositionY(), CENTER_Z+14.0f, SPLINETYPE_NORMAL , pHead->GetSplineFlags(), 1000);
                    //pHead->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), CENTER_Z+14.0f);
                }
                JumpNextStep(5000);
                break;
            case 2:
                DoScriptText(SAY_HEAD_DEATH, m_creature);
                JumpNextStep(8000);
                break;
            case 3:
                if (m_pInstance)
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                        pTank->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, CENTER_Z);
                }
                JumpNextStep(1000);
                break;
            case 4:
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                {
                    pTorso->RemoveAurasDueToSpell(SPELL_TORSO_DISABLED);
                    pTorso->CastSpell(pTorso, 58930, false);
                }
                JumpNextStep(4500);
                break;
            case 5:
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                    TorsoJump(pTorso, CENTER_X, CENTER_Y, CENTER_Z+5.0f, 11.0f, 1500);
                JumpNextStep(1500);
                break;
            case 6:
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                { 
                    pTorso->RemoveAurasDueToSpell(58930);
                    m_bIsProc = true;
                }
                JumpNextStep(1500);
                break;
            case 7:
                if (Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
                    pHead->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, CENTER_Z+12.0f);
                JumpNextStep(5500);
                break;
            case 8:
                DoScriptText(SAY_ROBOT_ACTIVE, m_creature);
                JumpNextStep(13000);
                break;
            case 9:
                if (m_pInstance)
                {
                    m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_ROBOT);
                    if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                    { 
                        pTorso->SetHealth(pTorso->GetMaxHealth()*0.3);
                        pTorso->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Unit* pTarget = pTorso->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                            pTorso->AI()->AttackStart(pTarget);
                    }
                    if (Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
                    {
                        pHead->SetHealth(pHead->GetMaxHealth()*0.3);
                        pHead->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Unit* pTarget = pHead->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                            pHead->AI()->AttackStart(pTarget);
                    }
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                    {
                        pTank->SetHealth(pTank->GetMaxHealth()*0.3);
                        pTank->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Unit* pTarget = pTank->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        pTank->AI()->AttackStart(pTarget);
                    }
                }
                m_uiStep = 1;
                m_uiStepTimer = 50;
                break;
        }
    }

    void Outro()
    {
        switch(m_uiStep)
        {
            case 1:
                if (Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                    pTorso->DealDamage(pTorso, pTorso->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                if (Creature* pHead = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
                    pHead->DealDamage(pHead, pHead->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                if (m_pInstance)
                {
                    if (Creature* pTank = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LEVIATHAN_MK)))
                        pTank->DealDamage(pTank, pTank->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
                JumpNextStep(1500);
                break;
            case 2:
                JumpTo(2737.214f, 2582.876f, 364.313f, 2.0f, 4000);
                JumpNextStep(4500);
                break;
            case 3:
                DoScriptText(SAY_ROBOT_DEATH, m_creature);
                JumpNextStep(14500);
                break;
            case 4:
                DoCast(m_creature, SPELL_TELEPORT_BOT);
                JumpNextStep(1000);
                break;
            case 5:
                m_creature->SetVisibility(VISIBILITY_OFF);
                JumpNextStep(2000);
                break;
            case 6:
                m_pInstance->SetData(TYPE_MIMIRON, DONE);
                m_creature->ForcedDespawn();
                JumpNextStep(2000);
                break;
         }
    }

    void JumpNextStep(uint32 uiTimer)
    {
        m_uiStepTimer = uiTimer;
        m_uiStep++;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
           switch(m_pInstance->GetData(TYPE_MIMIRON_PHASE))
           {
              case PHASE_INTRO:
                   if(m_uiStepTimer < uiDiff)
                   {
                       Intro();
                   } else m_uiStepTimer -= uiDiff;
                   break;      
              case PHASE_TRANS_1:
                   if(m_uiStepTimer < uiDiff)
                   {
                       Trans01();
                   } else m_uiStepTimer -= uiDiff;
                   break;    
              case PHASE_TRANS_2:
                   if(m_uiStepTimer < uiDiff)
                   {
                       Trans02();
                   } else m_uiStepTimer -= uiDiff;
                   break;  
              case PHASE_TRANS_3:
                   if(m_bIsProc)
                   {
                      if(m_uiProcTimer < uiDiff)
                      {
                         if(Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
                            pTorso->CastSpell(pTorso, 60865, true);
                         m_uiProcTimer = 500;
                      } else m_uiProcTimer -= uiDiff;
                   }

                   if(m_uiStepTimer < uiDiff)
                   {
                       Trans03();
                   } else m_uiStepTimer -= uiDiff;
                   break;  
              case PHASE_OUTRO:
                   if(m_uiStepTimer < uiDiff)
                   {
                       Outro();
                   } else m_uiStepTimer -= uiDiff;
                   break;  
           } 
        }
        return; 
     }       
};

struct MANGOS_DLL_DECL mob_proximity_mineAI : public ScriptedAI
{   
    mob_proximity_mineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiExplosionTimer;
    uint32 m_uiRangeCheckTimer;
    uint32 m_uiDieTimer;

    bool m_bIsActive;

    void Reset()
    {
        m_uiExplosionTimer  = 60000;
        m_uiRangeCheckTimer = 1000;
        m_uiDieTimer        = 65000;
        m_bIsActive = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if(!m_bIsActive && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 2.0f))
        {
            m_bIsActive = true;
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLOSION : SPELL_EXPLOSION_H);
            m_uiDieTimer = 1000;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiDieTimer < uiDiff)
            m_creature->ForcedDespawn();
        else
            m_uiDieTimer -= uiDiff;

        if(m_uiExplosionTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLOSION : SPELL_EXPLOSION_H);
            m_uiDieTimer        = 1000;
            m_uiExplosionTimer = 20000;
        }
        else m_uiExplosionTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_bomb_botAI : public ScriptedAI
{   
    mob_bomb_botAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiRangeCheckTimer;
    uint32 m_uiDieTimer;
  
    bool m_bIsActive;

    void Reset()
    {
        m_uiRangeCheckTimer = 1000;
        m_uiDieTimer        = 600000;
        m_bIsActive = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if(!m_bIsActive && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 2.0f))
        {
            m_bIsActive = true;
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLOSION : SPELL_EXPLOSION_H);
            m_uiDieTimer = 1000;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if(uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;
            if(!m_bIsActive)
            {
               m_bIsActive = true;
               DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLOSION : SPELL_EXPLOSION_H);
               m_uiDieTimer = 500;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDieTimer < uiDiff)
            m_creature->ForcedDespawn();
        else
            m_uiDieTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_assault_botAI : public ScriptedAI
{   
    mob_assault_botAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiMagneticFieldTimer;

    void Reset()
    {
        m_uiMagneticFieldTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMagneticFieldTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_MAGNETIC_FIELD);
            m_uiMagneticFieldTimer = urand(10000, 15000);
        }
        else m_uiMagneticFieldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_emergency_botAI : public ScriptedAI
{   
    mob_emergency_botAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiWaterSprayTimer;

    void Reset()
    {
        m_uiWaterSprayTimer = urand(5000, 10000);
        if(!m_bIsRegularMode)
            DoCast(m_creature, SPELL_DEAFENING_SIREN);
    }

    void SuppressFires()
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, 15.0f);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, 15.0f);
        if (!lFires.empty())
        {
            for(std::list<Creature*>::iterator iter = lFires.begin(); iter != lFires.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->ForcedDespawn();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiWaterSprayTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_WATER_SPRAY);
            SuppressFires();
            m_uiWaterSprayTimer = urand(7000, 12000);
        }
        else m_uiWaterSprayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_mimiron_focusAI : public ScriptedAI
{   
    npc_mimiron_focusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pCreature->SetSpeedRate(MOVE_RUN, 3.0f, true);
        pCreature->SetSpeedRate(MOVE_WALK, 3.0f, true);
        SetCombatMovement(false);
        Reset();
    }

    uint32 m_uiMoveTimer;
    bool m_bReadyForWPMove;

    void Reset()
    {
        m_creature->setFaction(35); 
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_uiMoveTimer = 4000;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiMoveTimer < uiDiff)
        {
           if(Creature* pTorso = GetClosestCreatureWithEntry(m_creature, NPC_VX001, 80.0f))
              pTorso->SetOrientation(pTorso->GetAngle(m_creature));
           float fAng = 9.0f + m_creature->GetAngle(CENTER_X, CENTER_Y);
           m_creature->NearTeleportTo(CENTER_X+40.76f*cos(fAng), CENTER_Y+40.76f*sin(fAng), CENTER_Z, 0);
           m_uiMoveTimer = 1000;
           //m_creature->GetMotionMaster()->MovePoint(1, CENTER_X+40.76f*cos(fAng), CENTER_Y+40.76f*sin(fAng), CENTER_Z);
        }
         else m_uiMoveTimer -= uiDiff;
    }
};

// item script, used to bring the aerial unit down
struct MANGOS_DLL_DECL mob_magnetic_coreAI : public ScriptedAI
{   
    mob_magnetic_coreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSpellTimer;
    uint32 m_uiDieTimer;

    void Reset()
    {
        m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), CENTER_Z, SPLINETYPE_NORMAL , SPLINEFLAG_DONE, 10);
        m_uiSpellTimer = 2000;
        m_uiDieTimer = 23000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiSpellTimer < uiDiff)
        {
            if(Creature* pAerial = GetClosestCreatureWithEntry(m_creature, NPC_AERIAL_UNIT, 80.0f))
            {
                if(m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_AERIAL)
                   ((boss_aerial_command_unitAI*)pAerial->AI())->MagneticCast();
                DoCast(m_creature, SPELL_MAGNETIC_CORE);
            }
            m_uiSpellTimer = 100000;
        }
        else m_uiSpellTimer -= uiDiff;

        if (m_uiDieTimer < uiDiff)
            m_creature->ForcedDespawn();
        else
            m_uiDieTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL npc_mimiron_computerAI : public ScriptedAI
{   
    npc_mimiron_computerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->setFaction(35);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiExplodeTimer;
    uint32 m_uiLastTimer;
    uint32 m_uiStep;
    uint32 m_uiStepTimer;

    bool m_bIsDone;

    void Reset()
    {
        m_uiStep = 1;
        m_uiStepTimer = 13000;
        m_bIsDone = false;
    }

    void Next()
    {
        m_uiStep++;
        m_uiStepTimer = 60000;
    }
 
    void BOOMBOOM()
    {
        m_creature->CastSpell(m_creature, 64613, true);
        m_creature->CastSpell(m_creature, 64610, false); //Goodbye America and Blizzard
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_MIMIRON) == DONE && !m_bIsDone && m_uiStep != 11)
        {
           m_bIsDone = true;
           DoScriptText(SAY_COMPUTER_ENDTIMER, m_creature);
        }

        if(m_pInstance->GetData(TYPE_MIMIRON) == IN_PROGRESS)
        {
           if(m_uiStepTimer < uiDiff) 
           {
              switch(m_uiStep)
              {
                 case 1: DoScriptText(SAY_COMPUTER_10MIN, m_creature); Next(); break;
                 case 2: DoScriptText(SAY_COMPUTER_09MIN, m_creature); Next(); break;
                 case 3: DoScriptText(SAY_COMPUTER_08MIN, m_creature); Next(); break;
                 case 4: DoScriptText(SAY_COMPUTER_07MIN, m_creature); Next(); break;
                 case 5: DoScriptText(SAY_COMPUTER_06MIN, m_creature); Next(); break;
                 case 6: DoScriptText(SAY_COMPUTER_05MIN, m_creature); Next(); break;
                 case 7: DoScriptText(SAY_COMPUTER_04MIN, m_creature); Next(); break;
                 case 8: DoScriptText(SAY_COMPUTER_03MIN, m_creature); Next(); break;
                 case 9: DoScriptText(SAY_COMPUTER_02MIN, m_creature); Next(); break;
                 case 10: DoScriptText(SAY_COMPUTER_01MIN, m_creature); Next(); break;
                 case 11: DoScriptText(SAY_COMPUTER_00MIN, m_creature); Next(); BOOMBOOM(); break;
             }
          } else m_uiStepTimer -= uiDiff;
       }
       return;
    }
};

struct MANGOS_DLL_DECL mob_frost_bombAI : public ScriptedAI
{   
    mob_frost_bombAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance *pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiExplosionTimer;
    uint32 m_uiDieTimer;

    void Reset()
    {
        m_uiExplosionTimer  = 10000;
        m_uiDieTimer        = 15000;
        DoCast(m_creature, SPELL_FROST_BOMB_VISUAL);
    }

    void SuppressFires()
    {
        std::list<Creature*> lFires;
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34363, 30.0f);
        GetCreatureListWithEntryInGrid(lFires, m_creature, 34121, 30.0f);
        if (!lFires.empty())
        {
            for(std::list<Creature*>::iterator iter = lFires.begin(); iter != lFires.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->ForcedDespawn();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiDieTimer < uiDiff)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else
            m_uiDieTimer -= uiDiff;

        if (m_uiExplosionTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_FROST_BOMB_EXPL);
            SuppressFires();
            m_uiExplosionTimer = 100000;
            m_uiDieTimer = 500;
        }
        else m_uiExplosionTimer -= uiDiff;
    }
};

// Flames used in hard mode
struct MANGOS_DLL_DECL mob_mimiron_flamesAI : public ScriptedAI
{   
    mob_mimiron_flamesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFlamesSpreadTimer;

    void Reset()
    {
        DoCast(m_creature, SPELL_FLAMES);
        m_uiFlamesSpreadTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MIMIRON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
    }
};

bool GOUse_go_red_button(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (Creature* pMimiron = pGo->GetMap()->GetCreature(m_pInstance->GetData64(NPC_MIMIRON)))
        {
            if (Creature* pComputer = pGo->SummonCreature(NPC_COMPUTER, CENTER_X, CENTER_Y, CENTER_Z, 3.16f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 190000))
                DoScriptText(SAY_COMPUTER_START_TIMER, pComputer);
            pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1); 
            if (pMimiron->isAlive() && m_pInstance->GetData(TYPE_MIMIRON_PHASE) == PHASE_IDLE && m_pInstance->GetData(TYPE_MIMIRON) == NOT_STARTED)
            {
                m_pInstance->SetData(TYPE_MIMIRON_PHASE, PHASE_INTRO);
                m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
                m_pInstance->SetData(TYPE_MIMIRON_HARD, IN_PROGRESS);
            }
        }
    }

    return false;
}

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

CreatureAI* GetAI_boss_leviathan_mk(Creature* pCreature)
{
    return new boss_leviathan_mkAI(pCreature);
}

CreatureAI* GetAI_leviathan_turret(Creature* pCreature)
{
    return new leviathan_turretAI(pCreature);
}

CreatureAI* GetAI_boss_vx001(Creature* pCreature)
{
    return new boss_vx001AI(pCreature);
}

CreatureAI* GetAI_boss_aerial_command_unit(Creature* pCreature)
{
    return new boss_aerial_command_unitAI(pCreature);
}

CreatureAI* GetAI_mob_proximity_mine(Creature* pCreature)
{
    return new mob_proximity_mineAI(pCreature);
}

CreatureAI* GetAI_mob_bomb_bot(Creature* pCreature)
{
    return new mob_bomb_botAI(pCreature);
}

CreatureAI* GetAI_mob_assault_bot(Creature* pCreature)
{
    return new mob_assault_botAI(pCreature);
}

CreatureAI* GetAI_mob_emergency_bot(Creature* pCreature)
{
    return new mob_emergency_botAI(pCreature);
}

CreatureAI* GetAI_npc_mimiron_focus(Creature* pCreature)
{
    return new npc_mimiron_focusAI(pCreature);
}

CreatureAI* GetAI_mob_magnetic_core(Creature* pCreature)
{
    return new mob_magnetic_coreAI(pCreature);
}

CreatureAI* GetAI_npc_mimiron_computer(Creature* pCreature)
{
    return new npc_mimiron_computerAI(pCreature);
}

CreatureAI* GetAI_mob_frost_bomb(Creature* pCreature)
{
    return new mob_frost_bombAI(pCreature);
}

CreatureAI* GetAI_mob_mimiron_flames(Creature* pCreature)
{
    return new mob_mimiron_flamesAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_mimiron";
    newscript->GetAI = &GetAI_boss_mimiron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_leviathan_mk";
    newscript->GetAI = &GetAI_boss_leviathan_mk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "leviathan_turret";
    newscript->GetAI = &GetAI_leviathan_turret;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_vx001";
    newscript->GetAI = &GetAI_boss_vx001;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_aerial_command_unit";
    newscript->GetAI = &GetAI_boss_aerial_command_unit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_proximity_mine";
    newscript->GetAI = &GetAI_mob_proximity_mine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_bomb_bot";
    newscript->GetAI = &GetAI_mob_bomb_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_assault_bot";
    newscript->GetAI = &GetAI_mob_assault_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_emergency_bot";
    newscript->GetAI = &GetAI_mob_emergency_bot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mimiron_focus";
    newscript->GetAI = &GetAI_npc_mimiron_focus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_magnetic_core";
    newscript->GetAI = &GetAI_mob_magnetic_core;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mimiron_computer";
    newscript->GetAI = &GetAI_npc_mimiron_computer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frost_bomb";
    newscript->GetAI = &GetAI_mob_frost_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mimiron_flames";
    newscript->GetAI = &GetAI_mob_mimiron_flames;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_red_button";
    newscript->pGOUse = &GOUse_go_red_button;
    newscript->RegisterSelf();
}
