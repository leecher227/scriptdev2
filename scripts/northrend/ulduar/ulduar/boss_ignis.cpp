/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_ignis
SD%Complete:
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"
#include "Vehicle.h"

enum
{
    //yells
    SAY_AGGRO       = -1603010,
    SAY_SCORCH1     = -1603011,
    SAY_SCORCH2     = -1603012,
    SAY_SLAGPOT     = -1603013,
    EMOTE_FLAMEJETS = -1603014,
    SAY_SUMMON      = -1603015,
    SAY_SLAY1       = -1603016,
    SAY_SLAY2       = -1603017,
    SAY_BERSERK     = -1603018,
    SAY_DEATH       = -1603019,

    //ignis the furnace master
    SPELL_FLAME_JETS			= 62680,
    SPELL_FLAME_JETS_H			= 63472,
    SPELL_SLAG_POT				= 62717,
    SPELL_SLAG_POT_H			= 63477,
    SPELL_SLAG_POT_DMG			= 65722,
    SPELL_SLAG_POT_DMG_H		= 65723,
    SPELL_SCORCH				= 62546,
    SPELL_SCORCH_H				= 63474,
    BUFF_STRENGHT_OF_CREATOR	= 64473,
    SPELL_STRENGHT_OF_CREATOR2	= 64474,
    SPELL_STRENGHT_OF_CREATOR3	= 64475,
    SPELL_HASTE					= 66045,
    SPELL_ENRAGE                = 26662,
    //iron construct
    SPELL_HEAT					= 65667,
    SPELL_MOLTEN				= 62373,
    SPELL_BRITTLE				= 62382,
    SPELL_SHATTER				= 62383,
    //scorch target
    AURA_SCORCH					= 62548,
    AURA_SCORCH_H				= 63476,
    SPELL_FREEZE_ANIM           = 16245,
    //NPC ids
    MOB_IRON_CONSTRUCT			= 33121,
    MOB_SCORCH_TARGET			= 33221,

    ACHIEV_STOKIN_THE_FURNACE   = 2930,
    ACHIEV_STOKIN_THE_FURNACE_H = 2929,
    ACHIEV_SHATTERED            = 2925,
    ACHIEV_SHATTERED_H          = 2926,
    ACHIEV_HOT_POCKET           = 2927,
    ACHIEV_HOT_POCKET_H         = 2928,
};

#define HOME_X                      586.747009f
#define HOME_Y                      381.997986f

uint32 m_uiLastBrittleTime;
bool m_bAchievShatteredCompleted;

// scorch target
struct MANGOS_DLL_DECL mob_scorch_targetAI : public ScriptedAI
{
    mob_scorch_targetAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiScorchTimer;
    uint32 m_uiHeatTimer;
    bool m_bScorchCasted;

    void Reset()
    {
        m_uiScorchTimer = 2000;
        m_uiHeatTimer = 3000;
        m_bScorchCasted = false;
        m_creature->SetDisplayId(11686);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->ForcedDespawn(55000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bScorchCasted)
        {
            if (m_uiScorchTimer < uiDiff)
            {
                if (SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(m_bIsRegularMode ? AURA_SCORCH : AURA_SCORCH_H))
                {
                    pTempSpell->Effect[1] = 0;
                    m_creature->CastSpell(m_creature, pTempSpell, true);
                }
                m_bScorchCasted = true;
            }
            else
                m_uiScorchTimer -= uiDiff;   
        }
        if (m_uiHeatTimer < uiDiff)
        {
            if (SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_HEAT))
            {
                pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                pTempSpell->EffectImplicitTargetA[0] = 0;

                std::list<Creature*> lConstructs;
                GetCreatureListWithEntryInGrid(lConstructs, m_creature, MOB_IRON_CONSTRUCT, 13.0f);
                if (!lConstructs.empty())
                {
                    for (std::list<Creature*>::iterator iter = lConstructs.begin(); iter != lConstructs.end(); ++iter)
                    {
                        if ((*iter) && (*iter)->isAlive() && !(*iter)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) && !(*iter)->HasAura(SPELL_MOLTEN) && !(*iter)->HasAura(SPELL_BRITTLE))
                            (*iter)->CastSpell(*iter, pTempSpell, true);
                    }
                }
            }
            m_uiHeatTimer = 1000;
        }
        else
            m_uiHeatTimer -= uiDiff;   
    }
};

