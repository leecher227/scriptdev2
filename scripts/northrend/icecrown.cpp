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
SDName: Icecrown
SD%Complete: 100
SDComment: Quest support: 12807, Vendor support: 34885
SDCategory: Icecrown
EndScriptData */

/* ContentData
npc_arete
npc_dame_evniki_kapsalis
EndContentData */

#include "precompiled.h"
#include "TemporarySummon.h"

/*######
## npc_arete
######*/

#define GOSSIP_ARETE_ITEM1 "Lord-Commander, I would hear your tale."
#define GOSSIP_ARETE_ITEM2 "<You nod slightly but do not complete the motion as the Lord-Commander narrows his eyes before he continues.>"
#define GOSSIP_ARETE_ITEM3 "I thought that they now called themselves the Scarlet Onslaught?"
#define GOSSIP_ARETE_ITEM4 "Where did the grand admiral go?"
#define GOSSIP_ARETE_ITEM5 "That's fine. When do I start?"
#define GOSSIP_ARETE_ITEM6 "Let's finish this!"
#define GOSSIP_ARETE_ITEM7 "That's quite a tale, Lord-Commander."

enum
{
    GOSSIP_TEXTID_ARETE1        = 13525,
    GOSSIP_TEXTID_ARETE2        = 13526,
    GOSSIP_TEXTID_ARETE3        = 13527,
    GOSSIP_TEXTID_ARETE4        = 13528,
    GOSSIP_TEXTID_ARETE5        = 13529,
    GOSSIP_TEXTID_ARETE6        = 13530,
    GOSSIP_TEXTID_ARETE7        = 13531,

    QUEST_THE_STORY_THUS_FAR    = 12807
};

bool GossipHello_npc_arete(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_THE_STORY_THUS_FAR) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE1, pCreature->GetGUID());
        return true;
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_arete(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE3, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE4, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE5, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE6, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE7, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(QUEST_THE_STORY_THUS_FAR);
            break;
    }

    return true;
}

/*######
## npc_dame_evniki_kapsalis
######*/

enum
{
    TITLE_CRUSADER              = 123,

    FACTION_ARGENT_CRUSADE      = 1106,
    
    //Horde
    FACTION_ORGRIMMAR           = 76,
    FACTION_DARKSPEAR_TROLLS    = 530,
    FACTION_UNDERCITY           = 68,
    FACTION_THUNDER_BLUFF       = 81,
    FACTION_SILVERMOON_CITY     = 911,

    //Alliance
    FACTION_STORMWIND           = 72,
    FACTION_IRONFORGE           = 47,
    FACTION_GNOMEREGAN          = 54,
    FACTION_DARNASSUS           = 69,
    FACTION_EXODAR              = 930,
};

bool GossipHello_npc_dame_evniki_kapsalis(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetReputationRank(FACTION_ARGENT_CRUSADE) >= REP_EXALTED &&
        ((pPlayer->GetTeam() == HORDE &&
        pPlayer->GetReputationRank(FACTION_ORGRIMMAR) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_DARKSPEAR_TROLLS) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_UNDERCITY) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_THUNDER_BLUFF) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_SILVERMOON_CITY) >= REP_EXALTED) ||
        (pPlayer->GetTeam() == ALLIANCE &&
        pPlayer->GetReputationRank(FACTION_STORMWIND) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_IRONFORGE) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_GNOMEREGAN) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_DARNASSUS) >= REP_EXALTED &&
        pPlayer->GetReputationRank(FACTION_EXODAR) >= REP_EXALTED)))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_dame_evniki_kapsalis(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
    return true;
}

/*##### QUEST: ORBAZ FATE #####
SDAuthor: MaxXx2021 aka Mioka
SDComplete: 80%
SDComment: Need Core Support Chain Dest on Map Allow.
*/

enum
{
    SAY_ORBAZ_YELL01      = -1954301,
    SAY_ORBAZ_YELL02      = -1954302,
    SAY_ORBAZ_YELL03      = -1954303,

    SAY_ORBAZ_SLAY        = -1954304,
   
