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
SDName: Borean_Tundra
SD%Complete: 100
SDComment: Quest support: 11570, 11590, 11692, 11676, 11708, 11865, 11919, 11940, 11961, 12998. Taxi vendors. 
SDCategory: Borean Tundra
EndScriptData */

/* ContentData
npc_fizzcrank_fullthrottle
npc_iruk
npc_kara_thricestar
npc_nesingwary_trapper
go_caribou_trap
npc_surristrasz
npc_tiare
npc_lurgglbr
npc_nexus_drake
go_heart_of_the_storm
npc_overseer_narvir
go_scourge_cage
npc_beryl_sorcerer
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "follower_ai.h"

/*######
## npc_fizzcrank_fullthrottle
######*/

#define GOSSIP_ITEM_GO_ON   "Go on."
#define GOSSIP_ITEM_TELL_ME "Tell me what's going on out here, Fizzcrank."

enum
{
    GOSSIP_TEXTID_FIZZCRANK1    = 12456,
    GOSSIP_TEXTID_FIZZCRANK2    = 12457,
    GOSSIP_TEXTID_FIZZCRANK3    = 12458,
    GOSSIP_TEXTID_FIZZCRANK4    = 12459,
    GOSSIP_TEXTID_FIZZCRANK5    = 12460,
    GOSSIP_TEXTID_FIZZCRANK6    = 12461,
    GOSSIP_TEXTID_FIZZCRANK7    = 12462,
    GOSSIP_TEXTID_FIZZCRANK8    = 12463,
    GOSSIP_TEXTID_FIZZCRANK9    = 12464,

    QUEST_THE_MECHAGNOMES       = 11708
};

bool GossipHello_npc_fizzcrank_fullthrottle(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_THE_MECHAGNOMES) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELL_ME, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_fizzcrank_fullthrottle(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK1, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK3, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK4, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK5, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK6, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK7, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+8:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK8, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+9:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FIZZCRANK9, pCreature->GetGUID());
            pPlayer->AreaExploredOrEventHappens(QUEST_THE_MECHAGNOMES);
            break;
    }
    return true;
}

/*######
## npc_iruk
######*/

#define GOSSIP_ITEM_IRUK        "<Search corpse for Issliruk's Totem.>"

enum
{
    QUEST_SPIRITS_WATCH_OVER_US     = 11961,
    SPELL_CREATE_TOTEM              = 46816
};

bool GossipHello_npc_iruk(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_SPIRITS_WATCH_OVER_US) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_IRUK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_iruk(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,SPELL_CREATE_TOTEM,true);
    }

    return true;
}

/*######
## npc_kara_thricestar
######*/

#define GOSSIP_ITEM_THRICESTAR1      "Do you think I could take a ride on one of those flying machines?"
#define GOSSIP_ITEM_THRICESTAR2      "Kara, I need to be flown out the Dens of Dying to find Bixie."

enum
{
    QUEST_CHECK_IN_WITH_BIXIE       = 11692,
    SPELL_FIZZCRANK_AIRSTRIP        = 51446
};

bool GossipHello_npc_kara_thricestar(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isTaxi())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_ITEM_THRICESTAR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetQuestStatus(QUEST_CHECK_IN_WITH_BIXIE) == QUEST_STATUS_COMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THRICESTAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_kara_thricestar(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->GetSession()->SendTaxiMenu(pCreature);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_FIZZCRANK_AIRSTRIP, false);
            break;
    }

    return true;
}

/*######
## npc_nesingwary_trapper
######*/

enum
{
    NPC_NESINGWARY_TRAPPER  = 25835,
    GO_QUALITY_FUR          = 187983,

    SAY_PHRASE_1            = -1000599,
    SAY_PHRASE_2            = -1000600,
    SAY_PHRASE_3            = -1000601,
    SAY_PHRASE_4            = -1000602
};

struct MANGOS_DLL_DECL npc_nesingwary_trapperAI : public ScriptedAI
{
    npc_nesingwary_trapperAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint64 m_uiPlayerGUID;
    uint64 m_uiGobjectTrapGUID;

    void Reset()
    {
        m_uiPhase = 0;
        m_uiPhaseTimer = 0;
        m_uiPlayerGUID = 0;
        m_uiGobjectTrapGUID = 0;
    }

