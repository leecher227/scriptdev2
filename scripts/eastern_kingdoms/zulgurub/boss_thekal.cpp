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
SDName: Boss_Thekal
SD%Complete: 95
SDComment: Almost finished.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum
{
    SAY_AGGRO           = -1309009,
    SAY_DEATH           = -1309010,

    SPELL_MORTALCLEAVE  = 22859,
    SPELL_SILENCE       = 22666,
    SPELL_FRENZY        = 8269,
    SPELL_FORCEPUNCH    = 24189,
    SPELL_CHARGE        = 24193,
    SPELL_SUMMONTIGERS  = 24183,
    SPELL_TIGER_FORM    = 24169,

    //Zealot Lor'Khan Spells
    SPELL_SHIELD        = 20545,
    SPELL_BLOODLUST     = 24185,
    SPELL_GREATERHEAL   = 24208,
    SPELL_DISARM        = 6713,

    //Zealot Lor'Khan Spells
    SPELL_SWEEPINGSTRIKES = 18765,
    SPELL_SINISTERSTRIKE  = 15581,
    SPELL_GOUGE         = 12540,
    SPELL_KICK          = 15614,
    SPELL_BLIND         = 21060
};

struct MANGOS_DLL_DECL boss_thekalAI : public ScriptedAI
{
    boss_thekalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiMortalCleave_Timer;
    uint32 m_uiSilence_Timer;
    uint32 m_uiForcePunch_Timer;
    uint32 m_uiCharge_Timer;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiSummonTigers_Timer;
    uint32 m_uiCheck_Timer;
    uint32 m_uiResurrectBuddy_Timer;

    bool m_bIsEnraged;
    bool m_bPhaseTwo;
    bool m_bNeedToResurrect;
    bool m_bIsFakeDead;

    void Reset()
    {
        m_uiMortalCleave_Timer = 4000;
        m_uiSilence_Timer = 9000;
        m_uiForcePunch_Timer = 4000;
        m_uiCharge_Timer = 12000;
        m_uiEnrage_Timer = 32000;
        m_uiSummonTigers_Timer = 25000;
        m_uiCheck_Timer = 1000;
        m_uiResurrectBuddy_Timer = 5000;

        m_bIsEnraged = false;
        m_bPhaseTwo = false;
        m_bNeedToResurrect = false;
        m_bIsFakeDead = false;

        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        if (m_pInstance)
        {
            if (Creature* pLorKhan = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_LORKHAN)))
                if (pLorKhan->isDead())
                    pLorKhan->Respawn();

            if (Creature* pZath = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_ZATH)))
                if (pZath->isDead())
                    pZath->Respawn();
        }
    }

    void Aggro(Unit *pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THEKAL, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THEKAL, NOT_STARTED);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bPhaseTwo)
            return;

        if (m_creature->GetHealth() < uiDamage)
        {
            uiDamage = 0;
            m_creature->SetHealth(0);
            m_creature->RemoveAllAuras();
            m_creature->GetMotionMaster()->Clear();
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
            m_bIsFakeDead = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        if (!m_bPhaseTwo)
        {
            if (m_bNeedToResurrect)
            {
                if (m_uiResurrectBuddy_Timer < uiDiff)
                {
                    m_bNeedToResurrect = false;

                    bool bLorKhanIsDead = false;
                    bool bZathIsDead = false;
                    Creature* pLorKhan = NULL;
                    Creature* pZath = NULL;

                    if (m_pInstance)
                    {
                        pLorKhan = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_LORKHAN));
                        pZath = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_ZATH));
                    }

                    if (pLorKhan && pLorKhan->isDead())
                        bLorKhanIsDead = true;

                    if (pZath && pZath->isDead())
                        bZathIsDead = true;

                    if (m_bIsFakeDead)
                    {
                        m_bIsFakeDead = false;
                        m_creature->SetHealth(m_creature->GetMaxHealth());
                        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            m_creature->GetMotionMaster()->MoveChase(pTarget);
                        if (bLorKhanIsDead && bZathIsDead)
                        {
                            DoCastSpellIfCan(m_creature, SPELL_TIGER_FORM);
                            m_bPhaseTwo = true;
                            return;
                        }
                    }

                    if (bLorKhanIsDead)
                    {
                        pLorKhan->Respawn();
                        pLorKhan->AI()->AttackStart(m_creature->getVictim());
                    }

                    if (bZathIsDead)
                    {
                        pZath->Respawn();
                        pZath->AI()->AttackStart(m_creature->getVictim());
                    }
                }
                else
                    m_uiResurrectBuddy_Timer -= uiDiff;
            }
            else
            {
               if (m_uiCheck_Timer < uiDiff)
                {
                    Creature* pLorKhan = NULL;
                    Creature* pZath = NULL;

                    if (m_pInstance)
                    {
                        pLorKhan = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_LORKHAN));
                        pZath = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_ZATH));
                    }

                    if (m_bIsFakeDead || (pLorKhan && pLorKhan->isDead()) || (pZath && pZath->isDead()))
                    {
                        m_bNeedToResurrect = true;
                        m_uiResurrectBuddy_Timer = 5000;
                    }

                    m_uiCheck_Timer = 1000;
                }
                else
                    m_uiCheck_Timer -= uiDiff;
            }

            if (m_bIsFakeDead)
                return;

            if (m_uiMortalCleave_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTALCLEAVE);
                m_uiMortalCleave_Timer = urand(15000, 20000);
            }
            else
                m_uiMortalCleave_Timer -= uiDiff;

            if (m_uiSilence_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SILENCE);
                m_uiSilence_Timer = urand(20000, 25000);
            }
            else
                m_uiSilence_Timer -= uiDiff;
        }
        else
        {
            if (m_uiCharge_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0), SPELL_CHARGE);
                m_uiCharge_Timer = urand(15000, 22000);
            }
            else
                m_uiCharge_Timer -= uiDiff;

            if (m_uiForcePunch_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_FORCEPUNCH);
                m_uiForcePunch_Timer = urand(16000, 21000);
            }
            else
                m_uiForcePunch_Timer -= uiDiff;

            if (m_uiSummonTigers_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUMMONTIGERS);
                m_uiSummonTigers_Timer = urand(10000, 14000);
            }
            else
                m_uiSummonTigers_Timer -= uiDiff;

            if (m_creature->GetHealthPercent() < 10.0f && !m_bIsEnraged)
            {
                DoCastSpellIfCan(m_creature, SPELL_FRENZY);
                m_bIsEnraged = true;
            }
        }

        DoMeleeAttackIfReady();
    }
};

