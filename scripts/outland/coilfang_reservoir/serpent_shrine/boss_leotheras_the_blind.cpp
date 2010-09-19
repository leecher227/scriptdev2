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
SDName: Boss_Leotheras_The_Blind
SD%Complete: 50
SDComment: Missing Inner Demons
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "serpent_shrine.h"

enum
{
    SAY_AGGRO               = -1548009,
    SAY_SWITCH_TO_DEMON     = -1548010,
    SAY_INNER_DEMONS        = -1548011,
    SAY_DEMON_SLAY1         = -1548012,
    SAY_DEMON_SLAY2         = -1548013,
    SAY_DEMON_SLAY3         = -1548014,
    SAY_NIGHTELF_SLAY1      = -1548015,
    SAY_NIGHTELF_SLAY2      = -1548016,
    SAY_NIGHTELF_SLAY3      = -1548017,
    SAY_FINAL_FORM          = -1548018,
    SAY_FREE                = -1548019,
    SAY_DEATH               = -1548020,

    SPELL_ENRAGE            = 26662,

    // --- Spells used by Leotheras The Blind
    SPELL_WHIRLWIND         = 37640,
    SPELL_CHAOS_BLAST       = 37674,
    SPELL_INSIDIOUS_WHISPER = 37676,                        //not implemented yet. After cast (spellHit), do the inner demon
    SPELL_CONS_MADNESS      = 37749,

    // --- Spells used in banish phase ---
    BANISH_BEAM             = 38909,
    AURA_BANISH             = 37833,

    // --- Spells used by Greyheart Spellbinders
    SPELL_EARTHSHOCK        = 39076,
    SPELL_MINDBLAST         = 37531,

    // --- Spells used by Inner Demons
    SPELL_DEMON_ALIGNMENT   = 37713,                        //inner demon have this aura
    SPELL_SHADOW_BOLT       = 39309,                        //inner demon spell spam
    SPELL_SOUL_LINK         = 38007,

    MODEL_NIGHTELF          = 20514,
    MODEL_DEMON             = 20125,

    NPC_INNER_DEMON         = 21857,
    NPC_SHADOW_LEO          = 21875
};

struct MANGOS_DLL_DECL mob_inner_demonAI : public ScriptedAI
{
    mob_inner_demonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiVictimGUID = 0;
        Reset();
    }

    uint32 m_uiShadowBoltTimer;

    uint32 m_uiLinkTimer;
    uint64 m_uiVictimGUID;

    void Reset()
    {
        m_uiShadowBoltTimer = 10000;
        m_uiLinkTimer = 1000;
    }

    void JustDied(Unit* pVictim)
    {
        Unit* pUnit = m_creature->GetMap()->GetUnit(m_uiVictimGUID);
        if (pUnit)
            pUnit->RemoveAurasDueToSpell(SPELL_INSIDIOUS_WHISPER);
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (pDoneBy->GetGUID() != m_uiVictimGUID && pDoneBy->GetGUID() != m_creature->GetGUID())
        {
            uiDamage = 0;
            m_creature->getThreatManager().modifyThreatPercent(pDoneBy, -100);
        }
    }

    void Aggro(Unit *pWho)
    {
        if (!m_uiVictimGUID)
            return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->getVictim()->GetGUID() != m_uiVictimGUID)
            if (Unit* pOwner = m_creature->GetMap()->GetUnit(m_uiVictimGUID))
                AttackStart(pOwner);
        
        if (m_uiLinkTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SOUL_LINK, true);
            m_uiLinkTimer = 1000;
        }
        else
            m_uiLinkTimer -= uiDiff;

        if (!m_creature->HasAura(SPELL_DEMON_ALIGNMENT))
            DoCast(m_creature, SPELL_DEMON_ALIGNMENT, true);

        if (m_uiShadowBoltTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SHADOW_BOLT, false);
            m_uiShadowBoltTimer = 10000;
        }
        else
            m_uiShadowBoltTimer -= uiDiff; 

        DoMeleeAttackIfReady();
    }

};

