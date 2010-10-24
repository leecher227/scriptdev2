/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_xt002
SD%Complete:
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //xt yells
    SAY_AGGRO				= -1603038,
    SAY_DEATH				= -1603030,
    SAY_TANCTRUM			= -1603037,
    SAY_SLAY_01				= -1603036,
    SAY_SLAY_02				= -1603035,
    SAY_BERSERK				= -1603031,
    SAY_ADDS				= -1603032,
    SAY_HEART_OPEN			= -1603034,
    SAY_HEART_CLOSE			= -1603033,
    EMOTE_HEART             = -1603350,
    EMOTE_REPAIR            = -1603351,

    //xt-002
    SPELL_TANCTRUM			= 62776,
    SPELL_LIGHT_BOMB_TRIG   = 65598,
    SPELL_LIGHT_BOMB		= 63018,
    SPELL_LIGHT_BOMB_H		= 65121,
    SPELL_GRAVITY_BOMB		= 63024,
    SPELL_GRAVITY_BOMB_H	= 64234,
    SPELL_ENRAGE			= 47008,
    SPELL_STUN				= 3618,
    SPELL_SCRAP_REPAIR      = 62832,

    // hard mode
    SPELL_HEARTBREAK        = 65737,
    SPELL_HEARTBREAK_H      = 64193,
    SPELL_VOIDZONE          = 64203,
    SPELL_VOIDZONE_H        = 64235,
    SPELL_LIFE_SPARK        = 64210,
    SPELL_STATIC_CHARGED    = 64227,
    SPELL_STATIC_CHARGED_H  = 64236,

    NPC_XT002_HARD          = 332931,
    
    NPC_VOIDZONE            = 34001,
    NPC_LIFESPARK           = 34004,

    //heart of the deconstructor
    SPELL_EXPOSED_HEART		= 63849,

    //XE-321 Boombot
    SPELL_BOOM				= 62834,

    //XM-024 Pummeller
    SPELL_CLEAVE			= 8374,
    SPELL_TRAMPLE			= 5568,
    SPELL_UPPERCUT			= 10966,

    //NPC ids
    NPC_HEART				= 33329,
    NPC_SCRAPBOT			= 33343,
    NPC_BOOMBOT				= 33346,
    NPC_PUMMELER			= 33344, 

    // Achievs
    ACHIEV_HEARTBREAKER         = 3058,
    ACHIEV_HEARTBREAKER_H       = 3059,
    ACHIEV_DECONSTRUCT_FAST     = 2937,
    ACHIEV_DECONSTRUCT_FAST_H   = 2938,
    ACHIEV_NERF_ENGINEERING     = 2931,
    ACHIEV_NERF_ENGINEERING_H   = 2932,
    ACHIEV_NERF_GRAVITY_BOMBS   = 2934,
    ACHIEV_NERF_GRAVITY_BOMBS_H = 2936,
    ACHIEV_NERF_SCRAPBOTS       = 2933,
    ACHIEV_NERF_SCRAPBOTS_H     = 2935,
};

//Positional defines 
struct LocationsXY
{
    float x, y, z, o;
    uint32 id;
};

static LocationsXY SummonLoc[]=
{
    {792.706f, 64.033f, 409.632f},  // lower left
    {879.750f, 64.815f, 409.804f},  // upper left
    {896.488f, -93.018f, 409.731f}, // upper right
    {791.016f, -83.516f, 409.804f}, // lower right
};

// void zone
struct MANGOS_DLL_DECL mob_xt002voidzoneAI : public ScriptedAI
{
    mob_xt002voidzoneAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 Spell_Timer;

    void Reset()
    {
        Spell_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// should be an aura here. Couldn't find it
		// hacky way, needs fixing!
        if (Spell_Timer < diff)
        {
            Map *map = m_creature->GetMap();
            if (map->IsDungeon())
            {
                Map::PlayerList const &PlayerList = map->GetPlayers();

                if (PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (i->getSource()->isAlive() && m_creature->GetDistance2d(i->getSource()) < 5.0f)
                        i->getSource()->DealDamage(i->getSource(), m_bIsRegularMode ? 5000 : 7500, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, true);
                }
            } 
            Spell_Timer = 1000;
        }
        else
            Spell_Timer -= diff;  
    }
};

CreatureAI* GetAI_mob_xt002voidzone(Creature* pCreature)
{
    return new mob_xt002voidzoneAI(pCreature);
}

