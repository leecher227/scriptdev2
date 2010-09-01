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
SDComment:
SDAuthor: MaxXx2021 (aka Mioka) and PSZ
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
    SAY_SUMMON_TOR              = -1575035,
    SAY_SLAY_1                  = -1575036,
    SAY_SLAY_2                  = -1575037,
    SAY_SLAY_3                  = -1575038,
    SAY_SLAY_4                  = -1575039,
    SAY_DEATH                   = -1575040,

    SPELL_BANE_N                            = 48294,
    SPELL_BANE_H                            = 59301,
    SPELL_DARK_SLASH                        = 48292,
    SPELL_FETID_ROT_N                       = 48291,
    SPELL_FETID_ROT_H                       = 59300,
    SPELL_SCREAMS_OF_THE_DEAD               = 51750,

    SPELL_SPIRIT_BURST_N                    = 48529, //granted by Ranulf
    SPELL_SPIRIT_BURST_H                    = 59305,
    SPELL_SPIRIT_STRIKE_N                   = 48423, //granted by Haldor
    SPELL_SPIRIT_STRIKE_H                   = 59304,
    SPELL_SUMMON_SPIRIT_FOUNT               = 48386, //granted by Bjorn
    SPELL_SUMMON_SPIRIT_FOUNT_VISUAL        = 48385,
    SPELL_SPIRIT_FOUNT_N                    = 48380,
    SPELL_SPIRIT_FOUNT_H                    = 59320,
    SPELL_AVENGING_SPIRITS                  = 48590, //granted by Tor
    SPELL_SUMMON_AVENGING_SPIRIT            = 48592,
    SPELL_SUMMON_AVENGING_SPIRIT_VISUAL     = 48593,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON          = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT          = 48307,
    SPELL_SPIRIT_EMERGE                     = 56864,
    SPELL_SPIRIT_DIES                       = 48596,

    SPELL_RANULF_VISUAL                     = 48312,
    SPELL_HALDOR_VISUAL                     = 48311,    
    SPELL_BJORN_VISUAL                      = 48308,    
    SPELL_TOR_VISUAL                        = 48313,

    MODEL_ID_INVISIBLE                      = 11686,

    NPC_SPIRIT_FOUNT                        = 27339
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations YmironPoints[] =
{
    {402.577f, -335.175f, 104.757f},  // RANULF
    {403.175f, -314.411f, 104.755f},  // HALDOR
    {381.753f, -314.570f, 104.757f},  // BJORN
    {381.044f, -335.059f, 104.754f},  // TOR
};

