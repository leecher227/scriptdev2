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
SDName: Boss_Magmadar
SD%Complete: 90
SDComment: Lava Bomb not working
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
    EMOTE_GENERIC_FRENZY_KILL   = -1000001,

	SPELL_FRENZY			=	19451,
	SPELL_MAGMA_SPIT_AURA	=	19449,		//applied at the beginning
	SPELL_PANIC				=	19408,
	SPELL_LAVA_BOMB			=	20474		//not working -> no effect on target
};

struct MANGOS_DLL_DECL boss_magmadarAI : public ScriptedAI
{
    boss_magmadarAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiFrenzy_Timer;
    uint32 uiPanic_Timer;
    uint32 uiLavabomb_Timer;	

    void Reset()
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_MAGMADAR) != DONE)
			m_pInstance->SetData(TYPE_MAGMADAR, NOT_STARTED);

		uiFrenzy_Timer = 15000;
		uiPanic_Timer = 20000;
		uiLavabomb_Timer = 12000;
		m_creature->RemoveAurasDueToSpell(SPELL_MAGMA_SPIT_AURA);
        DoCast(m_creature,SPELL_MAGMA_SPIT_AURA);		
    }

    void Aggro(Unit* pWho)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_MAGMADAR, IN_PROGRESS);
    }

	void JustDied(Unit* Killer)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_MAGMADAR, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || !m_pInstance)
            return;		

        if (uiFrenzy_Timer < diff)
        {
            DoScriptText(EMOTE_GENERIC_FRENZY_KILL, m_creature);
            DoCastSpellIfCan(m_creature,SPELL_FRENZY);
            uiFrenzy_Timer = 15000;
        }
		else 
			uiFrenzy_Timer -= diff;

        if (uiPanic_Timer < diff)
        {
            DoCast(m_creature,SPELL_PANIC);
            uiPanic_Timer = 35000;
        }
		else 
			uiPanic_Timer -= diff;

        //Lavabomb_Timer
        if (uiLavabomb_Timer < diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget,SPELL_LAVA_BOMB);

            uiLavabomb_Timer = 12000;
        }
		else 
			uiLavabomb_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magmadar(Creature* pCreature)
{
    return new boss_magmadarAI(pCreature);
}

void AddSC_boss_magmadar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_magmadar";
    newscript->GetAI = &GetAI_boss_magmadar;
    newscript->RegisterSelf();
}