CreatureAI* GetAI_mob_scorch_target(Creature* pCreature)
{
    return new mob_scorch_targetAI(pCreature);
}

// iron construct
struct MANGOS_DLL_DECL mob_iron_constructAI : public ScriptedAI
{
    mob_iron_constructAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiDeath_Timer;
    uint32 m_uiMoltenTimer;
    uint32 m_uiBrittleTimer;
    bool m_bIsBrittle;
    bool m_bIsShatter;
    bool m_bIsMolten;
    bool m_bIsFreezed;

    uint32 m_uiWaterCheckTimer;
    uint32 m_uiHeatCheckTimer;

    void Reset()
    {
        m_bIsShatter        = false;
        m_bIsBrittle        = false;
        m_bIsMolten         = false;
        m_bIsFreezed        = false;
        m_uiWaterCheckTimer = 1000;
        m_uiHeatCheckTimer  = 1000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        // remove 1 stack of the buff from Ignis, hacky way, should be done by spell
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_IGNIS)))
        {
            if (pTemp->isAlive())
            {
                if (pTemp->HasAura(BUFF_STRENGHT_OF_CREATOR))
                {
                    if (Aura* pTempAura = pTemp->GetAura(BUFF_STRENGHT_OF_CREATOR, EFFECT_INDEX_0))
                        if (pTempAura->modStackAmount(-1))
                            pTemp->RemoveAurasDueToSpell(BUFF_STRENGHT_OF_CREATOR);
                    /* GENERATION_NEXT VERSION
                    if (SpellAuraHolder* strenght = pTemp->GetSpellAuraHolder(BUFF_STRENGHT_OF_CREATOR))
                    {
                        if (strenght->ModStackAmount(-1))
                            pTemp->RemoveAurasDueToSpell(BUFF_STRENGHT_OF_CREATOR);
                    }
                    */
                }
            }
        }
    }

    // shatter if is brittle
    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsBrittle)
        {
            if (uiDamage > 5000)
            {
                DoCast(m_creature, SPELL_SHATTER);
                m_bIsShatter = true;
                uint32 uiCurrTime = getMSTime();
                if (uiCurrTime-m_uiLastBrittleTime <= 5000)
                    m_bAchievShatteredCompleted = true;
                m_uiLastBrittleTime = getMSTime();
                m_uiDeath_Timer = 500;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (!m_bIsFreezed)
            {
                m_bIsFreezed = true;
                DoCast(m_creature, SPELL_FREEZE_ANIM);
            }
            return;
        }

        // death after casted shatter
        if (m_bIsShatter)
        {
            if (m_uiDeath_Timer < uiDiff)
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            else
                m_uiDeath_Timer -= uiDiff;   

            return;
        }

        //Water checks
        if (m_bIsMolten)
        {
            if (m_uiWaterCheckTimer <= uiDiff)
            {
                if (m_creature->GetDistance2d(524.15f, 277.0f) < 18 || m_creature->GetDistance2d(648.5f, 277.0f) < 18)
                {
                    DoCast(m_creature, SPELL_BRITTLE);
                    m_creature->RemoveAurasDueToSpell(SPELL_MOLTEN);
                    m_bIsBrittle = true;
                    m_bIsMolten = false;
                }
                /* GENERATION_NEXT VERSION (should work with Vmaps3)
                if (m_creature->IsInWater())
                {
                    DoCast(m_creature, SPELL_BRITTLE);
                    m_bIsBrittle = true;
                    m_bIsMolten = false;
                }*/
                m_uiWaterCheckTimer = 1000;
            }
            else
                m_uiWaterCheckTimer -= uiDiff;
        }

        // check for aura
        if (m_uiHeatCheckTimer < uiDiff)
        {
            if (Aura* aura = m_creature->GetAura(SPELL_HEAT, EFFECT_INDEX_0))
            {
                if (aura->GetStackAmount() > 9)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAT);
                    DoCast(m_creature, SPELL_MOLTEN);
                    m_bIsMolten = true;
                }
            }
            m_uiHeatCheckTimer = 1000;
        }
        else
            m_uiHeatCheckTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_iron_construct(Creature* pCreature)
{
    return new mob_iron_constructAI(pCreature);
}

