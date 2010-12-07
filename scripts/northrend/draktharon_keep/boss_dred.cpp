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
SDName: Boss_Tharonja
SD%Complete: 80%
SDComment: Timers
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

enum
{
    SAY_KING_DRED_TALON                = -1600020,
    SAY_CALL_FOR_RAPTOR                = -1600021,
    
    SPELL_BELLOWING_ROAR               = 22686,
    SPELL_FEARSOME_ROAR_N              = 48849,
    SPELL_FEARSOME_ROAR_H              = 59422,
    SPELL_GRIEVOUS_BITE                = 48920,
    SPELL_MANGLING_SLASH               = 48873,
    SPELL_PIERCING_SLASH               = 48878,
    SPELL_RAPTOR_CALL                  = 59416,            //not yet implemented

    NPC_DRAKKARI_GUTRIPPER             = 26641,
    NPC_DRAKKARI_SCYTHECLAW            = 26628
};

/*######
## boss_dred
######*/

struct MANGOS_DLL_DECL boss_dredAI : public ScriptedAI
{
    boss_dredAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 m_uiFearsomeRoarTimer;
    uint32 m_uiManglingSlashTimer;
    uint32 m_uiPiercingSlashTimer;
    uint32 m_uiGrievousBiteTimer;
    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiCallForRaptorTimer;
    uint32 m_uiCallForRaptorSpawnTimer;

    bool m_bIsNeedSummon;
    
    void Reset()
    {
        m_uiCallForRaptorSpawnTimer = 1000;
        m_uiFearsomeRoarTimer = 15000;
        m_uiManglingSlashTimer = urand(5000, 10000);
        m_uiPiercingSlashTimer = urand(10000, 15000);
        m_uiGrievousBiteTimer = urand (15000, 20000);
        m_uiBellowingRoarTimer = 60000;
        m_uiCheckTimer = 15000;
        m_uiCallForRaptorTimer = 25000;
        m_bIsNeedSummon = false;
    }

    void Aggro(Unit* pWho)
    {
       
    }

    void KilledUnit(Unit* pVictim)
    {
        
    }

    void JustDied(Unit* pKiller)
    {
        Map* pMap = m_creature->GetMap();
        if (pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_GRIEVOUS_BITE))
                    i->getSource()->RemoveAurasDueToSpell(SPELL_GRIEVOUS_BITE);
            }
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFearsomeRoarTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FEARSOME_ROAR_N : SPELL_FEARSOME_ROAR_H);
            m_uiFearsomeRoarTimer = 15000;
        }
        else
            m_uiFearsomeRoarTimer -= uiDiff;

        if (m_uiPiercingSlashTimer < uiDiff)
        {    
            DoCast(m_creature->getVictim(), SPELL_PIERCING_SLASH);
            m_uiPiercingSlashTimer = urand(20000, 25000);
        }
        else
            m_uiPiercingSlashTimer -= uiDiff;

        if (m_uiManglingSlashTimer < uiDiff)
        {    
            DoCast(m_creature->getVictim(), SPELL_MANGLING_SLASH);
            m_uiManglingSlashTimer = urand(10000, 15000);
        }
        else
            m_uiManglingSlashTimer -= uiDiff;

        if (m_uiGrievousBiteTimer < uiDiff)
        {    
            DoCast(m_creature->getVictim(), SPELL_GRIEVOUS_BITE);
            m_uiGrievousBiteTimer = urand(20000, 25000);
        }
        else
            m_uiGrievousBiteTimer -= uiDiff;

        if (m_uiCheckTimer < uiDiff)
        {
           Map* pMap = m_creature->GetMap();
           if (pMap->IsDungeon())
           {
               Map::PlayerList const &PlayerList = pMap->GetPlayers();

               if (PlayerList.isEmpty())
                   return;

               for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
               {
                   if (i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_GRIEVOUS_BITE) && i->getSource()->GetHealth() == i->getSource()->GetMaxHealth())
                       i->getSource()->RemoveAurasDueToSpell(SPELL_GRIEVOUS_BITE);
               }
           }
           m_uiCheckTimer = 500;
        }
        else
            m_uiCheckTimer -= uiDiff;
        
        if (m_uiBellowingRoarTimer < uiDiff)
        {    
            DoCast(m_creature, SPELL_BELLOWING_ROAR);
            m_uiBellowingRoarTimer = 60000;
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        if (m_uiCallForRaptorTimer < uiDiff)
        {    
            DoScriptText(SAY_CALL_FOR_RAPTOR, m_creature);
            m_creature->CastSpell(m_creature, SAY_CALL_FOR_RAPTOR, true);
            m_uiCallForRaptorTimer = 25000;
            m_uiCallForRaptorSpawnTimer = 1000;
            m_bIsNeedSummon = true;
        }
        else
            m_uiCallForRaptorTimer -= uiDiff;
 
        if (m_bIsNeedSummon)
        {
            if (m_uiCallForRaptorSpawnTimer < uiDiff)
            {    
                switch (urand(0, 1))
                {
                    case 0:
                    {
                        if (Creature* pRaptor1 = m_creature->SummonCreature(NPC_DRAKKARI_GUTRIPPER, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0 , TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240000))
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                pRaptor1->AI()->AttackStart(pTarget);
                        }
                    }
                    case 1:
                    {
                        if (Creature* pRaptor2 = m_creature->SummonCreature(NPC_DRAKKARI_SCYTHECLAW, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0 , TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240000))
                        {
                            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                pRaptor2->AI()->AttackStart(pTarget);
                        }
                    }
                }
                
                m_bIsNeedSummon = false;
            }
            else
                m_uiCallForRaptorSpawnTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dred(Creature* pCreature)
{
    return new boss_dredAI(pCreature);
}

void AddSC_boss_dred()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_dred";
    pNewScript->GetAI = &GetAI_boss_dred;
    pNewScript->RegisterSelf();
}