    SAY_MOGRAINE_EVENT01  = -1954305,
    SAY_MOGRAINE_EVENT02  = -1954306,
    SAY_ORBAZ_EVENT03     = -1954307,
    SAY_MOGRAINE_EVENT04  = -1954308,

    SAY_ORBAZ_DEATH       = -1954309,
    SAY_MOGRAINE_END      = -1954310,

    NPC_ORBAZ             = 31283,
    NPC_DARION            = 31290,
    NPC_OMINOS            = 32406,
 
    SPELL_PLAGUE_STRIKE   = 60186,
    SPELL_POWER_BLADE     = 61015,
    SPELL_POWER_DAMAGE    = 61016,
    SPELL_UNHOLY_GROUND   = 55741,
    SPELL_FLY_CLOUD       = 60984,
    SPELL_FLY_CLOUD_VIS   = 60977,
    SPELL_FLY_CLOUD_DMG   = 60984,
    SPELL_SUMMON_MOGRAINE = 58939,
};

struct MANGOS_DLL_DECL npc_orbaz_bloodbaneAI : public ScriptedAI
{
    npc_orbaz_bloodbaneAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }
  
    uint64 m_uiDarionGUID;

    uint32 m_uiPlagueStrikeTimer;
    uint32 m_uiCloudTimer;
    uint32 m_uiEnergyTimer; //in phase II when darion come!
    uint32 m_uiYellTimer;

    uint8 m_uiYell;
    uint8 m_uiPhase;

    void Reset() 
    {
        m_uiPlagueStrikeTimer = 5000;
        m_uiCloudTimer = urand(10000, 18000);
        m_uiEnergyTimer = 7000;
        m_uiYellTimer = 9000;
        m_uiYell = 0;
        m_uiPhase = 1;
        if (Creature* pDarion = m_creature->GetMap()->GetCreature(m_uiDarionGUID))
           pDarion->ForcedDespawn(1000);
        m_uiDarionGUID = 0;
    }

    void Aggro(Unit* pWho)
    { 
        m_creature->MonsterYell("Ah. I'll be waiting.",LANG_UNIVERSAL,NULL);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_ORBAZ_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_ORBAZ_SLAY, m_creature);
    }

    void SpellHitTarget(Unit *pTarget, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_PLAGUE_STRIKE)
            m_creature->CastSpell(pTarget, SPELL_UNHOLY_GROUND, false);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_DARION)
        {
            pSummoned->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
            pSummoned->SetFacingToObject(m_creature);
            m_uiDarionGUID = pSummoned->GetGUID(); 
        } 
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho, 2.0f, urand(0, 6.0f));
        }
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        if (damage > m_creature->GetHealth() && done_by->GetEntry() != NPC_DARION)
            damage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPlagueStrikeTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_PLAGUE_STRIKE);
            m_uiPlagueStrikeTimer = urand(9000, 15000);
        }
        else 
            m_uiPlagueStrikeTimer -= uiDiff;

        switch(m_uiPhase)
        {
            case 1:
                if (m_uiYellTimer < uiDiff)
                {
                    switch(m_uiYell)
                    {
                        case 0: DoScriptText(SAY_ORBAZ_YELL01, m_creature); m_uiYell++; break;
                        case 1: DoScriptText(SAY_ORBAZ_YELL02, m_creature); m_uiYell++; break;
                        case 2: DoScriptText(SAY_ORBAZ_YELL03, m_creature); m_uiYell++; break;
                    }
                    m_uiYellTimer = urand(15000, 20000);
                } 
                else 
                    m_uiYellTimer -= uiDiff;

                if (m_creature->GetHealthPercent() < 40.0f)
                {
                    m_uiPhase = 2;
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->CastSpell(m_creature, SPELL_SUMMON_MOGRAINE, true);
                    m_creature->CastSpell(m_creature, SPELL_POWER_BLADE, false);
                    m_uiYellTimer = 3000;
                    m_uiYell = 0;
                }
                break;
            case 2:
                if (m_creature->HasAura(SPELL_POWER_BLADE))
                {
                    if (m_uiEnergyTimer < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_POWER_DAMAGE);
                        m_uiEnergyTimer = 3000;
                    }
                    else 
                        m_uiEnergyTimer -= uiDiff;
                }
                else 
                {
                    if (m_uiEnergyTimer < uiDiff)
                    {
                        m_creature->CastSpell(m_creature, SPELL_POWER_BLADE, false);
                        m_uiEnergyTimer = 7000;
                    }
                    else 
                        m_uiEnergyTimer -= uiDiff;
                }

                if (m_uiYellTimer < uiDiff)
                {
                    m_uiYellTimer = 15000;
                    switch(m_uiYell)
                    {
                        case 0: 
                            if (Creature* pDarion = m_creature->GetMap()->GetCreature(m_uiDarionGUID))
                                DoScriptText(SAY_MOGRAINE_EVENT01, pDarion); 
                            m_uiYellTimer = 5000;
                            m_uiYell++;
                            break;
                        case 1: 
                            if (Creature* pDarion = m_creature->GetMap()->GetCreature(m_uiDarionGUID))
                                DoScriptText(SAY_MOGRAINE_EVENT02, pDarion);
                            m_uiYellTimer = 7000;
                            m_uiYell++;
                            break;
                        case 2: 
                            if (Creature* pDarion = m_creature->GetMap()->GetCreature(m_uiDarionGUID))
                                DoScriptText(SAY_ORBAZ_EVENT03, m_creature);
                            m_uiYellTimer = 5000;
                            m_uiYell++;
                            break;
                        case 3:
                            if (Creature* pDarion = m_creature->GetMap()->GetCreature(m_uiDarionGUID))
                                DoScriptText(SAY_MOGRAINE_EVENT04, pDarion);
                            m_uiYellTimer = 5000;
                            m_uiYell++;
                            break;
                        case 4:
                            if (Creature* pDarion = m_creature->GetMap()->GetCreature(m_uiDarionGUID))
                                pDarion->AI()->AttackStart(m_creature);
                            m_uiYellTimer = 7000;
                            m_uiYell++;
                            break;
                    }
                }
                else 
                    m_uiYellTimer -= uiDiff;
                break;
        }
         
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_orbaz_bloodbane(Creature* pCreature)
{
    return new npc_orbaz_bloodbaneAI(pCreature);
}

