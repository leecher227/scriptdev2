/* Copyright (C) 2006 - 2010 TrinityCore <http://www.trinitycore.org>
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
SDName: Oculus
SD%Complete: ??%
SDComment:
SDAuthor: MaxXx2021 Aka Mioka.
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"
#include "Spell.h"

#define GOSSIP_ITEM_DRAKES         "So where do we go from here?"
#define GOSSIP_ITEM_BELGARISTRASZ1 "I want to fly on the wings of the Red Flight"
#define GOSSIP_ITEM_BELGARISTRASZ2 "What abilities do Ruby Drakes have?"
#define GOSSIP_ITEM_VERDISA1       "I want to fly on the wings of the Green Flight"
#define GOSSIP_ITEM_VERDISA2       "What abilities do Emerald Drakes have?"
#define GOSSIP_ITEM_ETERNOS1       "I want to fly on the wings of the Bronze Flight"
#define GOSSIP_ITEM_ETERNOS2       "What abilities do Amber Drakes have?"

#define HAS_ESSENCE(a) ((a)->HasItemCount(ITEM_EMERALD_ESSENCE,1) || (a)->HasItemCount(ITEM_AMBER_ESSENCE,1) || (a)->HasItemCount(ITEM_RUBY_ESSENCE,1))

enum Drakes
{
    GOSSIP_TEXTID_DRAKES                          = 13267,
    GOSSIP_TEXTID_BELGARISTRASZ1                  = 12916,
    GOSSIP_TEXTID_BELGARISTRASZ2                  = 13466,
    GOSSIP_TEXTID_BELGARISTRASZ3                  = 13254,
    GOSSIP_TEXTID_VERDISA1                        = 1,
    GOSSIP_TEXTID_VERDISA2                        = 1,
    GOSSIP_TEXTID_VERDISA3                        = 1,
    GOSSIP_TEXTID_ETERNOS1                        = 1,
    GOSSIP_TEXTID_ETERNOS2                        = 1,
    GOSSIP_TEXTID_ETERNOS3                        = 13256,

    SPELL_RUBY_DRAKE                              = 49462,
    AREA_ID_SHATTERED_STRAITS                     = 4064,
    MAP_ID_OCULUS                                 = 578,

    ITEM_EMERALD_ESSENCE                          = 37815,
    ITEM_AMBER_ESSENCE                            = 37859,
    ITEM_RUBY_ESSENCE                             = 37860,  //spell 49462 port in start location Oo
   
    SPELL_ITEM_RUBY                               = 43185,

    NPC_VERDISA                                   = 27657,
    NPC_BELGARISTRASZ                             = 27658,
    NPC_ETERNOS                                   = 27659,

    NPC_RUBY_DRAKE                                = 27756,
    NPC_AMBER_DRAKE                               = 27755,
    NPC_EMERALD_DRAKE                             = 27692,

    POINT_ID_LAND_TO_PLAYER                       = 1,
    POINT_ID_MOVE_DRAGON                          = 2,

    //Azure Dragons
    SPELL_ICE_BLAST_N                             = 49549,
    SPELL_ICE_BLAST_H                             = 59211,

    //Emerald Dragon
    SPELL_NIGHTMARE_TOUCH                         = 50341,

    //Hatch
    SPELL_ICE_BOLT_N                              = 51243,
    SPELL_ICE_BOLT_H                              = 59215
};

/*#####
# npc_oculus_vehicle
#####*/