    void StartAction(uint64 uiPlayerGUID, uint64 uiGoTrapGUID)
    {
        m_uiPhase = 1;
        m_uiPhaseTimer = 3000;
        m_uiPlayerGUID = uiPlayerGUID;
        m_uiGobjectTrapGUID = uiGoTrapGUID;

        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_PHRASE_1, m_creature); break;
            case 1: DoScriptText(SAY_PHRASE_2, m_creature); break;
            case 2: DoScriptText(SAY_PHRASE_3, m_creature); break;
            case 3: DoScriptText(SAY_PHRASE_4, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->getVictim() && m_uiPhase)
        {
            if (m_uiPhaseTimer <= uiDiff)
            {
                switch(m_uiPhase)
                {
                    case 1:
                        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_uiGobjectTrapGUID))
                        {
                            if (pTrap->isSpawned())
                                m_creature->GetMotionMaster()->MovePoint(0, pTrap->GetPositionX(), pTrap->GetPositionY(), pTrap->GetPositionZ());
                        }
                        break;
                    case 2:
                        if (GameObject* pTrap = m_creature->GetMap()->GetGameObject(m_uiGobjectTrapGUID))
                        {
                            if (pTrap->isSpawned())
                            {
                                pTrap->Use(m_creature);

                                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
                                {
                                    if (pPlayer->isAlive())
                                        pPlayer->KilledMonsterCredit(m_creature->GetEntry());
                                }
                            }
                        }
                        break;
                }

                m_uiPhase = 0;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        m_creature->HandleEmote(EMOTE_ONESHOT_LOOT);
        m_uiPhaseTimer = 2000;
        m_uiPhase = 2;
    }
};

CreatureAI* GetAI_npc_nesingwary_trapper(Creature* pCreature)
{
    return new npc_nesingwary_trapperAI(pCreature);
}

/*######
## go_caribou_trap
######*/

bool GOUse_go_caribou_trap(Player* pPlayer, GameObject* pGo)
{
    float fX, fY, fZ;
    pGo->GetClosePoint(fX, fY, fZ, pGo->GetObjectBoundingRadius(), 2*INTERACTION_DISTANCE, frand(0, M_PI_F*2));

    if (Creature* pCreature = pGo->SummonCreature(NPC_NESINGWARY_TRAPPER, fX, fY, fZ, pGo->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10000))
    {
        if (npc_nesingwary_trapperAI* pTrapperAI = dynamic_cast<npc_nesingwary_trapperAI*>(pCreature->AI()))
            pTrapperAI->StartAction(pPlayer->GetGUID(), pGo->GetGUID());

        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

        if (GameObject* pGoFur = GetClosestGameObjectWithEntry(pGo, GO_QUALITY_FUR, INTERACTION_DISTANCE))
        {
            if (!pGoFur->isSpawned())
            {
                pGoFur->SetRespawnTime(10);
                pGoFur->Refresh();
            }
        }
    }

    return true;
}

/*######
## npc_surristrasz
######*/

#define GOSSIP_ITEM_FREE_FLIGHT "I'd like passage to the Transitus Shield."
#define GOSSIP_ITEM_FLIGHT      "May I use a drake to fly elsewhere?"

enum
{
    SPELL_ABMER_TO_COLDARRA     = 46064
};

bool GossipHello_npc_surristrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isTaxi())
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FREE_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_GOSSIP);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_ITEM_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_TAXIVENDOR);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_surristrasz(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_OPTION_GOSSIP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        //TaxiPath 795 (amber to coldarra)
        pPlayer->CastSpell(pPlayer, SPELL_ABMER_TO_COLDARRA, true);
    }

    if (uiAction == GOSSIP_OPTION_TAXIVENDOR)
        pPlayer->GetSession()->SendTaxiMenu(pCreature);

    return true;
}

/*######
## npc_tiare
######*/

#define GOSSIP_ITEM_TELEPORT    "Teleport me to Amber Ledge, please."

enum
{
    SPELL_TELEPORT_COLDARRA     = 50135
};

bool GossipHello_npc_tiare(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_GOSSIP);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_tiare(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_OPTION_GOSSIP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_TELEPORT_COLDARRA, true);
    }
    return true;
}

/*######
## npc_lurgglbr
######*/

enum
{
    QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS = 11570,
    GO_CAGE                             = 187369,

    SAY_START_1                         = -1000575,
    SAY_START_2                         = -1000576,
    SAY_END_1                           = -1000577,
    SAY_END_2                           = -1000578
};

