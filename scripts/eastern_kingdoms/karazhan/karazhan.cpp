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
SDName: Karazhan
SD%Complete: 100
SDComment: Support for Barnes (Opera controller) and Berthold (Doorman).
SDCategory: Karazhan
EndScriptData */

/* ContentData
npc_barnes
npc_berthold
EndContentData */

#include "precompiled.h"
#include "karazhan.h"
#include "escort_ai.h"

/*######
# npc_barnesAI
######*/

#define GOSSIP_READY        "I'm not an actor."

#define SAY_READY           "Splendid, I'm going to get the audience ready. Break a leg!"
#define SAY_OZ_INTRO1       "Finally, everything is in place. Are you ready for your big stage debut?"
#define OZ_GOSSIP1          "I'm not an actor."
#define SAY_OZ_INTRO2       "Don't worry, you'll be fine. You look like a natural!"
#define OZ_GOSSIP2          "Ok, I'll give it a try, then."

#define SAY_RAJ_INTRO1      "The romantic plays are really tough, but you'll do better this time. You have TALENT. Ready?"
#define RAJ_GOSSIP1         "I've never been more ready."

#define OZ_GM_GOSSIP1       "[GM] Change event to EVENT_OZ"
#define OZ_GM_GOSSIP2       "[GM] Change event to EVENT_HOOD"
#define OZ_GM_GOSSIP3       "[GM] Change event to EVENT_RAJ"

struct Dialogue
{
    int32  iTextId;
    uint32 uiTimer;
};

static Dialogue aOzDialogue[4]=
{
    {-1532103, 6000},
    {-1532104, 18000},
    {-1532105, 9000},
    {-1532106, 15000}
};

static Dialogue aHoodDialogue[4]=
{
    {-1532107, 6000},
    {-1532108, 10000},
    {-1532109, 14000},
    {-1532110, 15000}
};

static Dialogue aRAJDialogue[4]=
{
    {-1532111, 5000},
    {-1532112, 7000},
    {-1532113, 14000},
    {-1532114, 14000}
};

struct Spawn
{
    uint32  uiEntry;
    float   fPosX;
};

// Entries and spawn locations for creatures in Oz event
Spawn aSpawns_OZ[4]=
{
    {17535, -10896.0f},                                     // Dorothee
    {17546, -10891.0f},                                     // Roar
    {17547, -10884.0f},                                     // Tinhead
    {17543, -10902.0f},                                     // Strawman
};
Spawn Spawn_HOOD = {17603, -10892.0f};                      // Grandmother
Spawn Spawn_RAJ  = {17534, -10900.0f};                      // Julianne

enum
{
    NPC_SPOTLIGHT       = 19525,

    SPELL_SPOTLIGHT     = 25824,
    SPELL_TUXEDO        = 32616
};

const float SPAWN_Z = 90.5f;
const float SPAWN_Y = -1758.0f;
const float SPAWN_O = 4.738f;

struct MANGOS_DLL_DECL npc_barnesAI : public npc_escortAI
{
    npc_barnesAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_bRaidWiped = false;
        m_uiEventId  = 0;
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint64 m_uiSpotlightGUID;

    uint32 m_uiTalkCount;
    uint32 m_uiTalkTimer;
    uint32 m_uiWipeTimer;
    uint32 m_uiEventId;

    bool m_bPerformanceReady;
    bool m_bRaidWiped;

    void Reset()
    {
        m_uiSpotlightGUID = 0;

        m_uiTalkCount = 0;
        m_uiTalkTimer = 2000;
        m_uiWipeTimer = 5000;

        m_bPerformanceReady = false;

        if (m_pInstance)
            m_uiEventId = m_pInstance->GetData(DATA_OPERA_PERFORMANCE);
    }

