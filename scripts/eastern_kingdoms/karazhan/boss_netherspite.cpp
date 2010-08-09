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
SDName: Boss_Netherspite
SD%Complete: 98%
SDComment: sometimes root in banish phase dispels if he takes damage (core issue?)
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"
#include "GameObject.h"

enum
{
    //netherspite spells
    SPELL_NETHERBURN            = 30522,
    SPELL_VOID_ZONE             = 30533,
    SPELL_NETHERBREATH          = 38523,
    SPELL_EMPOWERMENT           = 38549,
    SPELL_NETHER_INFUSION       = 38688,
    SPELL_NETHERSPITE_ROAR      = 38684,
    SPELL_BANISH_VISUAL         = 39833,
    SPELL_ROOT                  = 42716,

    //beam buffs
    SPELL_PERSEVERENCE_NS       = 30466,
    SPELL_PERSEVERENCE_PLR      = 30421,
    SPELL_SERENITY_NS           = 30467,
    SPELL_SERENITY_PLR          = 30422,
    SPELL_DOMINANCE_NS          = 30468,
    SPELL_DOMINANCE_PLR         = 30423,

    //beam debuffs
    SPELL_EXHAUSTION_DOM        = 38639,
    SPELL_EXHAUSTION_SER        = 38638,
    SPELL_EXHAUSTION_PER        = 38637,

    //beam spells
    SPELL_BEAM_DOM              = 30402,
    SPELL_BEAM_SER              = 30401,
    SPELL_BEAM_PER              = 30400,
    SPELL_BLUE_PORTAL           = 30491,
    SPELL_GREEN_PORTAL          = 30490,
    SPELL_RED_PORTAL            = 30487,

    //emotes
    EMOTE_PHASE_BEAM            = -1532089,
    EMOTE_PHASE_BANISH          = -1532090,

    //npcs
    NPC_PORTAL_CREATURE         = 17369
};

struct Spawn_Locations
{
    float x, y, z;
};

static Spawn_Locations Basic_Coordinates[] =
{
    {-11105.508789f, -1600.851685f, 279.950256f},
    {-11195.353516f, -1613.237183f, 278.237258f},
    {-11137.846680f, -1685.607422f, 278.239258f}
};

static Spawn_Locations Fixed_Coordinates[] =
{
    {-11105.508789f, -1600.851685f, 279.950256f},
    {-11195.353516f, -1613.237183f, 278.237258f},
    {-11137.846680f, -1685.607422f, 278.239258f}
};

enum Phases
{
    NULL_PHASE = 0,
    BEAM_PHASE = 1,
    BANISH_PHASE = 2,
};

enum Events
{
    NULL_EVENT = 0,
    EVENT_ENRAGE = 1,
    EVENT_EXHAUST_CHECK = 2,
    EVENT_PHASE_END_TIMER = 3,

    //portal phase events
    EVENT_NETHERSPITE_ROAR = 4,
    EVENT_PORTALS_SPAWN = 5,
    EVENT_BEAM_INITIAL = 6,
    EVENT_BEAM_PERIODIC = 7,
    EVENT_NETHERBURN = 8,
    EVENT_VOID_ZONE = 9,

    //banish phase events
    EVENT_NETHERBREATH_INITIAL = 4,
    EVENT_NETHERBREATH = 5
};

static int Beam_Buff[3][2] =
{
    {SPELL_DOMINANCE_NS,SPELL_DOMINANCE_PLR},
    {SPELL_SERENITY_NS,SPELL_SERENITY_PLR},
    {SPELL_PERSEVERENCE_NS,SPELL_PERSEVERENCE_PLR}
};

static int Beam_Debuff[3] =
{
    SPELL_EXHAUSTION_DOM,
    SPELL_EXHAUSTION_SER,
    SPELL_EXHAUSTION_PER
};

static int Portals[3] =
{
    SPELL_BLUE_PORTAL,
    SPELL_GREEN_PORTAL,
    SPELL_RED_PORTAL
};

static int Beam_Visuals[3] =
{
    SPELL_BEAM_DOM,
    SPELL_BEAM_SER,
    SPELL_BEAM_PER
};