struct MANGOS_DLL_DECL npc_lurgglbrAI : public npc_escortAI
{
    npc_lurgglbrAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiSayTimer = 0;
        m_uiSpeech = 0;
        Reset();
    }

    uint32 m_uiSayTimer;
    uint8 m_uiSpeech;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSayTimer = 0;
            m_uiSpeech = 0;
        }
    }

    void JustStartedEscort()
    {
        if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_CAGE, INTERACTION_DISTANCE))
        {
            if (pCage->GetGoState() == GO_STATE_READY)
                pCage->Use(m_creature);
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_START_2, m_creature, pPlayer);

                // Cage actually closes here, however it's normally determined by GO template and auto close time

                break;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    m_creature->SetFacingToObject(pPlayer);
                    DoScriptText(SAY_START_1, m_creature, pPlayer);
                }
                break;
            case 25:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_END_1, m_creature, pPlayer);
                    m_uiSayTimer = 3000;
                }
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiSayTimer)
            {
                if (m_uiSayTimer <= uiDiff)
                {
                    Player* pPlayer = GetPlayerForEscort();

                    if (!pPlayer)
                    {
                        m_uiSayTimer = 0;
                        return;
                    }

                    m_creature->SetFacingToObject(pPlayer);

                    switch(m_uiSpeech)
                    {
                        case 0:
                            DoScriptText(SAY_END_2, m_creature, pPlayer);
                            m_uiSayTimer = 3000;
                            break;
                        case 1:
                            pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS, m_creature);
                            m_uiSayTimer = 0;
                            break;
                    }

                    ++m_uiSpeech;
                }
                else
                    m_uiSayTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_lurgglbr(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_WINTERFIN_CAVERNS)
    {
        if (npc_lurgglbrAI* pEscortAI = dynamic_cast<npc_lurgglbrAI*>(pCreature->AI()))
        {
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_lurgglbr(Creature* pCreature)
{
    return new npc_lurgglbrAI(pCreature);
}

/*######
## npc_nexus_drake_hatchling
######*/

enum
{
    SPELL_DRAKE_HARPOON             = 46607,
    SPELL_RED_DRAGONBLOOD           = 46620,
    SPELL_DRAKE_HATCHLING_SUBDUED   = 46691,
    SPELL_SUBDUED                   = 46675,

    NPC_RAELORASZ                   = 26117,
    DRAKE_HUNT_KILL_CREDIT          = 26175,

    QUEST_DRAKE_HUNT                = 11919,
    QUEST_DRAKE_HUNT_D              = 11940

};

struct MANGOS_DLL_DECL npc_nexus_drakeAI : public FollowerAI
{
    npc_nexus_drakeAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }
    
     uint64 uiHarpoonerGUID;
     bool bWithRedDragonBlood;
     bool bIsFollowing;

     void Reset()
     {
         bWithRedDragonBlood = false;
         bIsFollowing = false;
     }

     void EnterCombat(Unit* pWho)
     {
         AttackStart(pWho);
     }
     
     void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
     {
            if (pSpell->Id == SPELL_DRAKE_HARPOON && pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                uiHarpoonerGUID = pCaster->GetGUID();
                DoCast(m_creature, SPELL_RED_DRAGONBLOOD, true);
            }
            m_creature->Attack(pCaster,true);
            bWithRedDragonBlood = true;
     }

     void MoveInLineOfSight(Unit *pWho)
     {
         FollowerAI::MoveInLineOfSight(pWho);


         if (pWho->GetEntry() == NPC_RAELORASZ && m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
         {
           if (Player *pHarpooner = m_creature->GetMap()->GetPlayer(uiHarpoonerGUID))
                 {
                    
                     pHarpooner->KilledMonsterCredit(DRAKE_HUNT_KILL_CREDIT,m_creature->GetGUID());
                     pHarpooner->RemoveAurasByCasterSpell(SPELL_DRAKE_HATCHLING_SUBDUED,uiHarpoonerGUID);
                     SetFollowComplete();
                     uiHarpoonerGUID = 0;
                     m_creature->ForcedDespawn(1000);
                 }
              
          }
      }
     
     void UpdateAI(const uint32 uidiff)
        {
            if (bWithRedDragonBlood && uiHarpoonerGUID && !m_creature->HasAura(SPELL_RED_DRAGONBLOOD))
            {
                if (Player *pHarpooner = m_creature->GetMap()->GetPlayer(uiHarpoonerGUID))
                {
                    EnterEvadeMode();
                    StartFollow(pHarpooner, 35, NULL);

                    DoCast(m_creature, SPELL_SUBDUED, true);
                    pHarpooner->CastSpell(pHarpooner, SPELL_DRAKE_HATCHLING_SUBDUED, true);

                    m_creature->AttackStop();
                    bIsFollowing = true;
                    bWithRedDragonBlood = false;
                }
            }
            if(bIsFollowing && !m_creature->HasAura(SPELL_SUBDUED))
            {
                m_creature->ForcedDespawn(1000);
            }

            if (!m_creature->getVictim())
                return;

            DoMeleeAttackIfReady();
        }
};

CreatureAI* GetAI_npc_nexus_drake(Creature* pCreature)
{
    return new npc_nexus_drakeAI(pCreature);
}

/*#####
## go_scourge_cage
#####*/

enum
{
    QUEST_MERCIFUL_FREEDOM      =  11676,
    NPC_SCOURGE_PRISONER        =  25610,
};

bool GOUse_go_scourge_cage(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_MERCIFUL_FREEDOM) == QUEST_STATUS_INCOMPLETE)
    {
        Creature *pCreature = GetClosestCreatureWithEntry(pGo, NPC_SCOURGE_PRISONER, INTERACTION_DISTANCE);
        if(pCreature)
        {
            pPlayer->KilledMonsterCredit(NPC_SCOURGE_PRISONER, pCreature->GetGUID());
            pCreature->CastSpell(pCreature, 43014, false);
        }
    }
    return false;
};

