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
SDName: Boss_Shazzrah
SD%Complete: 100
SDComment: Counter Spell not working
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
	SPELL_ARCANE_EXPLOSION		=	19712,
	SPELL_SHAZZRAH_CURSE		=	19713,
	SPELL_MAGIC_GROUNDING		=	19714,
	SPELL_COUNTER_SPELL			=	19715,
	SPELL_GATE_OF_SHAZZRAH		=	23138
};

struct MANGOS_DLL_DECL boss_shazzrahAI : public ScriptedAI
{
    boss_shazzrahAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiArcaneExplosion_Timer;
    uint32 uiShazzrahCurse_Timer;
    uint32 uiMagicGrounding_Timer;
    uint32 uiCountspell_Timer;
    uint32 uiGateOfShazzrah_Timer;

    void Reset()
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_SHAZZRAH, NOT_STARTED);

		//These times are probably wrong
		uiArcaneExplosion_Timer	= 6000;
		uiShazzrahCurse_Timer	= 10000;
		uiMagicGrounding_Timer	= 24000;
		uiCountspell_Timer		= 15000;
		uiGateOfShazzrah_Timer	= 45000;
    }

    void Aggro(Unit* pWho)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_SHAZZRAH, IN_PROGRESS);
    }

	void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_SHAZZRAH, DONE);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || !m_pInstance)
            return;

        if (uiArcaneExplosion_Timer < diff)
        {
            DoCast(m_creature,SPELL_ARCANE_EXPLOSION);

            uiArcaneExplosion_Timer = 5000 + rand()%4000;
        }
		else 
			uiArcaneExplosion_Timer -= diff;

        if (uiShazzrahCurse_Timer < diff)
        {
			DoCast(m_creature,SPELL_SHAZZRAH_CURSE);

            uiShazzrahCurse_Timer = 25000 + rand()%5000;
        }
		else 
			uiShazzrahCurse_Timer -= diff;

        if (uiMagicGrounding_Timer < diff)
        {
            DoCast(m_creature,SPELL_MAGIC_GROUNDING);

            uiMagicGrounding_Timer = 35000;
        }
		else 
			uiMagicGrounding_Timer -= diff;

        if (uiCountspell_Timer < diff)
        {
            DoCast(m_creature,SPELL_COUNTER_SPELL);

            uiCountspell_Timer = 16000 + rand()%4000;
        }
		else 
			uiCountspell_Timer -= diff;

        if (uiGateOfShazzrah_Timer < diff)
        {
			DoCast(m_creature,SPELL_GATE_OF_SHAZZRAH);
            
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
			{
				m_creature->NearTeleportTo(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), m_creature->GetOrientation());
				DoResetThreat();
			}
            uiGateOfShazzrah_Timer = 45000;
        }
		else 
			uiGateOfShazzrah_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shazzrah(Creature* pCreature)
{
    return new boss_shazzrahAI(pCreature);
}

void AddSC_boss_shazzrah()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_shazzrah";
    newscript->GetAI = &GetAI_boss_shazzrah;
    newscript->RegisterSelf();
}
