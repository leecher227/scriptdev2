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
SDName: Boss_Ragnaros
SD%Complete: 90
SDComment: Ready for test
SDCategory: Molten Core
EndScriptData */

/*

DELETE FROM `creature` WHERE `id`=11502;
UPDATE `creature_template` SET `spell1` = 0 WHERE `entry` = 12143;
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 12143;
UPDATE `creature_template` SET `spell1` = 0, `spell2` = 0, `spell3` = 0, `spell4` = 0 WHERE `entry` = 11502;

//old status
UPDATE `creature_template` SET `spell1` = 20566, `spell2` = 19773, `spell3` = 19780, `spell4` = 20565 WHERE `entry` = 11502;
UPDATE `creature_template` SET `ScriptName` = 'event_ai' WHERE `entry` = 12143;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
(97299, 11502, 409, 1, 0, 0, 834.55, -826.868, -231.599, 5.01111, 25, 0, 0, 1099230, 0, 0, 0);
*/

#include "precompiled.h"
#include "molten_core.h"

enum
{
    // the add
	NPC_SON_OF_FLAME			    =	12143,

	SAY_REINFORCEMENTS_1		    =	-1409013,
	SAY_REINFORCEMENTS_2		    =	-1409014,
	SAY_HAND					    =	-1409015,
	SAY_WRATH					    =	-1409016,
	SAY_KILL					    =	-1409017,
	SAY_MAGMA_BURST				    =	-1409018,

	SPELL_HAND_OF_RAGNAROS		    =	19780,
	SPELL_WRATH_OF_RAGNAROS		    =	20566,

	SPELL_MAGMA_BURST			    =	20565,

	SPELL_RAGNAROS_SUBMERGE_FADE    =	21107,
	SPELL_RAGNAROS_SUBMERGE_VISUAL	=	20567,
	SPELL_RAGNAROS_EMERGE		    =	20568,
	SPELL_ERRUPTION				    =	17731,

	SPELL_MELT_WEAPON_AURA		    =	21388,

	//Son of Flame Aura
	SPELL_LAVA_SHIELD_AURA		    =	21857
};

