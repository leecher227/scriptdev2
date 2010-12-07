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
SDName: Boss_Novos
SD%Complete: 95%
SDComment: Timers
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

enum
{
    SAY_AGGRO                       = -1600005,
    SAY_DEATH                       = -1600006,
    SAY_KILL                        = -1600007,
    SAY_ADDS                        = -1600008,
    SAY_BUBBLE_1                    = -1600009,
    SAY_BUBBLE_2                    = -1600010,

    EMOTE_ASSISTANCE                = -1600011,
    
    SPELL_ARCANE_FIELD              = 47346,
    SPELL_FROSTBOLT_N               = 49037,
    SPELL_FROSTBOLT_H               = 59855,
    SPELL_ARCANE_BLAST_N            = 49198,
    SPELL_ARCANE_BLAST_H            = 59909,
    SPELL_BLIZZARD_N                = 49034,
    SPELL_BLIZZARD_H                = 59854,
    SPELL_WRATH_OF_MISERY_N         = 50089,
    SPELL_WRATH_OF_MISERY_H         = 59856,

    SPELL_RITUAL_CRYSTAL_KEY        = 51404,
    SPELL_EFFECT                    = 52106, //need config support ;P
    SPELL_DEAD_EFFECT               = 47336,

    //shadow riser
    SPELL_SHADOW_BOLT_N             = 51363,
    SPELL_SHADOW_BOLT_H             = 59016,
    
    //crystal handler
    SPELL_SHADOW_LIGHT_N            = 49668,
    SPELL_SHADOW_LIGHT_H            = 59004,

    POS                             = 3,
};

const float PosSummonHandler[POS][3] =
{
    {-337.78f, -720.39f, 28.58f},
    {-379.31f, -818.36f, 59.70f},
    {-412.45f, -726.96f, 28.58f},
};

/*######
## boss_novos
######*/

struct MANGOS_DLL_DECL boss_novosAI : public ScriptedAI
{
    boss_novosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint32 m_uiSummonAddsTimer;
    uint32 m_uiCrystalHandlerTimer;
    uint32 m_uiFrostBoltTimer;
    uint32 m_uiArcaneBlastTimer;
    uint32 m_uiSpecialCastTimer;
    uint32 m_uiSummonMinionTimer;
    
    void Reset()
    {
        m_uiPhase = 1;
        m_uiSummonAddsTimer = 5000;
        m_uiCrystalHandlerTimer = 30000;
        m_uiSummonMinionTimer = urand (15000,20000);
        m_uiFrostBoltTimer = 500;
        m_uiArcaneBlastTimer = urand(5000, 30000);
        m_uiSpecialCastTimer = urand(7000, 20000);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CRYSTALS, 0);
        ActiveTrigger(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        
    }

