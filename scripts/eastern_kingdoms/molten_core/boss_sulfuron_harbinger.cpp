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
SDName: Boss_Sulfuron_Harbringer
SD%Complete: 95
SDComment: Hand of Ragnaros - Knockback not working
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
    NPC_FLAMEWAKER_PRIEST       =   11662,

	SPELL_DARK_STRIKE			=	19777,
	SPELL_DEMORALIZING_SHOUT	=	19778,
	SPELL_INSPIRE				=	19779,
	SPELL_HAND_OF_RAGNAROS		=	19780,
	SPELL_FLAME_SPEAR			=	19781
};

struct MANGOS_DLL_DECL boss_sulfuronAI : public ScriptedAI
{
    boss_sulfuronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiDemoralizingShout_Timer;
    uint32 uiInspire_Timer;
    uint32 uiKnockdown_Timer;
    uint32 uiFlamespear_Timer;

    void Reset()
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_SULFURON) != DONE)
			m_pInstance->SetData(TYPE_SULFURON, NOT_STARTED);

		uiDemoralizingShout_Timer = 15000;
		uiInspire_Timer = 13000;
		uiKnockdown_Timer = 6000;
		uiFlamespear_Timer = 2000;

        std::list<Creature*> m_lFlamewakers;
        GetCreatureListWithEntryInGrid(m_lFlamewakers,m_creature,NPC_FLAMEWAKER_PRIEST,100.0f);
        if (!m_lFlamewakers.empty())
            for(std::list<Creature*>::iterator iter = m_lFlamewakers.begin(); iter != m_lFlamewakers.end(); ++iter)
                if ((*iter) && (*iter)->isDead())                
                    (*iter)->Respawn();
    }

    void Aggro(Unit* pWho)
	{
		if(m_pInstance)
			m_pInstance->SetData(TYPE_SULFURON, IN_PROGRESS);
	}

	void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_SULFURON, DONE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiDemoralizingShout_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_DEMORALIZING_SHOUT);

            uiDemoralizingShout_Timer = 15000 + rand()%5000;
        }
		else 
			uiDemoralizingShout_Timer -= uiDiff;

        if (uiInspire_Timer < uiDiff)
        {
            Creature* pTarget = NULL;
            std::list<Creature*> pList = DoFindFriendlyMissingBuff(45.0f,SPELL_INSPIRE);

            if (!pList.empty())
            {
                std::list<Creature*>::iterator i = pList.begin();
                advance(i, (rand()%pList.size()));
                pTarget = (*i);
            }

            if (pTarget)
				DoCast(pTarget,SPELL_INSPIRE);

            DoCastSpellIfCan(m_creature,SPELL_INSPIRE);

            uiInspire_Timer = 20000 + rand()%6000;
        }
		else 
			uiInspire_Timer -= uiDiff;

        if (uiKnockdown_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_HAND_OF_RAGNAROS);

            uiKnockdown_Timer = urand(12000, 15000);
        }
		else 
			uiKnockdown_Timer -= uiDiff;

        if (uiFlamespear_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(pTarget,SPELL_FLAME_SPEAR);

            uiFlamespear_Timer = 12000 + rand()%4000;
        }
		else 
			uiFlamespear_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

enum
{	
	//Adds Spells
	SPELL_HEAL					=	19775,
	SPELL_SHADOW_WORD_PAIN		=	19776,
	SPELL_IMMOLATE				=	20294
};

struct MANGOS_DLL_DECL mob_flamewaker_priestAI : public ScriptedAI
{
    mob_flamewaker_priestAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiHeal_Timer;
    uint32 uiShadowWordPain_Timer;
    uint32 uiImmolate_Timer;
	uint32 uiDarkstrike_Timer;

    void Reset()
    {
		uiHeal_Timer = 15000 + rand()%15000;     
        uiShadowWordPain_Timer = 2000;
        uiImmolate_Timer = 8000;
		uiDarkstrike_Timer=10000;       //These times are probably wrong
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Casting Heal on Sulfuron or other Guards if target is in range
        if (uiHeal_Timer < uiDiff)
        {
			Unit* pFriendlyUnit = DoSelectLowestHpFriendly(60.0f,1);
			if (pFriendlyUnit && m_creature->IsWithinDistInMap(pFriendlyUnit,30.0f))
				DoCast(pFriendlyUnit,SPELL_HEAL);
                       
            uiHeal_Timer = 15000 + rand()%5000;
        }
		else 
			uiHeal_Timer -= uiDiff;

        if (uiShadowWordPain_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(pTarget,SPELL_SHADOW_WORD_PAIN);

            uiShadowWordPain_Timer = 18000 + rand()%8000;
        }
		else
			uiShadowWordPain_Timer -= uiDiff;

        if (uiImmolate_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(pTarget,SPELL_IMMOLATE);

            uiImmolate_Timer = 15000 + rand()%10000;
        }
		else 
			uiImmolate_Timer -= uiDiff;

        if (uiDarkstrike_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_DARK_STRIKE);

            uiDarkstrike_Timer = 15000 + rand()%3000;
        } 
		else
			uiDarkstrike_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sulfuron(Creature* pCreature)
{
    return new boss_sulfuronAI(pCreature);
}

CreatureAI* GetAI_mob_flamewaker_priest(Creature* pCreature)
{
    return new mob_flamewaker_priestAI(pCreature);
}

void AddSC_boss_sulfuron()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_sulfuron";
    newscript->GetAI = &GetAI_boss_sulfuron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_flamewaker_priest";
    newscript->GetAI = &GetAI_mob_flamewaker_priest;
    newscript->RegisterSelf();
}