struct MANGOS_DLL_DECL npc_oculus_vehicleAI : public ScriptedAI
{
    npc_oculus_vehicleAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
    }

    void JustDied(Unit* pKiller)
    {
        if(Creature* pVisual = m_creature->SummonCreature(m_creature->GetEntry(), m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5000))
        {
          pVisual->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
          pVisual->AddSplineFlag(SPLINEFLAG_FLYING);
          pVisual->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
          pVisual->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
          pVisual->SetStandState(UNIT_STAND_STATE_DEAD);
          pVisual->GetMap()->CreatureRelocation(pVisual, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, 0);
          pVisual->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, SPLINETYPE_NORMAL , pVisual->GetSplineFlags(), 6000);
        }
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_NIGHTMARE_TOUCH)
           m_creature->DealDamage(m_creature, (m_creature->GetMaxHealth() * 0.3),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if(uiPointId == POINT_ID_LAND_TO_PLAYER)
        {
           m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_npc_oculus_vehicle(Creature* pCreature)
{
    return new npc_oculus_vehicleAI (pCreature);
}

/*#####
# npc_azure_dragon
#####*/

struct MANGOS_DLL_DECL npc_azure_dragonAI : public ScriptedAI
{
    npc_azure_dragonAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiIceBlastTimer;
    
    bool m_bIsMove;

    void Reset()
    {
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        uiIceBlastTimer = 5000;
        m_bIsMove = true;
    }

    void JustDied(Unit* pKiller)
    {
       m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, 0);
       m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, SPLINETYPE_NORMAL , m_creature->GetSplineFlags(), 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(m_creature->GetDistance2d(m_creature->getVictim()) > 35.0f && !m_bIsMove)
        {
           m_bIsMove = true;
           SetCombatMovement(m_bIsMove);
           if(Unit* pTarget = m_creature->getVictim())
              m_creature->GetMotionMaster()->MoveChase(pTarget);
        }

        if(m_creature->GetDistance2d(m_creature->getVictim()) < 20.0f && m_bIsMove)
        {
           m_bIsMove = false;
           SetCombatMovement(m_bIsMove);
           m_creature->GetMotionMaster()->Clear(false);
           m_creature->GetMotionMaster()->MoveIdle();
           m_creature->StopMoving();
        }

        if(uiIceBlastTimer <= uiDiff)
        {
           if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
              DoCast(pTarget, m_bIsRegularMode ? SPELL_ICE_BLAST_N : SPELL_ICE_BLAST_N);
           uiIceBlastTimer = 5000;
        } else uiIceBlastTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_azure_dragon(Creature* pCreature)
{
    return new npc_azure_dragonAI (pCreature);
}

/*#####
# npc_oculus_hatch
#####*/

struct MANGOS_DLL_DECL npc_oculus_hatchAI : public ScriptedAI
{
    npc_oculus_hatchAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiIceBlastTimer;
    
    bool m_bIsMove;

    void Reset()
    {
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        uiIceBlastTimer = 5000;
        m_bIsMove = true;
    }

    void JustDied(Unit* pKiller)
    {
       m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, 0);
       m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, SPLINETYPE_NORMAL , m_creature->GetSplineFlags(), 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if(m_creature->GetDistance2d(m_creature->getVictim()) > 35.0f && !m_bIsMove)
        {
           m_bIsMove = true;
           SetCombatMovement(m_bIsMove);
           if(Unit* pTarget = m_creature->getVictim())
              m_creature->GetMotionMaster()->MoveChase(pTarget);
        }

        if(m_creature->GetDistance2d(m_creature->getVictim()) < 20.0f && m_bIsMove)
        {
           m_bIsMove = false;
           SetCombatMovement(m_bIsMove);
           m_creature->GetMotionMaster()->Clear(false);
           m_creature->GetMotionMaster()->MoveIdle();
           m_creature->StopMoving();
        }

        if(uiIceBlastTimer <= uiDiff)
        {
           if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
              DoCast(pTarget, m_bIsRegularMode ? SPELL_ICE_BOLT_N : SPELL_ICE_BOLT_H);
           uiIceBlastTimer = 5000;
        } else uiIceBlastTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_oculus_hatch(Creature* pCreature)
{
    return new npc_oculus_hatchAI (pCreature);
}

/*#####
# npc_robot
#####*/

struct MANGOS_DLL_DECL npc_robotAI : public ScriptedAI
{
    npc_robotAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiIceBlastTimer;

    void Reset()
    {
        uiIceBlastTimer = 5000;
    }

    void JustDied(Unit* pKiller)
    {
        if(m_pInstance)
           m_pInstance->SetData(TYPE_ROBOTS, 1);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_robot(Creature* pCreature)
{
    return new npc_robotAI (pCreature);
}

/*#####
# item_summon_drake
#####*/

bool ItemUse_item_summon_drake(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    if (pPlayer->GetMapId() != MAP_ID_OCULUS)
        return false;

//    if (!pPlayer->GetTransport() && !pPlayer->GetVehicleGUID() && !pPlayer->isInCombat())
    {
        switch(pItem->GetEntry())
        {
           case ITEM_RUBY_ESSENCE:
              if(Creature* pDragon = pPlayer->SummonCreature(NPC_RUBY_DRAKE, (pPlayer->GetPositionX()-40)+rand()%80, (pPlayer->GetPositionY()-50)+rand()%100, pPlayer->GetPositionZ() + 45.0f, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 27000))
              {
                 pDragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                 pDragon->GetMotionMaster()->MovePoint(POINT_ID_LAND_TO_PLAYER, (pPlayer->GetPositionX()-5)+rand()%10, (pPlayer->GetPositionY()-5)+rand()%10, pPlayer->GetPositionZ());
              }
              break;
           case ITEM_AMBER_ESSENCE:
              if(Creature* pDragon = pPlayer->SummonCreature(NPC_AMBER_DRAKE, (pPlayer->GetPositionX()-40)+rand()%80, (pPlayer->GetPositionY()-50)+rand()%100, pPlayer->GetPositionZ() + 45.0f, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 27000))
              {
                 pDragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                 pDragon->GetMotionMaster()->MovePoint(POINT_ID_LAND_TO_PLAYER, (pPlayer->GetPositionX()-5)+rand()%10, (pPlayer->GetPositionY()-5)+rand()%10, pPlayer->GetPositionZ());
              }
              break;
           case ITEM_EMERALD_ESSENCE:
              if(Creature* pDragon = pPlayer->SummonCreature(NPC_EMERALD_DRAKE, (pPlayer->GetPositionX()-40)+rand()%80, (pPlayer->GetPositionY()-50)+rand()%100, pPlayer->GetPositionZ() + 45.0f, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 27000))
              {
                 pDragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                 pDragon->GetMotionMaster()->MovePoint(POINT_ID_LAND_TO_PLAYER, (pPlayer->GetPositionX()-5)+rand()%10, (pPlayer->GetPositionY()-5)+rand()%10, pPlayer->GetPositionZ());
              }
              break;
        }

        //if (const SpellEntry* pSpellInfo = GetSpellStore()->LookupEntry(SPELL_ITEM_RUBY))
          //  Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_NOT_HERE);

        return true;
    }

    return false;
}

/*#####
# npc_oculus_drake
#####*/

bool GossipHello_npc_oculus_drake(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->GetInstanceData()->GetData(TYPE_DRAKOS) == DONE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DRAKES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DRAKES, pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_oculus_drake(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(pCreature->GetEntry())
    {
    case NPC_VERDISA: //Verdisa
        switch(uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (!HAS_ESSENCE(pPlayer))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VERDISA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VERDISA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VERDISA1, pCreature->GetGUID());
            }
            else
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VERDISA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VERDISA2, pCreature->GetGUID());
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            ItemPosCountVec dest;
            uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_EMERALD_ESSENCE, 1);
            if (msg == EQUIP_ERR_OK)
                pPlayer->StoreNewItem(dest, ITEM_EMERALD_ESSENCE, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VERDISA3, pCreature->GetGUID());
            break;
        }
        break;
    case NPC_BELGARISTRASZ: //Belgaristrasz
        switch(uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (!HAS_ESSENCE(pPlayer))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BELGARISTRASZ1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BELGARISTRASZ2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BELGARISTRASZ1, pCreature->GetGUID());
            }
            else
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BELGARISTRASZ2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BELGARISTRASZ2, pCreature->GetGUID());
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            ItemPosCountVec dest;
            uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_RUBY_ESSENCE, 1);
            if (msg == EQUIP_ERR_OK)
                pPlayer->StoreNewItem(dest, ITEM_RUBY_ESSENCE, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BELGARISTRASZ3, pCreature->GetGUID());
            break;
        }
        break;
    case NPC_ETERNOS: //Eternos
        switch(uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (!HAS_ESSENCE(pPlayer))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ETERNOS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ETERNOS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ETERNOS1, pCreature->GetGUID());
            }
            else
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ETERNOS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ETERNOS2, pCreature->GetGUID());
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            ItemPosCountVec dest;
            uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_AMBER_ESSENCE, 1);
            if (msg == EQUIP_ERR_OK)
                pPlayer->StoreNewItem(dest, ITEM_AMBER_ESSENCE, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ETERNOS3, pCreature->GetGUID());
            break;
        }
        break;
    }

    return true;
}

void AddSC_oculus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_oculus_vehicle";
    newscript->GetAI = &GetAI_npc_oculus_vehicle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_azure_dragon";
    newscript->GetAI = &GetAI_npc_azure_dragon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oculus_hatch";
    newscript->GetAI = &GetAI_npc_oculus_hatch;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_robot";
    newscript->GetAI = &GetAI_npc_robot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oculus_drake";
    newscript->pGossipHello = &GossipHello_npc_oculus_drake;
    newscript->pGossipSelect = &GossipSelect_npc_oculus_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_summon_drake";
    newscript->pItemUse = &ItemUse_item_summon_drake;
    newscript->RegisterSelf();
}