    void ActivatedCrystal()
    {
        if (!m_pInstance)
            return;

        if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CRYSTAL_01)))
        {
            pGo->ResetDoorOrButton();
            pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        }
        
        if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CRYSTAL_02)))
        {
            pGo->ResetDoorOrButton();
            pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        }
        
        if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CRYSTAL_03)))
        {
            pGo->ResetDoorOrButton();
            pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        }
        
        if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_CRYSTAL_04)))
        {
            pGo->ResetDoorOrButton();
            pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
        }
    }

    void Aggro(Unit* pWho)
    {
        ActivatedCrystal();
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
        ActiveTrigger(true);
        DoCast(m_creature, SPELL_ARCANE_FIELD); 
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void ActiveTrigger(bool Active)
    {
        std::list<Creature*> pCreatures;
        GetCreatureListWithEntryInGrid(pCreatures, m_creature, NPC_CRYSTAL_CHANNEL, 50.0f);

        if (pCreatures.empty())
            return;

        if(Active)
        {
           for(std::list<Creature*>::iterator iter = pCreatures.begin(); iter != pCreatures.end(); ++iter)
              (*iter)->CastSpell(m_creature, SPELL_EFFECT, false);
        }
        else 
        {
           for(std::list<Creature*>::iterator iter = pCreatures.begin(); iter != pCreatures.end(); ++iter)
              (*iter)->InterruptNonMeleeSpells(false);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void PhaseTwo()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->InterruptNonMeleeSpells(false);
        m_uiPhase = 2;
    }

    void JustSummoned(Creature* pSummoned)
    { 
        pSummoned->SetRespawnDelay(DAY);
        if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
           pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_uiPhase)
        {
           case 2:
              if(m_uiArcaneBlastTimer < uiDiff)
              {     
                  m_creature->InterruptNonMeleeSpells(false);
                  m_creature->CastSpell(m_creature,  m_bIsRegularMode ? SPELL_ARCANE_BLAST_N : SPELL_ARCANE_BLAST_H, true);
                  m_uiArcaneBlastTimer = urand(25000, 30000);
              }else m_uiArcaneBlastTimer -= uiDiff;

              if (m_uiSpecialCastTimer < uiDiff)
              {
                 m_creature->InterruptNonMeleeSpells(false);
                 switch(urand(0, 1))
                 {
                     case 0: 
                         if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget,  m_bIsRegularMode ? SPELL_WRATH_OF_MISERY_N : SPELL_WRATH_OF_MISERY_H);
                         break;
                     case 1: 
                         if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget,  m_bIsRegularMode ? SPELL_BLIZZARD_N : SPELL_BLIZZARD_H);
                         break;
                 }
                 m_uiSpecialCastTimer = urand(10000, 15000);
              }else m_uiSpecialCastTimer -= uiDiff;
            
              if (m_uiFrostBoltTimer < uiDiff)
              {
                  if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                     DoCast(pTarget,  m_bIsRegularMode ? SPELL_FROSTBOLT_N : SPELL_FROSTBOLT_H);
                  m_uiFrostBoltTimer = 1000;
              }else m_uiFrostBoltTimer -= uiDiff;
            
              if(!m_bIsRegularMode)
              {
                 if(m_uiSummonMinionTimer < uiDiff)
                 {    
                    m_uiSummonMinionTimer = urand (15000,20000);
                    uint8 SummonLoc = rand()%POS;
                    for(uint8 i = 0; i < 4; i++)
                       m_creature->SummonCreature(NPC_FETID_TROLL_CORPSE, PosSummonHandler[SummonLoc][0],PosSummonHandler[SummonLoc][1],PosSummonHandler[SummonLoc][2],0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                 }else m_uiSummonMinionTimer -= uiDiff;
              }
              break;
           case 1:
              if(m_pInstance && m_pInstance->GetData(TYPE_CRYSTALS) > 3)
                 PhaseTwo();

              if (m_uiSummonAddsTimer < uiDiff)
              {    
                 uint8 SummonLoc = rand()%POS;
                 m_creature->SummonCreature(NPC_HULKING_CORPSE, PosSummonHandler[SummonLoc][0],PosSummonHandler[SummonLoc][1],PosSummonHandler[SummonLoc][2],0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                 m_creature->SummonCreature(NPC_RISEN_SHADOWCASTER, PosSummonHandler[SummonLoc][0],PosSummonHandler[SummonLoc][1],PosSummonHandler[SummonLoc][2],0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                 m_creature->SummonCreature(NPC_FETID_TROLL_CORPSE, PosSummonHandler[SummonLoc][0],PosSummonHandler[SummonLoc][1],PosSummonHandler[SummonLoc][2],0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                 m_creature->SummonCreature(NPC_FETID_TROLL_CORPSE, PosSummonHandler[SummonLoc][0],PosSummonHandler[SummonLoc][1],PosSummonHandler[SummonLoc][2],0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                 m_uiSummonAddsTimer = 17500;
              }else m_uiSummonAddsTimer -= uiDiff;

              if (m_uiCrystalHandlerTimer < uiDiff)
              {    
                 DoScriptText(EMOTE_ASSISTANCE, m_creature);
                 DoScriptText(SAY_ADDS, m_creature);
                 uint8 SummonLoc = rand()%POS;
                 m_creature->SummonCreature(NPC_CRYSTAL_HANDLER, PosSummonHandler[SummonLoc][0],PosSummonHandler[SummonLoc][1],PosSummonHandler[SummonLoc][2],0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                 m_uiCrystalHandlerTimer = 30000;
              }else m_uiCrystalHandlerTimer -= uiDiff;
              break;
           }
    }
};

CreatureAI* GetAI_boss_novos(Creature* pCreature)
{
    return new boss_novosAI(pCreature);
}

struct MANGOS_DLL_DECL npc_risen_shadowcasterAI : public ScriptedAI
{
    npc_risen_shadowcasterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_uiCheckTimer = 1000;
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCheckTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BOLT_N : SPELL_SHADOW_BOLT_H);
            m_uiCheckTimer = 1000;
        }else m_uiCheckTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_risen_shadowcaster(Creature* pCreature)
{
    return new npc_risen_shadowcasterAI(pCreature);
}

struct MANGOS_DLL_DECL npc_crystal_handlerAI : public ScriptedAI
{
    npc_crystal_handlerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 m_uiSpellTimer;

    void Reset()
    {
        m_uiSpellTimer = urand(5000, 10000);
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_LIGHT_N : SPELL_SHADOW_LIGHT_H);
            m_uiSpellTimer = urand(5000, 10000);
        }else m_uiSpellTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_crystal_handler(Creature* pCreature)
{
    return new npc_crystal_handlerAI(pCreature);
}

bool GOHello_novos_crystal(Player* pPlayer, GameObject* pGo)
{       
    if(Creature* Trigger = GetClosestCreatureWithEntry(pGo, NPC_CRYSTAL_CHANNEL, 20.0f))
       Trigger->InterruptNonMeleeSpells(false);

    if(ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData())
       m_pInstance->SetData(TYPE_CRYSTALS, 1);

    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);

    return false;
};


void AddSC_boss_novos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_novos";
    newscript->GetAI = &GetAI_boss_novos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_risen_shadowcaster";
    newscript->GetAI = &GetAI_npc_risen_shadowcaster;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_crystal_handler";
    newscript->GetAI = &GetAI_npc_crystal_handler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_novos_crystal";
    newscript->pGOHello = &GOHello_novos_crystal;
    newscript->RegisterSelf();
}
