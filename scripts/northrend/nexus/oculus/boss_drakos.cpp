/* Copyright (C) 2008 - 2010 TrinityCore <http://www.trinitycore.org>
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
SDName: Boss_Drakos
SD%Complete: 98%
SDComment:
SDAuthor: MaxXx2021 Aka Mioka.
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

enum
{
    SAY_AGGRO                                     = -1578000,
    SAY_KILL_1                                    = -1578001,
    SAY_KILL_2                                    = -1578002,
    SAY_KILL_3                                    = -1578003,
    SAY_DEATH                                     = -1578004,
    SAY_PULL_1                                    = -1578005,
    SAY_PULL_2                                    = -1578006,
    SAY_PULL_3                                    = -1578007,
    SAY_PULL_4                                    = -1578008,
    SAY_STOMP_1                                   = -1578009,
    SAY_STOMP_2                                   = -1578010,
    SAY_STOMP_3                                   = -1578011,

    SPELL_MAGIC_PULL                              = 51336,
    SPELL_MAGIC_PULL_EFFECT                       = 50770,
    SPELL_THUNDERING_STOMP_N                      = 50774,
    SPELL_THUNDERING_STOMP_H                      = 59370,
    SPELL_UNSTABLE_SPHERE_PASSIVE                 = 50756,
    SPELL_UNSTABLE_SPHERE_PULSE                   = 50757,
    SPELL_UNSTABLE_SPHERE_TIMER                   = 50758,
    SPELL_UNSTABLE_SPHERE_EXPLODE                 = 50759,

    NPC_UNSTABLE_SPHERE                           = 28166
};

#define CENTER_X                960.120f
#define CENTER_Y                1049.413f

struct MANGOS_DLL_DECL boss_drakosAI : public ScriptedAI
{
    boss_drakosAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiMagicPullTimer ;
    uint32 uiStompTimer ;
    uint32 uiBombSummonTimer ;
    uint32 uiPostPullTimer ;
    bool bIsPulling ;
    bool bPostPull ;

    void Reset()
    {
        DespawnAllSphere();
        uiMagicPullTimer = urand(12000, 15000);
        uiStompTimer = urand(3000, 6000);
        uiBombSummonTimer = 7000;
        bPostPull = false;
        bIsPulling = false;
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKOS, NOT_STARTED);
    }

    void DespawnAllSphere()
    {
        std::list<Creature*> m_pSpheres;
        GetCreatureListWithEntryInGrid(m_pSpheres, m_creature, NPC_UNSTABLE_SPHERE, DEFAULT_VISIBILITY_INSTANCE);

        if (!m_pSpheres.empty())
            for(std::list<Creature*>::iterator iter = m_pSpheres.begin(); iter != m_pSpheres.end(); ++iter)
            {
               (*iter)->DealDamage((*iter), (*iter)->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKOS, IN_PROGRESS);
    }

    void JustDied(Unit* killer)
    {
        DespawnAllSphere();
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKOS, DONE);
    }

    void KilledUnit(Unit *victim)
    { 
        uint8 uiText = urand(0, 2);
        switch (uiText)
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_MAGIC_PULL)
          if(target->GetTypeId() == TYPEID_PLAYER)
            DoCast(target, SPELL_MAGIC_PULL_EFFECT, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiBombSummonTimer < diff)
        {
            if (m_bIsRegularMode)
                m_creature->SummonCreature(NPC_UNSTABLE_SPHERE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
            else
            {
                m_creature->SummonCreature(NPC_UNSTABLE_SPHERE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
                m_creature->SummonCreature(NPC_UNSTABLE_SPHERE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
            }
            uiBombSummonTimer = 3000;
        } else uiBombSummonTimer -= diff;

        if (uiMagicPullTimer < diff)
        {
            DoCast(m_creature, SPELL_MAGIC_PULL);
            uiMagicPullTimer = urand(15000, 25000);
            uint8 uiText = urand(0, 3);
            switch (uiText)
            {
               case 0: DoScriptText(SAY_PULL_1, m_creature); break;
               case 1: DoScriptText(SAY_PULL_2, m_creature); break;
               case 2: DoScriptText(SAY_PULL_3, m_creature); break;
               case 3: DoScriptText(SAY_PULL_4, m_creature); break;
            }
        } else uiMagicPullTimer -= diff;

        if (uiStompTimer < diff)
        {
            //DoScriptText(RAND(SAY_STOMP_1,SAY_STOMP_2,SAY_STOMP_3), me);
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_THUNDERING_STOMP_N : SPELL_THUNDERING_STOMP_H);
            uiStompTimer = urand(15000, 18000);
            uint8 uiText = urand(0, 2);
            switch (uiText)
            {
               case 0: DoScriptText(SAY_STOMP_1, m_creature); break;
               case 1: DoScriptText(SAY_STOMP_2, m_creature); break;
               case 2: DoScriptText(SAY_STOMP_3, m_creature); break;
            }
        } else uiStompTimer -= diff ;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakos(Creature* pCreature)
{
    return new boss_drakosAI (pCreature);
}

struct MANGOS_DLL_DECL npc_unstable_sphereAI : public ScriptedAI
{
    npc_unstable_sphereAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
       Reset();
    }

    uint32 uiPulseTimer;
    uint32 uiDeathTimer;

    void Reset()
    {
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->GetMotionMaster()->MovePoint(0, (CENTER_X-35)+rand()%70, (CENTER_Y-35)+rand()%70, m_creature->GetPositionZ());
        m_creature->SetSpeedRate(MOVE_RUN, 2, true);
        m_creature->setFaction(14);
        DoCast(m_creature, SPELL_UNSTABLE_SPHERE_PASSIVE, true);
        DoCast(m_creature, SPELL_UNSTABLE_SPHERE_TIMER, true);
        uiPulseTimer = 3000;
        uiDeathTimer = 19000;
    }

    void AttackStart(Unit* pWho)
    {

    }

    void UpdateAI(const uint32 diff)
    {
        if (uiPulseTimer < diff)
        {
            DoCast(m_creature, SPELL_UNSTABLE_SPHERE_PULSE, true);
            uiPulseTimer = 3000;
        } else uiPulseTimer -= diff;

        if (uiDeathTimer < diff)
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else uiDeathTimer -= diff;
    }
};

CreatureAI* GetAI_npc_unstable_sphere(Creature* pCreature)
{
    return new npc_unstable_sphereAI (pCreature);
}

void AddSC_boss_drakos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_drakos";
    newscript->GetAI = &GetAI_boss_drakos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_unstable_sphere";
    newscript->GetAI = &GetAI_npc_unstable_sphere;
    newscript->RegisterSelf();
}
