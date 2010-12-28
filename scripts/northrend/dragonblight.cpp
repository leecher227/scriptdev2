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
SDName: Dragonblight
SD%Complete: 100
SDComment: Quest support: 12166, 12499/12500(end sequenze). Taxi paths Wyrmrest temple.
SDCategory: Dragonblight
EndScriptData */

/* ContentData
npc_afrasastrasz
npc_alexstrasza_wr_gate
npc_liquid_fire_of_elune
npc_tariolstrasz
npc_torastrasza
EndContentData */

#include "precompiled.h"

/*######
## npc_afrasastrasz
######*/

enum
{
    TAXI_PATH_ID_MIDDLE_DOWN            = 882,
    TAXI_PATH_ID_MIDDLE_TOP             = 881
};

#define GOSSIP_ITEM_TAXI_MIDDLE_DOWN    "I would like to take a flight to the ground, Lord Of Afrasastrasz."
#define GOSSIP_ITEM_TAXI_MIDDLE_TOP     "My Lord, I must go to the upper floor of the temple."

bool GossipHello_npc_afrasastrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_MIDDLE_DOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_MIDDLE_TOP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_afrasastrasz(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_MIDDLE_DOWN);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_MIDDLE_TOP);
    }
    return true;
}

/*######
## npc_alexstrasza_wr_gate
######*/

enum
{
    QUEST_RETURN_TO_AG_A    = 12499,
    QUEST_RETURN_TO_AG_H    = 12500,
    MOVIE_ID_GATES          = 14
};

#define GOSSIP_ITEM_WHAT_HAPPENED   "Alexstrasza, can you show me what happened here?"

bool GossipHello_npc_alexstrasza_wr_gate(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestRewardStatus(QUEST_RETURN_TO_AG_A) || pPlayer->GetQuestRewardStatus(QUEST_RETURN_TO_AG_H))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_HAPPENED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_alexstrasza_wr_gate(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->SendMovieStart(MOVIE_ID_GATES);
    }

    return true;
}

/*######
## npc_tariolstrasz
######*/

enum
{
    QUEST_INFORM_QUEEN_A                = 12123,            //need to check if quests are required before gossip available
    QUEST_INFORM_QUEEN_H                = 12124,
    TAXI_PATH_ID_BOTTOM_TOP             = 878,
    TAXI_PATH_ID_BOTTOM_MIDDLE          = 883
};

#define GOSSIP_ITEM_TAXI_BOTTOM_TOP     "My Lord, I must go to the upper floor of the temple."
#define GOSSIP_ITEM_TAXI_BOTTOM_MIDDLE  "Can you spare a drake to travel to Lord Of Afrasastrasz, in the middle of the temple?"

bool GossipHello_npc_tariolstrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_BOTTOM_TOP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_BOTTOM_MIDDLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_tariolstrasz(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_BOTTOM_TOP);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_BOTTOM_MIDDLE);
    }
    return true;
}

/*######
## npc_torastrasza
######*/

enum
{
    TAXI_PATH_ID_TOP_MIDDLE             = 880,
    TAXI_PATH_ID_TOP_BOTTOM             = 879
};

#define GOSSIP_ITEM_TAXI_TOP_MIDDLE     "I would like to see Lord Of Afrasastrasz, in the middle of the temple."
#define GOSSIP_ITEM_TAXI_TOP_BOTTOM     "Yes, Please. I would like to return to the ground floor of the temple."

bool GossipHello_npc_torastrasza(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_TOP_MIDDLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_TOP_BOTTOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_torastrasza(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_TOP_MIDDLE);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_TOP_BOTTOM);
    }
    return true;
}

/*### Quest: My Old Enemy ###
SDAuthor: MaxXx2021 aka Mioka
SDComplete: 98%
*/ 

enum
{
    //Abbendis
    SAY_ABBENDIS_YELL_01         = -1999423,
    SAY_ABBENDIS_YELL_02         = -1999424,
    SAY_ABBENDIS_YELL_03         = -1999425,
    SAY_ABBENDIS_YELL_04         = -1999426,
    SAY_ABBENDIS_50HP            = -1999427,

    SPELL_FIRE_LAND              = 50915,
    SPELL_ARMOR_SEAL             = 50905,
    SPELL_BUFF                   = 50908,

    //Westwind
    SAY_WESTWIND_01              = -1999420,
    SAY_WESTWIND_02              = -1999421,
    SAY_WESTWIND_03              = -1999422,

    SPELL_SHIELD                 = 50161,