struct MANGOS_DLL_DECL npc_darion_mograine_fbAI : public ScriptedAI
{
    npc_darion_mograine_fbAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }

    uint8 m_uiStep;
   
    uint32 m_uiStepTimer;

    void Reset()
    {
        m_uiStep = 0;
        m_uiStepTimer = 1000;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho, 2.0f, urand(0, 360.0f));
        }
    }

    void EnterEvadeMode()
    {
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        m_creature->SetLootRecipient(NULL);
        m_uiStepTimer = 5000;
        if (m_creature->IsTemporarySummon())
            m_uiStep = 1;
    }

    void JumpNextStep(uint32 Timer)
    {
        m_uiStep++;
        m_uiStepTimer = Timer;
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 1:
                DoScriptText(SAY_MOGRAINE_END, m_creature);
                JumpNextStep(5000);
                break;
            case 2:
                m_creature->GetMotionMaster()->MovePoint(0, 5969.264f, 1965.250f, 514.225f);
                m_creature->ForcedDespawn(10000);
                JumpNextStep(5000);
                break;
        }
    }

    void DoMeleeAttackIfReady()
    {
        //Make sure our attack is ready before checking distance
        if (m_creature->isAttackReady())
        {
            //If we are within range melee the target
            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
            {
                WeaponAttackType attack = urand(0,1) ? BASE_ATTACK : OFF_ATTACK;
                m_creature->AttackerStateUpdate(m_creature->getVictim(), attack);
                m_creature->resetAttackTimer(attack);
            }  
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiStepTimer < uiDiff)
            Event();
        else 
            m_uiStepTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_darion_mograine_fb(Creature* pCreature)
{
    return new npc_darion_mograine_fbAI(pCreature);
}

//################ QUEST: The Admiral Revealed ###################

enum
{
    SAY_MALGANIS_ESCAPE_01   = -1999407,
    SAY_MALGANIS_ESCAPE_02   = -1999408,
    SAY_MALGANIS_SLAY_01     = -1999403,
    SAY_MALGANIS_SLAY_02     = -1999404,
    SAY_MALGANIS_SLAY_03     = -1999405,
    SAY_MALGANIS_SLAY_04     = -1999406,
    SAY_MALGANIS_40HP        = -1999401,
    SAY_MALGANIS_15HP        = -1999402,
    SAY_WESTWIND_AGGRO       = -1999409,
    SAY_WESTWIND_50HP        = -1999410,
    SAY_WESTWIND_SHIELD      = -1999411,
    SAY_WESTWIND_SLAY_01     = -1999412,
    SAY_WESTWIND_SLAY_02     = -1999413,
    SAY_WESTWIND_SLAY_03     = -1999414,
    SAY_WESTWIND_SLAY_04     = -1999415,

    //Main Spells
    SPELL_SHIELD             = 50161,
    SPELL_MALGANIS_PORTAL    = 27731,
    SPELL_MALGANIS_DUMMY     = 27768,
    SPELL_MALGANIS_RETURN    = 31699,
    SPELL_REMOVE_SHIELD      = 20211,
    SPELL_SHIELD_TRIGG       = 31699,

    //Weswind Form
    SPELL_HEROIC_STRIKE      = 57846,
    SPELL_CLEAVE             = 15284,
    SPELL_WHIRLWIND          = 50622,

    //Malganis Form
    SPELL_SWAMP              = 60502,
    SPELL_MIND_BLAST         = 60500,
    SPELL_VAMPIRE            = 60501,

    NPC_MALGANIS             = 29620, //He is Alive?
    NPC_WESTWIND             = 29621,
    NPC_CREDIT               = 29627, //spell on this credit empty, i think this npc escape portal and kill credit, and killed by spell MALGANIS_DUMMY

    PHASE_WESTWIND           = 1,
    PHASE_SHIELD             = 2,
    PHASE_TRANSFORM          = 3,
    PHASE_MALGANIS           = 4,
    PHASE_ESCAPE             = 5,
};

struct MANGOS_DLL_DECL boss_hight_admiral_westwindAI : public ScriptedAI
{
    boss_hight_admiral_westwindAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }
  
    uint64 m_uiCreditGUID;

    uint32 m_uiSpell01Timer;
    uint32 m_uiSpell02Timer;
    uint32 m_uiSpell03Timer;
    uint32 m_uiStepTimer;

    uint8 m_uiStep;
    uint8 m_uiPhase;

    bool m_bNonFight;
  
    Unit* pOwner;

    void Reset() 
    {
        m_uiSpell01Timer = 5000;
        m_uiSpell02Timer = urand(10000, 18000);
        m_uiSpell03Timer = 7000;
        m_uiStepTimer = 9000;
        m_uiStep = 0;
        m_uiPhase = PHASE_WESTWIND;
        m_uiCreditGUID = 0;
        m_bNonFight = false;
        pOwner = NULL;
        m_creature->SetVisibility(VISIBILITY_ON);
        SetEquipmentSlots(true, 0, 0, 0);
    }

    void Aggro(Unit* pWho)
    { 
        DoScriptText(SAY_WESTWIND_AGGRO, m_creature);
        pOwner = pWho;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 3))
        {
            case 0: 
                if (m_uiPhase != PHASE_MALGANIS)
                    DoScriptText(SAY_WESTWIND_SLAY_01, m_creature); 
                else 
                    DoScriptText(SAY_MALGANIS_SLAY_01, m_creature); 
                break;
            case 1:
                if (m_uiPhase != PHASE_MALGANIS)
                    DoScriptText(SAY_WESTWIND_SLAY_02, m_creature); 
                else
                    DoScriptText(SAY_MALGANIS_SLAY_02, m_creature); 
                break;
            case 2:
                if (m_uiPhase != PHASE_MALGANIS)
                    DoScriptText(SAY_WESTWIND_SLAY_03, m_creature); 
                else
                    DoScriptText(SAY_MALGANIS_SLAY_03, m_creature); 
                break;
            case 3:
                if (m_uiPhase != PHASE_MALGANIS)
                    DoScriptText(SAY_WESTWIND_SLAY_04, m_creature); 
                else 
                    DoScriptText(SAY_MALGANIS_SLAY_04, m_creature); 
                break;
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (m_bNonFight)
            return;

        if(m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho, 2.0f);
        }
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        if (damage > m_creature->GetHealth())
        {
            damage = 0;
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_bNonFight = true;
                m_creature->SetHealth(1);
                DoScriptText(SAY_MALGANIS_ESCAPE_01, m_creature);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->AttackStop();
                m_creature->StopMoving();
                if (Creature* pCredit = m_creature->SummonCreature(NPC_CREDIT, 7493.825f, 4866.314f, -12.477f, 1.432f, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    m_uiCreditGUID = pCredit->GetGUID();
                    pCredit->SetDisplayId(11686);
                    pCredit->CastSpell(pCredit, SPELL_MALGANIS_PORTAL, false);
                    pCredit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                m_uiStep = 0;
                m_uiStepTimer = 7000;
                m_uiPhase = PHASE_ESCAPE;
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_REMOVE_SHIELD && m_uiPhase == PHASE_SHIELD)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_SHIELD);
            DoScriptText(SAY_WESTWIND_SHIELD, m_creature);
            m_uiStepTimer = 10500;
            m_uiPhase = PHASE_TRANSFORM;
        }
    }

    void EscapePhase()
    {
        switch(m_uiStep)
        {
            case 0:
                m_creature->GetMotionMaster()->MoveTargetedHome(); 
                m_uiStepTimer = 1000;
                m_uiStep++;
                break;
            case 2:
                DoScriptText(SAY_MALGANIS_ESCAPE_02, m_creature);
                if (pOwner && pOwner->IsInWorld())
                    m_creature->SetFacingToObject(pOwner);
                m_uiStepTimer = 10000;
                m_uiStep++;
                break;
            case 3:
                m_creature->SetVisibility(VISIBILITY_OFF);
                m_uiStepTimer = 3000;
                m_uiStep++;
                break;
            case 4:
                if (pOwner && pOwner->IsInWorld())
                    if (Creature* pCredit = m_creature->GetMap()->GetCreature(m_uiCreditGUID))
                        pOwner->DealDamage(pCredit, pCredit->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->ForcedDespawn(500);
                m_uiStep++;
                break;
        }
    }

    void JustReachedHome()
    {
        if (m_uiPhase == PHASE_ESCAPE)
            m_uiStep = 2;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            case PHASE_WESTWIND:
            case PHASE_SHIELD:
            case PHASE_TRANSFORM:
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if (m_uiSpell01Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HEROIC_STRIKE) == CAST_OK)
                        m_uiSpell01Timer = urand(4000, 8000);
                }
                else 
                    m_uiSpell01Timer -= uiDiff;

                if (m_uiSpell02Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                        m_uiSpell02Timer = urand(6000, 12000);
                }
                else
                    m_uiSpell02Timer -= uiDiff;

                if (m_uiSpell03Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND) == CAST_OK)
                        m_uiSpell03Timer = urand(9000, 15000);
                }
                else
                    m_uiSpell03Timer -= uiDiff;


                if (m_creature->GetHealthPercent() < 50 && m_uiPhase == PHASE_WESTWIND)
                {
                    m_creature->CastSpell(m_creature, SPELL_SHIELD, false);
                    DoScriptText(SAY_WESTWIND_50HP, m_creature); 
                    m_uiPhase = PHASE_SHIELD;
                }

                if (m_uiPhase == PHASE_TRANSFORM)
                {
                    if (m_uiStepTimer < uiDiff)
                    {
                        m_uiPhase = PHASE_MALGANIS;
                        m_creature->UpdateEntry(NPC_MALGANIS);
                        SetEquipmentSlots(false, 0, 0, 0);
                    }
                    else
                        m_uiStepTimer -= uiDiff;
                }
                DoMeleeAttackIfReady();
                break;
            case PHASE_MALGANIS:
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if (m_uiSpell01Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWAMP) == CAST_OK)
                        m_uiSpell01Timer = urand(6000, 13000);
                }
                else
                    m_uiSpell01Timer -= uiDiff;

                if (m_uiSpell02Timer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                        if (DoCastSpellIfCan(pTarget, SPELL_MIND_BLAST) == CAST_OK)
                            m_uiSpell02Timer = urand(5000, 10000);
                }
                else
                    m_uiSpell02Timer -= uiDiff;

                if (m_uiSpell03Timer < uiDiff)
                {
                    if (!m_creature->HasAura(SPELL_VAMPIRE))
                        DoCastSpellIfCan(m_creature, SPELL_VAMPIRE);
                    m_uiSpell03Timer = 2000;
                }
                else
                    m_uiSpell03Timer -= uiDiff;

                if( m_creature->GetHealthPercent() < 40 && m_uiStep == 0)
                {
                    m_uiStep++;
                    DoScriptText(SAY_MALGANIS_40HP, m_creature);
                }

                if (m_creature->GetHealthPercent() < 15 && m_uiStep == 1)
                {
                    m_uiStep++;
                    DoScriptText(SAY_MALGANIS_15HP, m_creature);
                }
                DoMeleeAttackIfReady();
                break;
            case PHASE_ESCAPE:
                if (m_uiStepTimer < uiDiff)
                    EscapePhase();
                else
                    m_uiStepTimer -= uiDiff;
                break;
        }
    }
};