/*######
## npc_beryl_sorcerer
######*/

enum eBerylSorcerer
{
    NPC_CAPTURED_BERLY_SORCERER         = 25474,
    NPC_LIBRARIAN_DONATHAN              = 25262,

    SPELL_ARCANE_CHAINS                 = 45611,
    SPELL_COSMETIC_CHAINS               = 54324,
    SPELL_COSMETIC_ENSLAVE_CHAINS_SELF  = 45631
};

struct MANGOS_DLL_DECL npc_beryl_sorcererAI : public FollowerAI
{
    npc_beryl_sorcererAI(Creature* pCreature) : FollowerAI(pCreature) { 
        Reset(); 
        m_uiNormalFaction = pCreature->getFaction();
    }

    bool bEnslaved;
    uint64 uiChainerGUID;
    uint32 m_uiNormalFaction;

    void Reset()
    {
         m_creature->setFaction(m_uiNormalFaction);
         bEnslaved = false;
    }
    void EnterCombat(Unit* pWho)
    {
         AttackStart(pWho);
    }
    
    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_ARCANE_CHAINS && pCaster->GetTypeId() == TYPEID_PLAYER && !bEnslaved)
            {
                EnterEvadeMode(); //We make sure that the npc is not attacking the player!
                m_creature->setFaction(35);
                uiChainerGUID = pCaster->GetGUID();
                if(Player *pChainer = m_creature->GetMap()->GetPlayer(uiChainerGUID))
                {
                StartFollow(pChainer, 35, NULL);
                m_creature->UpdateEntry(NPC_CAPTURED_BERLY_SORCERER);
                DoCast(m_creature, SPELL_COSMETIC_ENSLAVE_CHAINS_SELF, true);
               
                bEnslaved = true;
                }
            }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
            FollowerAI::MoveInLineOfSight(pWho);

            if (pWho->GetEntry() == NPC_LIBRARIAN_DONATHAN && m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if(Player *pChainer = m_creature->GetMap()->GetPlayer(uiChainerGUID))
                {
                    pChainer->KilledMonsterCredit(NPC_CAPTURED_BERLY_SORCERER,m_creature->GetGUID());
                    SetFollowComplete();
                    m_creature->ForcedDespawn(1000);
                }
            }
     }
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->getVictim())
                return;

            DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_npc_beryl_sorcerer(Creature* pCreature)
{
    return new npc_beryl_sorcererAI(pCreature);
}

/*#####
## go_heart_of_the_storm
#####*/

enum
{
    QUEST_HEART_OF_THE_STORM = 12998,
    SPELL_STORMS_FURY        = 56485,
    NPC_OVERSEER_NARVIR      = 30299,
    DESPAWN_TIMER            = 30000,
    SAY_NARVIR1              = -1532116,
    SAY_NARVIR2              = -1532117
};

