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
SDName: Boss_Gehennas
SD%Complete: 80
SDComment: Adds have not to respawn after death
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
    NPC_FLAMEWAKER          =   11661,

	SPELL_SHADOW_BOLT		=	19728,
	SPELL_RAIN_OF_FIRE		=	19717,
	SPELL_GEHENNAS_CURSE	=	19716
};

struct MANGOS_DLL_DECL boss_gehennasAI : public ScriptedAI
{
    boss_gehennasAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiShadowBolt_Timer;
    uint32 uiRainOfFire_Timer;
    uint32 uiGehennasCurse_Timer;

    void Reset()
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_GEHENNAS) != DONE)
			m_pInstance->SetData(TYPE_GEHENNAS, NOT_STARTED);

		uiShadowBolt_Timer = 6000;
		uiRainOfFire_Timer = 10000;
		uiGehennasCurse_Timer = 12000;

        std::list<Creature*> m_lFlamewakers;
        GetCreatureListWithEntryInGrid(m_lFlamewakers,m_creature,NPC_FLAMEWAKER,100.0f);
        if (!m_lFlamewakers.empty())
            for(std::list<Creature*>::iterator iter = m_lFlamewakers.begin(); iter != m_lFlamewakers.end(); ++iter)
                if ((*iter) && (*iter)->isDead())                
                    (*iter)->Respawn();
    }

    void Aggro(Unit* pwho)
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GEHENNAS, IN_PROGRESS);
		m_creature->CallForHelp(50.0f);
	}

	void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GEHENNAS, DONE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiShadowBolt_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCast(pTarget,SPELL_SHADOW_BOLT);

            uiShadowBolt_Timer = 7000;
        }
		else 
			uiShadowBolt_Timer -= uiDiff;

        if (uiRainOfFire_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget,SPELL_RAIN_OF_FIRE);

            uiRainOfFire_Timer = 4000 + rand()%8000;
        }
		else 
			uiRainOfFire_Timer -= uiDiff;

        if (uiGehennasCurse_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_GEHENNAS_CURSE);

            uiGehennasCurse_Timer = 22000 + rand()%8000;
        }
		else 
			uiGehennasCurse_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gehennas(Creature* pCreature)
{
    return new boss_gehennasAI(pCreature);
}

void AddSC_boss_gehennas()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gehennas";
    newscript->GetAI = &GetAI_boss_gehennas;
    newscript->RegisterSelf();
}