    NPC_ABBENDIS                 = 27210,
    NPC_WESTWIND                 = 27951,
    NPC_RAVEN_PRIEST             = 27202,
    NPC_SCARLET_WARRIOR          = 27203,
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations AbbendisSummonPos[]=
{
    {2673.357f, -342.697f, 141.218f},
    {2681.778f, -341.368f, 141.218f},
    {2689.731f, -340.671f, 141.218f},

    {2694.241f, -370.571f, 141.218f},
    {2686.296f, -371.957f, 141.218f},
    {2677.566f, -372.946f, 141.218f},
};

struct MANGOS_DLL_DECL npc_admiral_westwindAI : public ScriptedAI
{  
    npc_admiral_westwindAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }

    uint32 m_uiStepTimer;
    uint32 m_uiStep;
    uint64 m_uiAbbendisGUID;

    void Reset()
    { 
         m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
         m_creature->CastSpell(m_creature, SPELL_SHIELD, false);
         m_uiStepTimer = 3000;
         m_uiStep = 0;
         m_uiAbbendisGUID = 0;
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 1:
                if (Creature* pAbbendis = m_creature->GetMap()->GetCreature(m_uiAbbendisGUID))
                {
                    float X, Y, Z, fAng;
                    fAng = m_creature->GetAngle(pAbbendis);
                    X = pAbbendis->GetPositionX()-1*cos(fAng);
                    Y = pAbbendis->GetPositionY()-1*sin(fAng);
                    Z = pAbbendis->GetPositionZ()+5;
                    m_creature->UpdateAllowedPositionZ(X, Y, Z);
                    m_creature->GetMotionMaster()->MovePoint(1, X, Y, Z);
                } 
                m_uiStepTimer = 1000;
                m_uiStep++;
                break;
            case 3:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_uiStepTimer = 2000;
                m_uiStep++;
                break;
            case 4:
                DoScriptText(SAY_WESTWIND_02, m_creature);
                m_uiStepTimer = 5000;
                m_uiStep++;
                break;
            case 5:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_uiStepTimer = 2000;
                m_uiStep++;
                break;
            case 6:
                DoScriptText(SAY_WESTWIND_03, m_creature);
                m_creature->GetMotionMaster()->MovePoint(0, 2688.124f, -356.010f, 141.216f);
                m_uiStepTimer = 4000;
                m_uiStep++;
                break;
            case 7:
                m_creature->GetMotionMaster()->MovementExpired(false);
                m_creature->GetMotionMaster()->MovePoint(0, 2719.409f, -350.953f, 141.216f); 
                m_creature->ForcedDespawn(14000);
                m_uiStep++;
                break;
        }
    }

    void AttackStart(Unit* pWho) {}

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType == POINT_MOTION_TYPE && uiPointId == 1)
            m_uiStep = 3;
    }

    void StartEvent(uint8 EventId, uint64 AbbendisGUID)
    {
        m_uiStep = EventId;
        m_uiAbbendisGUID = AbbendisGUID;
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_uiStepTimer < uiDiff)
           Event();
       else 
           m_uiStepTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_admiral_westwind(Creature* pCreature)
{
    return new npc_admiral_westwindAI(pCreature);
}

struct MANGOS_DLL_DECL boss_hight_general_abbendisAI : public ScriptedAI
{
    boss_hight_general_abbendisAI(Creature *pCreature) : ScriptedAI(pCreature) 
    { 
        Reset();
    }

    uint8 m_uiStep;
    uint8 m_uiPhase;
   
    uint32 m_uiStepTimer;
    uint32 m_uiFireLandTimer;
    uint32 m_uiArmorTimer;

    uint64 m_uiWestwindGUID;

    bool m_bHpLow;