static uint32 MaxTimeCounter[3] = {0,10,6};

struct MANGOS_DLL_DECL boss_netherspiteAI : public ScriptedAI
{
    boss_netherspiteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    Phases m_Active_Phase;

    Events m_Active_Event;

    uint32 m_auiTimers[10];

    uint64 m_auiNpc_BeamGUID[3];
    uint64 m_auiNpc_PortalGUID[3];
    uint64 m_auiCurrent_CandidateGUID[3];
    uint64 m_auiLast_CandidateGUID[3];
    uint64 m_auiExhaust_CandidateGUID[3][10];
    uint64 m_auiRed_Beam_BlockerGUID[10];

    float m_fDISTANCE, p_fDISTANCE, m_fANGLE, p_fANGLE, fRadian;

    bool m_bExists[3], m_bThreat_Increased;

    int m_iRANDcolor[3];

    //0: dominance
    //1: serenity
    //2: perseverence

    void Reset()
    {
        m_Active_Phase = NULL_PHASE;
        m_Active_Event = NULL_EVENT;
        m_creature->RemoveAllAuras();
        fRadian = 0.2f;  //this can be reduced to require more precise placement between netherspite and the portal, for less precision it can be increased.

        for (uint8 i=0; i<3; i++)
        {
            m_auiCurrent_CandidateGUID[i] = 0;
            m_auiLast_CandidateGUID[i] = 0;
            for (uint8 j=0; j<10; j++)
            {
                m_auiExhaust_CandidateGUID[i][j] = 0;
            }
        }
        for(uint8 i=0; i<10; i++)
        {
            m_auiRed_Beam_BlockerGUID[i] = 0;
        }
    }

    void Shuffle_Coordinates()
    {
        //we shuffle the portal locations here
        int iRANGE_MAX2 = 2;
        int iRANGE_MIN3 = 0;
        int iRANGE_MAX1a = 2;
        int iRANGE_MIN2a = 1;
        int iRANGE_MAX1b = 1;
        int iRANGE_MIN2b = 0;
        int color1 = (((double) rand() / (double) RAND_MAX) * iRANGE_MAX2 + iRANGE_MIN3);
        int color2a = (((double) rand() / (double) RAND_MAX) * iRANGE_MAX1a + iRANGE_MIN2a);
        int color2b = (((double) rand() / (double) RAND_MAX) * iRANGE_MAX1b + iRANGE_MIN2b);

        m_iRANDcolor[0] = color1;

        if (m_iRANDcolor[0] == 0)
        {
            Fixed_Coordinates[0].x = Basic_Coordinates[0].x;
            Fixed_Coordinates[0].y = Basic_Coordinates[0].y;
            Fixed_Coordinates[0].z = Basic_Coordinates[0].z;
            m_iRANDcolor[1] = color2a;

            if (m_iRANDcolor[1] == 1)
            {
                Fixed_Coordinates[1].x = Basic_Coordinates[1].x;
                Fixed_Coordinates[1].y = Basic_Coordinates[1].y;
                Fixed_Coordinates[1].z = Basic_Coordinates[1].z;
                Fixed_Coordinates[2].x = Basic_Coordinates[2].x;
                Fixed_Coordinates[2].y = Basic_Coordinates[2].y;
                Fixed_Coordinates[2].z = Basic_Coordinates[2].z;
            }
            else
            {
                Fixed_Coordinates[1].x = Basic_Coordinates[2].x;
                Fixed_Coordinates[1].y = Basic_Coordinates[2].y;
                Fixed_Coordinates[1].z = Basic_Coordinates[2].z;
                Fixed_Coordinates[2].x = Basic_Coordinates[1].x;
                Fixed_Coordinates[2].y = Basic_Coordinates[1].y;
                Fixed_Coordinates[2].z = Basic_Coordinates[1].z;
            }
        }

        if (m_iRANDcolor[0] == 2)
         {
            Fixed_Coordinates[0].x = Basic_Coordinates[2].x;
            Fixed_Coordinates[0].y = Basic_Coordinates[2].y;
            Fixed_Coordinates[0].z = Basic_Coordinates[2].z;
            m_iRANDcolor[1] = color2b;

            if (m_iRANDcolor[1] == 0)
            {
                Fixed_Coordinates[1].x = Basic_Coordinates[0].x;
                Fixed_Coordinates[1].y = Basic_Coordinates[0].y;
                Fixed_Coordinates[1].z = Basic_Coordinates[0].z;
                Fixed_Coordinates[2].x = Basic_Coordinates[1].x;
                Fixed_Coordinates[2].y = Basic_Coordinates[1].y;
                Fixed_Coordinates[2].z = Basic_Coordinates[1].z;
            }
            else
            {
                Fixed_Coordinates[1].x = Basic_Coordinates[1].x;
                Fixed_Coordinates[1].y = Basic_Coordinates[1].y;
                Fixed_Coordinates[1].z = Basic_Coordinates[1].z;
                Fixed_Coordinates[2].x = Basic_Coordinates[0].x;
                Fixed_Coordinates[2].y = Basic_Coordinates[0].y;
                Fixed_Coordinates[2].z = Basic_Coordinates[0].z;
            }
        }

        if (m_iRANDcolor[0] == 1)
        {
            Fixed_Coordinates[0].x = Basic_Coordinates[1].x;
            Fixed_Coordinates[0].y = Basic_Coordinates[1].y;
            Fixed_Coordinates[0].z = Basic_Coordinates[1].z;
            Fixed_Coordinates[1].x = Basic_Coordinates[2].x;
            Fixed_Coordinates[1].y = Basic_Coordinates[2].y;
            Fixed_Coordinates[1].z = Basic_Coordinates[2].z;
            Fixed_Coordinates[2].x = Basic_Coordinates[0].x;
            Fixed_Coordinates[2].y = Basic_Coordinates[0].y;
            Fixed_Coordinates[2].z = Basic_Coordinates[0].z;
        }
    }

