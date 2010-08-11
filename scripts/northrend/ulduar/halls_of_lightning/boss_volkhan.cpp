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
SDName: Boss Volkhan
SD%Complete: 60%
SDComment: Not considered complete. Some events may fail and need further development
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"

enum
{
    SAY_AGGRO                               = -1602032,
    SAY_SLAY_1                              = -1602033,
    SAY_SLAY_2                              = -1602034,
    SAY_SLAY_3                              = -1602035,
    SAY_DEATH                               = -1602036,
    SAY_STOMP_1                             = -1602037,
    SAY_STOMP_2                             = -1602038,
    SAY_FORGE_1                             = -1602039,
    SAY_FORGE_2                             = -1602040,
    EMOTE_TO_ANVIL                          = -1602041,
    EMOTE_SHATTER                           = -1602042,

    SPELL_HEAT_N                            = 52387,
    SPELL_HEAT_H                            = 59528,
    SPELL_SHATTERING_STOMP_N                = 52237,
    SPELL_SHATTERING_STOMP_H                = 59529,
    SPELL_TEMPER                            = 52238,
    SPELL_TEMPER2                           = 52654, // not sure
    SPELL_TEMPER_VISUAL                     = 52661, // not works

    NPC_VOLKHANS_ANVIL                      = 28823,

    //Molten Golem
    NPC_MOLTEN_GOLEM                        = 28695,
    SPELL_BLAST_WAVE                        = 23113,
    SPELL_IMMOLATION_STRIKE_N               = 52433,
    SPELL_IMMOLATION_STRIKE_H               = 59530,
    SPELL_SHATTER_N                         = 52429,
    SPELL_SHATTER_H                         = 59527
};

/*######
## Boss Volkhan
######*/

struct MANGOS_DLL_DECL boss_volkhanAI : public ScriptedAI
{
    boss_volkhanAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    std::list<uint64> m_lGolemGUIDList;

    bool m_bIsRegularMode;
    bool m_bIsStriking;
    bool m_bStrikeItself;
    bool m_bPause_After_Temper;

    uint32 m_uiPause_Timer;
    uint32 m_uiTemper_Timer;
    uint32 m_uiPause_After_Temper;
    uint32 m_uiShatteringStomp_Timer;
    uint32 m_uiShatter_Timer;
    bool m_bShatterGolems;

