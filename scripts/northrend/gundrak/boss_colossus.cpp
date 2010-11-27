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
SDName: Boss_Colossus
SD%Complete: 80%
SDComment:
SDAuthor: MaxXx2021 Aka Mioka.
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"

enum
{
    EMOTE_SURGE                 = -1604008,
    EMOTE_SEEP                  = -1604009,
    EMOTE_GLOW                  = -1604010,

    SPELL_EMERGE                 = 54850,
    SPELL_SUMMON_ELEMENTAL       = 54851,
    SPELL_MIGHTY_BLOW            = 54719,
    SPELL_MERGE                  = 54878,
    SPELL_SURGE                  = 54801,
    SPELL_SURGE_EFFECT           = 54827,
    SPELL_MOJO_PUDDLE            = 55627,
    SPELL_MOJO_PUDDLE_H          = 58994,
    SPELL_MOJO_WAVE              = 55626,
    SPELL_MOJO_WAVE_H            = 58993,
    SPELL_FREEZE_ANIM            = 16245,

    NPC_LIVING_MOJO              = 29830,
};

/*######
## boss_colossus
######*/

struct MANGOS_DLL_DECL boss_colossusAI : public ScriptedAI
{
    boss_colossusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiMightyBlowTimer;
    uint32 uiStepTimer;

    uint8 uiStep;
    uint8 uiMojoCount;

    bool m_bIsElementalDead;
    bool m_bIsFirstActive;
    bool m_bIsSecondActive;
    bool m_bIsElementalEvent;

    void Reset()
    {
        if (m_pInstance)
        {
            uiMojoCount = 0;
            if (m_pInstance->GetData(TYPE_COLOSSUS) != IN_PROGRESS)
            {
                m_creature->addUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                DoCast(m_creature, SPELL_FREEZE_ANIM, true);

                std::list<Creature*> m_pMojo;
                GetCreatureListWithEntryInGrid(m_pMojo, m_creature, NPC_LIVING_MOJO, 60.0f);

                if (!m_pMojo.empty())
                {
                   for (std::list<Creature*>::iterator itr = m_pMojo.begin(); itr != m_pMojo.end(); ++itr)
                   {
                     if ((*itr)->isDead())
                        (*itr)->Respawn();
                   }
                }
            }
            else
            {
                m_creature->clearUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                if (m_creature->HasAura(SPELL_FREEZE_ANIM))
                    m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
            }
            m_pInstance->SetData(TYPE_COLOSSUS, NOT_STARTED);
        }

        m_bIsFirstActive = false;
        m_bIsSecondActive = false;
        m_bIsElementalDead = false;
        m_bIsElementalEvent = false;
        uiMightyBlowTimer = 10000;
        uiStepTimer = 2000;
        uiStep = 0;

        if (Creature* pSummon = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_ELEMENTAL)))
            pSummon->ForcedDespawn();
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
           return;

        ScriptedAI::AttackStart(pWho);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, DONE);
    }

    void JustSummoned(Creature* pSummon)
    {
        if (pSummon->GetEntry() == NPC_ELEMENTAL)
        {
            pSummon->AI()->AttackStart(m_creature->getVictim());
            
            if (m_creature->GetHealthPercent() < 10.0f)
                pSummon->SetHealth(pSummon->GetHealth() / 2);
            
            m_pInstance->SetData64(NPC_ELEMENTAL, pSummon->GetGUID());
        }
    }

    void GolemPhase()
    {
        m_creature->clearUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (m_creature->HasAura(SPELL_FREEZE_ANIM))
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
        m_creature->SetInCombatWithZone();
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            m_creature->GetMotionMaster()->MoveChase(pTarget);
    }

    void SummonElemental()
    {
        m_creature->CastSpell(m_creature, SPELL_SUMMON_ELEMENTAL, true);
        m_creature->addUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->InterruptNonMeleeSpells(true);
        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->StopMoving();
        m_creature->AttackStop();
        DoCast(m_creature, SPELL_FREEZE_ANIM, true);
        m_bIsElementalEvent = false;
    }

    void DamageTaken(Unit *done_by, uint32 &damage) 
    {
        if (damage >= m_creature->GetHealth())
        {
            if (!m_bIsElementalDead)
            {
                damage = 0;
                m_creature->SetHealth(1);
            }
           
            if (!m_bIsSecondActive)
            {
                m_bIsSecondActive = true;
                uiStepTimer = 2000;
                m_bIsElementalEvent = true;
                DoCast(m_creature, SPELL_EMERGE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiMojoCount == 5)
        {
            GolemPhase();
            uiMojoCount = 6;
        };

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiMightyBlowTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_MIGHTY_BLOW);
            uiMightyBlowTimer = 10000;
        }
        else
            uiMightyBlowTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < 50.0f && !m_bIsFirstActive)
        {
           uiStepTimer = 2000;
           m_bIsFirstActive = true;
           m_bIsElementalEvent = true;
           DoCast(m_creature, SPELL_EMERGE);
        }

        if (m_bIsElementalEvent)
        {
            if (uiStepTimer <= uiDiff)
            {
                SummonElemental();
                uiStepTimer = 2000;
            }
            else
                uiStepTimer -= uiDiff;
        }

        if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_elementalAI : public ScriptedAI
{
    boss_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiSurgeTimer;
    uint32 uiMojoPuddleTimer;

    bool bGoToColossus;

    void Reset()
    {
        uiSurgeTimer = 7000;
        uiMojoPuddleTimer = 5000;
        bGoToColossus = false;
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_MERGE)
          if(target->GetTypeId() != TYPEID_PLAYER)
            if(Creature *pColossus = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_COLOSSUS)))
            {
              ((boss_colossusAI*)pColossus->AI())->GolemPhase();
              m_creature->ForcedDespawn();
            }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!bGoToColossus && m_creature->GetHealthPercent() < 50.0f)
        {
            if (Creature *pColossus = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_COLOSSUS)))
            {
                if(pColossus->GetHealthPercent() > 10.0f)
                {
                   DoCast(pColossus,SPELL_MERGE);
                   bGoToColossus = true;
                }
            }
        }

        if (uiSurgeTimer <= diff)
        {
            DoScriptText(EMOTE_SURGE, m_creature);
            if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
               DoCast(pTarget, SPELL_SURGE);
            uiSurgeTimer = 15000;
        } else uiSurgeTimer -= diff;

        if (uiMojoPuddleTimer <= diff)
        {
            if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
               DoCast(pTarget, m_bIsRegularMode ? SPELL_MOJO_PUDDLE : SPELL_MOJO_PUDDLE_H);
            uiMojoPuddleTimer = urand(7000, 18000);
        } else uiMojoPuddleTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if(Creature *pColossus = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_COLOSSUS)))
        {
           ((boss_colossusAI*)pColossus->AI())->m_bIsElementalDead = true;
           pColossus->DealDamage(pColossus, pColossus->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }
};

