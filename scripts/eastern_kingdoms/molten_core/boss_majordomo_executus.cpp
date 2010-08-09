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
SDName: Boss_Majordomo_Executus
SD%Complete: 80
SDComment: Adds are not fully supported(spells are missing) 
           Hot Coals has be scripted 
           Test timer, mabye the english speech takes longer than the german
           The shields are casted on player - remove this , maybe core problem
SDCategory: Molten Core
EndScriptData */

/*
	DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=11663);
	INSERT INTO `creature_ai_scripts` VALUES 
	( 1166301, 11663, 0, 0, 100, 3, 9000, 9000, 9000, 9000, 11, 20603, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Flamewaker Healer - Cast Shadow Shock'),
	(1166302, 11663, 0, 0, 100, 3, 2000, 2000, 2000, 2000, 11, 22677, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Flamewaker Healer - Cast Shadow Bolt');
	UPDATE creature_template SET ScriptName = 'mob_eventai';

	DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=11664);
	INSERT INTO `creature_ai_scripts` VALUES 
	( 1166401, 11664, 0, 0, 85, 3, 1000, 1000, 8000, 8000, 11, 36711, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Flamewaker Elite - Cast Fireball'),
	( 1166402, 11664, 0, 0, 100, 3, 12000, 12000, 12000, 12000, 11, 20229, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Flamewaker Elite - Cast Blast Wave'),
	(1166403, 11664, 0, 0, 80, 3, 5000, 5000, 15000, 15000, 11, 20623, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Flamewaker Elite - Cast Fire Blast');
	UPDATE `creature_template` SET `ScriptName` = 'mob_eventai' WHERE `entry` = 11664;

    // use this for getting the spell SPELL_TELEPORT_TO_COALS working
	INSERT INTO `spell_target_position` (`id`,`target_map`,`target_position_x`,`target_position_y`,`target_position_z`) VALUES (20534,409,736.466,-1177.063,-119.181);
*/

#include "precompiled.h"
#include "molten_core.h"

enum
{
	FACTION_DARK_IRON_DWARVES	=	54,
	FACTION_MONSTER				=	14,

	NPC_FLAMEWAKER_PRIEST		=	11663,
	NPC_FLAMEWAKER_ELITE		=	11664,

	SAY_AGGRO					=	-1409003,
	SAY_SPAWN					=	-1409004,
	SAY_SLAY					=	-1409005,
	SAY_SPECIAL					=	-1409006,
	SAY_DEFEAT					=	-1409007,

	//Majordomo Speech at Ragnaros
	SAY_SUMMON      			=	-1409008,
	SAY_ARRIVAL_MAJOR			=	-1409010,

	//Ragnaros Answer to Majordomo
	SAY_ARRIVAL_RAG_1			=	-1409009,
	SAY_ARRIVAL_RAG_2			=	-1409011,
	SAY_ARRIVAL_RAG_3			=	-1409012,

	//Majordomo Spells
	SPELL_MAGIC_REFLECTION		=	20619,
	SPELL_DAMAGE_REFLECTION		=	21075,
	SPELL_BLAST_WAVE			=	20229,      // Also used by Flamewaker Elites!
	SPELL_AEGIS					=	20620,	    //This is self casted whenever we are below 50%
	SPELL_TELEPORT_TO_COALS		=	20534,

	//Priest spells
	SPELL_GREATER_HELL			=	29564,      // Not Yet Implemented
	SPELL_SHADOW_BOLT           =   21077,      // Not Yet Implemented

	//Elite spells
	SPELL_FIRE_BLAST			=	20623,      // Not Yet Implemented

	//For Intro Handling
	SPELL_VISUAL_TELEPORT       =   19484,

	SPELL_SUMMON_RAGNAROS		=	19774,
	SPELL_RAGE_MERGE			=	20568,
	SPELL_ELEMENTAL_FIRE		=	19773
};

#define GOSSIP_ITEM_DOMO   "Let me talk with your master!"

//Orientation	= 3.541
float PositionOfAdds[8][3]=
{	
    {760.444f, -1183.175f, -119.181f},		//First Elite, right Side
    {753.009f, -1198.237f, -118.346f},		//Second Elite, right Side
	{746.649f, -1154.512f, -119.181f},		//Third Elite, left Side		
	{729.771f, -1153.243f, -119.181f},		//Fourth Elite, left side
    {761.088f, -1174.606f, -119.181f},		//First Priest, right Side
    {756.907f, -1191.007f, -119.181f},		//Second Priest, right Side
	{753.748f, -1159.354f, -119.181f},		//Third Priest , left Side
	{738.322f, -1152.389f, -119.181f},		//Fourth Pirest, left Side
};

