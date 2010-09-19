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
SDName: Boss_Ormorok
SD%Complete: 50%
SDComment: TODO: Correct timers. Research how spikes work, and attempt code it properly from mangos side.
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

enum
{
    SAY_AGGRO                   = -1576011,
    SAY_KILL                    = -1576012,
    SAY_REFLECT                 = -1576013,
    SAY_ICESPIKE                = -1576014,
    SAY_DEATH                   = -1576015,
    EMOTE_BOSS_GENERIC_FRENZY   = -1000005,

    SPELL_REFLECTION            = 47981,

    SPELL_CRYSTAL_SPIKES        = 47958,
    SPELL_CRYSTAL_SPIKES_H1     = 57082,
    SPELL_CRYSTAL_SPIKES_H2     = 57083,

    //Walkaround for spells Crystal Spikes --
    SPELL_CRYSTALL_SPIKE_DAMAGE_N   = 47944,
    SPELL_CRYSTALL_SPIKE_DAMAGE_H   = 57067,
    SPELL_CRYSTAL_SPIKE_PREVISUAL   = 50442,
    MOB_CRYSTAL_SPIKE               = 27099,
    //---------------------------------------

    SPELL_FRENZY                = 48017,
    SPELL_FRENZY_H              = 57086,

    SPELL_TRAMPLE               = 48016,
    SPELL_TRAMPLE_H             = 57066,

    SPELL_SUMMON_TANGLER_H      = 61564,

    MOB_CRYSTALLINE_TANGLER     = 32665,
    
    SPELL_ROOTS                 = 28858                    //proper spell id is unknown
};

#define SPIKE_DISTANCE          5.0f

/*######
## boss_ormorok
######*/

struct MANGOS_DLL_DECL boss_ormorokAI : public ScriptedAI
{
    boss_ormorokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsEnraged;

    bool m_bCrystalSpikes;
    uint8 CrystalSpikes_Count;
    float BaseX;
    float BaseY;
    float BaseZ;
    float BaseO;
    float SpikeXY[4][2];

    uint32 CRYSTAL_SPIKES_Timer;
    uint32 m_uiTrampleTimer;
    uint32 m_uiSpellReflectTimer;
    uint32 m_uiCrystalSpikeTimer;
    uint32 m_uiTanglerTimer;
    uint32 m_uiEvadeCheckCooldown;

    void Reset()
    {
        m_bIsEnraged = false;

        m_uiTrampleTimer = urand(10000, 35000);
        m_uiSpellReflectTimer = urand(5000, 10000);
        m_uiCrystalSpikeTimer = urand(15000, 30000);
        m_uiTanglerTimer = 20000;

        m_bCrystalSpikes = false;
        m_uiEvadeCheckCooldown = 2000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ORMOROK, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = NULL;
        uint8 uiHealer = 0;
        for (uint8 j = 1; j <= 4; ++j)
        {
            switch (j)
            {
                case 1: uiHealer = CLASS_PRIEST; break;
                case 2: uiHealer = CLASS_PALADIN; break;
                case 3: uiHealer = CLASS_DRUID; break;
                case 4: uiHealer = CLASS_SHAMAN; break;
            }
            ThreatList const& lThreatList = m_creature->getThreatManager().getThreatList();
            for (ThreatList::const_iterator i = lThreatList.begin(); i != lThreatList.end(); ++i)
            {
                Unit* pTemp = m_creature->GetMap()->GetUnit((*i)->getUnitGuid());
                if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER && pTemp->getClass() == uiHealer)
                {
                    pTarget = pTemp;
                    break;
                }
            }
            if (pTarget)
                break;
        }
        if (!pTarget)
            if (pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            if (m_creature->GetDistance2d(265.0f, -225.42f) > 80.0f)
                EnterEvadeMode();
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_bCrystalSpikes)
            if (CRYSTAL_SPIKES_Timer < uiDiff)
            {
                SpikeXY[0][0] = BaseX+(SPIKE_DISTANCE*CrystalSpikes_Count*cos(BaseO));
                SpikeXY[0][1] = BaseY+(SPIKE_DISTANCE*CrystalSpikes_Count*sin(BaseO));
                SpikeXY[1][0] = BaseX-(SPIKE_DISTANCE*CrystalSpikes_Count*cos(BaseO));
                SpikeXY[1][1] = BaseY-(SPIKE_DISTANCE*CrystalSpikes_Count*sin(BaseO));
                SpikeXY[2][0] = BaseX+(SPIKE_DISTANCE*CrystalSpikes_Count*cos(BaseO-(M_PI/2)));
                SpikeXY[2][1] = BaseY+(SPIKE_DISTANCE*CrystalSpikes_Count*sin(BaseO-(M_PI/2)));
                SpikeXY[3][0] = BaseX-(SPIKE_DISTANCE*CrystalSpikes_Count*cos(BaseO-(M_PI/2)));
                SpikeXY[3][1] = BaseY-(SPIKE_DISTANCE*CrystalSpikes_Count*sin(BaseO-(M_PI/2)));
                for (uint8 i = 0; i < 4; i++)
                    m_creature->SummonCreature(MOB_CRYSTAL_SPIKE, SpikeXY[i][0], SpikeXY[i][1], BaseZ, 0, TEMPSUMMON_TIMED_DESPAWN, 7000);
                if (++CrystalSpikes_Count >= 13)
                    m_bCrystalSpikes = false;
                CRYSTAL_SPIKES_Timer = 200;
            }else CRYSTAL_SPIKES_Timer -= uiDiff;