struct MANGOS_DLL_DECL npc_living_mojoAI : public ScriptedAI
{
    npc_living_mojoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiMojoWaveTimer;
    uint32 uiMojoPuddleTimer;
    uint32 uiMoveTimer;

    bool m_bIsColossusEvent;

    void Reset()
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        uiMojoWaveTimer = 2000;
        uiMojoPuddleTimer = 7000;
        uiMoveTimer = 500;
        m_bIsColossusEvent = false;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance->GetData(TYPE_COLOSSUS) != NOT_STARTED)
           return;

         if(Creature* pColossus = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_COLOSSUS)))
           if(m_creature->GetDistance2d(pColossus) < 15.0f)
           {
             std::list<Creature*> m_pMojo;
             GetCreatureListWithEntryInGrid(m_pMojo, m_creature, NPC_LIVING_MOJO, 15.0f);

             if(!m_pMojo.empty())
               for(std::list<Creature*>::iterator itr = m_pMojo.begin(); itr != m_pMojo.end(); ++itr)
                  (*itr)->AI()->AttackStart(pWho);

             m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
             m_bIsColossusEvent = true;
             uiMoveTimer = 200;
             m_creature->RemoveAllAuras();
             m_creature->InterruptNonMeleeSpells(true);
             m_creature->GetMotionMaster()->Clear(false);
             m_creature->GetMotionMaster()->MoveIdle();
             m_creature->StopMoving();
             m_creature->AttackStop();
             m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
           }
    }
    
    void AttackStart(Unit* pWho)
    {
         if(m_bIsColossusEvent)
            return;

         ScriptedAI::AttackStart(pWho);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_pInstance) return;

        if(m_bIsColossusEvent && m_creature->GetVisibility() != VISIBILITY_OFF)
        {
          if(Creature* pColossus = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_COLOSSUS)))
            if(m_creature->GetDistance2d(pColossus) < 2.0f)
            {
               m_creature->DeleteThreatList();
               m_creature->addUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
               m_creature->SetVisibility(VISIBILITY_OFF);
               m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
               ((boss_colossusAI*)pColossus->AI())->uiMojoCount++;
            }

           if(uiMoveTimer <= diff)
           {
               if(Creature *pColossus = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_COLOSSUS)))
                  m_creature->GetMotionMaster()->MovePoint(0, pColossus->GetPositionX(), pColossus->GetPositionY(), pColossus->GetPositionZ());
               uiMoveTimer = 12000;
           } else uiMoveTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiMojoWaveTimer <= diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MOJO_WAVE : SPELL_MOJO_WAVE_H);
            uiMojoWaveTimer = 15000;
        } else uiMojoWaveTimer -= diff;

        if (uiMojoPuddleTimer <= diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MOJO_PUDDLE : SPELL_MOJO_PUDDLE_H);
            uiMojoPuddleTimer = 18000;
        } else uiMojoPuddleTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_colossus(Creature* pCreature)
{
    return new boss_colossusAI(pCreature);
}

CreatureAI* GetAI_boss_elemental(Creature* pCreature)
{
    return new boss_elementalAI (pCreature);
}

CreatureAI* GetAI_npc_living_mojo(Creature* pCreature)
{
    return new npc_living_mojoAI (pCreature);
}

void AddSC_boss_colossus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_colossus";
    newscript->GetAI = &GetAI_boss_colossus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elemental";
    newscript->GetAI = &GetAI_boss_elemental;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_living_mojo";
    newscript->GetAI = &GetAI_npc_living_mojo;
    newscript->RegisterSelf();
}
