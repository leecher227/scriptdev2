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
SDName: Boss_Golemagg
SD%Complete: 80
SDComment: Trust doesn not work - players get the buff oO
UPDATE `creature_template` SET `spell1` = 0, `spell2` = 0 WHERE `entry` = 11672;
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
    NPC_CORE_RAGER          =   11672,
	EMOTE_GOLEMAGG_HELP		=	-1409002,

	SPELL_MAGMA_SPLASH_AURA	=	13879,
	SPELL_PYRO_BLAST		=	20228,
	SPELL_EARTH_QUAKE		=	19798,

	//CoreRager Spells
	SPELL_MANGLE			=	19820,
	SPELL_GOLEMAGG_TRUST	=	20553	//if golemagg is in rage 
};

struct MANGOS_DLL_DECL boss_golemaggAI : public ScriptedAI
{
    boss_golemaggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiPyroblast_Timer;
    uint32 uiEarthQuake_Timer;

	bool bHasPerformedAggro;	

    void Reset()
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GOLEMAGG, NOT_STARTED);

		uiPyroblast_Timer = 7000;      //These times are probably wrong
		uiEarthQuake_Timer = 3000;

		bHasPerformedAggro = false;
		m_creature->RemoveAurasDueToSpell(SPELL_MAGMA_SPLASH_AURA);
        DoCast(m_creature,SPELL_MAGMA_SPLASH_AURA);

        //shouldn't happen they die and you wipe but still added
        std::list<Creature*> m_lCoreragers;
        GetCreatureListWithEntryInGrid(m_lCoreragers,m_creature,NPC_CORE_RAGER,100.0f);
        if (!m_lCoreragers.empty())
            for(std::list<Creature*>::iterator iter = m_lCoreragers.begin(); iter != m_lCoreragers.end(); ++iter)
                if ((*iter) && (*iter)->isDead())                
                    (*iter)->Respawn();
	}

    void Aggro(Unit* pWho)
    {	
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GOLEMAGG, IN_PROGRESS);
        m_creature->CallForHelp(30.0f);
    }

    void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GOLEMAGG, DONE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() <= 11.0f)
        {
			if (!bHasPerformedAggro)
			{
				m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), 90);
				bHasPerformedAggro = true;
			}

            if (uiEarthQuake_Timer < uiDiff)
            {
                DoCast(m_creature,SPELL_EARTH_QUAKE);

                uiEarthQuake_Timer = 3000;
            }
			else 
				uiEarthQuake_Timer -= uiDiff;
        }

        if (uiPyroblast_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_PYRO_BLAST);

            uiPyroblast_Timer = 7000;
        }
		else 
			uiPyroblast_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
}; 

struct MANGOS_DLL_DECL mob_core_ragerAI : public ScriptedAI
{
    mob_core_ragerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
    }

    ScriptedInstance* m_pInstance;

	Unit* pGolemagg;

    uint32 uiMangle_Timer;

	bool bHasPerformedAggro;

    void Reset()
    {
		if (m_pInstance)
			pGolemagg = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_GOLEMAGG));

		uiMangle_Timer = 7000;      //These times are probably wrong 

		bHasPerformedAggro = false;
    }

	void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
	{
		if (m_pInstance && pGolemagg && m_creature->GetHealthPercent() <= 50.0f)
		{
			if (pGolemagg->isAlive())
			{
				DoScriptText(EMOTE_GOLEMAGG_HELP,m_creature);
				m_creature->SetHealth(m_creature->GetMaxHealth());
			}
			else
				uiDamage = m_creature->GetHealth();
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim()  || !pGolemagg)
            return;

		/*if (m_creature->IsWithinDistInMap(pGolemagg, 20.0f) && !m_creature->HasAura(SPELL_GOLEMAGG_TRUST))
			DoCast(m_creature, SPELL_GOLEMAGG_TRUST);*/       //keeps spamming making their AI stuck

        if (uiMangle_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),SPELL_MANGLE);

            uiMangle_Timer = 10000;
        }
		else 
			uiMangle_Timer -= uiDiff;

		if (m_creature->GetHealthPercent() <= 11.0f)
        {
			if (!bHasPerformedAggro)
			{
				m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), 90);
				bHasPerformedAggro = true;
			}
		}

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_golemagg(Creature* pCreature)
{
    return new boss_golemaggAI(pCreature);
}

CreatureAI* GetAI_mob_core_rager(Creature* pCreature)
{
    return new mob_core_ragerAI(pCreature);
}

void AddSC_boss_golemagg()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_golemagg";
    newscript->GetAI = &GetAI_boss_golemagg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_core_rager";
    newscript->GetAI = &GetAI_mob_core_rager;
    newscript->RegisterSelf();
}
