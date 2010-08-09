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
SDName: Boss_Lucifron
SD%Complete: 80
SDComment: Adds NYI (Mindcontrol)
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
    NPC_FLAMEWAKER_PROTECTOR=   12119,

	SPELL_IMPENDING_DOOM	=	19702,
	SPELL_LUCIFRON_CURSE	=	19703,
	SPELL_SHADOW_SHOCK		=	19460
};

struct MANGOS_DLL_DECL boss_lucifronAI : public ScriptedAI
{
    boss_lucifronAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiImpendingDoom_Timer;
    uint32 uiLucifronCurse_Timer;
    uint32 uiShadowShock_Timer;

    void Reset()
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_LUCIFRON) != DONE)
			m_pInstance->SetData(TYPE_LUCIFRON, NOT_STARTED);

		uiImpendingDoom_Timer = 10000;        //Initial cast after 10 seconds so the debuffs alternate
		uiLucifronCurse_Timer = 20000;        //Initial cast after 20 seconds
		uiShadowShock_Timer = 6000;           //6 seconds

        std::list<Creature*> m_lFlamewakers;
        GetCreatureListWithEntryInGrid(m_lFlamewakers,m_creature,NPC_FLAMEWAKER_PROTECTOR,100.0f);
        if (!m_lFlamewakers.empty())
            for(std::list<Creature*>::iterator iter = m_lFlamewakers.begin(); iter != m_lFlamewakers.end(); ++iter)
                if ((*iter))
                {
                    if ((*iter)->isDead())
                        (*iter)->Respawn();
                    (*iter)->GetMotionMaster()->MoveFollow(m_creature,3.0f,0);
                }
    }

    void Aggro(Unit* pWho)
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_LUCIFRON, IN_PROGRESS);
        m_creature->CallForHelp(20.0f);        
    }

	void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_LUCIFRON, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || !m_pInstance)
            return;

        if (uiImpendingDoom_Timer < diff)
        {
            DoCast(m_creature,SPELL_IMPENDING_DOOM);

            uiImpendingDoom_Timer = 20000;
        }
		else 
			uiImpendingDoom_Timer -= diff;

        if (uiLucifronCurse_Timer < diff)
        {
            DoCast(m_creature,SPELL_LUCIFRON_CURSE);

            uiLucifronCurse_Timer = 15000;
        }
		else 
			uiLucifronCurse_Timer -= diff;

        if (uiShadowShock_Timer < diff)
        {
			if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))                
				DoCast(pTarget,SPELL_SHADOW_SHOCK);

            uiShadowShock_Timer = 6000;
        }
		else 
			uiShadowShock_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lucifron(Creature* pCreature)
{
    return new boss_lucifronAI(pCreature);
}

void AddSC_boss_lucifron()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_lucifron";
    newscript->GetAI = &GetAI_boss_lucifron;
    newscript->RegisterSelf();
}