    void Restore_Coordinates()
    {
        Fixed_Coordinates[0].x = Basic_Coordinates[0].x;
        Fixed_Coordinates[0].y = Basic_Coordinates[0].y;
        Fixed_Coordinates[0].z = Basic_Coordinates[0].z;
        Fixed_Coordinates[1].x = Basic_Coordinates[1].x;
        Fixed_Coordinates[1].y = Basic_Coordinates[1].y;
        Fixed_Coordinates[1].z = Basic_Coordinates[1].z;
        Fixed_Coordinates[2].x = Basic_Coordinates[2].x;
        Fixed_Coordinates[2].y = Basic_Coordinates[2].y;
        Fixed_Coordinates[2].z = Basic_Coordinates[2].z;
    }

    void Nether_Beam(int color)
    {
        Unit* pPlayer = NULL;
        Unit* pNpc_Beam = NULL;
        Unit* pNpc_Portal = NULL;

        m_auiCurrent_CandidateGUID[color] = m_creature->GetGUID();
        pNpc_Beam = Unit::GetUnit((*m_creature), m_auiNpc_BeamGUID[color]);
        pNpc_Portal = Unit::GetUnit((*m_creature), m_auiNpc_PortalGUID[color]);

        Map *map = m_creature->GetMap();
        if (map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();
            if (PlayerList.isEmpty())
                return;

            //here we calculate possible player blockers, else beam goes to netherspite
            for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            {
                if (itr->getSource()->isAlive())
                {
                    pPlayer = itr->getSource();
                    m_fDISTANCE = m_creature->GetDistance(pNpc_Portal);
                    m_fANGLE = m_creature->GetAngle(pNpc_Portal);
                    p_fDISTANCE = pPlayer->GetDistance(pNpc_Portal);
                    p_fANGLE = pPlayer->GetAngle(pNpc_Portal);

                    if ((m_fANGLE - fRadian) < p_fANGLE && p_fANGLE < (m_fANGLE + fRadian)
                        && p_fDISTANCE < m_fDISTANCE && !pPlayer->HasAura(Beam_Debuff[color]))
                    {
                        if (color==0 && !pPlayer->HasAura(Beam_Buff[color+1][1]) && !pPlayer->HasAura(Beam_Buff[color+2][1]))
                            m_auiCurrent_CandidateGUID[color] = pPlayer->GetGUID();
                        if (color==1 && !pPlayer->HasAura(Beam_Buff[color-1][1]) && !pPlayer->HasAura(Beam_Buff[color+1][1]))
                        {
                            if (pPlayer->getPowerType() == POWER_MANA && pPlayer->GetMaxPower(POWER_MANA)>0)  //if player has <=0 maximum mana, beam goes to netherspite
                                m_auiCurrent_CandidateGUID[color] = pPlayer->GetGUID();
                            if (pPlayer->getPowerType() != POWER_MANA)
                                m_auiCurrent_CandidateGUID[color] = pPlayer->GetGUID();
                        }
                        if (color==2 && !pPlayer->HasAura(Beam_Buff[color-1][1]) && !pPlayer->HasAura(Beam_Buff[color-2][1]))
                            m_auiCurrent_CandidateGUID[color] = pPlayer->GetGUID();
                    }
                }
            }
        }

        if (m_auiCurrent_CandidateGUID[color] == m_creature->GetGUID())
        {
            if (m_auiCurrent_CandidateGUID[color] != m_auiLast_CandidateGUID[color])
            {
                //we need to kill the beamer, or else the beam visual won't disappear
                pNpc_Beam->DealDamage(pNpc_Beam, pNpc_Beam->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                pNpc_Beam = pNpc_Portal->SummonCreature(NPC_PORTAL_CREATURE, Fixed_Coordinates[color].x, Fixed_Coordinates[color].y, Fixed_Coordinates[color].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                pNpc_Beam->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);    //flag to hide our model
                pNpc_Beam->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (pNpc_Beam && pNpc_Beam->isAlive())
                    m_auiNpc_BeamGUID[color] = pNpc_Beam->GetGUID();
                m_auiLast_CandidateGUID[color] = m_creature->GetGUID();
                pNpc_Beam->CastSpell(m_creature, Beam_Visuals[color], true);
            }
                m_creature->CastSpell(m_creature, Beam_Buff[color][0], true);
        }
        else
        {
            if (pPlayer && pPlayer->isAlive())
            {
                if (m_auiCurrent_CandidateGUID[color] != m_auiLast_CandidateGUID[color])
                {
                    pNpc_Beam->DealDamage(pNpc_Beam, pNpc_Beam->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    pNpc_Beam = pNpc_Portal->SummonCreature(NPC_PORTAL_CREATURE, Fixed_Coordinates[color].x, Fixed_Coordinates[color].y, Fixed_Coordinates[color].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    pNpc_Beam->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);    //flag to hide our model
                    pNpc_Beam->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    if (pNpc_Beam && pNpc_Beam->isAlive())
                        m_auiNpc_BeamGUID[color] = pNpc_Beam->GetGUID();
                    m_auiLast_CandidateGUID[color] = pPlayer->GetGUID();
                    Exhaust_Handler(m_auiCurrent_CandidateGUID[color],color);
                    if (color==2)
                        threat_Increase_Handler(m_auiCurrent_CandidateGUID[color]);
                    pNpc_Beam->CastSpell(pPlayer, Beam_Visuals[color], true);
                }
                if (color==0)
                    pPlayer->CastSpell(pPlayer, Beam_Buff[color][1], true);
                if (color==1)
                {
                    pPlayer->ModifyPower(POWER_MANA, pPlayer->GetMaxPower(POWER_MANA));
                    pPlayer->CastSpell(pPlayer, Beam_Buff[color][1], true);
                }
                if (color==2)
                {
                    if (pPlayer->GetMaxHealth() <= 1000 && pPlayer->isAlive())
                    {
                        /*we have to do this check here, if we continue and apply one more perseverence buff, player will
                        get negative health (he dies but hes not flagged as dead, causing weird behavior). To prevent this
                        instead of applyng one more stack (which should have killed the player), we manually lower his
                        health -1000 by dealing damage and kill him properly*/
                        m_creature->DealDamage(pPlayer, 1000, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true); //it seems that durability loss despite being set to true doesn't work
                        return;
                    }
                    pPlayer->CastSpell(pPlayer, Beam_Buff[color][1], true);
                    pPlayer->SetMaxHealth(pPlayer->GetMaxHealth() + 31000);
                    pPlayer->ModifyHealth(pPlayer->GetMaxHealth());
                }
            }
        }
    }

    void Exhaust_Handler(uint64 iPlayerGUID, int color)
    {
        //storing the possible player's GUIDs that will get the exhaustion debuff
        for (uint8 i=0;i<3;i++)
            m_bExists[i] = false;

        for (uint8 i=0;i<10;i++)
        {
            if(m_auiExhaust_CandidateGUID[color][i] == iPlayerGUID)
            {
                m_bExists[color] = true;
                break;
            }
        }

        if (!m_bExists[color])
        {
            for (uint8 i=0;i<10;i++)
            {
                if (m_auiExhaust_CandidateGUID[color][i] == 0)
                {
                    m_auiExhaust_CandidateGUID[color][i] = iPlayerGUID;
                    break;
                }
            }
        }
    }

    void threat_Increase_Handler(uint64 iPlayerGUID)
    {
        Unit* pPlayer = NULL;

        m_bThreat_Increased = false;
        for (uint8 i=0;i<10;i++)
        {
            if (m_auiRed_Beam_BlockerGUID[i] == iPlayerGUID)
            {
                m_bThreat_Increased = true;
                break;
            }
        }

        if (!m_bThreat_Increased)
        {
           for (uint8 i=0;i<10;i++)
           {
                if (m_auiRed_Beam_BlockerGUID[i] == 0)
                {
                    m_auiRed_Beam_BlockerGUID[i] = iPlayerGUID;
                    pPlayer = Unit::GetUnit((*m_creature), m_auiRed_Beam_BlockerGUID[i]);
                    m_creature->getThreatManager().addThreat(pPlayer, 5000000.0f);
                    break;
                }
            }
        }
    }

    void Apply_Exhaustion(int color)
    {
        for (uint8 i=0;i<10;i++)
        {
            Unit* pPlayer = NULL;
            pPlayer = Unit::GetUnit((*m_creature), m_auiExhaust_CandidateGUID[color][i]);

            if (pPlayer && pPlayer->isAlive())
            {
                if (!pPlayer->HasAura(Beam_Buff[color][1]))
                {
                    if (color == 2)
                    {
                        for (uint8 j=0;j<10;j++)
                        {
                            Unit* pRedBeam_Player = NULL;
                            pRedBeam_Player = Unit::GetUnit((*m_creature), m_auiRed_Beam_BlockerGUID[j]);
                            if (pRedBeam_Player && pPlayer == pRedBeam_Player)
                            {
                                if (pRedBeam_Player->isAlive())
                                {
                                    m_creature->getThreatManager().modifyThreatPercent(pRedBeam_Player, -100);
                                    m_creature->getThreatManager().addThreat(pRedBeam_Player, 10.0f);
                                    pRedBeam_Player = NULL;
                                    m_auiRed_Beam_BlockerGUID[j] = 0;
                                }
                                else
                                    m_auiRed_Beam_BlockerGUID[j] = 0;
                            }
                        }
                    }
                    pPlayer->CastSpell(pPlayer, Beam_Debuff[color], true);
                    m_auiExhaust_CandidateGUID[color][i] = 0;
                }
            }
            else
                m_auiExhaust_CandidateGUID[color][i] = 0;
        }
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, IN_PROGRESS);
        SwitchPhase();
        m_creature->SetInCombatWithZone();
    }

    void RemovePortals()
    {
        Unit* pSummoned = NULL;
        
        for (uint8 i=0;i<3;i++)
        {
            pSummoned = Unit::GetUnit((*m_creature), m_auiNpc_BeamGUID[i]);
            if (pSummoned && pSummoned->isAlive())
                pSummoned->DealDamage(pSummoned, pSummoned->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        for (uint8 i=0;i<3;i++)
        {
            pSummoned = Unit::GetUnit((*m_creature), m_auiNpc_PortalGUID[i]);
            if (pSummoned && pSummoned->isAlive())
                pSummoned->DealDamage(pSummoned, pSummoned->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    bool SwitchPhase()
    {
        switch(m_Active_Phase)
        {
        case NULL_PHASE: //initialize
            m_creature->RemoveAllAuras();
            AttackStart(m_creature->getVictim());
            if (DoCastSpellIfCan(m_creature, SPELL_EMPOWERMENT, CAST_TRIGGERED) != CAST_OK)
                return false;
            m_Active_Phase = BEAM_PHASE;
            m_auiTimers[EVENT_ENRAGE] = 540000;
            m_auiTimers[EVENT_EXHAUST_CHECK] = 2000;
            m_auiTimers[EVENT_PHASE_END_TIMER] = 60000;
            m_auiTimers[EVENT_NETHERSPITE_ROAR] = 0;
            m_auiTimers[EVENT_PORTALS_SPAWN] = 4000;
            m_auiTimers[EVENT_BEAM_INITIAL] = 14000;
            m_auiTimers[EVENT_BEAM_PERIODIC] = 0;
            m_auiTimers[EVENT_NETHERBURN] = 5500;
            m_auiTimers[EVENT_VOID_ZONE] = 15000;
            break;
        case BEAM_PHASE:  //enter banish phase
            DoCastSpellIfCan(m_creature, SPELL_BANISH_VISUAL, CAST_TRIGGERED);
            if (DoCastSpellIfCan(m_creature, SPELL_ROOT, CAST_TRIGGERED) != CAST_OK)
                return false;
            m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);   //adding immunity to melee during banish phase
            m_Active_Phase = BANISH_PHASE;
            DoResetThreat();
            RemovePortals();
            Shuffle_Coordinates();
            AttackStart(m_creature->getVictim());
            DoScriptText(EMOTE_PHASE_BANISH, m_creature);
            m_auiTimers[EVENT_PHASE_END_TIMER] = 30000;
            m_auiTimers[EVENT_NETHERBREATH_INITIAL] = 10000;
            m_auiTimers[EVENT_NETHERBREATH] = 0;
            m_auiLast_CandidateGUID[0] = 0;
            m_auiLast_CandidateGUID[1] = 0;
            m_auiLast_CandidateGUID[2] = 0;
            break;
        case BANISH_PHASE:  //enter beam phase
            m_Active_Phase = BEAM_PHASE;
            m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);  //removing immunity to melee after banish phase
            m_creature->RemoveAurasDueToSpell(SPELL_BANISH_VISUAL, 0);
            m_creature->RemoveAurasDueToSpell(SPELL_ROOT, 0);
            DoResetThreat();
            AttackStart(m_creature->getVictim());
            m_auiTimers[EVENT_PHASE_END_TIMER] = 60000;
            m_auiTimers[EVENT_NETHERSPITE_ROAR] = 500;
            m_auiTimers[EVENT_PORTALS_SPAWN] = 4000;
            m_auiTimers[EVENT_BEAM_INITIAL] = 14000;
            m_auiTimers[EVENT_BEAM_PERIODIC] = 0;
            m_auiTimers[EVENT_NETHERBURN] = 0;
            m_auiTimers[EVENT_VOID_ZONE] = 15000;
            break;
        }
        m_Active_Event = NULL_EVENT;
        return true;
    }

    void SpawnPortals()
    {
        Unit* pSummoned = NULL;

        for (uint8 i=0;i<3;i++)
        {
            pSummoned = m_creature->SummonCreature(NPC_PORTAL_CREATURE, Fixed_Coordinates[i].x, Fixed_Coordinates[i].y, Fixed_Coordinates[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            pSummoned->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);    //flag to hide our model
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (pSummoned && pSummoned->isAlive())
                m_auiNpc_BeamGUID[i] = pSummoned->GetGUID();
        }

        for (uint8 i=0;i<3;i++)
        {
            pSummoned = m_creature->SummonCreature(NPC_PORTAL_CREATURE, Fixed_Coordinates[i].x, Fixed_Coordinates[i].y, Fixed_Coordinates[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            pSummoned->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);    //flag to hide our model
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pSummoned->CastSpell(pSummoned, Portals[i], true);
            if (pSummoned && pSummoned->isAlive())
                m_auiNpc_PortalGUID[i] = pSummoned->GetGUID();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_Active_Phase == NULL_PHASE)                        // Just for Case, that Cast on Aggro failed
            if (!SwitchPhase())
                return;

        m_Active_Event = NULL_EVENT;
        for (uint32 i=1; i<MaxTimeCounter[m_Active_Phase]; i++)
        {
            if (m_auiTimers[i])
                if (m_auiTimers[i] <= diff)
                {
                    if (!m_Active_Event)
                        m_Active_Event = (Events)i;
                }
                else m_auiTimers[i] -= diff;
        }

        switch (m_Active_Event)
        {
            case EVENT_ENRAGE:
                if (DoCastSpellIfCan(m_creature, SPELL_NETHER_INFUSION) !=CAST_OK)
                    return;
                m_auiTimers[EVENT_ENRAGE] = 0;
                break;
            case EVENT_EXHAUST_CHECK:
                Apply_Exhaustion(0);
                Apply_Exhaustion(1);
                Apply_Exhaustion(2);
                m_auiTimers[EVENT_EXHAUST_CHECK] = 2000;
                break;
            case EVENT_PHASE_END_TIMER:
                m_creature->InterruptNonMeleeSpells(false);
                if (!SwitchPhase())
                    return;
                break;
        }

        if (m_Active_Phase == BEAM_PHASE)
        {
            switch (m_Active_Event)
            {
            case EVENT_NETHERSPITE_ROAR:
                DoScriptText(EMOTE_PHASE_BEAM, m_creature);
                if (DoCastSpellIfCan(m_creature, SPELL_NETHERSPITE_ROAR) != CAST_OK)
                    return;
                m_auiTimers[EVENT_NETHERSPITE_ROAR] = 0;
                break;
            case EVENT_PORTALS_SPAWN:
                SpawnPortals();
                m_auiTimers[EVENT_PORTALS_SPAWN] = 0;
                break;
            case EVENT_BEAM_INITIAL:
                Nether_Beam(0);
                Nether_Beam(1);
                Nether_Beam(2);
                m_auiTimers[EVENT_BEAM_INITIAL] = 0;
                m_auiTimers[EVENT_BEAM_PERIODIC] = 1000;
                break;
            case EVENT_BEAM_PERIODIC:
                Nether_Beam(0);
                Nether_Beam(1);
                Nether_Beam(2);
                m_auiTimers[EVENT_BEAM_PERIODIC] = 1000;
                break;
            case EVENT_NETHERBURN:
                if (DoCastSpellIfCan(m_creature, SPELL_NETHERBURN) != CAST_OK)
                    return;
                m_auiTimers[EVENT_NETHERBURN] = 0;
                break;
            case EVENT_VOID_ZONE:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(pTarget, SPELL_VOID_ZONE, true) != CAST_OK)
                        return;
                m_auiTimers[EVENT_VOID_ZONE] = 15000;
                break;
            default:
                break;
            }
        }

        if (m_Active_Phase == BANISH_PHASE)
        {
            switch (m_Active_Event)
            {
            case EVENT_NETHERBREATH_INITIAL:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(pTarget, SPELL_NETHERBREATH) != CAST_OK)
                        return;
                m_auiTimers[EVENT_NETHERBREATH_INITIAL] = 0;
                m_auiTimers[EVENT_NETHERBREATH] = 5000;
                break;
            case EVENT_NETHERBREATH:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (DoCastSpellIfCan(pTarget, SPELL_NETHERBREATH) != CAST_OK)
                        return;
                m_auiTimers[EVENT_NETHERBREATH] = 5000;
                break;
            default:
                break;
            }
        }
        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, DONE);
        RemovePortals();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, FAIL);

        RemovePortals();
        Restore_Coordinates();
    }
};

CreatureAI* GetAI_boss_netherspite(Creature* pCreature)
{
    return new boss_netherspiteAI(pCreature);
}

void AddSC_boss_netherspite()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_netherspite";
    newscript->GetAI = &GetAI_boss_netherspite;
    newscript->RegisterSelf();
}