bool GOUse_go_heart_of_the_storm(Player* pPlayer, GameObject* pGo)
{
    Creature* pNarvir = GetClosestCreatureWithEntry(pPlayer, NPC_OVERSEER_NARVIR, 25.0f);
    if (pNarvir)
        return true;

    if (pPlayer->GetQuestStatus(QUEST_HEART_OF_THE_STORM) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pNarvir = pPlayer->SummonCreature(NPC_OVERSEER_NARVIR, 7315.48f, -711.069f, 791.611f, 4.65591f, TEMPSUMMON_TIMED_DESPAWN, DESPAWN_TIMER) )

        {
            pNarvir->CastSpell(pPlayer, SPELL_STORMS_FURY, false);
			pGo->DestroyForPlayer(pPlayer, false);
            pPlayer->KilledMonsterCredit(NPC_OVERSEER_NARVIR, pNarvir->GetGUID() );
        }
    }
    return true;
};

struct MANGOS_DLL_DECL npc_overseer_narvir : public ScriptedAI
{
    npc_overseer_narvir(Creature*pCreature) : ScriptedAI(pCreature) { Reset();}

    bool MovementStarted;
    uint32 uiTimer;
    uint32 uiPhase;


    void Reset () 
    {
    uiTimer = 1000;
    uiPhase = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiTimer<= uiDiff)
        {

            switch(uiPhase)
            {
            case 0: m_creature->GetMotionMaster()->MovePoint(1, 7314.01f, -727.78f, 791.611f);
                  uiTimer = 3000;
                  uiPhase++;
                  break;
            case 1: DoScriptText(SAY_NARVIR1, m_creature);
                uiTimer = 6000;
                uiPhase++;
                break;
            case 2: DoScriptText(SAY_NARVIR2, m_creature);
                uiTimer = 4000;
                uiPhase++;
                break;
            case 3: m_creature->ForcedDespawn(1000);
                uiTimer = 0;
                uiPhase++;
                break;
            }
        }
        else uiTimer -= uiDiff;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || !MovementStarted) 
            return;
    }
};

CreatureAI* GetAI_npc_overseer_narvir(Creature* pCreature)
{
    return new npc_overseer_narvir (pCreature);
}

/**** Quest: Last Rites ****/
enum
{
    /************* Speech and Sounds ***********************/
    SAY_DIA01           = -1534071,
    SAY_DIA02           = -1534072,
    SAY_DIA03           = -1534073,
    SAY_DIA04           = -1534074,
    SAY_DIA05           = -1534075,
    SAY_DIA06           = -1534076,
    SAY_DIA07           = -1534077,
    SAY_DIA08           = -1534078,
    SAY_DIA09           = -1534079,
    SAY_DIA10           = -1534080,
    SAY_DIA11           = -1534081,
    SAY_DIA12           = -1534082,
    SAY_DIA13           = -1534083,
    SAY_DIA14           = -1534084,
    SAY_DIA15           = -1534085,
    SAY_DIA16           = -1534086,
    SAY_DIA17           = -1534087,
    SAY_DIA18           = -1534088,
    SAY_DIA19           = -1534089,
    SAY_DIA20           = -1534090,

    /************* Npc's ***********************/
    NPC_THASSARIAN      = 26170,
    NPC_LICH_KING       = 26203,
    NPC_LERYSSA         = 25251,
    NPC_ARLOS           = 25250,
    NPC_VALANAR         = 28189,
    NPC_VISUAL_FORM     = 25301,

    /************* Other's ***********************/
    QUEST_LAST_RITES    = 12019,
    STAFF_ID            = 485,
    AREA_ID             = 4128,
};

#define GOSSIP_ITEM_T   "Let's do this, Thassarian. It's now or never."

