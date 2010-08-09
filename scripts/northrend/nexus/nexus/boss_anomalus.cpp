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
SDName: Boss_Anomalus
SD%Complete: 50%
SDComment: TODO: remove hacks, add support for rift charging
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

enum
{
    SAY_AGGRO                          = -1576006,
    SAY_RIFT                           = -1576007,
    SAY_SHIELD                         = -1576008,
    SAY_KILL                           = -1576009,
    SAY_DEATH                          = -1576010,
    EMOTE_OPEN_RIFT                    = -1576021,
    EMOTE_SHIELD                       = -1576022,

    // Anomalus
    SPELL_CREATE_RIFT                  = 47743,             //Don't work, using WA
    SPELL_CHARGE_RIFT                  = 47747,             //Works wrong (affect players, not rifts)
    SPELL_RIFT_SHIELD                  = 47748,

    SPELL_SPARK                        = 47751,
    SPELL_SPARK_H                      = 57062,

    SPELL_ARCANE_FORM                  = 48019,
    // Chaotic Rift
    SPELL_RIFT_AURA                    = 47687,
    SPELL_RIFT_SUMMON_AURA             = 47732,

    // Charged Chaotic Rift
    SPELL_CHARGED_RIFT_AURA            = 47733,
    SPELL_CHARGED_RIFT_SUMMON_AURA     = 47742,

    SPELL_CHAOTIC_ENERGY_BURST         = 47688,
    SPELL_CHARGED_CHAOTIC_ENERGY_BURST = 47737,
    SPELL_ARCANEFORM                   = 48019,             //Chaotic Rift visual

    SPELL_SUMMON_CRAZED_MANA_WRAITH    = 47692,
    NPC_CHAOTIC_RIFT                   = 26918,
    NPC_CRAZED_MANA_WRAITH             = 26746
};

float RiftLocation[6][3]=
{
    {652.64f, -273.70f, -8.75f},
    {634.45f, -265.94f, -8.44f},
    {620.73f, -281.17f, -9.02f},
    {626.10f, -304.67f, -9.44f},
    {639.87f, -314.11f, -9.49f},
    {651.72f, -297.44f, -9.37f}
};

/*######
## boss_anomalus
######*/

struct MANGOS_DLL_DECL boss_anomalusAI : public ScriptedAI
{
    boss_anomalusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 Phase;
    uint32 SPELL_SPARK_Timer;
    uint32 SPELL_CREATE_RIFT_Timer;
    uint64 ChaoticRiftGUID;