CreatureAI* GetAI_boss_hight_admiral_westwind(Creature* pCreature)
{
    return new boss_hight_admiral_westwindAI(pCreature);
}

//################ QUEST: The Second Chance ###################

enum
{
    SAY_EVENT_01                = -1999430,
    SAY_EVENT_02                = -1999431,
    SAY_EVENT_03                = -1999432,
    SAY_EVENT_04                = -1999433,
    SAY_EVENT_05                = -1999434,
    SAY_EVENT_06                = -1999435,
    SAY_EVENT_07                = -1999436,
    SAY_EVENT_08                = -1999437,
    SAY_EVENT_09                = -1999438,
    SAY_EVENT_10                = -1999439,
    SAY_EVENT_11                = -1999440,
    SAY_EVENT_12                = -1999441,
    SAY_EVENT_13                = -1999442,

    SPELL_SUMMON_GATE           = 26560, //spell script target
    SPELL_SUMMON_ARETE          = 18280,
    SPELL_SUMMON_SOUL           = 12600,
    SPELL_GET_SOUL              = 60452,
    SPELL_TELEPORT              = 52096,
   
    NPC_LANDGREN                = 29542,
    NPC_LANDGREN_SOUL           = 29572, //383 drowned state
    NPC_ARETE                   = 29560,
    NPC_SOUL_TARGET             = 29577,