struct MANGOS_DLL_DECL npc_thassarianAI : public ScriptedAI
{
    npc_thassarianAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }

    uint32 m_uiStepTimer;
    uint32 m_uiStep;

    uint64 m_uiValanarGUID;
    uint64 m_uiLichKingGUID;
    uint64 m_uiLeryssaGUID;
    uint64 m_uiArlosGUID;

    void Reset() 
    {  
        m_uiValanarGUID = 0;
        m_uiLichKingGUID = 0;
        m_uiLeryssaGUID = 0;
        m_uiArlosGUID = 0;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        if (m_creature->GetAreaId() == AREA_ID)
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_uiStep = 0;
        m_uiStepTimer = 100;
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
            Kill(pLeryssa);
        if (Creature* pArlos = m_creature->GetMap()->GetCreature(m_uiArlosGUID))
            Kill(pArlos);
        if (Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
            pValanar->RemoveFromWorld();
    }

    void EnterEvadeMode()
    {
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        m_creature->SetLootRecipient(NULL);

        if (m_creature->isAlive() && m_uiStep > 20)
        {
            m_creature->GetMotionMaster()->MoveTargetedHome();
            Reset();
        }
    }

    void StartEvent(uint8 EventId)
    {   
        m_uiStep = EventId;
        m_uiStepTimer = 100;
    }

    void JumpNextStep(uint32 Time)
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

    void Kill(Creature* pVictim)
    {
        pVictim->DealDamage(pVictim, pVictim->GetMaxHealth() ,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 1:
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                m_creature->GetMotionMaster()->MovePoint(0, 3712.466f, 3570.2911f, 477.443f);
                JumpNextStep(4000);
                break;
            case 2:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                if (Creature* pLichKing = m_creature->SummonCreature(NPC_LICH_KING,3730.15f,3519.43f,473.32f,1.37f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30))
                {
                    m_uiLichKingGUID = pLichKing->GetGUID();
                    pLichKing->GetMotionMaster()->MovePoint(0, 3738.691f, 3564.734f, 477.442f);
                }
                JumpNextStep(3000);
                break;
            case 3:
                if (Creature* pValanar = m_creature->SummonCreature(NPC_VALANAR,3748.80f,3615.27f,473.3232f,4.50f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,60000))
                {
                    m_uiValanarGUID = pValanar->GetGUID();
                    pValanar->SetRespawnDelay(DAY);
                    pValanar->GetMotionMaster()->MovePoint(0, 3739.823f, 3570.564f, 477.442f);
                    pValanar->UpdateEntry(NPC_VISUAL_FORM); //huck! Replace spell (spell "Valanar Transformation" don't work!) He must update Valanar Entry and Change Name!
                    pValanar->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    pValanar->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    pValanar->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(0));
                }
                JumpNextStep(20000);
                break;
            case 4:
                if (Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
                {
                    pValanar->UpdateEntry(NPC_VALANAR);
                    pValanar->SetStandState(UNIT_STAND_STATE_KNEEL);
                    pValanar->LoadEquipment(STAFF_ID); 
                    DoScriptText(SAY_DIA01, pValanar);
                }
                if (Creature* pLeryssa = m_creature->SummonCreature(NPC_LERYSSA,3753.105f,3614.296f,473.32f,1.37f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10))
                {  
                    m_uiLeryssaGUID = pLeryssa->GetGUID();
                    pLeryssa->GetMotionMaster()->MovePoint(0, 3745.636f, 3572.464f, 477.442f);
                    pLeryssa->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    pLeryssa->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
                if (Creature* pArlos = m_creature->SummonCreature(NPC_ARLOS,3744.82f,3616.11f,473.32f,1.37f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10))
                {
                    m_uiArlosGUID = pArlos->GetGUID();
                    pArlos->GetMotionMaster()->MovePoint(0, 3734.854f, 3573.028f, 477.442f);
                    pArlos->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    pArlos->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
                JumpNextStep(8000);
                break;
            case 5:
                if (Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
                    DoScriptText(SAY_DIA02, pValanar);
                JumpNextStep(11000);
                break;
            case 6:
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                    pLeryssa->CastSpell(pLeryssa,45066,false);
                if (Creature* pArlos = m_creature->GetMap()->GetCreature(m_uiArlosGUID))
                    pArlos->CastSpell(pArlos,45066,false);
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_DIA03, pLichKing);
                JumpNextStep(9000);
                break;
            case 7:
                m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                m_creature->GetMotionMaster()->MovePoint(0, 3722.271f, 3566.878f, 477.442f);
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    m_creature->SetFacingToObject(pLichKing);
                DoScriptText(SAY_DIA04, m_creature);
                JumpNextStep(2000);
                break;
            case 8:
                if (Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
                    pValanar->SetStandState(UNIT_STAND_STATE_STAND);
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    pLichKing->SetFacingToObject(m_creature);
                DoScriptText(SAY_DIA05, m_creature);
                JumpNextStep(6000);
                break;
            case 9:
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_DIA06, pLichKing);
                JumpNextStep(17000);
                break;
            case 10:
                DoScriptText(SAY_DIA07, m_creature);
                JumpNextStep(9000);
                break;
            case 11:
                if (Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
                    DoScriptText(SAY_DIA08, pValanar);
                JumpNextStep(6000);
                break;
            case 12:
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                {
                    if(Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
                    {
                        pLichKing->SetFacingToObject(pValanar);
                        DoScriptText(SAY_DIA09, pLichKing);
                    }
                }
                JumpNextStep(4000);
                break;
            case 13:
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    pLichKing->SetFacingToObject(m_creature);
                JumpNextStep(500);
                break;
            case 14:
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    pLichKing->HandleEmoteCommand(25);
                JumpNextStep(5500);
                break;
            case 15:
                if (Creature* pValanar = m_creature->GetMap()->GetCreature(m_uiValanarGUID))
                {
                    DoScriptText(SAY_DIA10, pValanar);
                    pValanar->SetFacingToObject(m_creature);
                    pValanar->setFaction(14); 
                    m_creature->AI()->AttackStart(pValanar);
                }
                if (Creature* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    Kill(pLichKing);              
                JumpNextStep(500);
                break;
            case 17:
                EnterEvadeMode();
                m_creature->GetMotionMaster()->MovePoint(0, 3721.361f, 3566.910f, 477.442f);
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, UNIT_STAND_STATE_STAND); 
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                {
                    DoScriptText(SAY_DIA14, pLeryssa);
                    pLeryssa->RemoveAurasDueToSpell(45066);
                }
                if (Creature* pArlos = m_creature->GetMap()->GetCreature(m_uiArlosGUID))
                    DoScriptText(SAY_DIA11, pArlos);
                JumpNextStep(3000);
                break;
            case 18:
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                {
                    pLeryssa->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pLeryssa->GetMotionMaster()->MovePoint(0, 3726.749f, 3568.097f, 477.442f);
                    pLeryssa->SetFacingToObject(m_creature);
                    pLeryssa->SetStandState(UNIT_STAND_STATE_SIT);                            
                    m_creature->SetFacingToObject(pLeryssa);
                    m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                }
                if (Creature* pArlos = m_creature->GetMap()->GetCreature(m_uiArlosGUID))
                {
                    DoScriptText(SAY_DIA12, pArlos);                                        
                    pArlos->RemoveAurasDueToSpell(45066);
                    pArlos->SetStandState(UNIT_STAND_STATE_DEAD);
                }
                JumpNextStep(5000);
                break;
            case 19: 
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                    DoScriptText(SAY_DIA15, pLeryssa);
                JumpNextStep(5000);
                break;
            case 20:
                DoScriptText(SAY_DIA13, m_creature);
                JumpNextStep(3000);
                break;
            case 21:
                DoScriptText(SAY_DIA16, m_creature);
                JumpNextStep(6000);
                break;
            case 22:
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                    DoScriptText(SAY_DIA17, pLeryssa);
                JumpNextStep(9000);
                break;
            case 23:
                DoScriptText(SAY_DIA18, m_creature);
                JumpNextStep(9000);
                break;
            case 24:
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                    DoScriptText(SAY_DIA19, pLeryssa);
                JumpNextStep(8000);
                break;
            case 25:
                DoScriptText(SAY_DIA20, m_creature);
                JumpNextStep(6000);
                break;
            case 26:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                JumpNextStep(60000);
                break;
            case 27:
                if (Creature* pLeryssa = m_creature->GetMap()->GetCreature(m_uiLeryssaGUID))
                    Kill(pLeryssa);
                if (Creature* pArlos = m_creature->GetMap()->GetCreature(m_uiArlosGUID))
                    Kill(pArlos);
                JumpNextStep(6000);
                EnterEvadeMode();
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
         }
    }

    void UpdateAI(const uint32 uiDiff)
    {
         if (m_uiStepTimer <= uiDiff)
             Event();
         else 
             m_uiStepTimer -= uiDiff;

         if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

         DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_thassarian(Creature* pCreature)
{
    return new npc_thassarianAI(pCreature);
}

/*Prince Valanar AI*/

enum
{
   SPELL_SHADOW_BOLT       = 15537,
   SPELL_SOUL_BLAST        = 50992,
   SPELL_SOUL_MIRROR       = 51009
};

struct MANGOS_DLL_DECL npc_valanarAI : public ScriptedAI
{
    npc_valanarAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        SetCombatMovement(false);
    }

    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShadowBlastTimer;
    uint32 m_uiSoulTimer;

    void Reset() 
    {
        m_uiShadowBoltTimer = 1300;
        m_uiShadowBlastTimer = 7300;
        m_uiSoulTimer = 17300;
    }
 
    void JustDied(Unit* pKiller)
    {
        if (Creature* pTas = GetClosestCreatureWithEntry(m_creature, NPC_THASSARIAN, 80.0f))
           ((npc_thassarianAI*)pTas->AI())->StartEvent(17);
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_SOUL_BLAST)
           m_creature->DealDamage(target, target->GetMaxHealth() / 5,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        if (done_by->HasAura(SPELL_SOUL_MIRROR))
           m_creature->DealDamage(done_by, damage ,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShadowBoltTimer < diff)
        {
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
               DoCast(pTarget, SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = 3300;
        }
        else 
            m_uiShadowBoltTimer -= diff;

        if (m_uiShadowBlastTimer < diff)
        {
           m_creature->InterruptNonMeleeSpells(false);
           if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
              DoCast(pTarget,SPELL_SOUL_BLAST);
           m_uiShadowBlastTimer = urand(6000, 12000);
        }
        else 
            m_uiShadowBlastTimer -= diff;

        if (m_uiSoulTimer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(),SPELL_SOUL_MIRROR);
            m_uiSoulTimer = urand(15000, 20000);
        }
        else 
            m_uiSoulTimer -= diff;
    }
};

bool GossipHello_npc_thassarian(Player *player, Creature *pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_INCOMPLETE)
        if(((npc_thassarianAI*)pCreature->AI())->m_uiStep == 0 && pCreature->GetAreaId() == AREA_ID)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_T, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(pCreature),pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_thassarian(Player *player, Creature *pCreature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
          ((npc_thassarianAI*)pCreature->AI())->StartEvent(1);
          player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

CreatureAI* GetAI_npc_valanar(Creature* pCreature)
{
    return new npc_valanarAI(pCreature);
}

void AddSC_borean_tundra()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_fizzcrank_fullthrottle";
    newscript->pGossipHello = &GossipHello_npc_fizzcrank_fullthrottle;
    newscript->pGossipSelect = &GossipSelect_npc_fizzcrank_fullthrottle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_iruk";
    newscript->pGossipHello = &GossipHello_npc_iruk;
    newscript->pGossipSelect = &GossipSelect_npc_iruk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kara_thricestar";
    newscript->pGossipHello = &GossipHello_npc_kara_thricestar;
    newscript->pGossipSelect = &GossipSelect_npc_kara_thricestar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_nesingwary_trapper";
    newscript->GetAI = &GetAI_npc_nesingwary_trapper;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_caribou_trap";
    newscript->pGOUse = &GOUse_go_caribou_trap;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_surristrasz";
    newscript->pGossipHello = &GossipHello_npc_surristrasz;
    newscript->pGossipSelect = &GossipSelect_npc_surristrasz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tiare";
    newscript->pGossipHello = &GossipHello_npc_tiare;
    newscript->pGossipSelect = &GossipSelect_npc_tiare;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lurgglbr";
    newscript->GetAI = &GetAI_npc_lurgglbr;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_lurgglbr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_nexus_drake";
    newscript->GetAI = &GetAI_npc_nexus_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_scourge_cage";
    newscript->pGOUse = &GOUse_go_scourge_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_beryl_sorcerer";
    newscript->GetAI = &GetAI_npc_beryl_sorcerer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_heart_of_the_storm";
    newscript->pGOUse = &GOUse_go_heart_of_the_storm;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "npc_overseer_narvir";
    newscript->GetAI = &GetAI_npc_overseer_narvir;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_valanar";
    newscript->GetAI = &GetAI_npc_valanar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_thassarian";
    newscript->GetAI = &GetAI_npc_thassarian;
    newscript->pGossipHello = &GossipHello_npc_thassarian;
    newscript->pGossipSelect = &GossipSelect_npc_thassarian;
    newscript->RegisterSelf();
}