    void Reset()
    {
        Phase = 0;
        SPELL_SPARK_Timer = 5000;
        SPELL_CREATE_RIFT_Timer = 25000;
        ChaoticRiftGUID = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANOMALUS, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if (m_creature->HasAura(SPELL_RIFT_SHIELD))
        {
            Unit* Rift;
            if (ChaoticRiftGUID)
                Rift = Unit::GetUnit((*m_creature), ChaoticRiftGUID);
            if (Rift && Rift->isDead())
            {
                m_creature->RemoveAurasDueToSpell(SPELL_RIFT_SHIELD);
                ChaoticRiftGUID = 0;
            }
            return;
        } else
            ChaoticRiftGUID = 0;

        if (Phase == 0 && m_creature->GetHealthPercent() <= 75.0f)
        {
            Phase = 1;
            DoScriptText(SAY_SHIELD, m_creature);
            DoScriptText(EMOTE_SHIELD, m_creature);
            DoCast(m_creature, SPELL_RIFT_SHIELD);

            int tmp = rand()%(2);
            Creature* Rift = m_creature->SummonCreature(NPC_CHAOTIC_RIFT, RiftLocation[tmp][0], RiftLocation[tmp][1], RiftLocation[tmp][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
            if (Rift)
            {
                DoScriptText(EMOTE_OPEN_RIFT, m_creature);
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    Rift->AI()->AttackStart(target);
                ChaoticRiftGUID = Rift->GetGUID();
                DoScriptText(SAY_RIFT, m_creature);
            }
        }

        if (Phase == 1 && m_creature->GetHealthPercent() <= 50.0f)
        {
            Phase = 2;
            DoScriptText(SAY_SHIELD, m_creature);
            DoScriptText(EMOTE_SHIELD, m_creature);
            DoCast(m_creature, SPELL_RIFT_SHIELD);

            int tmp = rand()%(2);
            Creature* Rift = m_creature->SummonCreature(NPC_CHAOTIC_RIFT, RiftLocation[tmp][0], RiftLocation[tmp][1], RiftLocation[tmp][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
            if (Rift)
            {
                DoScriptText(EMOTE_OPEN_RIFT, m_creature);
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    Rift->AI()->AttackStart(target);
                ChaoticRiftGUID = Rift->GetGUID();
                DoScriptText(SAY_RIFT, m_creature);
            }
        }

        if (Phase == 2 && m_creature->GetHealthPercent() <= 25.0f)
        {
            Phase = 3;
            DoScriptText(SAY_SHIELD, m_creature);
            DoScriptText(EMOTE_SHIELD, m_creature);
            DoCast(m_creature, SPELL_RIFT_SHIELD);

            int tmp = rand()%(2);
            Creature* Rift = m_creature->SummonCreature(NPC_CHAOTIC_RIFT, RiftLocation[tmp][0], RiftLocation[tmp][1], RiftLocation[tmp][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
            if (Rift)
            {
                DoScriptText(EMOTE_OPEN_RIFT, m_creature);
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    Rift->AI()->AttackStart(target);
                ChaoticRiftGUID = Rift->GetGUID();
                DoScriptText(SAY_RIFT, m_creature);
            }
        }

        if (SPELL_SPARK_Timer < diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SPARK : SPELL_SPARK_H);
            SPELL_SPARK_Timer = 5000;
        }else SPELL_SPARK_Timer -=diff;

        if (SPELL_CREATE_RIFT_Timer < diff)
        {
            DoScriptText(SAY_RIFT, m_creature);

            int tmp = rand()%(2);
            Creature* Rift = m_creature->SummonCreature(NPC_CHAOTIC_RIFT, RiftLocation[tmp][0], RiftLocation[tmp][1], RiftLocation[tmp][2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
            if (Rift)
            {
                DoScriptText(EMOTE_OPEN_RIFT, m_creature);
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    Rift->AI()->AttackStart(pTarget);
            }
            SPELL_CREATE_RIFT_Timer = 25000;
        }else SPELL_CREATE_RIFT_Timer -=diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_anomalus(Creature* pCreature)
{
    return new boss_anomalusAI(pCreature);
}

struct MANGOS_DLL_DECL mob_chaotic_riftAI : public Scripted_NoMovementAI
{
    mob_chaotic_riftAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 SPELL_CHAOTIC_ENERGY_BURST_Timer;
    uint32 SUMMON_CRAZED_MANA_WRAITH_Timer;

    void Reset()
    {
        SPELL_CHAOTIC_ENERGY_BURST_Timer = 1000;
        SUMMON_CRAZED_MANA_WRAITH_Timer = 5000;
        m_creature->SetDisplayId(25206); //For some reason in DB models for ally and horde are different.
                                         //Model for ally (1126) does not show auras. Horde model works perfect.
                                         //Set model to horde number
        DoCast(m_creature, SPELL_ARCANEFORM, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SPELL_CHAOTIC_ENERGY_BURST_Timer < diff)
        {
            if (m_pInstance)
            {
                Unit* pAnomalus = Unit::GetUnit(*m_creature, m_pInstance->GetData64(NPC_ANOMALUS));
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (pAnomalus && pAnomalus->HasAura(SPELL_RIFT_SHIELD))
                        DoCast(pTarget, SPELL_CHARGED_CHAOTIC_ENERGY_BURST);
                    else
                        DoCast(pTarget, SPELL_CHAOTIC_ENERGY_BURST);
                SPELL_CHAOTIC_ENERGY_BURST_Timer = 1000;
            }
        }else SPELL_CHAOTIC_ENERGY_BURST_Timer -=diff;

        if (SUMMON_CRAZED_MANA_WRAITH_Timer < diff)
        {
            Creature* pWraith = m_creature->SummonCreature(NPC_CRAZED_MANA_WRAITH, m_creature->GetPositionX()+1, m_creature->GetPositionY()+1, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
            if (pWraith)
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pWraith->AI()->AttackStart(pTarget);
            if (m_pInstance)
            {
                Unit* pAnomalus = Unit::GetUnit(*m_creature, m_pInstance->GetData64(NPC_ANOMALUS));
                if (pAnomalus && pAnomalus->HasAura(SPELL_RIFT_SHIELD))
                    SUMMON_CRAZED_MANA_WRAITH_Timer = 5000;
                else
                    SUMMON_CRAZED_MANA_WRAITH_Timer = 10000;
            }
        }else SUMMON_CRAZED_MANA_WRAITH_Timer -=diff;
    }
};

CreatureAI* GetAI_mob_chaotic_rift(Creature* pCreature)
{
    return new mob_chaotic_riftAI(pCreature);
}

void AddSC_boss_anomalus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_anomalus";
    newscript->GetAI = &GetAI_boss_anomalus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_chaotic_rift";
    newscript->GetAI = &GetAI_mob_chaotic_rift;
    newscript->RegisterSelf();
}