        if (!m_bIsEnraged && m_creature->GetHealthPercent() <= 25.0f)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                m_bIsEnraged = true;
                DoScriptText(EMOTE_BOSS_GENERIC_FRENZY, m_creature);
                DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FRENZY : SPELL_FRENZY_H);
            }
        }

        if (m_uiTrampleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_TRAMPLE : SPELL_TRAMPLE_H);
            m_uiTrampleTimer = urand(10000, 35000);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        if (m_uiSpellReflectTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_REFLECTION);
            m_uiSpellReflectTimer = urand(25000, 40000);
        }
        else
            m_uiSpellReflectTimer -= uiDiff;

        if (m_uiCrystalSpikeTimer < uiDiff)
        {
            DoScriptText(SAY_ICESPIKE, m_creature);
            m_bCrystalSpikes = true;
            CrystalSpikes_Count = 1;
            CRYSTAL_SPIKES_Timer = 0;
            BaseX = m_creature->GetPositionX();
            BaseY = m_creature->GetPositionY();
            BaseZ = m_creature->GetPositionZ();
            BaseO = m_creature->GetOrientation();
            m_uiCrystalSpikeTimer = urand(15000, 30000);
        }
        else
            m_uiCrystalSpikeTimer -= uiDiff;

        if (!m_bIsRegularMode)
        {
            if (m_uiTanglerTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_SUMMON_TANGLER_H);
                m_uiTanglerTimer = urand(15000, 25000);
            }
            else
                m_uiTanglerTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }

};

struct MANGOS_DLL_DECL mob_crystal_spikeAI : public Scripted_NoMovementAI
{
    mob_crystal_spikeAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 SPELL_CRYSTALL_SPIKE_DAMAGE_Timer;
    uint32 SPELL_CRYSTAL_SPIKE_PREVISUAL_Timer;

    void Reset()
    {
        SPELL_CRYSTALL_SPIKE_DAMAGE_Timer = 3700;
        SPELL_CRYSTAL_SPIKE_PREVISUAL_Timer = 1000;
        m_creature->SetLevel(80);                                        //
        m_creature->setFaction(16);                                      //Walkaround to be independent from data in DB
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); //
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); //
    }

    void UpdateAI(const uint32 diff) 
    {
        if (SPELL_CRYSTAL_SPIKE_PREVISUAL_Timer < diff)
        {
            DoCast(m_creature, SPELL_CRYSTAL_SPIKE_PREVISUAL);
            SPELL_CRYSTAL_SPIKE_PREVISUAL_Timer = 10000;
        }
        else
            SPELL_CRYSTAL_SPIKE_PREVISUAL_Timer -=diff;

        if (SPELL_CRYSTALL_SPIKE_DAMAGE_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_CRYSTALL_SPIKE_DAMAGE_N : SPELL_CRYSTALL_SPIKE_DAMAGE_H);
            SPELL_CRYSTALL_SPIKE_DAMAGE_Timer = 10000;
        }
        else
            SPELL_CRYSTALL_SPIKE_DAMAGE_Timer -=diff;
    } 
}; 

struct MANGOS_DLL_DECL mob_crystalline_tanglerAI : public ScriptedAI
{
    mob_crystalline_tanglerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 SPELL_ROOTS_Timer;

    void Reset()
    {
        SPELL_ROOTS_Timer = 1000;
    }

    void UpdateAI(const uint32 diff) 
    {
        if (SPELL_ROOTS_Timer < diff)
        {
            Unit* Victim = m_creature->getVictim();
            if (Victim && m_creature->IsWithinDist(Victim, 5.0f, false))
            {
                DoCast(m_creature->getVictim(), SPELL_ROOTS);
                SPELL_ROOTS_Timer = 15000;
            }
        }
        else
            SPELL_ROOTS_Timer -=diff;
    } 
}; 

CreatureAI* GetAI_mob_crystal_spike(Creature* pCreature)
{
    return new mob_crystal_spikeAI(pCreature);
}

CreatureAI* GetAI_mob_crystalline_tangler(Creature* pCreature)
{
    return new mob_crystalline_tanglerAI(pCreature);
}

CreatureAI* GetAI_boss_ormorok(Creature* pCreature)
{
    return new boss_ormorokAI(pCreature);
}

void AddSC_boss_ormorok()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ormorok";
    newscript->GetAI = &GetAI_boss_ormorok;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_crystal_spike";
    newscript->GetAI = &GetAI_mob_crystal_spike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_crystalline_tangler";
    newscript->GetAI = &GetAI_mob_crystalline_tangler;
    newscript->RegisterSelf();
}