// lifespark
struct MANGOS_DLL_DECL mob_lifesparkAI : public ScriptedAI
{
    mob_lifesparkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    void Reset()
    {  
        DoCast(m_creature, m_bIsRegularMode ? SPELL_STATIC_CHARGED : SPELL_STATIC_CHARGED_H);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lifespark(Creature* pCreature)
{
    return new mob_lifesparkAI(pCreature);
}

// XM-024 Pummeller
struct MANGOS_DLL_DECL mob_pummelerAI : public ScriptedAI
{
    mob_pummelerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Spell_Timer;

    void Reset()
    {
        Spell_Timer = urand(5000, 7000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Spell_Timer < diff)
        {
            switch(urand(0, 2))
            {
            case 0:
                DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                break;
            case 1:
                DoCast(m_creature->getVictim(), SPELL_TRAMPLE);
                break;
            case 2:
                DoCast(m_creature->getVictim(), SPELL_UPPERCUT);
                break;
            }
            Spell_Timer = urand(5000, 7000);
        }
        else
            Spell_Timer -= diff;        

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_pummeler(Creature* pCreature)
{
    return new mob_pummelerAI(pCreature);
}

// XE-321 Boombot
struct MANGOS_DLL_DECL mob_boombotAI : public ScriptedAI
{
    mob_boombotAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_uiCheckTimer = 1000;
        m_creature->SetSpeedRate(MOVE_RUN, 0.5, true);
    }

    void AttackStart(Unit* pWho)
    {
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;
            int32 uiBP0 = 3750;
            int32 uiBP1 = 1;
            m_creature->CastCustomSpell(m_creature, SPELL_BOOM, &uiBP0, &uiBP1, NULL, true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiCheckTimer < diff)
        {
            if (m_pInstance)
            {
                if (Creature* pXT002 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_XT002)))
                {
                    if (m_creature->GetDistance2d(pXT002) <= ATTACK_DISTANCE)
                    {
                        int32 uiBP0 = 3750;
                        int32 uiBP1 = 1;
                        m_creature->CastCustomSpell(m_creature, SPELL_BOOM, &uiBP0, &uiBP1, NULL, true);
                    }
                }
            }
            m_uiCheckTimer = 1000;
        }
        else
            m_uiCheckTimer -= diff;
    }
};

CreatureAI* GetAI_mob_boombot(Creature* pCreature)
{
    return new mob_boombotAI(pCreature);
}

// XS-013 Scrapbot
struct MANGOS_DLL_DECL mob_scrapbotAI : public ScriptedAI
{
    mob_scrapbotAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_uiCheckTimer = 1000;
        m_creature->SetSpeedRate(MOVE_RUN, 0.5, true);
    }

    void AttackStart(Unit* pWho)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiCheckTimer < diff)
        {
            if (m_pInstance)
            {
                if (Creature* pXT002 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_XT002)))
                {
                    if (m_creature->GetDistance2d(pXT002) <= ATTACK_DISTANCE)
                    {
                        pXT002->CastSpell(m_creature, SPELL_SCRAP_REPAIR, true);
                        m_creature->ForcedDespawn(2000);
                    }
                }
            }
            m_uiCheckTimer = 1000;
        }
        else
            m_uiCheckTimer -= diff;
    }
};

CreatureAI* GetAI_mob_scrapbot(Creature* pCreature)
{
    return new mob_scrapbotAI(pCreature);
}

// Heart of the Deconstructor
struct MANGOS_DLL_DECL mob_xtheartAI : public ScriptedAI
{
    mob_xtheartAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiDeathTimer;
    uint32 m_uiTotalDamage;

    void Reset()
    {
        DoCast(m_creature, SPELL_EXPOSED_HEART);
        m_uiTotalDamage = 0;
        m_uiDeathTimer = 30000;
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        m_uiTotalDamage += uiDamage;
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_XT002_HARD, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        // despawns the creature if not killed in 30 secs
        if (m_uiDeathTimer < diff)
        {
            // pass damage to boss
            if (m_pInstance)
            {
                if (Creature* pXT002 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_XT002)))
                {
                    if (pXT002->isAlive())
                        pXT002->DealDamage(pXT002, m_uiTotalDamage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }
            m_creature->ForcedDespawn();
        }
        else
            m_uiDeathTimer -= diff;
    }
};

CreatureAI* GetAI_mob_xtheart(Creature* pCreature)
{
    return new mob_xtheartAI(pCreature);
}