static uint32 Ancestors[3][4] = 
{
    {NPC_RANULF,          NPC_HALDOR,          NPC_BJORN,          NPC_TOR},
    {SPELL_RANULF_VISUAL, SPELL_HALDOR_VISUAL, SPELL_BJORN_VISUAL, SPELL_TOR_VISUAL},
    {SAY_SUMMON_RANULF,   SAY_SUMMON_HALDOR,   SAY_SUMMON_BJORN,   SAY_SUMMON_TOR},
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
        memset(&m_auiDummyCasterGUID, 0, sizeof(m_auiDummyCasterGUID));
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBaneTimer;
    uint32 m_uiDarkSlashTimer;
    uint32 m_uiFetidRotTimer;
    uint32 m_uiAncestorsVengeanceTimer;
    uint32 m_uiStepTimer;
    uint32 m_uiAncestorAbilityTimer;
    uint32 m_uiDelayTimer;
    bool m_bDelay;
    uint8 m_uiStep;
    uint8 m_uiPhase;
    bool m_bIsGhostEvent;
    uint8 m_auiAncestorsOrder[4];
    uint64 m_auiDummyCasterGUID[4];

    void Reset()
    {
        m_uiAncestorAbilityTimer = 0; 
        m_uiBaneTimer = urand(13000, 17000);
        m_uiDarkSlashTimer = urand(10000, 12000);
        m_uiFetidRotTimer = urand(8000, 10000);
        m_uiPhase = 0;
        m_bIsGhostEvent = false;
        m_bDelay = false;
        m_uiDelayTimer = 0;

    	for (uint8 i = 0; i < 4; ++i)
        {
            m_auiAncestorsOrder[i] = i;
            if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][i])))
                pAncestor->RemoveAllAuras();
            if (Creature* pDummyCaster = m_pInstance->instance->GetCreature(m_auiDummyCasterGUID[i]))
                pDummyCaster->ForcedDespawn();
            m_auiDummyCasterGUID[i] = 0;
        }
        srand(uint32(time));
    	for (uint8 i = 0; i < 4; ++i)
        {
            uint8 j = urand(0, 3);
	        uint8 uiTmp = m_auiAncestorsOrder[i];
	        m_auiAncestorsOrder[i] = m_auiAncestorsOrder[j];
	        m_auiAncestorsOrder[j] = uiTmp;
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
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_YMIRON, DONE);
            if (m_uiPhase > 0)
            {
                if (Creature* pDummyCaster = m_pInstance->instance->GetCreature(m_auiDummyCasterGUID[m_uiPhase-1]))
                    pDummyCaster->ForcedDespawn();
                if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][m_auiAncestorsOrder[m_uiPhase-1]])))
                {
                    pAncestor->RemoveAllAuras();
                    pAncestor->CastSpell(pAncestor, SPELL_SPIRIT_DIES, false);
                }
            }
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == 27339)
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, SPELL_SUMMON_SPIRIT_FOUNT_VISUAL, true);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType == POINT_MOTION_TYPE && uiPointId == 0)
        {
            if (m_pInstance)
                if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][m_auiAncestorsOrder[m_uiPhase]])))
                {
                    m_creature->SetFacingToObject(pAncestor);
                    m_creature->CastSpell(pAncestor, SPELL_CHANNEL_YMIRON_TO_SPIRIT, true);
                }
            DoScriptText(Ancestors[2][m_auiAncestorsOrder[m_uiPhase]], m_creature);
            m_uiStepTimer = 3000;
            ++m_uiStep;
        }
    }

    void SetDelay(uint32 uiTimer)
    {
        m_bDelay = true;
        m_uiDelayTimer = uiTimer;
        m_creature->GetMotionMaster()->MovementExpired(false);
        m_creature->GetMotionMaster()->Clear();
        m_creature->StopMoving();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bDelay)
        {
            if (m_uiDelayTimer <= uiDiff)
            {
                m_bDelay = false;
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
            else
                m_uiDelayTimer -= uiDiff;

            return;
        }

        if (m_bIsGhostEvent)
        {
            if (m_uiStepTimer <= uiDiff)
            {
                if (m_pInstance)
                {
                    switch (m_uiStep)
                    {
                        case 1:
                            if (m_uiPhase > 0)
                            {
                                if (Creature* pDummyCaster = m_pInstance->instance->GetCreature(m_auiDummyCasterGUID[m_uiPhase-1]))
                                    pDummyCaster->ForcedDespawn();
                                if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][m_auiAncestorsOrder[m_uiPhase-1]])))
                                    pAncestor->CastSpell(pAncestor, SPELL_SPIRIT_DIES, false);
                                SetDelay(2000);
                            }
                            ++m_uiStep;
                            break;
                        case 2:
                            if (m_uiPhase > 0)
                                if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][m_auiAncestorsOrder[m_uiPhase-1]])))
                                    pAncestor->RemoveAurasDueToSpell(Ancestors[1][m_auiAncestorsOrder[m_uiPhase-1]]);
                            m_creature->InterruptNonMeleeSpells(true);
                            m_creature->GetMotionMaster()->MovePoint(0, YmironPoints[m_auiAncestorsOrder[m_uiPhase]].x, YmironPoints[m_auiAncestorsOrder[m_uiPhase]].y, YmironPoints[m_auiAncestorsOrder[m_uiPhase]].z);
                            ++m_uiStep;
                            break;
                        case 4:
                            if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][m_auiAncestorsOrder[m_uiPhase]])))
                            {
                                pAncestor->CastSpell(pAncestor, Ancestors[1][m_auiAncestorsOrder[m_uiPhase]], false);
                                pAncestor->CastSpell(pAncestor, SPELL_SPIRIT_EMERGE, true);
                            }
                            m_uiStepTimer = 2500;
                            ++m_uiStep;
                            break;
                        case 5:
                            if (Creature* pAncestor = m_pInstance->instance->GetCreature(m_pInstance->GetData64(Ancestors[0][m_auiAncestorsOrder[m_uiPhase]])))
                            {
                                if (Creature* pDummyCaster = m_creature->SummonCreature(1921, pAncestor->GetPositionX(), pAncestor->GetPositionY(), pAncestor->GetPositionZ()+7.0f, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                                {
                                    pDummyCaster->SetDisplayId(MODEL_ID_INVISIBLE);
                                    pDummyCaster->setFaction(35);
                                    pDummyCaster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    pDummyCaster->GetMap()->CreatureRelocation(pDummyCaster, pDummyCaster->GetPositionX(), pDummyCaster->GetPositionY(), pDummyCaster->GetPositionZ()+15.0f, 0);
                                    pDummyCaster->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                                    m_auiDummyCasterGUID[m_uiPhase] = pDummyCaster->GetGUID();
                                }
                                pAncestor->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                            }
                            m_bIsGhostEvent = false;
                            switch (Ancestors[0][m_auiAncestorsOrder[m_uiPhase]])
                            {
                                case NPC_RANULF:
                                case NPC_HALDOR:
                                    m_uiAncestorAbilityTimer = 5000;
                                    break;
                                case NPC_BJORN:
                                    m_uiAncestorAbilityTimer = 4000;
                                    break;
                                case NPC_TOR:
                                    m_uiAncestorAbilityTimer = 7000;
                                    break;
                            }
                            ++m_uiPhase;
                            ++m_uiStep;
                            SetDelay(2000);
                            break;      
                    }
                }
            }
            else
                m_uiStepTimer -= uiDiff;

                return;
        }

        if (m_uiBaneTimer <= uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_BANE_N : SPELL_BANE_H);
            m_uiBaneTimer = urand(13000, 17000);
        }
        else
            m_uiBaneTimer -= uiDiff;

        if (m_uiDarkSlashTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->getVictim())
            {
                int32 iDmg = pTarget->GetHealth()/2;
                m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_DARK_SLASH, &iDmg, 0, 0, false);
            }
            m_uiDarkSlashTimer = urand(20000, 25000);
        }
        else
            m_uiDarkSlashTimer -= uiDiff;

        if (m_uiFetidRotTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FETID_ROT_N : SPELL_FETID_ROT_H);
            m_uiFetidRotTimer = urand(13000, 16000);
        }
        else
            m_uiFetidRotTimer -= uiDiff;

        if (!m_bIsGhostEvent && m_creature->GetHealthPercent() < 100 - (m_bIsRegularMode ? 33.4f : 20.0f) * (m_uiPhase+1))
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, SPELL_SCREAMS_OF_THE_DEAD, true);
            m_uiStepTimer = 0;
            m_uiStep = 1;
            m_bIsGhostEvent = true;
        }

        if (m_uiPhase > 0)
        {
            if (m_uiAncestorAbilityTimer <= uiDiff)
            {
                m_creature->InterruptNonMeleeSpells(true);
                switch (Ancestors[0][m_auiAncestorsOrder[m_uiPhase-1]])
                {
                    case NPC_RANULF:
                        DoCast(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_BURST_N : SPELL_SPIRIT_BURST_H);
                        m_uiAncestorAbilityTimer = urand(5000, 8000);
                        break;
                    case NPC_HALDOR:
                        DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_STRIKE_N : SPELL_SPIRIT_STRIKE_H);
                        m_uiAncestorAbilityTimer = 5000;
                        break;
                    case NPC_BJORN:
                        DoCast(m_creature, SPELL_SUMMON_SPIRIT_FOUNT);
                        m_uiAncestorAbilityTimer = 20000;
                        SetDelay(3500);
                        break;
                    case NPC_TOR:
                        DoCast(m_creature, SPELL_AVENGING_SPIRITS);
                        m_uiAncestorAbilityTimer = 22000;
                        break;
                }
            }
            else
                m_uiAncestorAbilityTimer -= uiDiff;
        }
        DoMeleeAttackIfReady();
    }
 };

