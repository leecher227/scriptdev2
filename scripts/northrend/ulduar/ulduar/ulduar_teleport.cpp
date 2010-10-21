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
SDName: ulduar_teleport
SD%Complete: 90%
SDComment: by /dev/rsa
SDCategory: Ulduar instance
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    PORTALS_COUNT               = 9,
    TELEPORT_GOSSIP_MESSAGE     = 14424,

    // spells
    SPELL_TELEPORT_BASE_CAMP    = 64014,
    SPELL_TELEPORT_GROUNDS      = 64032,
    SPELL_TELEPORT_FORGE        = 64028,
    SPELL_TELEPORT_SCRAPYARD    = 64031,
    SPELL_TELEPORT_ANTECHAMBER  = 64030,
    SPELL_TELEPORT_WALKWAY      = 64029,
    SPELL_TELEPORT_CONSERVATORY = 64024,
    SPELL_TELEPORT_SPARK        = 65061,
    SPELL_TELEPORT_PRISON       = 65042,

    MAP_ULDUAR                  = 603
};

struct t_Locations
{
    int textNum;
    float x, y, z, o;
    bool open;
    uint32 encounter;
};

static t_Locations PortalLoc[]=
{
{-3050001, -706.122f, -92.6024f, 429.876f, 0,     true, TYPE_IGNIS},      // base camp
{-3050002, 131.248f,  -35.3802f, 409.804f, 0,     true, TYPE_IGNIS},      // formation ground
{-3050003, 553.233f,  -12.3247f, 409.679f, 0,     true, TYPE_IGNIS},
{-3050004, 926.292f,  -11.4635f, 418.595f, 3.19f, true, TYPE_XT002_TP},
{-3050005, 1498.09f,  -24.246f,  420.967f, 0,     true, TYPE_XT002},
{-3050006, 1859.45f,  -24.1f,    448.9f,   0,     true, TYPE_KOLOGARN},
{-3050007, 2086.27f,  -24.3134f, 421.239f, 0,     true, TYPE_AURIAYA},
{-3050008, 2517.398f, 2568.89f,  412.69f,  6.17f, true, TYPE_MIMIRON_TP},
{-3050009, 1854.297f, -11.0173f, 334.4f,   0,     true, TYPE_VEZAX},
};

bool GOGossipHello_go_ulduar_teleporter(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();

    if (!pInstance || !pPlayer)
        return false;

    if (pPlayer->isInCombat())
        return true;

    for (uint8 i = 0; i < PORTALS_COUNT; ++i)
    {
        if (pPlayer->isGameMaster() || (PortalLoc[i].open == true && pInstance->GetData(PortalLoc[i].encounter) == DONE))
        {
             pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, PortalLoc[i].textNum, GOSSIP_SENDER_MAIN, i);
        }
    };

    pPlayer->SEND_GOSSIP_MENU(TELEPORT_GOSSIP_MESSAGE, pGo->GetGUID());
    return true;
}

bool GOGossipSelect_go_ulduar_teleporter(Player* pPlayer, GameObject* pGo, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return false;

    if (pPlayer->isInCombat())
        return false;

    if (action >= 0 && action <= PORTALS_COUNT)
        pPlayer->TeleportTo(MAP_ULDUAR, PortalLoc[action].x, PortalLoc[action].y, PortalLoc[action].z, PortalLoc[action].o);

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

void AddSC_ulduar_teleport()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_ulduar_teleporter";
    newscript->pGOGossipHello  = &GOGossipHello_go_ulduar_teleporter;
    newscript->pGOGossipSelect = &GOGossipSelect_go_ulduar_teleporter;
    newscript->RegisterSelf();
}