float _PositionOfAdds[8][4]=
{	
    {848.740356f, -816.103455f, -229.743270f, 2.615287f},
    {852.560791f, -849.861511f, -228.560974f, 2.836073f},
	{808.710632f, -852.845764f, -227.914963f, 0.964207f},
	{786.597107f, -821.132874f, -226.350128f, 0.949377f},
    {796.219116f, -800.948059f, -226.010361f, 0.560603f},
    {821.602539f, -782.744109f, -226.023575f, 6.157440f},
	{844.924744f, -769.453735f, -225.521698f, 4.4539958f},
	{839.823364f, -810.869385f, -229.683182f, 4.693108f},
};

 struct MANGOS_DLL_DECL boss_ragnarosAI : public ScriptedAI
{
    boss_ragnarosAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		SetCombatMovement(false);
		Reset();
	}

	ScriptedInstance* m_pInstance;

	Creature* pSonsOfFlame[8];

    uint32 uiWrathOfRagnaros_Timer;
    uint32 uiHandOfRagnaros_Timer;
    uint32 uiMagmaBurst_Timer;
    uint32 uiElementalFire_Timer;
    uint32 uiErruption_Timer;
    uint32 uiSubmerge_Timer;
    uint32 uiAttack_Timer;

	//timer for checking the adds whether they are dead or not
	uint32 uiCheckAdds_Timer;

    bool bHasYelledMagmaBurst;
    bool bHasSubmergedOnce;
    bool bWasBanished; 
    bool bHasAura;

    void Reset()
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_RAGNAROS, NOT_STARTED);

		uiWrathOfRagnaros_Timer = 25000;
		uiHandOfRagnaros_Timer = 20000;
		uiMagmaBurst_Timer = 2000;
		uiErruption_Timer = 15000;
		uiElementalFire_Timer = 3000;
		uiSubmerge_Timer = 180000;
		uiAttack_Timer = 90000;
		uiCheckAdds_Timer = 2000;
		bHasYelledMagmaBurst = false;
		bHasSubmergedOnce = false;
		bWasBanished = false;

		if(bHasAura)
			m_creature->RemoveAurasDueToSpell(SPELL_MELT_WEAPON_AURA);

		bHasAura = false;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (rand()%5)
            return;

        DoScriptText(SAY_KILL, m_creature);
    }

	void MoveInLineOfSight(Unit* pWho)
	{
		if (m_pInstance && m_pInstance->GetData(DATA_VAR_RAGNAROS_INTRO) == DONE && m_creature->IsWithinDistInMap(pWho, m_creature->GetAttackDistance(pWho)))
			AttackStart(pWho);
	}

    void Aggro(Unit* pWho)
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_RAGNAROS, IN_PROGRESS);
	}

	void JustDied(Unit* pKiller)
    {
		if (m_pInstance)
			m_pInstance->SetData(TYPE_RAGNAROS, DONE);
    }

	bool AddsAreDead()
	{
		for(uint8 i = 0; i < 8 ; ++i)
			if (pSonsOfFlame[i] && pSonsOfFlame[i]->isAlive())
				return false;

		return true;
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_pInstance || m_pInstance->GetData(DATA_VAR_RAGNAROS_INTRO) != DONE)
			return;

		if (bWasBanished && uiAttack_Timer < uiDiff)
		{
			//Become unbanished again 
			//m_creature->setFaction(14);
			m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			DoCast(m_creature,SPELL_RAGNAROS_EMERGE);
			bWasBanished = false;
		}
		else if (bWasBanished && uiCheckAdds_Timer < uiDiff)
		{
			if (AddsAreDead())
			{
				//Become unbanished again 
				m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				DoCast(m_creature,SPELL_RAGNAROS_EMERGE);
				bWasBanished = false;
			}
			uiCheckAdds_Timer = 2000;
		}			
		else if (bWasBanished)
		{
			uiAttack_Timer -= uiDiff;
			uiCheckAdds_Timer -= uiDiff;
			return;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if (!bHasAura)
		{
			DoCast(m_creature,SPELL_MELT_WEAPON_AURA);
			bHasAura = true;
		}

		if (uiWrathOfRagnaros_Timer < uiDiff)
		{
			DoCast(m_creature,SPELL_WRATH_OF_RAGNAROS);
			m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), 70);

			if (rand()%2 == 0)            
				DoScriptText(SAY_WRATH, m_creature);            

			uiWrathOfRagnaros_Timer = 25000;
		}
		else 
			uiWrathOfRagnaros_Timer -= uiDiff;

		if (uiHandOfRagnaros_Timer < uiDiff)
		{
			DoCast(m_creature,SPELL_HAND_OF_RAGNAROS);

			if (rand()%2 == 0)
				DoScriptText(SAY_HAND, m_creature);

			uiHandOfRagnaros_Timer = 20000 + rand()%10000;
		}
		else 
			uiHandOfRagnaros_Timer -= uiDiff;

		if (uiErruption_Timer < uiDiff)
		{
			DoCast(m_creature,SPELL_ERRUPTION);

			uiErruption_Timer = 20000 + rand()%25000;
		}
		else 
			uiErruption_Timer -= uiDiff;

		if (!bWasBanished && uiSubmerge_Timer < uiDiff)
		{
			//Creature spawning and ragnaros becomming unattackable
			//is not very well supported in the core
			//so added normaly spawning and banish workaround and attack again after 90 secs.

			m_creature->InterruptNonMeleeSpells(false);
			//Root self
			//m_creature->setFaction(35);
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			DoCast(m_creature,SPELL_RAGNAROS_SUBMERGE_VISUAL);
			m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);

			if (!bHasSubmergedOnce)
			{
				DoScriptText(SAY_REINFORCEMENTS_1, m_creature);
				bHasSubmergedOnce = true;
			}
			else
				DoScriptText(SAY_REINFORCEMENTS_2, m_creature);

			Unit* pTarget = NULL;
			// summon 8 Son of Flames
			for(uint8 i = 0; i < 8; ++i)
			{
				pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
				pSonsOfFlame[i] = m_creature->SummonCreature(NPC_SON_OF_FLAME, _PositionOfAdds[i][0], _PositionOfAdds[i][1], _PositionOfAdds[i][2], _PositionOfAdds[i][3], TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 900000);
				if (pSonsOfFlame[i] && pTarget)
				{
					// set on the aoe mana burn
					pSonsOfFlame[i]->CastSpell(pSonsOfFlame[i],SPELL_LAVA_SHIELD_AURA,false);
					pSonsOfFlame[i]->AI()->AttackStart(pTarget);
				}
			}
                
			bWasBanished = true;
			DoCast(m_creature,SPELL_RAGNAROS_SUBMERGE_FADE);
			uiAttack_Timer = 90000;
			uiCheckAdds_Timer = 2000;

			uiSubmerge_Timer = 180000;
		}
		else 
			uiSubmerge_Timer -= uiDiff;

		if (m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
			DoMeleeAttackIfReady();	        
		else
		{
			if (uiMagmaBurst_Timer < uiDiff)
			{
				// if nobody is in range
				if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
					DoCast(pTarget,SPELL_MAGMA_BURST);

				if (!bHasYelledMagmaBurst)
				{
					DoScriptText(SAY_MAGMA_BURST, m_creature);
					bHasYelledMagmaBurst = true;
				}
				uiMagmaBurst_Timer = 2500;
			}
			else 
				uiMagmaBurst_Timer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_boss_ragnaros(Creature* pCreature)
{
    return new boss_ragnarosAI(pCreature);
}

void AddSC_boss_ragnaros()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_ragnaros";
    newscript->GetAI = &GetAI_boss_ragnaros;
    newscript->RegisterSelf();
}
