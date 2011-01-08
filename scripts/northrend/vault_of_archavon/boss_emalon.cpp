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
SDName: boss_emalon
SD%Complete: 0
SDComment: Placeholder
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

enum
{
    // Emalon spells
    SPELL_CHAIN_LIGHTNING_N                 = 64213,
    SPELL_CHAIN_LIGHTNING_H                 = 64215,
    SPELL_LIGHTNING_NOVA_N                  = 64216,
    SPELL_LIGHTNING_NOVA_H                  = 65279,
    SPELL_OVERCHARGE                        = 64379,        //This spell is used by Time Warder, and temporary by Emalon, because 64218 is bugged
    SPELL_BERSERK                           = 26662,

    // Tempest Minion spells
    SPELL_SHOCK                             = 64363,
    SPELL_OVERCHARGED_BLAST                 = 64219,
    SPELL_OVERCHARGED                       = 64217
};

/*######
## npc_tempest_minion
######*/

struct MANGOS_DLL_DECL npc_tempest_minionAI : public ScriptedAI
{
    npc_tempest_minionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiEvadeCheckCooldown;

    uint32 m_uiShockTimer;
    uint32 m_uiOverchargedStacksCheckTimer;
    bool m_bTimeToDie;

    void Reset()
    {
        m_uiEvadeCheckCooldown = 2000;
        m_uiShockTimer = urand(8000, 12000);
        m_bTimeToDie = false;
        m_uiOverchargedStacksCheckTimer = 2000;
    }

    void Aggro(Unit* pWho)
    {
        m_creature->CallForHelp(80.0f);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            Creature* pEmalon = NULL;
            if (m_pInstance)
                pEmalon = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_EMALON));
            if ((pEmalon && pEmalon->IsInEvadeMode()) || (m_creature->GetDistance2d(-219.119f, -289.037f) > 80.0f))
            {
                EnterEvadeMode();
                return;
            }
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_bTimeToDie)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            return;
        }

        if (m_uiOverchargedStacksCheckTimer < uiDiff)
        {
            m_uiOverchargedStacksCheckTimer = 2000;
            Aura* pAuraOvercharged = m_creature->GetAura(SPELL_OVERCHARGED, EFFECT_INDEX_0);
            if (pAuraOvercharged && pAuraOvercharged->GetStackAmount() >= 10)
            {
                DoCast(m_creature, SPELL_OVERCHARGED_BLAST);
                m_bTimeToDie = true;
                return;
            }
        }
        else
            m_uiOverchargedStacksCheckTimer -= uiDiff;

        if (m_uiShockTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SHOCK);
            m_uiShockTimer = urand(8000, 12000);
        }
        else
            m_uiShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};


/*######
## boss_emalon
######*/

struct MANGOS_DLL_DECL boss_emalonAI : public ScriptedAI
{
    boss_emalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiEvadeCheckCooldown;

    uint64 m_auiTempestMinionGUID[4];
    uint32 m_uiChainLightningTimer;
    uint32 m_uiChainLightningCount;
    uint32 m_uiLightningNovaTimer;
    uint32 m_uiOverchargeTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiReviveAddTimer;

    void Reset()
    {
        m_uiEvadeCheckCooldown = 2000;
        memset(&m_auiTempestMinionGUID, 0, sizeof(m_auiTempestMinionGUID));
        m_uiChainLightningTimer = 15000;
        m_uiChainLightningCount = 0;
        m_uiLightningNovaTimer = 20000;
        m_uiOverchargeTimer = 45000;
        m_uiEnrageTimer = 360000;
        m_uiReviveAddTimer = 4000;

        if (m_pInstance)
        {
            m_auiTempestMinionGUID[0] = m_pInstance->GetData64(DATA_TEMPEST_MINION_1);
            m_auiTempestMinionGUID[1] = m_pInstance->GetData64(DATA_TEMPEST_MINION_2);
            m_auiTempestMinionGUID[2] = m_pInstance->GetData64(DATA_TEMPEST_MINION_3);
            m_auiTempestMinionGUID[3] = m_pInstance->GetData64(DATA_TEMPEST_MINION_4);
        }

        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = m_creature->GetMap()->GetCreature(m_auiTempestMinionGUID[i]);
            if (pMinion && pMinion->isDead())
                pMinion->Respawn();
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            m_auiTempestMinionGUID[0] = m_pInstance->GetData64(DATA_TEMPEST_MINION_1);
            m_auiTempestMinionGUID[1] = m_pInstance->GetData64(DATA_TEMPEST_MINION_2);
            m_auiTempestMinionGUID[2] = m_pInstance->GetData64(DATA_TEMPEST_MINION_3);
            m_auiTempestMinionGUID[3] = m_pInstance->GetData64(DATA_TEMPEST_MINION_4);
        }

