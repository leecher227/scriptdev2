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
SDName: Zuldrak
SD%Complete: 100
SDComment: Quest support: 12512, 12916, 12934.
SDCategory: Zuldrak
EndScriptData */

/* ContentData
npc_gurgthock
go_scourge_enclosure
EndContentData */

#include "precompiled.h"
#include "TemporarySummon.h"

/*######
## npc_gurgthock
######*/

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

struct MANGOS_DLL_DECL npc_gurgthockAI : public ScriptedAI
{
    npc_gurgthockAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint64 m_uiPlayerGUID;

    void SetPlayerGUID(uint64 uiPlayerGUID) { m_uiPlayerGUID = uiPlayerGUID; }

    void Reset()
    {
        m_uiPlayerGUID = 0;
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        uint32 uiEntry = pSummoned->GetEntry();
        for(uint8 i = 0; i < 4; ++i)
        {
            if (uiEntry == m_auiBosses[i])
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
                    pPlayer->GroupEventHappens(QUEST_FROM_BEYOND, m_creature);

                m_uiPlayerGUID = 0;
                return;
            }
        }
    }
};

bool QuestAccept_npc_gurgthock(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FROM_BEYOND)
    {
        pCreature->SummonCreature(m_auiBosses[urand(0, 3)], m_afSpawnLocation[0], m_afSpawnLocation[1], m_afSpawnLocation[2], 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);

        if (npc_gurgthockAI* pGurthockAI = dynamic_cast<npc_gurgthockAI*>(pCreature->AI()))
            pGurthockAI->SetPlayerGUID(pPlayer->GetGUID());
    }
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

bool GOUse_scourge_enclosure(Player* pPlayer, GameObject* pGo)
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

CreatureAI* GetAI_npc_gurgthock(Creature* pCreature)
{
    return new npc_gurgthockAI(pCreature);
}

/*##### QUEST: Betryal #####
SDAuthor: MaxXx2021
*/

enum
{
   SAY_DRAKURU_PREFIGHT01      = -1876000,
   SAY_DRAKURU_PREFIGHT02      = -1876001,
   SAY_DRAKURU_PREFIGHT03      = -1876002,

   SAY_DRAKURU_RANDOMYELL01    = -1876003,
   SAY_DRAKURU_RANDOMYELL02    = -1876004,
   SAY_DRAKURU_RANDOMYELL03    = -1876005,
   SAY_DRAKURU_RANDOMYELL04    = -1876006,
   SAY_DRAKURU_RANDOMYELL05    = -1876007,
   SAY_DRAKURU_RANDOMYELL06    = -1876008,

   SAY_DRAKURU_EVENT01         = -1876009,
   SAY_DRAKURU_EVENT02         = -1876010,
   SAY_DRAKURU_EVENT03         = -1876011,
   SAY_LICH_KING_EVENT01       = -1876012,
   SAY_LICH_KING_EVENT02       = -1876013,
   SAY_LICH_KING_EVENT03       = -1876014,
   SAY_LICH_KING_EVENT04       = -1876015,
   SAY_LICH_KING_EVENT05       = -1876016,
   SAY_LICH_KING_EVENT06       = -1876017,

   SPELL_THROW_CRYSTALL        = 54087,
   SPELL_SHADOWBOLT            = 54113,
   SPELL_ARTHAS_PORTAL         = 51807,  //cast this npc 28492
   SPELL_SUMMON_SKULL          = 54253,
   SPELL_DRAKURU_DEATH         = 54248,
   SPELL_SKULL_MISSILE         = 54250, //spell script
   SPELL_PORTAL_MISSILE        = 54209, //spell script
   SPELL_ENOUTH                = 54245,
   SPELL_DEATH_TOUCH           = 54236,
   SPELL_TROLL_CONTROLL        = 53102, //spell script target
   SPELL_ZONE_AURA             = 53104,
   SPELL_SPERE                 = 51966,
   SPELL_MEAT_01               = 52578,
   SPELL_MEAT_02               = 52580,
   SPELL_MEAT_03               = 52575,
   SPELL_DRAKURU_TELEPORT      = 51461,