//XT-002 Deconstructor
struct MANGOS_DLL_DECL boss_xt002AI : public ScriptedAI
{
    boss_xt002AI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    // spell timers
    uint32 m_uiHeart_Timer;
    uint32 m_uiLight_Bomb_Timer;
    uint32 m_uiGravity_Bomb_Timer;
    uint32 m_uiTanctrum_Timer;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiRange_Check_Timer;
    uint32 m_uiVoidZoneTimer;
    uint32 m_uiLifeSparkTimer;

    // summon timers
    uint32 m_uiScrapbotTimer;
    uint32 m_uiBoombotTimer;
    uint32 m_uiPummellerTimer;
    uint32 m_uiScrapbotGroupCount;
    uint32 m_uiBoombotCount;
    uint32 m_uiPummellerCount;
    uint32 m_uiMaxPummeler;
    uint32 m_uiMaxScrapbotGroup;
    uint32 m_uiMaxBoombot;

    bool m_bFullHealth;

    // health timers
    uint32 m_uiHealthPercent;
    bool m_bIsEnrage;
    bool m_bPhase2;

    uint64 pLightBombTarGUID;
    uint64 pGravityBombTarGUID;

    bool m_bIsHardMode;

    uint32 uiEncounterTimer;
    bool m_bAchiev_NerfEngineering;

