/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "../../sc_defines.h"

#define SAY_GAMESBEGIN          "Let the games begin!"
#define SAY_VAEL_INTRO          "<unknown>"

#define SOUND_GAMESBEGIN        8280
#define SOUND_VAEL_INTRO        8279

#define GOSSIP_ITEM             "Start Event <Needs Gossip Text>"

#define CREATURE_BRONZE_DRAKANOID       14263
#define CREATURE_BLUE_DRAKANOID         14261
#define CREATURE_RED_DRAKANOID          14264
#define CREATURE_GREEN_DRAKANOID        14262
#define CREATURE_BLACK_DRAKANOID        14265

#define CREATURE_CHROMATIC_DRAKANOID    14302

#define CREATURE_NEFARIAN               11583

#define ADD_X1 -7591.151855
#define ADD_X2 -7514.598633
#define ADD_Y1 -1204.051880
#define ADD_Y2 -1150.448853
#define ADD_Z1 476.800476
#define ADD_Z2 476.796570

#define NEF_X   -7445
#define NEF_Y   -1332
#define NEF_Z   536

#define HIDE_X  -7592
#define HIDE_Y  -1264
#define HIDE_Z  481

#define SPELL_SHADOWBOLT        21077

//This script is complicated
//Instead of morphing Victor Nefarius we will have him control phase 1
//And then have him spawn "Nefarian" for phase 2
//When phase 2 starts Victor Nefarius will go into hiding and stop attacking
//If Nefarian despawns because he killed the players then this guy will EnterEvadeMode
//and allow players to start the event over
//If nefarian dies then he will kill himself then he will kill himself in his hiding place
//To prevent players from doing the event twice


struct MANGOS_DLL_DECL boss_victor_nefariusAI : public ScriptedAI
{
    boss_victor_nefariusAI(Creature *c) : ScriptedAI(c) 
    {
        Nefarian = NULL;
        EnterEvadeMode();
        srand(time(NULL));
        switch (rand()%20)
        {
        case 0:
            DrakType1 = CREATURE_BRONZE_DRAKANOID;
            DrakType2 = CREATURE_BLUE_DRAKANOID;
            break;
        case 1:
            DrakType1 = CREATURE_BRONZE_DRAKANOID;
            DrakType2 = CREATURE_RED_DRAKANOID;
            break;
        case 2:
            DrakType1 = CREATURE_BRONZE_DRAKANOID;
            DrakType2 = CREATURE_GREEN_DRAKANOID;
            break;
        case 3:
            DrakType1 = CREATURE_BRONZE_DRAKANOID;
            DrakType2 = CREATURE_BLACK_DRAKANOID;
            break;
        case 4:
            DrakType1 = CREATURE_BLUE_DRAKANOID;
            DrakType2 = CREATURE_BRONZE_DRAKANOID;
            break;
        case 5:
            DrakType1 = CREATURE_BLUE_DRAKANOID;
            DrakType2 = CREATURE_RED_DRAKANOID;
            break;
        case 6:
            DrakType1 = CREATURE_BLUE_DRAKANOID;
            DrakType2 = CREATURE_GREEN_DRAKANOID;
            break;
        case 7:
            DrakType1 = CREATURE_BLUE_DRAKANOID;
            DrakType2 = CREATURE_BLACK_DRAKANOID;
            break;
        case 8:
            DrakType1 = CREATURE_RED_DRAKANOID;
            DrakType2 = CREATURE_BRONZE_DRAKANOID;
            break;
        case 9:
            DrakType1 = CREATURE_RED_DRAKANOID;
            DrakType2 = CREATURE_BLUE_DRAKANOID;
            break;
        case 10:
            DrakType1 = CREATURE_RED_DRAKANOID;
            DrakType2 = CREATURE_GREEN_DRAKANOID;
            break;
        case 11:
            DrakType1 = CREATURE_RED_DRAKANOID;
            DrakType2 = CREATURE_BLACK_DRAKANOID;
            break;
        case 12:
            DrakType1 = CREATURE_GREEN_DRAKANOID;
            DrakType2 = CREATURE_BRONZE_DRAKANOID;
            break;
        case 13:
            DrakType1 = CREATURE_GREEN_DRAKANOID;
            DrakType2 = CREATURE_BLUE_DRAKANOID;
            break;
        case 14:
            DrakType1 = CREATURE_GREEN_DRAKANOID;
            DrakType2 = CREATURE_RED_DRAKANOID;
            break;
        case 15:
            DrakType1 = CREATURE_GREEN_DRAKANOID;
            DrakType2 = CREATURE_BLACK_DRAKANOID;
            break;
        case 16:
            DrakType1 = CREATURE_BLACK_DRAKANOID;
            DrakType2 = CREATURE_BRONZE_DRAKANOID;
            break;
        case 17:
            DrakType1 = CREATURE_BLACK_DRAKANOID;
            DrakType2 = CREATURE_BLUE_DRAKANOID;
            break;
        case 18:
            DrakType1 = CREATURE_BLACK_DRAKANOID;
            DrakType2 = CREATURE_GREEN_DRAKANOID;
            break;
        case 19:
            DrakType1 = CREATURE_BLACK_DRAKANOID;
            DrakType2 = CREATURE_RED_DRAKANOID;
            break;
        }
    }