    GO_ARETE_GATE               = 191579,

    SPELL_MIND_BLAST_L          = 60453,
    SPELL_MIND_SHIRE            = 60440,
    SPELL_SHADOW_FORM           = 60449,
    SPELL_SHADOW_PAIN           = 60446,
};

struct MANGOS_DLL_DECL npc_archbishop_landgrenAI : public ScriptedAI
{
    npc_archbishop_landgrenAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_creature->SetCorpseDelay(300000);
        m_creature->SetRespawnDelay(300005);
        Reset();
    }

    uint32 m_uiBlastTimer;
    uint32 m_uiPainTimer;
    uint32 m_uiShireTimer;
    uint32 m_uiFormTimer;

    void Reset()
    {
        m_uiBlastTimer = urand(5000, 13000);
        m_uiShireTimer = urand(1000, 5000);
        m_uiPainTimer = urand(8000, 20000);
        m_uiFormTimer = 1000;
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_EVENT_01, m_creature);
        m_creature->CastSpell(m_creature, SPELL_SUMMON_ARETE, true);
    }

    inline float GetManaPercent() 
    {
        return (((float)m_creature->GetPower(POWER_MANA) / (float)m_creature->GetMaxPower(POWER_MANA)) * 100);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (GetManaPercent() < 30)
            m_creature->SetPower(POWER_MANA, m_creature->GetMaxPower(POWER_MANA));

        if (m_uiPainTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_PAIN) == CAST_OK)
                    m_uiPainTimer = urand(8000, 20000);
        }
        else
            m_uiPainTimer -= uiDiff;

        if (m_uiBlastTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if (DoCastSpellIfCan(pTarget, SPELL_MIND_BLAST_L) == CAST_OK)
                    m_uiBlastTimer = urand(5000, 10000);
        }
        else
            m_uiBlastTimer -= uiDiff;

        if (m_uiShireTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_SHIRE) == CAST_OK)
                m_uiShireTimer = urand(8000, 12000);
        }
        else
            m_uiShireTimer -= uiDiff;

        if (m_uiFormTimer < uiDiff && m_creature->GetHealthPercent() < 50 && !m_creature->HasAura(SPELL_SHADOW_FORM))
        {
            DoCastSpellIfCan(m_creature, SPELL_SHADOW_FORM);
        }
        else
            m_uiFormTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_archbishop_landgren(Creature* pCreature)
{
    return new npc_archbishop_landgrenAI(pCreature);
}