    void Reset()
    {
        // spell timers
        m_uiLight_Bomb_Timer    = urand(10000, 11000);
        m_uiGravity_Bomb_Timer  = urand(5000, 6000);
        m_uiTanctrum_Timer      = urand(58000, 60000);
        m_uiEnrage_Timer        = 600000;
        m_uiRange_Check_Timer   = 1000;
        m_uiVoidZoneTimer       = 60000;
        m_uiLifeSparkTimer      = 60000;
        // summon timers
        m_uiScrapbotTimer       = 5000;
        m_uiBoombotTimer        = 5000;
        m_uiPummellerTimer      = 5000;
        m_uiScrapbotGroupCount  = 0;
        m_uiBoombotCount        = 0;
        m_uiPummellerCount      = 0;
        m_uiMaxPummeler         = m_bIsRegularMode ? 1 : 2;
        m_uiMaxScrapbotGroup    = m_bIsRegularMode ? 6 : 10;
        m_uiMaxBoombot          = m_bIsRegularMode ? 4 : 7;
        // health timers
        m_uiHealthPercent       = 75;
        m_bFullHealth = false;

        m_bIsEnrage = false;
        m_bPhase2 = false;
        m_bIsHardMode = false;
        pLightBombTarGUID = 0;
        pGravityBombTarGUID = 0;
        uiEncounterTimer = 0;
        m_bAchiev_NerfEngineering = true;
        DespawnCreatures(NPC_HEART);
        DespawnCreatures(NPC_SCRAPBOT);
        DespawnCreatures(NPC_BOOMBOT);
        DespawnCreatures(NPC_PUMMELER);
        DespawnCreatures(NPC_VOIDZONE);
        DespawnCreatures(NPC_LIFESPARK);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_XT002, FAIL);
    }

    void JustDied(Unit* pKiller)
    {
        DespawnCreatures(NPC_HEART);
        DespawnCreatures(NPC_SCRAPBOT);
        DespawnCreatures(NPC_BOOMBOT);
        DespawnCreatures(NPC_PUMMELER);
        DespawnCreatures(NPC_VOIDZONE);
        DespawnCreatures(NPC_LIFESPARK);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_XT002, DONE);
            if (m_bIsHardMode)
                m_pInstance->SetData(TYPE_XT002_HARD, DONE);
            
            // hacky way to complete achievements; use only if you have this function
            // Deconstruct Fast
            if (uiEncounterTimer < 205000)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_DECONSTRUCT_FAST : ACHIEV_DECONSTRUCT_FAST_H);
            // Heartbreaker
            if (m_bIsHardMode)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_HEARTBREAKER : ACHIEV_HEARTBREAKER_H);
            // Nerf Engineer
            if (m_bAchiev_NerfEngineering)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_NERF_ENGINEERING : ACHIEV_NERF_ENGINEERING_H);
        }

        DoScriptText(SAY_DEATH, m_creature);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_XT002, IN_PROGRESS);
            if (m_pInstance->GetData(TYPE_XT002_TP) != DONE)
                m_pInstance->SetData(TYPE_XT002_TP, DONE);
        }

        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetInCombatWithZone();
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0, 1))
        {
        case 0: DoScriptText(SAY_SLAY_01, m_creature); break;
        case 1: DoScriptText(SAY_SLAY_02, m_creature); break;
        }
    }

    void DespawnCreatures(uint32 uiEntry)
    {
        std::list<Creature*> lList;
        GetCreatureListWithEntryInGrid(lList, m_creature, uiEntry, 200.0f);
        if (!lList.empty())
            for (std::list<Creature*>::iterator iter = lList.begin(); iter != lList.end(); ++iter)
                (*iter)->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Achiev timer
        uiEncounterTimer += uiDiff;

        if (m_uiEnrage_Timer < uiDiff && !m_bIsEnrage)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            if (m_creature->HasAura(SPELL_ENRAGE))
            {
                m_bIsEnrage = true;
                DoScriptText(SAY_BERSERK, m_creature);
            }
            else
                m_uiEnrage_Timer = 5000;
        }
        else
            m_uiEnrage_Timer -= uiDiff;

        if (!m_bPhase2)
        {
            if (m_uiLight_Bomb_Timer < uiDiff)
            {
                Unit* pTarget = NULL;
                for (uint8 i=0; i<25; ++i)
                {
                    pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                    if (pTarget && !pTarget->HasAura(SPELL_GRAVITY_BOMB) && !pTarget->HasAura(SPELL_GRAVITY_BOMB_H))
                        break;
                }
                if (pTarget)
                {
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_LIGHT_BOMB : SPELL_LIGHT_BOMB_H);
                    pLightBombTarGUID = pTarget->GetGUID();
        			// spawn a life spark from the target
                    if (m_bIsHardMode)
                        m_uiLifeSparkTimer = 9000;
                }
                m_uiLight_Bomb_Timer = urand(10000, 11000);
            }
            else
                m_uiLight_Bomb_Timer -= uiDiff;   

            if (m_uiGravity_Bomb_Timer < uiDiff)
            {
                Unit* pTarget = NULL;
                for (uint8 i=0; i<25; ++i)
                {
                    pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
                    if (pTarget && !pTarget->HasAura(SPELL_LIGHT_BOMB) && !pTarget->HasAura(SPELL_LIGHT_BOMB_H))
                        break;
                }
                if (pTarget)
                {
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_GRAVITY_BOMB : SPELL_GRAVITY_BOMB_H);
                    pLightBombTarGUID = pTarget->GetGUID();
                    pGravityBombTarGUID = pTarget->GetGUID();
        			// spawn a void zone from the target
                    if (m_bIsHardMode)
                        m_uiVoidZoneTimer = 9000;
                }
                m_uiGravity_Bomb_Timer = urand(10000, 11000);
            }
            else
                m_uiGravity_Bomb_Timer -= uiDiff;  

            if (m_uiTanctrum_Timer < uiDiff)
            {
                DoCast(m_creature, SPELL_TANCTRUM);
                m_creature->MonsterTextEmote("XT-002 Deconstructor begins to cause the earth to quake.", 0, true);
                DoScriptText(SAY_TANCTRUM, m_creature);
                m_uiTanctrum_Timer = urand(58000, 60000);
            }
            else
                m_uiTanctrum_Timer -= uiDiff;

            if (!m_bIsHardMode && m_creature->GetHealthPercent() < m_uiHealthPercent)
            {
                m_uiHeart_Timer = 30000;
                m_creature->CastStop();
                m_uiHealthPercent = m_uiHealthPercent - 25;
                m_bPhase2 = true;
                DoScriptText(SAY_HEART_OPEN, m_creature);
                DoCast(m_creature, SPELL_STUN);
                DoScriptText(EMOTE_HEART, m_creature);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                // timers
                m_uiScrapbotTimer       = 1000;
                m_uiBoombotTimer        = 3000;
                m_uiPummellerTimer      = 5000;
                m_uiScrapbotGroupCount  = 0;
                m_uiBoombotCount        = 0;   
                m_uiPummellerCount      = 0;

                if (Creature* pHeart = DoSpawnCreature(NPC_HEART, 0.0f, 0.0f, 2.0f, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    pHeart->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            if (m_bIsHardMode)
            {
                if (!m_bFullHealth)
                {
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                    m_bFullHealth = true;
                }
                
                if (m_uiLifeSparkTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->GetMap()->GetUnit(pLightBombTarGUID))
                        m_creature->SummonCreature(NPC_LIFESPARK, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                    m_uiLifeSparkTimer = 60000;
                }
                else
                    m_uiLifeSparkTimer -= uiDiff;

                if (m_uiVoidZoneTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->GetMap()->GetUnit(pGravityBombTarGUID))
                        m_creature->SummonCreature(NPC_VOIDZONE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 180000);         
                    m_uiVoidZoneTimer = 60000;
                }
                else
                    m_uiVoidZoneTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }
        else
        {
            // pummeller
            if (m_uiPummellerCount < m_uiMaxPummeler)
            {
                if (m_uiPummellerTimer < uiDiff)
                {
                    if (m_uiPummellerCount == 0)
                        DoScriptText(SAY_ADDS, m_creature);
                    uint8 i = urand(0, 4);
                    if (Creature* pTemp = m_creature->SummonCreature(NPC_PUMMELER, SummonLoc[i].x + urand(0, 10), SummonLoc[i].y + urand(0, 10), SummonLoc[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                        pTemp->SetInCombatWithZone();
                    ++m_uiPummellerCount;
                    m_uiPummellerTimer = 4000;
                }
                else
                    m_uiPummellerTimer -= uiDiff;
            }

            // boombot
            if (m_uiBoombotCount < m_uiMaxBoombot)
            {
                if (m_uiBoombotTimer < uiDiff)
                {
                    uint8 i = urand(0, 4);
                    if (Creature* pTemp = m_creature->SummonCreature(NPC_BOOMBOT, SummonLoc[i].x + urand(0, 10), SummonLoc[i].y + urand(0, 10), SummonLoc[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                        pTemp->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
                    ++m_uiBoombotCount;
                    m_uiBoombotTimer = 4000;
                }
                else
                    m_uiBoombotTimer -= uiDiff;
            }

            // scrapbot
            if (m_uiScrapbotGroupCount < m_uiMaxScrapbotGroup)
            {
                if (m_uiScrapbotTimer < uiDiff)
                {
                    uint8 i = urand(0, 4);
                    for (int j = 0; j < 5; ++j)
                    {
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_SCRAPBOT, SummonLoc[i].x + urand(0, 10), SummonLoc[i].y + urand(0, 10), SummonLoc[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            pTemp->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
                    }
                    ++m_uiScrapbotGroupCount;
                    m_uiScrapbotTimer = 2500;
                }
                else
                    m_uiScrapbotTimer -= uiDiff;
            }
            if (m_uiHeart_Timer < uiDiff)
            {
                DoScriptText(SAY_HEART_CLOSE, m_creature);
                m_creature->RemoveAurasDueToSpell(SPELL_STUN);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_bPhase2 = false;
                m_uiLight_Bomb_Timer    = urand(10000, 11000);
                m_uiGravity_Bomb_Timer  = urand(5000, 6000);
                m_uiTanctrum_Timer      = urand(58000, 60000);
            }
            else
                m_uiHeart_Timer -= uiDiff;
        }

        // Hard mode
        if (!m_bIsHardMode && m_pInstance->GetData(TYPE_XT002_HARD) == IN_PROGRESS)
        {
            DoScriptText(SAY_HEART_CLOSE, m_creature);
            m_creature->RemoveAurasDueToSpell(SPELL_STUN);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_HEARTBREAK : SPELL_HEARTBREAK_H);
            m_bPhase2 = false;
            m_bFullHealth = false;
            m_bIsHardMode = true;
            m_creature->UpdateEntry(NPC_XT002_HARD);
            m_creature->SetLootRecipient(m_creature->getVictim());
        }
    }
};

CreatureAI* GetAI_boss_xt002(Creature* pCreature)
{
    return new boss_xt002AI(pCreature);
}

void AddSC_boss_xt002()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_xt002";
    NewScript->GetAI = GetAI_boss_xt002;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_pummeler";
    NewScript->GetAI = &GetAI_mob_pummeler;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_boombot";
    NewScript->GetAI = &GetAI_mob_boombot;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_scrapbot";
    NewScript->GetAI = &GetAI_mob_scrapbot;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_xtheart";
    NewScript->GetAI = &GetAI_mob_xtheart;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_xt002voidzone";
    NewScript->GetAI = &GetAI_mob_xt002voidzone;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_lifespark";
    NewScript->GetAI = &GetAI_mob_lifespark;
    NewScript->RegisterSelf();
}