    void StartEvent()
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_OPERA, IN_PROGRESS);

        //resets count for this event, in case earlier failed
        if (m_uiEventId == EVENT_OZ)
            m_pInstance->SetData(DATA_OPERA_OZ_DEATHCOUNT, IN_PROGRESS);

        Start(false, 0, NULL, true);
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (!m_pInstance)
            return;

        switch(uiPointId)
        {
            case 0:
                m_creature->CastSpell(m_creature, SPELL_TUXEDO, false);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_STAGE_DOOR_LEFT));
                break;
            case 4:
                m_uiTalkCount = 0;
                SetEscortPaused(true);

                if (Creature* pSpotlight = m_creature->SummonCreature(NPC_SPOTLIGHT,
                    m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f,
                    TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000))
                {
                    pSpotlight->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pSpotlight->CastSpell(pSpotlight, SPELL_SPOTLIGHT, false);
                    m_uiSpotlightGUID = pSpotlight->GetGUID();
                }
                break;
            case 8:
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_STAGE_DOOR_LEFT));
                m_bPerformanceReady = true;
                break;
            case 9:
                PrepareEncounter();
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_STAGE_CURTAIN));
                break;
        }
    }

    void Talk(uint32 uiCount)
    {
        int32 iTextId = 0;

        switch(m_uiEventId)
        {
            case EVENT_OZ:
                if (aOzDialogue[uiCount].iTextId)
                    iTextId = aOzDialogue[uiCount].iTextId;
                if (aOzDialogue[uiCount].uiTimer)
                    m_uiTalkTimer = aOzDialogue[uiCount].uiTimer;
                break;

            case EVENT_HOOD:
                if (aHoodDialogue[uiCount].iTextId)
                    iTextId = aHoodDialogue[uiCount].iTextId;
                if (aHoodDialogue[uiCount].uiTimer)
                    m_uiTalkTimer = aHoodDialogue[uiCount].uiTimer;
                break;

            case EVENT_RAJ:
                if (aRAJDialogue[uiCount].iTextId)
                    iTextId = aRAJDialogue[uiCount].iTextId;
                if (aRAJDialogue[uiCount].uiTimer)
                    m_uiTalkTimer = aRAJDialogue[uiCount].uiTimer;
                break;
        }

        if (iTextId)
            DoScriptText(iTextId, m_creature);
    }

    void PrepareEncounter()
    {
        debug_log("SD2: Barnes Opera Event - Introduction complete - preparing encounter %d", m_uiEventId);

        switch(m_uiEventId)
        {
            case EVENT_OZ:
                for(int i=0; i < 4; ++i)
                    m_creature->SummonCreature(aSpawns_OZ[i].uiEntry, aSpawns_OZ[i].fPosX, SPAWN_Y, SPAWN_Z, SPAWN_O, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, HOUR*2*IN_MILLISECONDS);
                break;
            case EVENT_HOOD:
                m_creature->SummonCreature(Spawn_HOOD.uiEntry, Spawn_HOOD.fPosX, SPAWN_Y, SPAWN_Z, SPAWN_O, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, HOUR*2*IN_MILLISECONDS);
                break;
            case EVENT_RAJ:
                m_creature->SummonCreature(Spawn_RAJ.uiEntry, Spawn_RAJ.fPosX, SPAWN_Y, SPAWN_Z, SPAWN_O, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, HOUR*2*IN_MILLISECONDS);
                break;
            default:
                error_log("SD2: Barnes Opera Event - Wrong EventId set: %d", m_uiEventId);
                break;
        }

        m_bRaidWiped = false;
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_uiTalkTimer < uiDiff)
            {
                if (m_uiTalkCount > 3)
                {
                    if (Creature* pSpotlight = m_creature->GetMap()->GetCreature(m_uiSpotlightGUID))
                        pSpotlight->ForcedDespawn();

                    SetEscortPaused(false);
                    return;
                }

                Talk(m_uiTalkCount++);
            }
            else
                m_uiTalkTimer -= uiDiff;
        }

        if (m_bPerformanceReady)
        {
            if (!m_bRaidWiped)
            {
                if (m_uiWipeTimer < uiDiff)
                {
                    Map *pMap = m_creature->GetMap();
                    if (!pMap->IsDungeon())
                        return;

                    Map::PlayerList const &PlayerList = pMap->GetPlayers();
                    if (PlayerList.isEmpty())
                        return;

                    m_bRaidWiped = true;
                    for(Map::PlayerList::const_iterator i = PlayerList.begin();i != PlayerList.end(); ++i)
                    {
                        if (i->getSource()->isAlive() && !i->getSource()->isGameMaster())
                        {
                            m_bRaidWiped = false;
                            break;
                        }
                    }

                    if (m_bRaidWiped)
                        EnterEvadeMode();

                    m_uiWipeTimer = 15000;
                }
                else
                    m_uiWipeTimer -= uiDiff;
            }
        }
    }
};

CreatureAI* GetAI_npc_barnesAI(Creature* pCreature)
{
    return new npc_barnesAI(pCreature);
}