   SPELL_ESCAPE_VOLTAR         = 57533,
   
   NPC_DEATH_TROLL             = 28931,
   NPC_LICH_KING               = 28498,
   NPC_DRAKURU                 = 28503,
   NPC_INVISMAN                = 28492,

   QUEST_BETREYAL              = 12713,
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

struct Locations TelPos[]=
{
    {6175.637f, -2017.744f, 590.878f, 2.94f}, //summon drakuru
    {6158.543f, -2014.910f, 590.878f, 6.13f}, //summon player
    {6184.176f, -1969.550f, 586.760f, 4.60f}, //troll 1 position
    {6127.317f, -2008.511f, 586.759f, 6.04f}, //troll 2 position
    {6166.302f, -2065.412f, 586.758f, 1.32f}, //troll 3 position
    {6222.837f, -2026.676f, 586.758f, 2.91f}, //troll 4 position
    {6145.589f, -2012.081f, 590.878f, 6.06f}, //portal position
    {6163.186f, -2015.577f, 590.878f, 6.06f}, //lich king position
    {6180.624f, -2031.796f, 590.878f, 6.06f}, //skull position
};

struct MANGOS_DLL_DECL npc_overlord_drakuruAI : public ScriptedAI
{
    npc_overlord_drakuruAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        SetCombatMovement(false);
        Reset();
    }

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiCrystalTimer;
    uint32 m_uiYellTimer;
 
    uint64 m_uiLichKingGUID;
    uint64 m_uiPortalGUID;
    uint64 m_uiSkullSummonerGUID;

    Player* pPlayer;
    bool m_bIsNonFight;
   
    void Reset() 
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetActiveObjectState(false);
        m_uiStep = 0;
        m_uiStepTimer = 1000;
        m_uiYellTimer = 13000;
        m_uiShadowBoltTimer = 1000;
        m_uiCrystalTimer = 5000;
        m_uiLichKingGUID = 0;
        m_uiSkullSummonerGUID = 0;
        m_uiPortalGUID = 0;
        pPlayer = NULL;
        m_creature->SetVisibility(VISIBILITY_ON);
        CastTroll(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bIsNonFight = false;
    }

    void StartEvent(uint8 EventId, uint32 TimerCount, Player* Hero)
    {
        m_uiStep = EventId;
        m_uiStepTimer = TimerCount;
        pPlayer = Hero;
    }