//Original Leotheras the Blind AI
struct MANGOS_DLL_DECL boss_leotheras_the_blindAI : public ScriptedAI
{
    boss_leotheras_the_blindAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiShadowLeo = 0;

        m_uiChannelersCount = 0;
        
        m_creature->ApplySpellImmune(AURA_BANISH, IMMUNITY_MECHANIC, MECHANIC_BANISH, false);

        //damage & attack speed to dual wield    2.6s mainhand and 1.5s offhand
        const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
        m_creature->SetAttackTime(BASE_ATTACK, 2600);
        
        m_creature->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, cinfo->mindmg*0.5);
        m_creature->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, cinfo->maxdmg*0.5);
        
        m_creature->SetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, cinfo->mindmg*0.5);
        m_creature->SetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, cinfo->maxdmg*0.5);
        m_creature->SetAttackTime(OFF_ATTACK, 1500);

        // needs to reset offhand attack timer
        m_creature->setAttackTimer(BASE_ATTACK, 0);
        m_creature->setAttackTimer(OFF_ATTACK, 0);

        Reset();

    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiWhirlwind_Timer;
    uint32 m_uiChaosBlast_Timer;
    uint32 m_uiSwitchToDemon_Timer;
    uint32 m_uiSwitchToHuman_Timer;
    uint32 m_uiEnrage_Timer;
    uint32 m_uiInnerDemon_Timer;
    
    bool m_bNeedThreatReset;
    bool m_bDemonForm;
    bool m_bIsFinalForm;
    bool m_bIsEnraged;

    uint8 m_uiChannelersCount;
    uint8 m_uiInnerDemon_Count;
    uint64 m_auiInnerDemonGUID[5];

    uint64 m_uiShadowLeo;

    void Reset()
    {
        m_uiWhirlwind_Timer = 15000;
        m_uiChaosBlast_Timer = 1000;
        m_uiSwitchToDemon_Timer = 45000;
        m_uiSwitchToHuman_Timer = 60000;
        m_uiEnrage_Timer = MINUTE*10*IN_MILLISECONDS;
        m_uiInnerDemon_Timer = 30000;

        m_bDemonForm = false;
        m_bIsFinalForm = false;
        m_bNeedThreatReset = false;
        m_bIsEnraged = false;

        m_uiInnerDemon_Count = 0;

        m_creature->SetSpeedRate(MOVE_RUN, 2.0f, true);

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

        m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_NIGHTELF);


        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+0, 0);
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);


        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, NOT_STARTED); 
            //Respawn of the 3 Spellbinder
            uint64 uiGreyheartSpellbinderGUID[3];
            uiGreyheartSpellbinderGUID[0] = m_pInstance->GetData64(DATA_SPELLBINDER_1);
            uiGreyheartSpellbinderGUID[1] = m_pInstance->GetData64(DATA_SPELLBINDER_2);
            uiGreyheartSpellbinderGUID[2] = m_pInstance->GetData64(DATA_SPELLBINDER_3);
            for (uint8 i=0; i<3; ++i)
            {
                // Greyheart Spellbinder is dead, we must respawn it
                if(uiGreyheartSpellbinderGUID[i])
                {
                    Creature* pSpellbinder = m_creature->GetMap()->GetCreature(uiGreyheartSpellbinderGUID[i]);
                    if (pSpellbinder)
                    {
                        if(!pSpellbinder->isAlive())
                            pSpellbinder->Respawn();

                        pSpellbinder->AI()->EnterEvadeMode();
                        pSpellbinder->GetMotionMaster()->MoveTargetedHome();
                    }
                }
            }
        }
        m_creature->SetCorpseDelay(1000*60*60);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_creature->HasAura(AURA_BANISH))
            return;

        if (!m_creature->getVictim() && pWho->isTargetableForAttack() && (m_creature->IsHostileTo(pWho)) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->canFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                AttackStart(pWho);
            }
        }
    }

    void CheckBanish()
    {
        // m_uiChannelersCount == 0 remove banish aura
        if (m_uiChannelersCount == 0 && m_creature->HasAura(AURA_BANISH))
        {    
            // removing banish aura
            m_creature->RemoveAurasDueToSpell(AURA_BANISH);

            // Leotheras is getting immune again 
            m_creature->ApplySpellImmune(AURA_BANISH, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);

            // changing model to bloodelf 
            m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_NIGHTELF);

            // and reseting equipment
            m_creature->LoadEquipment(m_creature->GetEquipmentId());

            if (m_pInstance && m_pInstance->GetData64(DATA_LEOTHERAS_EVENT_STARTER))
                if (Unit* pVictim = m_creature->GetMap()->GetUnit(m_pInstance->GetData64(DATA_LEOTHERAS_EVENT_STARTER)))
                    m_creature->getThreatManager().addThreat(pVictim, 1);
        }
        else if (m_uiChannelersCount != 0 && !m_creature->HasAura(AURA_BANISH))
        {
            // m_uiChannelersCount != 0 apply banish aura
            // removing Leotheras banish immune to apply AURA_BANISH
            m_creature->ApplySpellImmune(AURA_BANISH, IMMUNITY_MECHANIC, MECHANIC_BANISH, false);
            DoCast(m_creature, AURA_BANISH);

            // changing model
            m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_DEMON);
            
            // and removing weapons
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+0, 0);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);
        }

    }

    void DoMeleeAttackIfReady()
    {
        if (!m_creature->getVictim())
            return;
        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE) && !m_creature->IsNonMeleeSpellCasted(false))
        {
            //Check for base attack
            if( m_creature->isAttackReady() && m_creature->getVictim())
            {
                m_creature->AttackerStateUpdate(m_creature->getVictim());
                m_creature->resetAttackTimer();
            }
            //Check for offhand attack
            if( m_creature->isAttackReady(OFF_ATTACK) && m_creature->getVictim())
            {
                m_creature->AttackerStateUpdate(m_creature->getVictim(), OFF_ATTACK);
                m_creature->resetAttackTimer(OFF_ATTACK);
            }
        }
    }

    //Despawn all Inner Demon summoned
    void DespawnDemon()
    {
        for (uint8 i=0; i<5; ++i)
        {
            if (m_auiInnerDemonGUID[i])
            {
                if (Creature* pDemon = m_creature->GetMap()->GetCreature(m_auiInnerDemonGUID[i]))
                {
                    pDemon->ForcedDespawn();
                    m_auiInnerDemonGUID[i] = 0;
                }
            }
        }
        m_uiInnerDemon_Count = 0;
    }

    void CastConsumingMadness()    //remove this once SPELL_INSIDIOUS_WHISPER is supported by core
    {
        for (uint8 i=0; i<5; ++i)
        {
            if (m_auiInnerDemonGUID[i] > 0 )
            {
                Unit* pUnit = m_creature->GetMap()->GetUnit(m_auiInnerDemonGUID[i]);
                if (pUnit && pUnit->isAlive())
                {
                    Unit* pUnitTarget = pUnit->GetMap()->GetUnit(((mob_inner_demonAI*)((Creature*)pUnit)->AI())->m_uiVictimGUID);
                    if (pUnitTarget && pUnitTarget->isAlive())
                    {
                        pUnit->CastSpell(pUnitTarget, SPELL_CONS_MADNESS, true);
                        m_creature->getThreatManager().modifyThreatPercent(pUnitTarget, -100);
                    }
                }
            }
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY1 : SAY_NIGHTELF_SLAY1, m_creature); break;
            case 1: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY2 : SAY_NIGHTELF_SLAY2, m_creature); break;
            case 2: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY3 : SAY_NIGHTELF_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* pVictim)
    {
        DoScriptText(SAY_DEATH, m_creature);

        //despawn copy
        if (m_uiShadowLeo)
        {
            if (Creature* pShadowLeo = m_creature->GetMap()->GetCreature(m_uiShadowLeo))
                pShadowLeo->ForcedDespawn();
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if(m_creature->HasAura(AURA_BANISH))
            return;

        m_creature->LoadEquipment(m_creature->GetEquipmentId());

        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEOTHERAS_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (m_creature->HasAura(AURA_BANISH) || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            CheckBanish();
            return;
        }

        if (m_creature->HasAura(SPELL_WHIRLWIND))
            if (m_uiWhirlwind_Timer < diff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    DoResetThreat();                
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(0, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
                }
                m_uiWhirlwind_Timer = 2000;
            }
            else
                m_uiWhirlwind_Timer -= diff;

        // reseting after changing forms and after ending whirlwind
        if(m_bNeedThreatReset && !m_creature->HasAura(SPELL_WHIRLWIND))
        {    
            // and reseting attack timer to 0
            m_creature->setAttackTimer(BASE_ATTACK, 0);
            m_creature->setAttackTimer(OFF_ATTACK, 0);

            // when changing forms seting timers (or when ending whirlwind - to avoid adding new variable i use m_uiWhirlwind_Timer to countdown 2s while whirlwinding)
            if (m_bDemonForm)
                m_uiInnerDemon_Timer = 30000;
            else
                m_uiWhirlwind_Timer =  15000;
            
            m_bNeedThreatReset = false;
            DoResetThreat();
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }

        //Enrage_Timer ( 10 min )
        if (m_uiEnrage_Timer < diff && !m_bIsEnraged)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            m_bIsEnraged = true;
        }
        else
            m_uiEnrage_Timer -= diff;

        if (!m_bDemonForm)
        {
            //m_uiWhirlwind_Timer
            if (!m_creature->HasAura(SPELL_WHIRLWIND))
            {
                if (m_uiWhirlwind_Timer < diff)
                {
                    DoCast(m_creature, SPELL_WHIRLWIND);
                    // while whirlwinding this variable is used to countdown target's change
                    m_uiWhirlwind_Timer = 2000;
                    m_bNeedThreatReset = true;
                }
                else
                    m_uiWhirlwind_Timer -= diff;
            }

            //Switch_Timer
            if (!m_bIsFinalForm)
                if (m_uiSwitchToDemon_Timer < diff)
                {
                    //switch to demon form
                    m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND,0);
                    m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_DEMON);
                    DoScriptText(SAY_SWITCH_TO_DEMON, m_creature);

                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+0, 0);
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, 0);
                    
                    m_bDemonForm = true;
                    m_bNeedThreatReset = true;

                    m_uiSwitchToDemon_Timer = 45000;
                }
                else
                    m_uiSwitchToDemon_Timer -= diff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (!m_creature->getVictim())
                return;
            if (m_creature->GetDistance(m_creature->getVictim()) < 30)
                m_creature->StopMoving();

            if (m_uiChaosBlast_Timer < diff)
            {
                // will cast only when in range of spell 
                if (m_creature->GetDistance(m_creature->getVictim()) < 30)
                {
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_CHAOS_BLAST, true);
                    m_uiChaosBlast_Timer = 3000;
                }
            }
            else
                m_uiChaosBlast_Timer -= diff;

            //summon Inner Demon
            if (m_uiInnerDemon_Timer < diff)
            {
                ThreatList const& lThreatList = m_creature->getThreatManager().getThreatList();
                std::vector<Unit *> TargetList; 

                if (!lThreatList.empty())
                    for (ThreatList::const_iterator itr = lThreatList.begin(); itr != lThreatList.end(); ++itr)
                    {
                        Unit* pTempTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());
                        if (pTempTarget && pTempTarget->GetTypeId() == TYPEID_PLAYER && pTempTarget->GetGUID() != m_creature->getVictim()->GetGUID() && TargetList.size()<5)
                            TargetList.push_back(pTempTarget);
                    }
                if (!TargetList.empty())
                    for(std::vector<Unit*>::iterator itr = TargetList.begin(); itr != TargetList.end(); ++itr)
                        if (*itr && (*itr)->isAlive())
                        {
                            Creature* pDemon = (Creature*)m_creature->SummonCreature(NPC_INNER_DEMON, (*itr)->GetPositionX()+10, (*itr)->GetPositionY()+10, (*itr)->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                            if (pDemon)
                            {
                                pDemon->AI()->AttackStart(*itr);
                                ((mob_inner_demonAI*)pDemon->AI())->m_uiVictimGUID = (*itr)->GetGUID();
                            }
                                                        
                            if (SpellEntry* pSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_INSIDIOUS_WHISPER))
                            {
                                pSpell->EffectImplicitTargetA[0] = TARGET_SELF;
                                pSpell->EffectImplicitTargetB[0] = 0;
                                (*itr)->CastSpell(*itr, pSpell, true);
                            }

                            if (m_uiInnerDemon_Count > 4)
                                m_uiInnerDemon_Count = 0;

                            //Safe storing of creatures
                            m_auiInnerDemonGUID[m_uiInnerDemon_Count] = pDemon->GetGUID();

                            //Update demon count
                            ++m_uiInnerDemon_Count;
                        }

                DoScriptText(SAY_INNER_DEMONS, m_creature);
                m_uiInnerDemon_Timer = 999999;
            }
            else
                m_uiInnerDemon_Timer -= diff;

            //Switch_Timer
            if (m_uiSwitchToHuman_Timer < diff)
            {
                //switch to nightelf form
                m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_NIGHTELF);
                m_creature->LoadEquipment(m_creature->GetEquipmentId());

                CastConsumingMadness();
                DespawnDemon();

                m_bDemonForm = false;
                m_bNeedThreatReset = true;

                m_uiSwitchToHuman_Timer = 60000;
            }
            else
                m_uiSwitchToHuman_Timer -= diff;
        }

        if (!m_bIsFinalForm && m_creature->GetHealthPercent() <= 15)
        {
            //at this point he divides himself in two parts
            if (Creature* pCopy = DoSpawnCreature(NPC_SHADOW_LEO, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6000))
            {
                m_uiShadowLeo = pCopy->GetGUID();
                if (m_creature->getVictim())
                    pCopy->AI()->AttackStart(m_creature->getVictim());
            }
            //set nightelf final form
            m_bIsFinalForm = true;
            m_bDemonForm = false;
            DoScriptText(SAY_FINAL_FORM, m_creature);
            m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_NIGHTELF);
            m_creature->LoadEquipment(m_creature->GetEquipmentId());
        }
    }
};