//ignis the furnace master
struct MANGOS_DLL_DECL boss_ignisAI : public ScriptedAI
{
    boss_ignisAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    std::list<uint64> m_lIronConstructGUIDList;

    uint32 m_uiFlame_Jets_Timer;
    uint32 m_uiSlag_Pot_Timer;
    uint32 m_uiSlag_Pot_Dmg_Timer;
    uint32 m_uiScorch_Timer;
    uint32 m_uiSummon_Timer;
    uint32 m_uiPotDmgCount;
    uint32 m_uiEnrageTimer;

    uint64 m_uiPotTargetGUID;
    std::list<Creature*> lConstructs;

    uint32 m_uiEncounterTimer;
    bool m_bHasSlagPotCasted;

    void Reset()
    {
        m_uiFlame_Jets_Timer    = urand(20000, 22000);
        m_uiSlag_Pot_Timer      = 15000;
        m_uiSlag_Pot_Dmg_Timer  = 0;
        m_uiScorch_Timer        = 25000;
        m_uiSummon_Timer        = 10000;
        m_uiEnrageTimer         = 600000;   // 10 MIN
        m_uiPotDmgCount         = 0;
        m_uiPotTargetGUID       = 0;
        m_lIronConstructGUIDList.clear();
        m_bHasSlagPotCasted     = false;

        m_uiEncounterTimer      = 0;
        m_uiLastBrittleTime     = 0;
        m_bAchievShatteredCompleted = false;

        // respawn constructs
        GetCreatureListWithEntryInGrid(lConstructs, m_creature, MOB_IRON_CONSTRUCT, 200.0f);
        if (!lConstructs.empty())
        {
            for (std::list<Creature*>::iterator iter = lConstructs.begin(); iter != lConstructs.end(); ++iter)
            {
                if ((*iter) && (*iter)->isDead())
                    (*iter)->Respawn();
            }
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, FAIL);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
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
        GetCreatureListWithEntryInGrid(lConstructs, m_creature, MOB_IRON_CONSTRUCT, 200.0f);
        if (!lConstructs.empty())
        {
            for (std::list<Creature*>::iterator iter = lConstructs.begin(); iter != lConstructs.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                    (*iter)->DealDamage(*iter, (*iter)->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, DONE);

        if (m_pInstance)
        {
            if (m_uiEncounterTimer < 240000)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_STOKIN_THE_FURNACE : ACHIEV_STOKIN_THE_FURNACE_H);
            if (m_bAchievShatteredCompleted)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_SHATTERED : ACHIEV_SHATTERED_H);
        }
    }