struct MANGOS_DLL_DECL npc_avenging_spirit_summonerAI : public ScriptedAI
{
    npc_avenging_spirit_summonerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
	
    uint32 m_uiSummonAvengingSpiritTimer;

    void Reset()
    {
        m_uiSummonAvengingSpiritTimer = 7500;
        m_creature->SetDisplayId(MODEL_ID_INVISIBLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->setFaction(35);
        DoCast(m_creature, SPELL_SUMMON_AVENGING_SPIRIT_VISUAL);
        m_creature->ForcedDespawn(9000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetInCombatWithZone();
    }
  
    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiSummonAvengingSpiritTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_SUMMON_AVENGING_SPIRIT);
            m_uiSummonAvengingSpiritTimer = 7500;
        }
        else
            m_uiSummonAvengingSpiritTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL npc_bjorn_sphereAI : public ScriptedAI
{
    npc_bjorn_sphereAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiSpiritFoutTimer;
	
    void Reset()
    {
        m_uiSpiritFoutTimer = 3000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->setFaction(14);
        m_creature->SetDisplayId(11686);
        m_creature->SetLevel(82);
        m_creature->SetSpeedRate(MOVE_RUN, 0.3f);
        m_creature->SetInCombatWithZone();
        m_creature->ForcedDespawn(21000);
    }
  
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if (m_uiSpiritFoutTimer <= uiDiff)
        {
            m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_FOUNT_N : SPELL_SPIRIT_FOUNT_H, false);
            m_uiSpiritFoutTimer = 30000;
        }
        else
            m_uiSpiritFoutTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI(pCreature);
}

CreatureAI* GetAI_npc_avenging_spirit_summoner(Creature* pCreature)
{
    return new npc_avenging_spirit_summonerAI(pCreature);
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
    newscript->Name = "npc_avenging_spirit_summoner";
    newscript->GetAI = &GetAI_npc_avenging_spirit_summoner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bjorn_sphere";
    newscript->GetAI = &GetAI_npc_bjorn_sphere;
    newscript->RegisterSelf();
}
