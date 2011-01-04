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
SDName: Boss_Baron_Geddon
SD%Complete: 80
SDComment: Test it
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

#include "WorldPacket.h"
#include "Spell.h"
enum
{
	EMOTE_SERVICE		=	-1409000,

	SPELL_INFERNO		=	19695,	//he can't move
	SPELL_INFERNO_BLAST	=	38728,

	SPELL_IGNITE_MANA	=	19659,	//35 yard radius	
	SPELL_LIVING_BOMB	=	20475,
	SPELL_ARMAGEDDON	=	20479,	//at 2% health there is the emote and after 5 seconds he casts this spell
};

struct MANGOS_DLL_DECL boss_baron_geddonAI : public ScriptedAI
{
    boss_baron_geddonAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}

	ScriptedInstance* m_pInstance;

    uint32 uiInferno_Timer;
    uint32 uiInfernoBlast_Timer;
	uint32 uiInfernoBlast_Count;
    uint32 uiIgniteMana_Timer;
    uint32 uiLivingBomb_Timer;
	uint32 uiArmageddon_Timer;

	bool bHasPerformedService;

    void Reset()
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_GEDDON) != DONE)
			m_pInstance->SetData(TYPE_GEDDON, NOT_STARTED);

		uiInferno_Timer = 45000;      //These times are probably wrong
		uiIgniteMana_Timer = 30000;
		uiLivingBomb_Timer = 35000;
		uiArmageddon_Timer = 5000;
		uiInfernoBlast_Timer = 1000;
		
		uiInfernoBlast_Count = 9;

		bHasPerformedService = false;
		
    }

    void Aggro(Unit* pWho)
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GEDDON, IN_PROGRESS);
	}

	void JustDied(Unit* pKiller)
    {	
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GEDDON, DONE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //If geddon's hp below 2% , cast Ammageddon
        if (m_creature->GetHealthPercent() <= 2.0f)
        {
            m_creature->InterruptNonMeleeSpells(true);
			m_creature->StopMoving();
			if (!bHasPerformedService)
			{
				DoScriptText(EMOTE_SERVICE, m_creature);
				bHasPerformedService = true;
			}

			if (uiArmageddon_Timer < uiDiff)
			{
				DoCast(m_creature,SPELL_ARMAGEDDON);
				uiArmageddon_Timer = 5000;
			}
			else 
				uiArmageddon_Timer -= uiDiff;
            
            return;
        }

        if (uiInferno_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_INFERNO);
			DoCast(m_creature, SPELL_INFERNO_BLAST);
            uiInferno_Timer = 45000;
			uiInfernoBlast_Count = 1;
        }
		else 
			uiInferno_Timer -= uiDiff;
	
        if(uiInfernoBlast_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_INFERNO_BLAST);

            uiInfernoBlast_Timer = 1000;
			++uiInfernoBlast_Count;
        }
		else if (uiInfernoBlast_Count < 9)
		    uiInfernoBlast_Timer -= uiDiff;

        if (uiIgniteMana_Timer < uiDiff)
        {
			DoCast(m_creature, SPELL_IGNITE_MANA);

            uiIgniteMana_Timer = 30000;
        }
		else 
			uiIgniteMana_Timer -= uiDiff;

        if (uiLivingBomb_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
			    DoCast(pTarget, SPELL_LIVING_BOMB, true);
            uiLivingBomb_Timer = 30000;
		}
		else 
			uiLivingBomb_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_baron_geddon(Creature* pCreature)
{
    return new boss_baron_geddonAI(pCreature);
}

void AddSC_boss_baron_geddon()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_baron_geddon";
    newscript->GetAI = &GetAI_boss_baron_geddon;
    newscript->RegisterSelf();
}
