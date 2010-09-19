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
SDName: Boss_Garr
SD%Complete: 90
SDComment: Adds respawn after wipe not implemented
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum
{
    NPC_FIRESWORN               =   12099,
	//his own spells
	SPELL_ANTI_MAGIC_PULS		=	19492,
	SPELL_MAGMA_SHACKLES		=	19496,
	SPELL_FRENZY				=	19516, //Stacking enrage (stacks to 10 times)

	//spells of his adds
	SPELL_ERUPTION				=	19497,
	SPELL_IMMOLATE				=	20294,
    SPELL_MASSIVE_ERUPTION      =   20483,
	SPELL_SEPERATION_ANXIETY	=	23492 // if distance between adds and gaar is over 50 yards
};

struct MANGOS_DLL_DECL boss_garrAI : public ScriptedAI
{
    boss_garrAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}

	ScriptedInstance* m_pInstance;

    uint32 uiAntiMagicPulse_Timer;
    uint32 uiMagmaShackles_Timer;

    uint8 uiFrenzyCount;

    void Reset()
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_GARR) != DONE)
			m_pInstance->SetData(TYPE_GARR, NOT_STARTED);

		m_creature->RemoveAllAuras();

		uiAntiMagicPulse_Timer = 25000;      //These times are probably wrong
		uiMagmaShackles_Timer = 15000;

		uiFrenzyCount = 0;

        std::list<Creature*> m_lFiresworns;
        GetCreatureListWithEntryInGrid(m_lFiresworns,m_creature,NPC_FIRESWORN,100.0f);
        if (!m_lFiresworns.empty())
            for(std::list<Creature*>::iterator iter = m_lFiresworns.begin(); iter != m_lFiresworns.end(); ++iter)
                if ((*iter) && (*iter)->isDead())                
                    (*iter)->Respawn();
    }

    void Aggro(Unit* pWho)
    {	
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GARR, IN_PROGRESS);
    }

	void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_GARR, DONE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiAntiMagicPulse_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_ANTI_MAGIC_PULS);

            //14-18 seconds until we should cast this agian
            uiAntiMagicPulse_Timer = 14000 + rand()%4000;
        }
		else 
			uiAntiMagicPulse_Timer -= uiDiff;

        if (uiMagmaShackles_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_MAGMA_SHACKLES);

            uiMagmaShackles_Timer = 8000 + rand()%4000;
        }
		else 
			uiMagmaShackles_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
}; 


struct MANGOS_DLL_DECL mob_fireswornAI : public ScriptedAI
{
    mob_fireswornAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}
	ScriptedInstance* m_pInstance;

	Unit* pGarr;

    uint32 uiImmolate_Timer;

	bool bErruption;

    void Reset()
    {
		pGarr = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_GARR));

        uiImmolate_Timer = 4000;      //These times are probably wrong

		bErruption = true;
    }

    void DamageTaken(Unit *pDoneBy,uint32 &uiDamage)
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;
            DoCast(pDoneBy,SPELL_MASSIVE_ERUPTION);
            /*if (pGarr)
                pGarr->CastSpell(pGarr,SPELL_FRENZY,false);*/
        }
    }

	void JustDied(Unit* pKiller)
    {
		if (pGarr)
			pGarr->CastSpell(pGarr,SPELL_FRENZY,false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;        

        if (uiImmolate_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->ModifyPower(POWER_MANA,300);
                DoCast(pTarget,SPELL_IMMOLATE,false);
            }
            uiImmolate_Timer = 5000 + rand()%5000;
        }
		else 
			uiImmolate_Timer -= uiDiff;

        //Cast Erruption and let them die
        /*if (bErruption && m_creature->GetHealth()*100 / m_creature->GetMaxHealth() <= 10 )
        {
			if (rand()%100 + 1 < 50)
			{
				DoCast(m_creature,SPELL_ERUPTION);
				m_creature->setDeathState(JUST_DIED);
				m_creature->RemoveCorpse();
			}
			else 
				bErruption = false;
        }*/
		if (pGarr)
		{
			if (!m_creature->IsWithinDistInMap(pGarr, 70.0f) && !m_creature->HasAura(SPELL_SEPERATION_ANXIETY))
				DoCast(m_creature,SPELL_SEPERATION_ANXIETY);

			if (m_creature->IsWithinDistInMap(pGarr, 70.0f) && m_creature->HasAura(SPELL_SEPERATION_ANXIETY))
				m_creature->RemoveAllAuras();
		}

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_garr(Creature* pCreature)
{
    return new boss_garrAI(pCreature);
}

CreatureAI* GetAI_mob_firesworn(Creature* pCreature)
{
    return new mob_fireswornAI(pCreature);
}

void AddSC_boss_garr()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_garr";
    newscript->GetAI = &GetAI_boss_garr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_firesworn";
    newscript->GetAI = &GetAI_mob_firesworn;
    newscript->RegisterSelf();

}