//Leotheras the Blind Demon Form AI
struct MANGOS_DLL_DECL boss_leotheras_the_blind_demonformAI : public ScriptedAI
{
    boss_leotheras_the_blind_demonformAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiChaosBlastTimer;

    void Reset()
    {
        m_uiChaosBlastTimer = 1000;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_DEMON_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_DEMON_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_DEMON_SLAY3, m_creature); break;
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_FREE, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance(m_creature->getVictim()) < 30)
            m_creature->StopMoving();

        if (m_uiChaosBlastTimer < diff)
        {
            // will cast only when in range od spell 
            if (m_creature->GetDistance(m_creature->getVictim()) < 30)
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_CHAOS_BLAST, true);
                m_uiChaosBlastTimer = 3000;
            }
        }
        else
            m_uiChaosBlastTimer -= diff;

        //Do NOT deal any melee damage to the target.
    }
};

struct MANGOS_DLL_DECL mob_greyheart_spellbinderAI : public ScriptedAI
{
    mob_greyheart_spellbinderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiLeoGUID = 0;
        m_bAddedBanish = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint64 m_uiLeoGUID;

    uint32 m_uiMindBlastTimer;
    uint32 m_uiEarthShockTimer;

    bool m_bAddedBanish;

    void Reset()
    {
        m_uiMindBlastTimer  = urand(3000, 8000);
        m_uiEarthShockTimer = urand(5000, 10000);

        if (m_pInstance)
            m_pInstance->SetData64(DATA_LEOTHERAS_EVENT_STARTER, 0);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->InterruptNonMeleeSpells(false);
        if (m_pInstance)
            m_pInstance->SetData64(DATA_LEOTHERAS_EVENT_STARTER, pWho->GetGUID());
    }