    void Reset()
    {
        m_bIsStriking = false;
        m_bStrikeItself = false;
        m_bPause_After_Temper = false;

        m_uiTemper_Timer = 10000;
        m_uiPause_After_Temper = 2500;
        m_uiShatteringStomp_Timer = 75000;
        m_uiShatter_Timer = 3200;
        m_bShatterGolems = false;

        DespawnGolem();

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnGolem();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void DespawnGolem()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for (std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                pTemp->ForcedDespawn();

        m_lGolemGUIDList.clear();
    }

    void ShatterGolem()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)) // only Shatters frozen golems
                {
                    pTemp->CastSpell(pTemp, m_bIsRegularMode ? SPELL_SHATTER_N : SPELL_SHATTER_H, true);
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == 0)
            m_bStrikeItself = true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bStrikeItself)
        {
            m_creature->SetSpeedRate(MOVE_RUN, 1.0f);
            DoCast(m_creature, SPELL_TEMPER);
            if (Creature* pTemp = m_creature->SummonCreature(NPC_MOLTEN_GOLEM, 1325.82f, -89.94f, 56.72f, 5.19f,  TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
            {
                m_lGolemGUIDList.push_back(pTemp->GetGUID());
                pTemp->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0));
            }
            if (Creature* pTemp = m_creature->SummonCreature(NPC_MOLTEN_GOLEM, 1322.14f, -92.58f, 56.72f, 5.69f,  TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
            {
                m_lGolemGUIDList.push_back(pTemp->GetGUID());
                pTemp->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0));
            }
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_bPause_After_Temper = true;
            m_uiPause_After_Temper = 2500;
            m_bStrikeItself = false;
        }

        if (m_bPause_After_Temper)
            if (m_uiPause_After_Temper < uiDiff)
            {
                m_bPause_After_Temper = false;
                m_bIsStriking = false;
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoResetThreat();
                if (pTarget)
                    m_creature->GetMotionMaster()->MoveChase(pTarget);
            }else m_uiPause_After_Temper -= uiDiff;

        if (m_bIsStriking)
            return;

        if(m_uiShatteringStomp_Timer < uiDiff)
        {
            for(std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
            {
                if (Creature* pTemp = (Creature*)Unit::GetUnit(*m_creature, *itr))
                {
                    if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    {
                        switch(rand()%2)
                        {
                            case 0: DoScriptText(SAY_STOMP_1, m_creature);break;
                            case 1: DoScriptText(SAY_STOMP_2, m_creature);break;
                        }
                        DoScriptText(EMOTE_SHATTER, m_creature);
                        DoCast(m_creature, m_bIsRegularMode ? SPELL_SHATTERING_STOMP_N : SPELL_SHATTERING_STOMP_H);
                        m_uiShatteringStomp_Timer = 75000;
                        m_uiShatter_Timer = 3200;
                        m_bShatterGolems = true;
                        break;
                    }
                }
            }
        }
        else
            m_uiShatteringStomp_Timer -= uiDiff;

        // Shatter Golems 3.2 seconds after Shattering Stomp
        if (m_bShatterGolems)
            if(m_uiShatter_Timer < uiDiff)
            {
                ShatterGolem();
                m_uiShatter_Timer = 3200;
                m_bShatterGolems=false;
            }else m_uiShatter_Timer -= uiDiff;

        if (m_uiTemper_Timer < uiDiff)
        {
            DoScriptText(urand(0, 1) ? SAY_FORGE_1 : SAY_FORGE_2, m_creature);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // If don't do this, Volkhan won't run because players keep hitting him
            m_creature->InterruptNonMeleeSpells(true);
            m_creature->GetMotionMaster()->Clear();
            // workaround for SPELL_TEMPER
            DoScriptText(EMOTE_TO_ANVIL, m_creature);
            m_creature->SetSpeedRate(MOVE_RUN, 2.5f);
            m_creature->GetMotionMaster()->MovePoint(0, 1325.911f, -93.036f, 56.717f);
            m_bIsStriking = true;
            m_uiTemper_Timer = 35000;
        } else m_uiTemper_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_volkhan_anvil
######*/
struct MANGOS_DLL_DECL npc_volkhan_anvilAI : public ScriptedAI
{
    npc_volkhan_anvilAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    void Reset() {}
    void Aggro(Unit* who) { return; } // don't aggro
    void AttackStart(Unit* who) { return; }
    void UpdateAI(const uint32 diff) {}
};

/*######
## mob_molten_golem
######*/
struct MANGOS_DLL_DECL mob_molten_golemAI : public ScriptedAI
{
    mob_molten_golemAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsFrozen;

    uint32 m_uiBlast_Timer;
    uint32 m_uiImmolation_Timer;

    void Reset()
    {
        m_bIsFrozen = false;

        m_uiBlast_Timer = 10000;
        m_uiImmolation_Timer = 5000;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || m_bIsFrozen)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() && !m_bIsFrozen)
        {
            m_bIsFrozen = true;
            m_creature->RemoveAllAuras();
            m_creature->InterruptNonMeleeSpells(true);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->StopMoving();
            m_creature->AttackStop();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetStandState(UNIT_STAND_STATE_SIT);
            m_creature->SetHealth(1);
            m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X)*0.75f);
            m_creature->SetDisplayId(5747);
        }
        if (m_bIsFrozen)
        {
            uiDamage = 0;
            return;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsFrozen)
        {
            if (m_uiBlast_Timer < uiDiff)
            {
                DoCast(m_creature, SPELL_BLAST_WAVE);
                m_uiBlast_Timer = 10000;
            }
            else
                m_uiBlast_Timer -= uiDiff;

            if (m_uiImmolation_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_IMMOLATION_STRIKE_N : SPELL_IMMOLATION_STRIKE_H);
                m_uiImmolation_Timer = 5000;
            }
            else
                m_uiImmolation_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_mob_molten_golem(Creature* pCreature)
{
    return new mob_molten_golemAI(pCreature);
}

CreatureAI* GetAI_npc_volkhan_anvil(Creature *_Creature)
{
    return new npc_volkhan_anvilAI (_Creature);
}

CreatureAI* GetAI_boss_volkhan(Creature *_Creature)
{
    return new boss_volkhanAI (_Creature);
}

void AddSC_boss_volkhan()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_volkhan";
    newscript->GetAI = &GetAI_boss_volkhan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_volkhan_anvil";
    newscript->GetAI = &GetAI_npc_volkhan_anvil;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_molten_golem";
    newscript->GetAI = &GetAI_mob_molten_golem;
    newscript->RegisterSelf();
}