    Creature* SelectRandomConstruct(float fRange)
    {
        std::list<Creature* > lConstructList;
        GetCreatureListWithEntryInGrid(lConstructList, m_creature, MOB_IRON_CONSTRUCT, fRange);

        if (lConstructList.empty()){
            m_uiSummon_Timer = 5000;
            return NULL;
        }

        std::list<Creature* >::iterator iter = lConstructList.begin();
        advance(iter, urand(0, lConstructList.size()-1));

        if ((*iter)->isAlive())
            return *iter;
        else
        {
            m_uiSummon_Timer = 500;
            return NULL;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_uiEncounterTimer += uiDiff;

        // enrage
        if (m_uiEnrageTimer < uiDiff)
        {
            DoScriptText(SAY_BERSERK, m_creature);
            DoCast(m_creature, SPELL_ENRAGE);
            m_uiEnrageTimer = 30000;
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (m_uiFlame_Jets_Timer < uiDiff)
        {
            DoScriptText(EMOTE_FLAMEJETS, m_creature);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_FLAME_JETS : SPELL_FLAME_JETS_H);
            m_uiFlame_Jets_Timer = urand(20000, 22000);
        }
        else
            m_uiFlame_Jets_Timer -= uiDiff;

        // need vehicle support!!!
        if (m_uiSlag_Pot_Timer < uiDiff)
        {
            DoScriptText(SAY_SLAGPOT, m_creature);
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                DoCast(pTarget, m_bIsRegularMode ? SPELL_SLAG_POT : SPELL_SLAG_POT_H);
                m_uiPotTargetGUID = pTarget->GetGUID();
                if (m_creature->CreateVehicleKit(342))
                    if (VehicleKit* Kit = m_creature->GetVehicleKit())
                        pTarget->EnterVehicle(Kit, 1);
            }
            m_uiSlag_Pot_Timer      = 15000;
            m_uiSlag_Pot_Dmg_Timer  = 1000;
            m_bHasSlagPotCasted     = true;
            m_uiPotDmgCount         = 0;
        }
        else
            m_uiSlag_Pot_Timer -= uiDiff;  

        // hacky way of doing damage
        if (m_bHasSlagPotCasted)
        {
            if (m_uiSlag_Pot_Dmg_Timer < uiDiff)
            {
                if (Unit* pPotTarget = Unit::GetUnit(*m_creature, m_uiPotTargetGUID))
                {
                    if (m_uiPotDmgCount < 10)
                        DoCast(pPotTarget, m_bIsRegularMode ? SPELL_SLAG_POT_DMG : SPELL_SLAG_POT_DMG_H);
                    else
                    {
                        if (pPotTarget->isAlive())
                        {
                            pPotTarget->CastSpell(pPotTarget, SPELL_HASTE, false);
                            pPotTarget->ExitVehicle();
                            ((Player*)pPotTarget)->CompletedAchievement(m_bIsRegularMode ? ACHIEV_HOT_POCKET : ACHIEV_HOT_POCKET_H);
                        }
                        m_bHasSlagPotCasted = false;
                    }
                }
                ++m_uiPotDmgCount;
                m_uiSlag_Pot_Dmg_Timer = 1000;
            }
            else
                m_uiSlag_Pot_Dmg_Timer -= uiDiff;
        }

        // call the golems
        if (m_uiSummon_Timer < uiDiff)
        {
            DoScriptText(SAY_SUMMON, m_creature);

            if (Creature* pConstruct = SelectRandomConstruct(200.0f))
            {
                pConstruct->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pConstruct->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pConstruct->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                pConstruct->SetInCombatWithZone();
            }

            m_uiSummon_Timer = 40000;

            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, BUFF_STRENGHT_OF_CREATOR);
        }
        else
            m_uiSummon_Timer -= uiDiff;

        if (m_uiScorch_Timer < uiDiff)
        {
            if (urand(0, 1))
                DoScriptText(SAY_SCORCH1, m_creature);
            else
                DoScriptText(SAY_SCORCH2, m_creature);

            DoCast(m_creature, m_bIsRegularMode ? SPELL_SCORCH : SPELL_SCORCH_H);
            m_creature->SummonCreature(MOB_SCORCH_TARGET, m_creature->getVictim()->GetPositionX(), m_creature->getVictim()->GetPositionY(), m_creature->getVictim()->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 55000);
            m_uiScorch_Timer = 25000;
        }
        else
            m_uiScorch_Timer -= uiDiff;

        DoMeleeAttackIfReady();

        if (m_creature->GetDistance2d(HOME_X, HOME_Y) > 200.0f)
            EnterEvadeMode();
    }
};

CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignisAI(pCreature);
}

void AddSC_boss_ignis()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_ignis";
    NewScript->GetAI = GetAI_boss_ignis;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_scorch_target";
    NewScript->GetAI = &GetAI_mob_scorch_target;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_iron_construct";
    NewScript->GetAI = &GetAI_mob_iron_construct;
    NewScript->RegisterSelf();
}