    void JustRespawned()
    {
        m_bAddedBanish = false;
        Reset();
    }

    void CastChanneling()
    {
        if (!m_creature->isInCombat() && !m_bAddedBanish)
        {
            m_creature->CastStop();
            if (m_uiLeoGUID)
            {
                Creature* pLeo = m_creature->GetMap()->GetCreature(m_uiLeoGUID);
                if(pLeo && pLeo->isAlive())
                {
                    DoCast(pLeo, BANISH_BEAM);
                    ((boss_leotheras_the_blindAI*)pLeo->AI())->m_uiChannelersCount++;
                    m_bAddedBanish = true;
                }
            }
        }
    }
    

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance)
        {    
            if (!m_uiLeoGUID)
                m_uiLeoGUID = m_pInstance->GetData64(DATA_LEOTHERAS);

            if (!m_creature->isInCombat() && m_pInstance->GetData64(DATA_LEOTHERAS_EVENT_STARTER))
                if (Unit* pTarget =m_creature->GetMap()->GetUnit(m_pInstance->GetData64(DATA_LEOTHERAS_EVENT_STARTER)))
                    AttackStart(pTarget);
        }
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            CastChanneling();
            return;
        }

        if (m_pInstance && !m_pInstance->GetData64(DATA_LEOTHERAS_EVENT_STARTER))
            EnterEvadeMode();

        if (m_uiMindBlastTimer < diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, SPELL_MINDBLAST);
            m_uiMindBlastTimer = urand(10000, 15000);
        }
        else
            m_uiMindBlastTimer -= diff;

        if (m_uiEarthShockTimer < diff)
        {
            Map *map = m_creature->GetMap();
            Map::PlayerList const &PlayerList = map->GetPlayers();
            if (!PlayerList.isEmpty())
                return;
            for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            {
                if (itr->getSource()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                {
                    DoCast(itr->getSource(), SPELL_EARTHSHOCK);
                    break;
                }
            }
            m_uiEarthShockTimer = urand(8000, 15000);
        }
        else
            m_uiEarthShockTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_bAddedBanish)
            if (m_pInstance && m_uiLeoGUID)
                if (Creature* pLeo = m_creature->GetMap()->GetCreature(m_uiLeoGUID))
                    ((boss_leotheras_the_blindAI*)pLeo->AI())->m_uiChannelersCount--;
    }
};

CreatureAI* GetAI_boss_leotheras_the_blind(Creature* pCreature)
{
    return new boss_leotheras_the_blindAI(pCreature);
}

CreatureAI* GetAI_boss_leotheras_the_blind_demonform(Creature* pCreature)
{
    return new boss_leotheras_the_blind_demonformAI(pCreature);
}

CreatureAI* GetAI_mob_greyheart_spellbinder(Creature* pCreature)
{
    return new mob_greyheart_spellbinderAI(pCreature);
}

CreatureAI* GetAI_mob_inner_demon(Creature* pCreature)
{
    return new mob_inner_demonAI(pCreature);
}

void AddSC_boss_leotheras_the_blind()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_leotheras_the_blind";
    newscript->GetAI = &GetAI_boss_leotheras_the_blind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_leotheras_the_blind_demonform";
    newscript->GetAI = &GetAI_boss_leotheras_the_blind_demonform;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_greyheart_spellbinder";
    newscript->GetAI = &GetAI_mob_greyheart_spellbinder;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_inner_demon";
    newscript->GetAI = &GetAI_mob_inner_demon;
    newscript->RegisterSelf();
}