        m_creature->CallForHelp(80.0f);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, DONE);
        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = m_creature->GetMap()->GetCreature(m_auiTempestMinionGUID[i]);
            if (pMinion)
                pMinion->DealDamage(pMinion, pMinion->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            if (m_creature->GetDistance2d(-219.119f, -289.037f) > 80.0f)
                EnterEvadeMode();
            m_creature->CallForHelp(80.0f);
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_uiReviveAddTimer < uiDiff)
        {
            for (uint8 i=0; i<4; ++i)
            {
                Creature* pMinion = m_creature->GetMap()->GetCreature(m_auiTempestMinionGUID[i]);
                if (pMinion && pMinion->isDead())
                {
                    pMinion->Respawn();
                    pMinion->SetInCombatWithZone();
                    m_creature->MonsterTextEmote("Tempest Minion appears to defend Emalon!", 0, true);
                }
            }
            m_uiReviveAddTimer = 4000;
        }
        else
            m_uiReviveAddTimer -= uiDiff;

        if (m_uiOverchargeTimer < uiDiff)
        {
            Creature* pMinion = m_creature->GetMap()->GetCreature(m_auiTempestMinionGUID[urand(0, 3)]);
            if(pMinion && pMinion->isAlive())
            {
                m_creature->MonsterTextEmote("%s overcharges Tempest Minion!", 0, true);
                pMinion->SetHealth(pMinion->GetMaxHealth());
                pMinion->CastSpell(pMinion, SPELL_OVERCHARGE, false);
            }
            m_uiOverchargeTimer = 45000;
        }
        else
            m_uiOverchargeTimer -= uiDiff;

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING_N : SPELL_CHAIN_LIGHTNING_H);
            m_uiChainLightningTimer = 10000 + rand()%15000;
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (m_uiLightningNovaTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_NOVA_N : SPELL_LIGHTNING_NOVA_H);
            m_uiLightningNovaTimer = 45000;
        }
        else
            m_uiLightningNovaTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiEnrageTimer = 30000;
        }
        else
            m_uiEnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_tempest_warder
######*/

struct MANGOS_DLL_DECL npc_tempest_warderAI : public ScriptedAI
{
    npc_tempest_warderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiShockTimer;
    bool m_bOvercharged;
    uint32 m_uiOverchargedStacksCheckTimer;
    bool m_bTimeToDie;

    void Reset()
    {
        m_uiShockTimer = urand(8000, 12000);
        m_bOvercharged = false;
        uint32 m_uiOverchargedStacksCheckTimer = 2000;
        m_bTimeToDie = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {  
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bTimeToDie)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            return;
        }

        if (!m_bOvercharged && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 37))
        {
            DoCast(m_creature, SPELL_OVERCHARGE);
            m_bOvercharged = true;
        }

        if (m_bOvercharged)
        {
            if (m_uiOverchargedStacksCheckTimer < uiDiff)
            {
                m_uiOverchargedStacksCheckTimer = 2000;
                Aura* pAuraOvercharged = m_creature->GetAura(SPELL_OVERCHARGED, EFFECT_INDEX_0);
                if(pAuraOvercharged && pAuraOvercharged->GetStackAmount() >= 10)
                {
                    DoCast(m_creature, SPELL_OVERCHARGED_BLAST);
                    m_bTimeToDie = true;
                    return;
                }
            }
            else
                m_uiOverchargedStacksCheckTimer -= uiDiff;
        }

        if (m_uiShockTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SHOCK);
            m_uiShockTimer = urand(8000, 12000);
        }
        else
            m_uiShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_emalonAI(Creature* pCreature)
{
    return new boss_emalonAI(pCreature);
}

CreatureAI* GetAI_npc_tempest_minionAI(Creature* pCreature)
{
    return new npc_tempest_minionAI(pCreature);
}

CreatureAI* GetAI_npc_tempest_warderAI(Creature* pCreature)
{
    return new npc_tempest_warderAI(pCreature);
}

void AddSC_boss_emalon()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_emalon";
    pNewScript->GetAI = &GetAI_boss_emalonAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tempest_minion";
    pNewScript->GetAI = &GetAI_npc_tempest_minionAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tempest_warder";
    pNewScript->GetAI = &GetAI_npc_tempest_warderAI;
    pNewScript->RegisterSelf();
}
