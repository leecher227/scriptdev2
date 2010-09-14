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
SDName: Zuldrak
SD%Complete: 12512, 12916
SDComment: 50%
SDCategory: Zuldrak
EndScriptData */

/* ContentData
npc_gurgthock
go_scourge_enclosure
EndContentData */

#include "precompiled.h"

enum
{
    QUEST_FROM_BEYOND = 12934,

    NPC_AZBARIN       = 30026,
    NPC_DUKE_SINGEN   = 30019,
    NPC_ERATHIUS      = 30025,
    NPC_GARGORAL      = 30024
};

static float m_afSpawnLocation[] = {5768.71f, -2969.29f, 273.816f};
static uint32 m_auiBosses[] = {NPC_AZBARIN, NPC_DUKE_SINGEN, NPC_ERATHIUS, NPC_GARGORAL};

bool QuestAccept_npc_gurgthock(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FROM_BEYOND)
        pCreature->SummonCreature(m_auiBosses[urand(0, 3)], m_afSpawnLocation[0], m_afSpawnLocation[1], m_afSpawnLocation[2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);

    return true;
}

/*#####
## go_scourge_enclosure
#####*/

enum
{
    QUEST_OUR_ONLY_HOPE = 12916,
    NPC_GYMER = 29928

};

bool GOHello_scourge_enclosure(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_OUR_ONLY_HOPE) == QUEST_STATUS_INCOMPLETE)
    {
        if(Creature *pGymer = GetClosestCreatureWithEntry(pPlayer, NPC_GYMER, INTERACTION_DISTANCE))
        {
            pPlayer->KilledMonsterCredit(NPC_GYMER, pGymer->GetGUID());

        }
    }
    return false;
};

void AddSC_zuldrak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_gurgthock";
    pNewScript->pQuestAccept = &QuestAccept_npc_gurgthock;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_scourge_enclosure";
    pNewScript->pGOHello = &GOHello_scourge_enclosure;
    pNewScript->RegisterSelf();
}