    uint32 SpawnedAdds;
    uint32 AddSpawnTimer;
    uint32 ShadowBoltTimer;
    uint32 MindControlTimer;
    uint32 ResetTimer;
    uint32 DrakType1;
    uint32 DrakType2;
    Creature* Nefarian;
    
    void EnterEvadeMode()
    {
        SpawnedAdds = 0;
        AddSpawnTimer = 10000;
        ShadowBoltTimer = 5000;
        ResetTimer = 60000;        //On official it takes him 15 minutes(900 seconds) to reset. We are only doing 1 minute to make testing easier
        Nefarian = NULL;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS,65);
        m_creature->setFaction(35);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void BeginEvent(Player* target)
    {
        DoYell(SAY_GAMESBEGIN,LANG_UNIVERSAL,NULL);
        DoPlaySoundToSet(m_creature,SOUND_GAMESBEGIN);

        //MaNGOS::Singleton<MapManager>::Instance().GetMap(m_creature->GetMapId(), m_creature)->GetPlayers().begin();
/*
        list <Player*>::iterator i = MapManager::Instance().GetMap(m_creature->GetMapId(), m_creature)->GetPlayers().begin();

        for (i = MapManager::Instance().GetMap(m_creature->GetMapId(), m_creature)->GetPlayers().begin(); i != MapManager::Instance().GetMap(m_creature->GetMapId(), m_creature)->GetPlayers().end(); ++i)
        {
            AttackStart((*i));            
        }
*/
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS,0);
        m_creature->setFaction(103);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        AttackStart(target);
        
    }
    
    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            //Begin melee attack if we are within range
            DoStartMeleeAttack(who);
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        //We simply use this function to find players until we can use Map->GetPlayers()

        if (who && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsHostileTo(who))
        {
            //Add them to our threat list
            m_creature->AddThreat(who,0.0f);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget())
            return;
        
        //Check if we have a current target
        if( m_creature->getVictim() && m_creature->isAlive())
        {

            //Only do this if we haven't spawned nef yet
            if (SpawnedAdds < 42)
            {
                //ShadowBoltTimer
                if (ShadowBoltTimer < diff)
                {
                    //Cast
                    Unit* target = NULL;
                    target = SelectUnit(SELECT_TARGET_RANDOM,0);
                    if (target)
                        DoCast(target,SPELL_SHADOWBOLT);

                    //Random time till recast between 3 and 10 seconds
                    ShadowBoltTimer = 3000 + (rand()%7000);
                }else ShadowBoltTimer -= diff;

                //Add spawning mechanism
                if (AddSpawnTimer < diff)
                {
                    //Spawn 2 random types of creatures at the 2 locations
                    uint32 CreatureID;
                    Creature* Spawned = NULL;
                    Unit* target = NULL;
                    
                     //1 in 3 chance it will be a chromatic
                    if (rand()%3 == 0)
                        CreatureID = CREATURE_CHROMATIC_DRAKANOID;
                    else CreatureID = DrakType1;

                    SpawnedAdds++;

                    //Spawn creature and force it to start attacking a random target
                    Spawned = m_creature->SummonCreature(CreatureID,469,ADD_X1,ADD_Y1,ADD_Z1,5.000,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    target = SelectUnit(SELECT_TARGET_RANDOM,0);
                    if (target && Spawned)
                        Spawned->AI()->AttackStart(target);
                    
                    //1 in 3 chance it will be a chromatic
                    if (rand()%3 == 0)
                        CreatureID = CREATURE_CHROMATIC_DRAKANOID;
                    else CreatureID = DrakType2;

                    SpawnedAdds++;

                    target = NULL;
                    Spawned = NULL;
                    Spawned = m_creature->SummonCreature(CreatureID,469,ADD_X2,ADD_Y2,ADD_Z2,5.000,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    target = SelectUnit(SELECT_TARGET_RANDOM,0);
                    if (target && Spawned)
                        Spawned->AI()->AttackStart(target);

                    //Begin phase 2 by spawning Nefarian and what not
                    if (SpawnedAdds >= 42)
                    {
                        //Teleport Victor Nefarius way out of the map
                        //MapManager::Instance().GetMap(m_creature->GetMapId(), m_creature)->CreatureRelocation(m_creature,0,0,-5000,0);
                        
                        //Inturrupt any spell casting
                        m_creature->InterruptSpell();

                        //Root self
                        DoCast(m_creature,23973);

                        //Make super invis
                        DoCast(m_creature,8149);

                        //Teleport self to a hiding spot (this causes errors in the mangos log but no real issues)
                        m_creature->Relocate(HIDE_X,HIDE_Y,HIDE_Z,0);
                        m_creature->SendMonsterMove(HIDE_X,HIDE_Y,HIDE_Z,0,true,0);
                        m_creature->addUnitState(UNIT_STAT_FLEEING);

                        //Spawn nef and have him attack a random target
                        Nefarian = m_creature->SummonCreature(CREATURE_NEFARIAN,469,NEF_X,NEF_Y,NEF_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,120000);
                        target = NULL;
                        target = SelectUnit(SELECT_TARGET_RANDOM,0);
                        if (target && Nefarian)
                            Nefarian->AI()->AttackStart(target);
                        else DoYell("UNABLE TO SPAWN NEF PROPERLY",LANG_UNIVERSAL,NULL);
                    }

                    AddSpawnTimer = 10000;
                }else AddSpawnTimer -= diff;
            }
            else if (Nefarian)
            {
                //If nef is dead then we die to so the players get out of combat
                //and cannot repeat the event
                if (!Nefarian->isAlive())
                {
                    Nefarian = NULL;
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), DIRECT_DAMAGE, 0, NULL, 0, false);
                }

            }
        }
    }
};

CreatureAI* GetAI_boss_victor_nefarius(Creature *_Creature)
{
    return new boss_victor_nefariusAI (_Creature);
}

bool GossipSelect_boss_victor_nefarius(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)//Fight time
    {
        player->PlayerTalkClass->CloseGossip();
        ((boss_victor_nefariusAI*)_Creature->AI())->BeginEvent(player);
    }

    return true;
}

bool GossipHello_boss_victor_nefarius(Player *player, Creature *_Creature)
{
    player->ADD_GOSSIP_ITEM( 0, GOSSIP_ITEM , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->PlayerTalkClass->SendGossipMenu(907,_Creature->GetGUID());

    return true;
}

void AddSC_boss_victor_nefarius()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_victor_nefarius";
    newscript->GetAI = GetAI_boss_victor_nefarius;
    newscript->pGossipHello = &GossipHello_boss_victor_nefarius;
    newscript->pGossipSelect = &GossipSelect_boss_victor_nefarius;
    m_scripts[nrscripts++] = newscript;
}