bool GossipHello_npc_barnes(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        // Check for death of Moroes and if opera event is not done already
        if (pInstance->GetData(TYPE_MOROES) == DONE && pInstance->GetData(TYPE_OPERA) != DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, OZ_GOSSIP1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (pPlayer->isGameMaster())                    // for GMs we add the possibility to change the event
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, OZ_GM_GOSSIP1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, OZ_GM_GOSSIP2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, OZ_GM_GOSSIP3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            }

            if (npc_barnesAI* pBarnesAI = dynamic_cast<npc_barnesAI*>(pCreature->AI()))
            {
                if (!pBarnesAI->m_bRaidWiped)
                    pPlayer->SEND_GOSSIP_MENU(8970, pCreature->GetGUID());
                else
                    pPlayer->SEND_GOSSIP_MENU(8975, pCreature->GetGUID());

                return true;
            }
        }
    }

    pPlayer->SEND_GOSSIP_MENU(8978, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_barnes(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    npc_barnesAI* pBarnesAI = dynamic_cast<npc_barnesAI*>(pCreature->AI());

    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, OZ_GOSSIP2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(8971, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (pBarnesAI)
                pBarnesAI->StartEvent();
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (pBarnesAI && pPlayer->isGameMaster())
                pBarnesAI->m_uiEventId = EVENT_OZ;
            outstring_log("SD2: pPlayer (GUID " UI64FMTD ") manually set Opera event to EVENT_OZ", pPlayer->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (pBarnesAI && pPlayer->isGameMaster())
                pBarnesAI->m_uiEventId = EVENT_HOOD;
            outstring_log("SD2: pPlayer (GUID " UI64FMTD ") manually set Opera event to EVENT_HOOD", pPlayer->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (pBarnesAI && pPlayer->isGameMaster())
                pBarnesAI->m_uiEventId = EVENT_RAJ;
            outstring_log("SD2: pPlayer (GUID " UI64FMTD ") manually set Opera event to EVENT_RAJ", pPlayer->GetGUID());
            break;
    }

    return true;
}

/*###
# npc_berthold
####*/

enum
{
    SPELL_TELEPORT              = 39567
};

#define GOSSIP_ITEM_TELEPORT    "Teleport me to the Guardian's Library"

bool GossipHello_npc_berthold(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        // Check if Shade of Aran event is done
        if (pInstance->GetData(TYPE_ARAN) == DONE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_berthold(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->CastSpell(pPlayer, SPELL_TELEPORT, true);

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*###
# npc_image_of_medivh
####*/

/*
TODO:
-arcanacros turns around when he cast a spell
-arcanacros don't get any dmg, because he's in evade mode...

/*
Image of Medivh yells: You've got my attention, dragon. You'll find I'm not as easily scared as the villagers below.
Image of Arcanagos yells: Your dabbling in the arcane has gone too far, Medivh. You've attracted the attention of powers beyond your understanding. You must leave Karazhan at once!
Image of Medivh yells: You dare challenge me at my own dwelling? Your arrogance is astounding, even for a dragon!
Image of Arcanagos yells: A dark power seeks to use you, Medivh! If you stay, dire days will follow. You must hurry, we don't have much time!
Image of Medivh yells: I do not know what you speak of, dragon... but I will not be bullied by this display of insolence. I'll leave Karazhan when it suits me!
Image of Arcanagos yells: You leave me no alternative. I will stop you by force if you won't listen to reason!
Image of Medivh begins to cast a spell of great power, weaving his own essence into the magic.
Image of Arcanagos yells: What have you done, wizard? This cannot be! I'm burning from... within!
Image of Medivh says: He should not have angered me. I must go... recover my strength now...
*/

/*
UPDATE `creature_template` SET `speed` = 3 WHERE `entry` = 17652;
*/

/*
-- DB scriptdev2
INSERT INTO script_texts (entry,content_default,sound,type,language,emote,comment) VALUES
(-1532201,'You\'ve got my attention, dragon. You\'ll find I\'m not as easily scared as the villagers below.',0,1,0,0,'image_of_medivh SAY_DIALOG_MEDIVH_1'),
(-1532202,'Your dabbling in the arcane has gone too far, Medivh. You\'ve attracted the attention of powers beyond your understanding. You must leave Karazhan at once!',0,1,0,0,'image_of_arcanagos SAY_DIALOG_ARCANAGOS_2'),
(-1532203,'You dare challenge me at my own dwelling? Your arrogance is astounding, even for a dragon!',0,1,0,0,'image_of_medivh SAY_DIALOG_MEDIVH_3'),
(-1532204,'A dark power seeks to use you, Medivh! If you stay, dire days will follow. You must hurry, we don\'t have much time!',0,1,0,0,'image_of_arcanagos SAY_DIALOG_ARCANAGOS_4'),
(-1532205,'I do not know what you speak of, dragon... but I will not be bullied by this display of insolence. I\'ll leave Karazhan when it suits me!',0,1,0,0,'image_of_medivh SAY_DIALOG_MEDIVH_5'),
(-1532206,'You leave me no alternative. I will stop you by force if you won\'t listen to reason!',0,1,0,0,'image_of_arcanagos SAY_DIALOG_ARCANAGOS_6'),
(-1532207,'begins to cast a spell of great power, weaving his own essence into the magic.',0,2,0,0,'image_of_medivh EMOTE_DIALOG_MEDIVH_7'),
(-1532208,'What have you done, wizard? This cannot be! I\'m burning from... within!',0,1,0,0,'image_of_arcanagos SAY_DIALOG_ARCANAGOS_8'),
(-1532209,'He should not have angered me. I must go... recover my strength now...',0,1,0,0,'image_of_medivh SAY_DIALOG_MEDIVH_9');
*/

#define SAY_DIALOG_MEDIVH_1         -1532201
#define SAY_DIALOG_ARCANAGOS_2      -1532202
#define SAY_DIALOG_MEDIVH_3         -1532203
#define SAY_DIALOG_ARCANAGOS_4      -1532204
#define SAY_DIALOG_MEDIVH_5         -1532205
#define SAY_DIALOG_ARCANAGOS_6      -1532206
#define EMOTE_DIALOG_MEDIVH_7       -1532207
#define SAY_DIALOG_ARCANAGOS_8      -1532208
#define SAY_DIALOG_MEDIVH_9         -1532209


//Medivhs Image Spells

#define SPELL_FIRE_BALL             30967
#define SPELL_CONFLAGRATION_BLAST   30977
#define SPELL_EVOCATION             30972
#define SPELL_MANASHIELD            30973
//Image of Arcanagos Spells

#define SPELL_REFLECTION            30969
#define SPELL_REFLECT_UBERFIREBALL  30971
#define SPELL_SHOOT_UBERFIREBALL    30968
#define SPELL_PREPARE_FOR_FIREBALL  30970
#define SPELL_CONLAGRATION          16806

#define MOB_ARCANAGOS               17652
#define QUEST_MASTERS_TERRACE		9645

static float MedivPos[4] = {-11161.49f, -1902.24f, 91.48f, 1.94f};
static float ArcanagosPos[4] = {-11173.24f, -1879.90f, 100.40f, 5.24f};
static float ArcanagosEndPos[4] = {-11003.53f, -1755.60f, 175.34f, 4.09f};

struct MANGOS_DLL_DECL npc_image_of_medivhAI : public ScriptedAI
{
    npc_image_of_medivhAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance  = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint64 m_uiArcanagosGUID;
    bool m_bEventStarted;
    uint32 m_uiNextStep_Timer;
    uint8 m_uiStep;

    void Reset()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData64(DATA_IMAGE_OF_MEDIVH, m_creature->GetGUID());
            m_creature->GetMotionMaster()->MovePoint(1, MedivPos[0], MedivPos[1], MedivPos[2]);
            m_uiStep = 0;
        }
    }

    void Aggro(Unit* pWho)
    {
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == 1)
        {
            StartEvent();
            m_creature->SetOrientation(MedivPos[3]);
        }
    }

    void StartEvent()
    {
        m_uiStep = 1;
        m_bEventStarted = true;
        if (Creature* pArcanagos = m_creature->SummonCreature(MOB_ARCANAGOS, ArcanagosPos[0]-40, ArcanagosPos[1]+40, ArcanagosPos[2], ArcanagosPos[3], TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 145000))
        {
            m_uiArcanagosGUID = pArcanagos->GetGUID();
            pArcanagos->SetSplineFlags(SPLINEFLAG_FLYING);
            pArcanagos->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
            pArcanagos->GetMotionMaster()->MovePoint(0, ArcanagosPos[0], ArcanagosPos[1], ArcanagosPos[2]);
            pArcanagos->SetOrientation(ArcanagosPos[3]);
        }
        m_uiNextStep_Timer = 10000;
    }


    uint32 NextStep(uint32 step)
    {
        Creature* pArcanagos = m_creature->GetMap()->GetCreature(m_uiArcanagosGUID);
        if (!pArcanagos)
            return 9999999;
        
        switch(step)
        {
            case 0: return 9999999;
            case 1:
                DoScriptText(SAY_DIALOG_MEDIVH_1, m_creature);
                return 10000;
            case 2:
                DoScriptText(SAY_DIALOG_ARCANAGOS_2, pArcanagos);
                return 20000;
            case 3:
                DoScriptText(SAY_DIALOG_MEDIVH_3, m_creature);
                return 10000;
            case 4:
                DoScriptText(SAY_DIALOG_ARCANAGOS_4, pArcanagos);
                return 20000;
            case 5:
				pArcanagos->SetInCombatWith(m_creature);
                DoScriptText(SAY_DIALOG_MEDIVH_5, m_creature);
                return 20000;
            case 6:
                DoScriptText(SAY_DIALOG_ARCANAGOS_6, pArcanagos);
                return 10000;
            case 7:
                m_creature->CastSpell(pArcanagos, SPELL_FIRE_BALL, false);
                return 4000;
            case 8:
                pArcanagos->CastSpell(pArcanagos, SPELL_REFLECTION, false);
                return 3000;
            case 9:
                pArcanagos->CastSpell(m_creature, SPELL_REFLECT_UBERFIREBALL, false);                
                return 1000;
            case 10:
                m_creature->CastSpell(m_creature, SPELL_MANASHIELD, false);
                return 3000;
            case 11:
                m_creature->RemoveAurasDueToSpell(SPELL_MANASHIELD);
                m_creature->CastSpell(m_creature, SPELL_EVOCATION, false);
                DoScriptText(EMOTE_DIALOG_MEDIVH_7, m_creature);
                return 5000;
            case 12:
                m_creature->RemoveAllAuras();
                m_creature->CastSpell(pArcanagos, SPELL_FIRE_BALL, false);
                return 4000;
            case 13:
                //m_creature->CastSpell(pArcanagos, SPELL_CONFLAGRATION_BLAST, true);
                pArcanagos->CastSpell(pArcanagos, SPELL_CONFLAGRATION_BLAST, false);
                return 1000;
            case 14:
                DoScriptText(SAY_DIALOG_ARCANAGOS_8, pArcanagos);
                return 5000;
            case 15:
                pArcanagos->GetMotionMaster()->MovePoint(0, ArcanagosEndPos[0], ArcanagosEndPos[1], ArcanagosEndPos[2]);
                return 5000;
            case 16:
                DoScriptText(SAY_DIALOG_MEDIVH_9, m_creature);
                return 10000;
            case 17:
            {
                Map* pMap = m_creature->GetMap();
                if (pMap && pMap->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = pMap->GetPlayers();
                    if (!PlayerList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (i->getSource()->isAlive() && i->getSource()->IsActiveQuest(QUEST_MASTERS_TERRACE) && (i->getSource()->GetQuestStatus(QUEST_MASTERS_TERRACE) == QUEST_STATUS_INCOMPLETE))
                                i->getSource()->GroupEventHappens(QUEST_MASTERS_TERRACE, m_creature);
                        }
                    }
                }
                m_creature->ClearInCombat();
                return 9999999;
            }
            default:
                return 9999999; 
        }  
         
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (m_uiNextStep_Timer < uiDiff)
        {
            if (m_bEventStarted)
                m_uiNextStep_Timer = NextStep(m_uiStep++);
        }
        else
            m_uiNextStep_Timer -= uiDiff;     
    }
};

CreatureAI* GetAI_npc_image_of_medivh(Creature *pCreature)
{
    return new npc_image_of_medivhAI(pCreature);
}


void AddSC_karazhan()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_barnes";
    newscript->GetAI = &GetAI_npc_barnesAI;
    newscript->pGossipHello = &GossipHello_npc_barnes;
    newscript->pGossipSelect = &GossipSelect_npc_barnes;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_berthold";
    newscript->pGossipHello = &GossipHello_npc_berthold;
    newscript->pGossipSelect = &GossipSelect_npc_berthold;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_image_of_medivh";
    newscript->GetAI = &GetAI_npc_image_of_medivh;
    newscript->RegisterSelf();
}