//Zealot Lor'Khan
struct MANGOS_DLL_DECL mob_zealot_lorkhanAI : public ScriptedAI
{
    mob_zealot_lorkhanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiShield_Timer;
    uint32 m_uiBloodLust_Timer;
    uint32 m_uiGreaterHeal_Timer;
    uint32 m_uiDisarm_Timer;

    void Reset()
    {
        m_uiShield_Timer = 1000;
        m_uiBloodLust_Timer = 16000;
        m_uiGreaterHeal_Timer = 32000;
        m_uiDisarm_Timer = 6000;
    }

    void UpdateAI (const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Shield_Timer
        if (m_uiShield_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SHIELD);
            m_uiShield_Timer = 61000;
        }
        else
            m_uiShield_Timer -= uiDiff;

        //BloodLust_Timer
        if (m_uiBloodLust_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BLOODLUST);
            m_uiBloodLust_Timer = urand(20000, 28000);
        }
        else
            m_uiBloodLust_Timer -= uiDiff;

        //Casting Greaterheal to Thekal or Zath if they are in meele range.
        if (m_uiGreaterHeal_Timer < uiDiff)
        {
            if (m_pInstance)
            {
                Unit *pThekal = m_creature->GetMap()->GetUnit(m_pInstance->GetData64(DATA_THEKAL));
                Unit *pZath = m_creature->GetMap()->GetUnit(m_pInstance->GetData64(DATA_ZATH));

                switch (urand(0, 1))
                {
                    case 0:
                        if (pThekal && m_creature->IsWithinDistInMap(pThekal, ATTACK_DISTANCE))
                            DoCastSpellIfCan(pThekal, SPELL_GREATERHEAL);
                        break;
                    case 1:
                        if (pZath && m_creature->IsWithinDistInMap(pZath, ATTACK_DISTANCE))
                            DoCastSpellIfCan(pZath, SPELL_GREATERHEAL);
                        break;
                }
            }

            m_uiGreaterHeal_Timer = urand(15000, 20000);
        }
        else
            m_uiGreaterHeal_Timer -= uiDiff;

        //Disarm_Timer
        if (m_uiDisarm_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM);
            m_uiDisarm_Timer = urand(15000, 25000);
        }
        else
            m_uiDisarm_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

//Zealot Zath
struct MANGOS_DLL_DECL mob_zealot_zathAI : public ScriptedAI
{
    mob_zealot_zathAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSweepingStrikes_Timer;
    uint32 m_uiSinisterStrike_Timer;
    uint32 m_uiGouge_Timer;
    uint32 m_uiKick_Timer;
    uint32 m_uiBlind_Timer;

    void Reset()
    {
        m_uiSweepingStrikes_Timer = 13000;
        m_uiSinisterStrike_Timer = 8000;
        m_uiGouge_Timer = 25000;
        m_uiKick_Timer = 18000;
        m_uiBlind_Timer = 5000;
    }

    void UpdateAI (const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //SweepingStrikes_Timer
        if (m_uiSweepingStrikes_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWEEPINGSTRIKES);
            m_uiSweepingStrikes_Timer = urand(22000, 26000);
        }
        else
            m_uiSweepingStrikes_Timer -= uiDiff;

        //SinisterStrike_Timer
        if (m_uiSinisterStrike_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SINISTERSTRIKE);
            m_uiSinisterStrike_Timer = urand(8000, 16000);
        }
        else
            m_uiSinisterStrike_Timer -= uiDiff;

        //Gouge_Timer
        if (m_uiGouge_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE);

            if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -100);

            m_uiGouge_Timer = urand(17000, 27000);
        }
        else
            m_uiGouge_Timer -= uiDiff;

        //Kick_Timer
        if (m_uiKick_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KICK);
            m_uiKick_Timer = urand(15000, 25000);
        }
        else
            m_uiKick_Timer -= uiDiff;

        //Blind_Timer
        if (m_uiBlind_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLIND);
            m_uiBlind_Timer = urand(10000, 20000);
        }
        else
            m_uiBlind_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thekal(Creature* pCreature)
{
    return new boss_thekalAI(pCreature);
}

CreatureAI* GetAI_mob_zealot_lorkhan(Creature* pCreature)
{
    return new mob_zealot_lorkhanAI(pCreature);
}

CreatureAI* GetAI_mob_zealot_zath(Creature* pCreature)
{
    return new mob_zealot_zathAI(pCreature);
}

void AddSC_boss_thekal()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_thekal";
    newscript->GetAI = &GetAI_boss_thekal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zealot_lorkhan";
    newscript->GetAI = &GetAI_mob_zealot_lorkhan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zealot_zath";
    newscript->GetAI = &GetAI_mob_zealot_zath;
    newscript->RegisterSelf();
}