    void Reset()
    {
        m_uiStep = 0;
        m_uiPhase = 0;
        m_uiStepTimer = 3000;
        m_uiFireLandTimer = 4000;
        m_uiArmorTimer = 6000;
        m_uiWestwindGUID = 0;
        m_bHpLow = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_uiPhase == 0 && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster() && m_creature->IsWithinDistInMap(pWho, 15.0f))
        {
            m_uiPhase = 1;
            DoScriptText(SAY_ABBENDIS_YELL_01, m_creature);
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature* pWestwind = m_creature->GetMap()->GetCreature(m_uiWestwindGUID))
            ((npc_admiral_westwindAI*)pWestwind->AI())->StartEvent(1, m_creature->GetGUID());
    }

    void AggroSummon(Creature* pMonster)
    {
        Map *map = m_creature->GetMap(); // search players with in 50 yards for quest credit
        Map::PlayerList const &PlayerList = map->GetPlayers();
        if (!PlayerList.isEmpty())
        {
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            { 
                if (pMonster->isInCombat())
                    return;

                if (i->getSource()->isAlive() && m_creature->GetDistance2d(i->getSource()) < 50)
                    pMonster->AI()->AttackStart(i->getSource());
            }
        }
    }

    void JumpNextStep(uint32 Timer)
    {
        m_uiStep++;
        m_uiStepTimer = Timer;
    }

    void SummonScarletWarrior(uint32 uiAmount)
    {
        for (uint32 i = 0; i < uiAmount; ++i)
        {
            uint8 uiPos = urand(0, 5);
            m_creature->SummonCreature(urand(0, 1) ? NPC_RAVEN_PRIEST : NPC_SCARLET_WARRIOR, AbbendisSummonPos[uiPos].x, AbbendisSummonPos[uiPos].y, AbbendisSummonPos[uiPos].z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
        }
    }

    void JustSummoned(Creature* pCreature)
    {
        pCreature->SetRespawnDelay(DAY);
        uint32 uiEntry = pCreature->GetEntry();
        if (uiEntry == NPC_RAVEN_PRIEST || uiEntry == NPC_SCARLET_WARRIOR)
            AggroSummon(pCreature);
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 0:
                DoScriptText(SAY_ABBENDIS_YELL_02, m_creature);
                JumpNextStep(5000);
                break;
            case 1:
                SummonScarletWarrior(2);
                JumpNextStep(15000);
                break;
            case 2:
                DoScriptText(SAY_ABBENDIS_YELL_03, m_creature);
                SummonScarletWarrior(3);
                JumpNextStep(15000);
                break;
            case 3:
                DoScriptText(SAY_ABBENDIS_YELL_04, m_creature);
                SummonScarletWarrior(4);
                JumpNextStep(15000);
                break;
            case 4:
               if (Creature* pWestwind = GetClosestCreatureWithEntry(m_creature, NPC_WESTWIND, 30.0f))
               {
                   m_uiWestwindGUID = pWestwind->GetGUID();
                   DoScriptText(SAY_WESTWIND_01, pWestwind);
               }
               JumpNextStep(2000);
               break;
            case 5:
               AggroSummon(m_creature);
               m_uiPhase = 2;
               break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            case 1:
                if (m_uiStepTimer < uiDiff)
                    Event();
                else 
                    m_uiStepTimer -= uiDiff;
                break;
            case 2:
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                {
                    m_creature->AI()->EnterEvadeMode();
                    return;
                }

                if (!m_creature->HasAura(SPELL_BUFF) && m_bHpLow)
                    m_creature->CastSpell(m_creature, SPELL_BUFF, false);

                if (!m_bHpLow && m_creature->GetHealthPercent() < 50)
                {
                    m_bHpLow = true;
                    DoScriptText(SAY_ABBENDIS_50HP, m_creature);
                }

                if (m_uiFireLandTimer <= uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_FIRE_LAND);
                    m_uiFireLandTimer = urand(7000, 15000);
                }
                else 
                    m_uiFireLandTimer -= uiDiff;

                if (m_uiArmorTimer <= uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_ARMOR_SEAL);
                    m_uiArmorTimer = urand(5000, 10000);
                }
                else 
                    m_uiArmorTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
        }
    }
};

CreatureAI* GetAI_boss_hight_general_abbendis(Creature* pCreature)
{
    return new boss_hight_general_abbendisAI(pCreature);
}

void AddSC_dragonblight()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_afrasastrasz";
    newscript->pGossipHello = &GossipHello_npc_afrasastrasz;
    newscript->pGossipSelect = &GossipSelect_npc_afrasastrasz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_alexstrasza_wr_gate";
    newscript->pGossipHello = &GossipHello_npc_alexstrasza_wr_gate;
    newscript->pGossipSelect = &GossipSelect_npc_alexstrasza_wr_gate;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tariolstrasz";
    newscript->pGossipHello = &GossipHello_npc_tariolstrasz;
    newscript->pGossipSelect = &GossipSelect_npc_tariolstrasz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_torastrasza";
    newscript->pGossipHello = &GossipHello_npc_torastrasza;
    newscript->pGossipSelect = &GossipSelect_npc_torastrasza;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_hight_general_abbendis";
    newscript->GetAI = &GetAI_boss_hight_general_abbendis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_admiral_westwind";
    newscript->GetAI = &GetAI_npc_admiral_westwind;
    newscript->RegisterSelf();
}