    void JumpNextStep(uint32 Time)
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        if(damage > m_creature->GetHealth())
        {
            damage = 0;
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            {
                m_bIsNonFight = true;
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveAllAuras();
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->AttackStop();
                StartEvent(7, 2000, NULL);
            }
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) || m_bIsNonFight)
            return;
 
        ScriptedAI::AttackStart(pWho);
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 1:
                m_creature->SetVisibility(VISIBILITY_ON);
                m_creature->CastSpell(m_creature, SPELL_DRAKURU_TELEPORT, false);
                JumpNextStep(3000);
                break;
            case 2:
                for (uint8 i = 2; i < 6; i++)
                    if (Creature* pTroll = m_creature->SummonCreature(NPC_DEATH_TROLL,TelPos[i].x, TelPos[i].y, TelPos[i].z, TelPos[i].o,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000))
                        pTroll->CastSpell(pTroll, 64446, false);
                DoScriptText(SAY_DRAKURU_PREFIGHT01, m_creature);
                m_creature->MonsterTextEmote("Scourge Disgulse failling! Find a safe place!", 0, true);
                JumpNextStep(7000);
                break;
            case 3:
                m_creature->MonsterTextEmote("The Scepter of Domination glows brightly! Use it NOW!", 0, true);
                DoScriptText(SAY_DRAKURU_PREFIGHT02, m_creature);
                CastTroll(true);
                JumpNextStep(7000);
                break;
            case 4:
                if (pPlayer && pPlayer->IsInWorld() && pPlayer->HasAura(SPELL_SPERE))
                   pPlayer->RemoveAurasDueToSpell(SPELL_SPERE);
                JumpNextStep(1000);
                break;
            case 5:
                DoScriptText(SAY_DRAKURU_PREFIGHT03, m_creature);
                if (pPlayer && pPlayer->IsInWorld())
                    m_creature->AI()->AttackStart(pPlayer);
                JumpNextStep(7000);
                break;
            case 7:
                if (Creature* pPortal = m_creature->SummonCreature(NPC_INVISMAN,TelPos[6].x, TelPos[6].y, TelPos[6].z, TelPos[6].o,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,100000))
                {
                    m_uiPortalGUID = pPortal->GetGUID();
                    m_creature->SetFacingToObject(pPortal);
                }
                JumpNextStep(2000);
                break;
            case 8:
                DoScriptText(SAY_DRAKURU_EVENT01, m_creature);
                CastTroll(false);
                JumpNextStep(4000);
                break;
            case 9:
                DoScriptText(SAY_DRAKURU_EVENT02, m_creature);
                JumpNextStep(2000);
                break;
            case 10:
                if (Unit* pPortal = m_creature->GetMap()->GetCreature(m_uiPortalGUID))
                    m_creature->CastSpell(pPortal, SPELL_PORTAL_MISSILE, false);
                JumpNextStep(2000);
                break;
            case 11:
                if (Unit* pPortal = m_creature->GetMap()->GetCreature(m_uiPortalGUID))
                    pPortal->CastSpell(pPortal, SPELL_ARTHAS_PORTAL, false);
                JumpNextStep(6000);
                break;
            case 12:
                if (Creature* pLichKing = m_creature->SummonCreature(NPC_LICH_KING,TelPos[6].x, TelPos[6].y, TelPos[6].z, TelPos[6].o,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,76000))
                {
                    m_uiLichKingGUID = pLichKing->GetGUID();
                    pLichKing->GetMotionMaster()->MovePoint(0, TelPos[7].x, TelPos[7].y, TelPos[7].z);
                }
                JumpNextStep(10000);
                break;
            case 13:
                DoScriptText(SAY_DRAKURU_EVENT03, m_creature);
                JumpNextStep(9000);
                break;
            case 14:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICH_KING_EVENT01, pLichKing);
                JumpNextStep(7000);
                break;
            case 15:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICH_KING_EVENT02, pLichKing);
                JumpNextStep(4000);
                break;
            case 16:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    pLichKing->CastSpell(m_creature, SPELL_DEATH_TOUCH, false);
                JumpNextStep(3400);
                break;
            case 17:
                if (Creature* pSkull = m_creature->SummonCreature(NPC_INVISMAN,TelPos[8].x, TelPos[8].y, TelPos[8].z, TelPos[8].o,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000))
                {
                    m_uiSkullSummonerGUID = pSkull->GetGUID();
                    m_creature->CastSpell(pSkull, SPELL_SKULL_MISSILE, true);
                    m_creature->CastSpell(m_creature, SPELL_MEAT_01, true); 
                    m_creature->CastSpell(m_creature, SPELL_MEAT_02, true); 
                    m_creature->CastSpell(m_creature, SPELL_MEAT_03, true); 
                    m_creature->CastSpell(m_creature, SPELL_MEAT_01, true); 
                    m_creature->CastSpell(m_creature, SPELL_MEAT_02, true); 
                    m_creature->CastSpell(m_creature, SPELL_MEAT_03, true);
                    m_creature->CastSpell(m_creature, SPELL_DRAKURU_DEATH, false);
                }
                JumpNextStep(1600);
                break;
            case 18:
                m_creature->SetVisibility(VISIBILITY_OFF);
                JumpNextStep(1400);
                break;
            case 19:
                if (Unit* pSkull = m_creature->GetMap()->GetCreature(m_uiSkullSummonerGUID))
                    pSkull->CastSpell(pSkull, SPELL_SUMMON_SKULL, false);
                JumpNextStep(1400);
                break;
            case 20:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICH_KING_EVENT03, pLichKing);
                JumpNextStep(4000);
                break;
            case 21:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                   DoScriptText(SAY_LICH_KING_EVENT04, pLichKing);
                JumpNextStep(13000);
                break;
            case 22:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICH_KING_EVENT05, pLichKing);
                JumpNextStep(4000);
                break;
            case 23:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICH_KING_EVENT06, pLichKing);
                JumpNextStep(8000);
                break;
            case 24:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                   pLichKing->GetMotionMaster()->MovePoint(0, TelPos[6].x, TelPos[6].y, TelPos[6].z);
                JumpNextStep(2000);
                break;
            case 25:
                if (Unit* pLichKing = m_creature->GetMap()->GetCreature(m_uiLichKingGUID))
                {
                    pLichKing->HandleEmoteCommand(11);
                    pLichKing->PlayDirectSound(14820);
                }
                JumpNextStep(10000);
                break;
            case 26:
                EnterEvadeMode();
                break;
        }
    }

    void CastTroll(bool m_bIsCast)
    {
        std::list<Creature*> pCreatures;
        GetCreatureListWithEntryInGrid(pCreatures, m_creature, NPC_DEATH_TROLL, 100.0f);

        if (pCreatures.empty())
            return;
 
        for (std::list<Creature*>::iterator iter = pCreatures.begin(); iter != pCreatures.end(); ++iter)
            if (m_bIsCast)
                (*iter)->CastSpell((*iter), SPELL_ZONE_AURA, false);
            else
                (*iter)->DealDamage((*iter), (*iter)->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiStepTimer <= uiDiff)
            Event();
        else
            m_uiStepTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance2d(m_creature->getVictim()) > 4.0f && (m_creature->getVictim())->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_uiShadowBoltTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_SHADOWBOLT);
                m_uiShadowBoltTimer = 2500;
            }
            else
                m_uiShadowBoltTimer -= uiDiff;
        }

        if ((m_creature->getVictim())->GetTypeId() != TYPEID_PLAYER)
        {
            if (m_uiCrystalTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1))
                {
                    if (pTarget->GetTypeId() == TYPEID_PLAYER)
                    {
                        m_creature->CastSpell(pTarget, SPELL_THROW_CRYSTALL, false);
                        m_uiCrystalTimer = 5000;
                    }
                }
            }
            else
                m_uiCrystalTimer -= uiDiff;
        }

        if (m_uiYellTimer < uiDiff)
        {
            switch(urand(0,5))
            {
                case 0: DoScriptText(SAY_DRAKURU_RANDOMYELL01, m_creature); break;
                case 1: DoScriptText(SAY_DRAKURU_RANDOMYELL02, m_creature); break;
                case 2: DoScriptText(SAY_DRAKURU_RANDOMYELL03, m_creature); break;
                case 3: DoScriptText(SAY_DRAKURU_RANDOMYELL04, m_creature); break;
                case 4: DoScriptText(SAY_DRAKURU_RANDOMYELL05, m_creature); break;
                case 5: DoScriptText(SAY_DRAKURU_RANDOMYELL06, m_creature); break;
            }
            m_uiYellTimer = urand(15000, 20000);
        } else m_uiYellTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_npc_overlord_drakuru(Player *player, Creature *pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(QUEST_BETREYAL) == QUEST_STATUS_INCOMPLETE)
        if (((npc_overlord_drakuruAI*)pCreature->AI())->m_uiStep == 0)
            player->ADD_GOSSIP_ITEM(0, "I'am ready to accompany you, Drakuru.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(pCreature),pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_overlord_drakuru(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+2)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        ((npc_overlord_drakuruAI*)pCreature->AI())->StartEvent(1, 4000, pPlayer);
        pPlayer->TeleportTo(pPlayer->GetMapId(), TelPos[1].x, TelPos[1].y, TelPos[1].z, TelPos[1].o, false);
        pCreature->SetActiveObjectState(true);
        pCreature->SetVisibility(VISIBILITY_OFF);
        pCreature->NearTeleportTo(TelPos[0].x, TelPos[0].y, TelPos[0].z, TelPos[0].o);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

CreatureAI* GetAI_npc_overlord_drakuru(Creature* pCreature)
{
    return new npc_overlord_drakuruAI(pCreature);
}

/*##### Quest: Leave No One Behind #####
SDAuthor: MaxXx2021
*/

enum
{
    SPELL_AID               = 50662,
    SPELL_JOSETHINA_BLOOD   = 50693,
    SPELL_JOSETHINA_SUMMON  = 50692,

    SPELL_JONATHAN_BLOOD    = 50666,
    SPELL_JONATHAN_SUMMON   = 50664,

    SPELL_LAMUF_SUMMON      = 50678,
    SPELL_LAMUF_BLOOD       = 50679,

    NPC_JOSETHINA           = 28148,
    NPC_JOSETHINA_START     = 28143,

    NPC_LAMUF               = 28142,
    NPC_LAMUF_START         = 28141,

    NPC_JONATHAN            = 28136,
    NPC_JONATHAN_START      = 28133,

    NPC_DOCTOR              = 28125,
};

static uint32 KnightSpells[3][2] = 
{
    {SPELL_JOSETHINA_BLOOD, SPELL_JOSETHINA_SUMMON},
    {SPELL_JONATHAN_BLOOD, SPELL_JONATHAN_SUMMON},
    {SPELL_LAMUF_BLOOD, SPELL_LAMUF_SUMMON},
};

struct MANGOS_DLL_DECL npc_rescue_knightsAI : public ScriptedAI
{
    npc_rescue_knightsAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        pOwner = NULL;
        uiKnight = 0;
        InitKnight();
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pSummon = (TemporarySummon*)m_creature;
            pOwner = m_creature->GetMap()->GetUnit(pSummon->GetSummonerGuid());
            if (pOwner)
                Reset();
        }
        else
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
    }

    uint8 uiKnight;
    bool m_bAuraApply;
    Unit* pOwner;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        m_creature->CastSpell(m_creature, KnightSpells[uiKnight][0], false);
        m_bAuraApply = true;
    }

    void InitKnight()
    {
        switch(m_creature->GetEntry())
        {
            case NPC_JOSETHINA_START: 
            case NPC_JOSETHINA: 
                uiKnight = 0; 
                break;
            case NPC_JONATHAN_START: 
            case NPC_JONATHAN:
                uiKnight = 1; 
                break;
            case NPC_LAMUF_START: 
            case NPC_LAMUF:
                uiKnight = 2; 
                break;
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_AID && !m_creature->IsTemporarySummon())
        {
            pCaster->CastSpell(pCaster, KnightSpells[uiKnight][1], true);
            m_creature->ForcedDespawn();
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pOwner && pWho->GetEntry() == NPC_DOCTOR && m_creature->GetDistance(pWho) < 5.0f)
        {
            ((Player*)pOwner)->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetGUID());
            m_creature->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!pOwner || !m_bAuraApply)
            return;

        if (m_bAuraApply && !m_creature->HasAura(KnightSpells[uiKnight][0]))
            m_creature->ForcedDespawn();

        if (!pOwner || !pOwner->IsInWorld())
        {
            m_creature->ForcedDespawn();
            return;
        }

        if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveFollow(pOwner, 2.0f, pOwner->GetOrientation()-4+uiKnight);
        }
    }
};

CreatureAI* GetAI_npc_rescue_knights(Creature* pCreature)
{
    return new npc_rescue_knightsAI(pCreature);
}

void AddSC_zuldrak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_gurgthock";
    pNewScript->GetAI = &GetAI_npc_gurgthock;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_gurgthock;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_scourge_enclosure";
    pNewScript->pGOUse = &GOUse_scourge_enclosure;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_overlord_drakuru";
    pNewScript->GetAI = &GetAI_npc_overlord_drakuru;
    pNewScript->pGossipHello = &GossipHello_npc_overlord_drakuru;
    pNewScript->pGossipSelect = &GossipSelect_npc_overlord_drakuru;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rescue_knights";
    pNewScript->GetAI = &GetAI_npc_rescue_knights;
    pNewScript->RegisterSelf();
}