struct MANGOS_DLL_DECL boss_majordomoAI : public ScriptedAI
{
    boss_majordomoAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
    }

    ScriptedInstance* m_pInstance;

	Creature* pAdds[8]; //First four elite, second four priests
	
	Creature* pRagnaros; // for intro handling

    uint32 uiMagicReflection_Timer;
    uint32 uiDamageReflection_Timer;
    uint32 uiBlastwave_Timer;
	uint32 uiTeleport_Timer;

	uint32 uiCheckFlamewaker_Timer;

	uint32 uiIntro_Timer;
	uint8 uiIntroCounter;
	uint8 uiWPdone;

	bool bSaySpawn;
	bool bIntroDone;
	bool bMoving;
	bool bSetVisible;

    void Reset()
    {
		if (!m_pInstance)
			return;

		if (m_pInstance->GetData(TYPE_MAJORDOMO) != SPECIAL)
		{
			m_pInstance->SetData(TYPE_MAJORDOMO, NOT_STARTED);

			uiMagicReflection_Timer =  30000;
			uiDamageReflection_Timer = 15000;
			uiBlastwave_Timer = 10000;
			uiTeleport_Timer = 20000;

			uiCheckFlamewaker_Timer = 2000;
			bSaySpawn = false;
			bSetVisible = false;
            uiIntroCounter = 0;

            // kill adds before respawn - but not on first spawn when creating the instance
            if (m_pInstance->GetData(DATA_DOMO_ADDS_SPAWNED) == DONE)
            {
                for(uint8 i = 0 ; i < 8; ++i)
			    {
			        if (pAdds[i] && pAdds[i]->isAlive())
                        pAdds[i]->ForcedDespawn();
			    }
			}
			// spawn the adds
			for(uint8 i = 0; i < 8 ; ++i)
			{
		        if (i < 4)
		            pAdds[i] = m_creature->SummonCreature(NPC_FLAMEWAKER_ELITE, PositionOfAdds[i][0], PositionOfAdds[i][1], PositionOfAdds[i][2], 3.541f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1200000);
		        else
		            pAdds[i] = m_creature->SummonCreature(NPC_FLAMEWAKER_PRIEST, PositionOfAdds[i][0], PositionOfAdds[i][1], PositionOfAdds[i][2], 3.541f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1200000);
			    if (pAdds[i])
			        // make them invisible as majordomo - IMPLEMENT, THAT THEY DO NOT ATTACK WHILE INVISIBLE!
                    pAdds[i]->SetVisibility(VISIBILITY_OFF);
	        }
	        m_pInstance->SetData(DATA_DOMO_ADDS_SPAWNED,DONE);
		}
		else
		{
			//Intro handling
			uiIntro_Timer = 36000;
			uiWPdone = 0;

			bIntroDone = false;
			bMoving = false;
		}
    }

	void BeginEvent(Player* pTarget)
    {
		uiIntroCounter = 2;
    }

	void MoveInLineOfSight(Unit* pWho)
    {		
		if (!m_pInstance || m_pInstance->GetData(TYPE_MAJORDOMO) == SPECIAL || m_creature->GetVisibility() == VISIBILITY_OFF)
			return;

		if (!bSaySpawn && !m_creature->getVictim() && pWho->isTargetableForAttack() && ( m_creature->IsHostileTo( pWho )) && pWho->isInAccessablePlaceFor(m_creature))
		{
			DoScriptText(SAY_SPAWN, m_creature);
			bSaySpawn = true;
		}

		if (m_creature->IsWithinDistInMap(pWho, m_creature->GetAttackDistance(pWho)) && m_creature->IsWithinLOSInMap(pWho) 
			&& !(pWho->getFaction()== FACTION_DARK_IRON_DWARVES || pWho->getFaction() == FACTION_MONSTER))
		{
			pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
			AttackStart(pWho);
		}		
    }

    void KilledUnit(Unit* pVictim)
    {
        if (rand()%5)
            DoScriptText(SAY_SLAY, m_creature);
    }
    
    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        //reset the health points of majordomo, when his health is too low, because he can't get defeated
        if ((m_creature->GetHealth() - uiDamage)*100 / m_creature->GetMaxHealth() <= 30)
            m_creature->SetHealth(m_creature->GetHealth());
    }       

    void Aggro(Unit* pWho)
    {
        // only aggro , when it is before the ragnaros event
		if (m_pInstance && m_pInstance->GetData(TYPE_MAJORDOMO) != SPECIAL)
		{
			m_pInstance->SetData(TYPE_MAJORDOMO, IN_PROGRESS);

		    for(uint8 i = 0 ; i < 8 ; ++i)
		        if (pAdds[i] && pAdds[i]->isAlive())
		            pAdds[i]->AI()->AttackStart(pWho);
			
            m_creature->CallForHelp(20.0f);
        
		    DoScriptText(SAY_AGGRO, m_creature);
		}
		else
		    return;
    }

	void MovementInform(uint32 uiType, uint32 uiWaypointId)
    {
		if (uiType != POINT_MOTION_TYPE)
			return;

		if (uiWaypointId == 1)
		{
			DoCast(m_creature,SPELL_SUMMON_RAGNAROS);
			uiWPdone = 1;
			bMoving = false;
		}

        else if (uiWaypointId == 2)
		{
			uiWPdone = 2;
			bMoving = false;
		}
    }
	
	bool AddsAreDead()
	{
		for(uint8 i = 0; i < 8 ; ++i)
		{
			if (pAdds[i] && pAdds[i]->isAlive())
				return false;
		}
		return true;
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_pInstance || m_creature->GetVisibility() == VISIBILITY_OFF)
			return;

		if (!bSetVisible && m_pInstance->GetData(DATA_BOSSES_ARE_DEAD) == DONE && m_pInstance->GetData(DATA_DOMO_ADDS_SPAWNED) == DONE)
		{
			for(uint8 i = 0; i < 8 ; ++i)
				if (pAdds[i])
					pAdds[i]->SetVisibility(VISIBILITY_ON);
			bSetVisible = true;
		}

		if (!bIntroDone)
		{
			switch(uiIntroCounter)
			{
			case 1:
			    
				if (uiIntro_Timer < uiDiff && m_pInstance->GetData(DATA_DOMO_PORTED) != DONE)
				{
				    DoCast(m_creature,SPELL_VISUAL_TELEPORT);
					m_creature->SetVisibility(VISIBILITY_OFF);
					
					// "teleport" Majordomo to Ragnaros - this has to be handeld in that way
					// because nothing will happen or the server will crash, when we relocate him
					// when the grid isn't load at the moment
					Creature* pDomo = m_creature->SummonCreature(12018, 854.975f, -827.593f, -228.504f, 4.99008f, TEMPSUMMON_TIMED_DESPAWN,3600000);
					
					/// make him friendly and talkable
					pDomo->setFaction(35);
					pDomo->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					pDomo->SetVisibility(VISIBILITY_ON);
					m_pInstance->SetData(DATA_DOMO_PORTED, DONE);
				}
				else 
					uiIntro_Timer -= uiDiff;

				break;

			case 2:
				if (!bMoving && uiWPdone == 0 )
				{
					m_creature->GetMotionMaster()->Clear();
					m_creature->GetMotionMaster()->MovePoint(1, 851.266f, -816.847f, -229.358f);
					bMoving = true;
				}

				if (uiWPdone == 1)
				{
					m_creature->GetMotionMaster()->Clear();
					m_creature->GetMotionMaster()->MovePoint(2, 843.145f, -813.479f, -230.067f);
					bMoving = true;
					++uiIntroCounter;
				}
				break;

			case 3:
				if (uiIntro_Timer < uiDiff)
				{
					DoScriptText(SAY_SUMMON, m_creature);
					pRagnaros = m_creature->SummonCreature(NPC_RAGNAROS, 839.380066f ,-833.359558f, -229.358f, 1.380336f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,7200000);

					++uiIntroCounter;
					uiIntro_Timer = 15000;
				}
				else 
					uiIntro_Timer -= uiDiff;

				break;
				
			case 4:
				if (uiIntro_Timer < uiDiff)
				{   
				    if (pRagnaros)
				    {
					    pRagnaros->SetVisibility(VISIBILITY_ON);
					    pRagnaros->CastSpell(pRagnaros,SPELL_RAGE_MERGE,false);
                    }
					++uiIntroCounter;
					uiIntro_Timer = 5000;
				}
				else 
					uiIntro_Timer -= uiDiff;
				break;
				
			case 5:
					if (uiIntro_Timer < uiDiff)
					{
					    if (pRagnaros)
						    DoScriptText(SAY_ARRIVAL_RAG_1, pRagnaros);

						++uiIntroCounter;
						uiIntro_Timer = 17000;
					}
					else 
						uiIntro_Timer -=uiDiff;
					break;

			case 6:
				if (uiIntro_Timer < uiDiff)
				{
					DoScriptText(SAY_ARRIVAL_MAJOR, m_creature);
					++uiIntroCounter;
					uiIntro_Timer = 10000;
				}
				else 
					uiIntro_Timer -= uiDiff;

				break;
				
			case 7:
					if (uiIntro_Timer < uiDiff)
					{   
					    if (pRagnaros)
						    DoScriptText(SAY_ARRIVAL_RAG_2, pRagnaros);

						++uiIntroCounter;
						uiIntro_Timer = 22000;
					}
					else 
						uiIntro_Timer -= uiDiff;
					break;

			case 8:
				if (uiIntro_Timer < uiDiff)
				{
					//kill majordomo
					if (pRagnaros)
					    pRagnaros->CastSpell(m_creature,SPELL_ELEMENTAL_FIRE,false);

					++uiIntroCounter;
					uiIntro_Timer = 6000;
				}
				else 
					uiIntro_Timer -= uiDiff;
				break;

			case 9:
				if (uiIntro_Timer < uiDiff)
				{
					DoScriptText(SAY_ARRIVAL_RAG_3, pRagnaros);

					m_pInstance->SetData(DATA_VAR_RAGNAROS_INTRO,DONE);
					bIntroDone = true;
				}
				else 
					uiIntro_Timer -= uiDiff;
			}
		}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_pInstance->GetData(TYPE_MAJORDOMO) == SPECIAL)
            return;

		if (uiCheckFlamewaker_Timer < uiDiff)
		{
			if (AddsAreDead())
			{
				m_pInstance->SetData(TYPE_MAJORDOMO, SPECIAL);

				uiCheckFlamewaker_Timer = 600000;

				DoScriptText(SAY_DEFEAT, m_creature);
				uiIntroCounter = 1;

				EnterEvadeMode();
				m_creature->setFaction(35);
			}
			uiCheckFlamewaker_Timer = 2000;
		}
		else 
			uiCheckFlamewaker_Timer -= uiDiff;

        if (m_creature->GetHealthPercent() <= 50.0f)        
            DoCast(m_creature,SPELL_AEGIS);        

        if (uiMagicReflection_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_MAGIC_REFLECTION);
			/*for(uint8 i = 0 ; i < 8 ; ++i)
			{
				if (pAdds[i] && pAdds[i]->isAlive())
					DoCast(pAdds[i],SPELL_MAGIC_REFLECTION);
			}*/
            uiMagicReflection_Timer = 30000;
        }
		else 
			uiMagicReflection_Timer -= uiDiff;

        if (uiDamageReflection_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_DAMAGE_REFLECTION);
			/*for(uint8 i = 0 ; i < 8 ; ++i)
			{
				if( pAdds[i] && pAdds[i]->isAlive())
					DoCast(pAdds[i],SPELL_DAMAGE_REFLECTION);
			}*/
            uiDamageReflection_Timer = 30000;
        }
		else 
			uiDamageReflection_Timer -= uiDiff;

        if (uiBlastwave_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_BLAST_WAVE);

            uiBlastwave_Timer = 10000;
        }
		else 
			uiBlastwave_Timer -= uiDiff;

		if (uiTeleport_Timer < uiDiff)
		{
			Unit* pTarget = NULL;

			if(rand()%2 == 1)
				m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), 80);
			else
				pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

			if (pTarget)
			{
				DoCast(pTarget,SPELL_TELEPORT_TO_COALS);
				DoScriptText(SAY_SPECIAL,m_creature);
				//while on the fire ground, player gets randomly 2000-3000 damage
				DoCast(pTarget,SPELL_FIRE_BLAST);
			}

			uiTeleport_Timer = 20000 + rand()%5000;
		}
		else 
			uiTeleport_Timer -= uiDiff;

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_majordomo(Creature* pCreature)
{
    return new boss_majordomoAI(pCreature);
}

bool GossipHello_boss_majordomo(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_DOMO , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(68,pCreature->GetGUID());
    return true;
}

bool GossipSelect_boss_majordomo(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction )
{
	if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
	{
	    ((boss_majordomoAI*)pCreature->AI())->BeginEvent(pPlayer);
		pPlayer->CLOSE_GOSSIP_MENU();
	}
    return true;
}

void AddSC_boss_majordomo()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_majordomo";
    newscript->GetAI = &GetAI_boss_majordomo;
	newscript->pGossipHello = &GossipHello_boss_majordomo;
    newscript->pGossipSelect = &GossipSelect_boss_majordomo;
    newscript->RegisterSelf();
}
