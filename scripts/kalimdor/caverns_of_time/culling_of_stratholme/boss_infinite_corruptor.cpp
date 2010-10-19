#include "precompiled.h"
#include "culling_of_stratholme.h"

enum
{
   SPELL_COURSE      = 60588,
   SPELL_STRIKE      = 60590
};

struct MANGOS_DLL_DECL boss_infinite_corruptorAI : public ScriptedAI
{
   boss_infinite_corruptorAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
   }

   ScriptedInstance* m_pInstance;

   uint32 m_uiStrikeTimer;
   uint32 m_uiCourseTimer;

   void Reset() 
   {
     m_uiCourseTimer = 7000;
     m_uiStrikeTimer = 5000;
   }

   void Aggro(Unit* who)
   {
      if(m_pInstance)
         m_pInstance->SetData(TYPE_BONUS, SPECIAL);
   }

   void JustDied(Unit *killer)
   {
       if(m_pInstance)
         m_pInstance->SetData(TYPE_BONUS, DONE);
   }

    void EnterEvadeMode()
    {
       if(!m_pInstance) return;

       m_creature->RemoveAllAuras();
       m_creature->DeleteThreatList();
       m_creature->CombatStop(true);
       m_creature->LoadCreatureAddon();
       if(m_pInstance)
         m_pInstance->SetData(TYPE_BONUS, IN_PROGRESS);

       if(m_creature->isAlive())
          m_creature->GetMotionMaster()->MoveTargetedHome();

       m_creature->SetLootRecipient(NULL);
 
       Reset();
    }

   void UpdateAI(const uint32 diff)
   {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();

        if (m_uiCourseTimer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, SPELL_COURSE);

            m_uiCourseTimer = 17000;
        }else m_uiCourseTimer -= diff;

        if (m_uiStrikeTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_STRIKE);

            m_uiStrikeTimer = 5000;
        }else m_uiStrikeTimer -= diff;
  }
};

CreatureAI* GetAI_boss_infinite_corruptor(Creature* pCreature)
{
    return new boss_infinite_corruptorAI(pCreature);
}

void AddSC_boss_infinite_corruptor()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_infinite_corruptor";
    newscript->GetAI = &GetAI_boss_infinite_corruptor;
    newscript->RegisterSelf();
}