struct MANGOS_DLL_DECL npc_lord_commander_areteAI : public ScriptedAI
{
    npc_lord_commander_areteAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pSummon = (TemporarySummon*)m_creature;
            if (Unit* pOwner = m_creature->GetMap()->GetUnit(pSummon->GetSummonerGuid()))
                m_uiLandgrenGUID = pOwner->GetGUID();
            m_bEvent = true;
            m_creature->ForcedDespawn(300000);
        }
    }

    bool m_bEvent;

    uint32 m_uiStepTimer;
    uint32 m_uiStep;
    uint32 m_uiCheckGateTimer;

    uint64 m_uiLandgrenGUID;
    uint64 m_uiSoulGUID;

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiCheckGateTimer = 2000;
        m_bEvent = false;
        m_uiStepTimer = 2000;
        m_uiStep = 0;
        m_uiLandgrenGUID = 0;
        m_uiSoulGUID = 0;
    }
   
    void JumpNextStep(uint32 Time)
    {
        m_uiStepTimer = Time;
        m_uiStep++;
    }

    void StartEvent(GameObject* pGate)
    {
        m_bEvent = false;
        m_uiStepTimer = 2000;
        m_uiStep = 1;
        m_creature->NearTeleportTo(pGate->GetPositionX(), pGate->GetPositionY(), pGate->GetPositionZ(), 0);
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->CastSpell(m_creature, SPELL_TELEPORT, false);
        pGate->RemoveFromWorld();
    }

    void Event()
    {
        switch(m_uiStep)
        {
            case 1:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiLandgrenGUID))
                {
                    float X, Y, Z, fAng;
                    fAng = m_creature->GetAngle(pLandgren);
                    X = pLandgren->GetPositionX()-3*cos(fAng);
                    Y = pLandgren->GetPositionY()-3*sin(fAng);
                    Z = pLandgren->GetPositionZ()+5;
                    m_creature->UpdateAllowedPositionZ(X, Y, Z);
                    m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
                }
                DoScriptText(SAY_EVENT_02, m_creature);
                JumpNextStep(5000);
                 break;
            case 2:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiLandgrenGUID))
                {
                    if (Creature* pSoul = m_creature->SummonCreature(NPC_SOUL_TARGET, pLandgren->GetPositionX(), pLandgren->GetPositionY(), pLandgren->GetPositionZ(), 1.432f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 15000))
                    { 
                        pSoul->SetDisplayId(11686);
                        pSoul->SetObjectScale(0.3f);
                        m_creature->CastSpell(pSoul, SPELL_GET_SOUL, false);
                    }
                }
                DoScriptText(SAY_EVENT_03, m_creature);
                JumpNextStep(5000);
                break;
            case 3:
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->CastSpell(m_creature, SPELL_SUMMON_SOUL, false);
                JumpNextStep(2000);
                break;
            case 4:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                {
                    DoScriptText(SAY_EVENT_04, pLandgren);
                    pLandgren->GetMotionMaster()->MovePoint(0, pLandgren->GetPositionX()-0.3, pLandgren->GetPositionY()+0.3, pLandgren->GetPositionZ()+2);
                }
                JumpNextStep(7000);
                break;
            case 5:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                    pLandgren->SetFacingToObject(m_creature);
                DoScriptText(SAY_EVENT_05, m_creature);
                JumpNextStep(7000);
                break;
            case 6:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                    DoScriptText(SAY_EVENT_06, pLandgren);
                JumpNextStep(1000);
                break;
            case 7:
                DoScriptText(SAY_EVENT_07, m_creature);
                JumpNextStep(4000);
                break;
            case 8:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                    m_creature->CastSpell(pLandgren, 46685, false);
                JumpNextStep(6000);
                break;
            case 9:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                    DoScriptText(SAY_EVENT_08, pLandgren);
                JumpNextStep(4000);
                break;
            case 10:
                m_creature->InterruptNonMeleeSpells(false);
                JumpNextStep(2000);
                break;
            case 11:
                DoScriptText(SAY_EVENT_09, m_creature);
                JumpNextStep(3000);
                break;
            case 12:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                    DoScriptText(SAY_EVENT_10, pLandgren);
                JumpNextStep(12000);
                break;
            case 13:
                DoScriptText(SAY_EVENT_11, m_creature);
                JumpNextStep(10000);
                break;
            case 14:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                   m_creature->CastSpell(pLandgren, 46685, false);
                JumpNextStep(2000);
                break;
            case 15:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                    DoScriptText(SAY_EVENT_12, pLandgren);
                JumpNextStep(4000);
                break;
            case 16:
                if (Creature* pLandgren = m_creature->GetMap()->GetCreature(m_uiSoulGUID))
                {
                    pLandgren->SetVisibility(VISIBILITY_OFF);
                    pLandgren->ForcedDespawn(1000);
                }
                JumpNextStep(1000);
                break;
            case 17:
                DoScriptText(SAY_EVENT_13, m_creature);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                JumpNextStep(3000);
                break;
            
        }
    }

    void JustSummoned(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_LANDGREN_SOUL)
        { 
            pCreature->setFaction(35);
            pCreature->SetUInt32Value(UNIT_NPC_EMOTESTATE, 383);
            m_uiSoulGUID = pCreature->GetGUID();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bEvent)
        {
            if (m_uiCheckGateTimer < uiDiff)
            {
                if (GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_ARETE_GATE, 20.0f))
                    StartEvent(pPortal);
                m_uiCheckGateTimer = 2000;
            }
            else
                m_uiCheckGateTimer -= uiDiff;
        }

        if (m_uiStepTimer < uiDiff)
            Event();
        else
            m_uiStepTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_lord_commander_arete(Creature* pCreature)
{
    return new npc_lord_commander_areteAI(pCreature);
}

void AddSC_icecrown()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_arete";
    newscript->pGossipHello = &GossipHello_npc_arete;
    newscript->pGossipSelect = &GossipSelect_npc_arete;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dame_evniki_kapsalis";
    newscript->pGossipHello = &GossipHello_npc_dame_evniki_kapsalis;
    newscript->pGossipSelect = &GossipSelect_npc_dame_evniki_kapsalis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_orbaz_bloodbane";
    newscript->GetAI = &GetAI_npc_orbaz_bloodbane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_darion_mograine_fb";
    newscript->GetAI = &GetAI_npc_darion_mograine_fb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_hight_admiral_westwind";
    newscript->GetAI = &GetAI_boss_hight_admiral_westwind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_archbishop_landgren";
    newscript->GetAI = &GetAI_npc_archbishop_landgren;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lord_commander_arete";
    newscript->GetAI = &GetAI_npc_lord_commander_arete;
    newscript->RegisterSelf();
}
