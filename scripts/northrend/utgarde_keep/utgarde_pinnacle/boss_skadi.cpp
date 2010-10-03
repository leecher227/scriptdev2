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
SDName: Boss_Skadi
SD%Complete: 95%
SDComment:
SDAuthor: MaxXx2021 (aka Mioka) and PSZ
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"
#include "Vehicle.h"

enum
{
    SAY_AGGRO                       = -1575019,
    SAY_DRAKEBREATH_1               = -1575020,
    SAY_DRAKEBREATH_2               = -1575021,
    SAY_DRAKEBREATH_3               = -1575022,
    SAY_DRAKE_HARPOON_1             = -1575023,
    SAY_DRAKE_HARPOON_2             = -1575024,
    SAY_KILL_1                      = -1575025,
    SAY_KILL_2                      = -1575026,
    SAY_KILL_3                      = -1575027,
    SAY_DEATH                       = -1575028,
    SAY_DRAKE_DEATH                 = -1575029,
    EMOTE_HARPOON_RANGE             = -1575030,

    SPELL_CRUSH                     = 50234,
    SPELL_CRUSH_H                   = 59330,

    SPELL_WHIRLWIND                 = 50228,
    SPELL_WHIRLWIND_H               = 59322,

    SPELL_POISONED_SPEAR            = 50255,
    SPELL_POISONED_SPEAR_PERIODIC   = 50258,
    SPELL_POISONED_SPEAR_H          = 59331,
    SPELL_POISONED_SPEAR_PERIODIC_H = 59334,

    // casted with base of creature 22515 (World Trigger), so we must make sure
    // to use the close one by the door leading further in to instance.
    SPELL_SUMMON_GAUNTLET_MOBS      = 48630,   // tick every 30 sec
    SPELL_SUMMON_GAUNTLET_MOBS_H    = 59275,   // tick every 25 sec

    SPELL_GAUNTLET_PERIODIC         = 47546,   // what is this? Unknown use/effect, but probably related

    //56790?
    SPELL_LAUNCH_HARPOON            = 51368,   //48642 - dont shoot, into grauf strange... // this spell hit drake to reduce HP (force triggered from 48641)
    SPELL_GRAUF_BREATH_L            = 47590,   //Yesssss... I've found it.... :P
    SPELL_GRAUF_BREATH_R            = 47592,   //Burn Right triggers...
    SPELL_GRAUF_BREATH_L_H          = 47563,
    SPELL_GRAUF_BREATH_R_H          = 47593,
    SPELL_TRIGGER_BREATH_FIRE_N     = 47579,
    SPELL_TRIGGER_BREATH_FIRE_H     = 60020,
    SPELL_BREATH_UNK                = 47594,    //What is it?

    SPELL_SUMMON_HARPOON            = 56789,
    
    MODEL_ID_INVISIBLE              = 11686
};

#define SKADI_X         343.02f
#define SKADI_Y         -507.33f
#define SKADI_Z         104.57f
#define SKADI_O         2.97f

#define SUMMON_X        474.46f
#define SUMMON_Y        -483.8f
#define SUMMON_Z        104.74f
#define SUMMON_O        4.76f

/*######
## boss_skadi
######*/

struct MANGOS_DLL_DECL boss_skadiAI : public ScriptedAI
{
    boss_skadiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsBreathAttack;
    bool m_bIsSkadiMove;
    bool m_bIsFirstPhase;
    bool m_bSummonHarpooner;
    bool m_bCanLaunchHarpoon;

    uint32 m_uiIntroCount;
    uint32 m_uiIntroTimer;
    uint32 m_uiMoveTimer;
    uint32 m_uiStopTimer;
    uint32 m_uiWpCount;
    uint32 m_uiSummonAddsTimer;

    uint32 m_uiCrushTimer;
    uint32 m_uiPoisonedSpearTimer;
    uint32 m_uiWhirlwindTimer;

    uint8 m_uiFireStack;
    uint8 m_uiBreathSide;

    void Reset()
    {
        m_bIsBreathAttack = false;
        m_bIsSkadiMove = false;
        m_bIsFirstPhase = true;
        m_bSummonHarpooner = true;
        m_bCanLaunchHarpoon = false;
    
        m_uiIntroCount = 0;
        m_uiIntroTimer = 2000;
        m_uiMoveTimer = 0;
        m_uiWpCount = 0;
        m_uiStopTimer = 0;
        m_uiSummonAddsTimer = 10000;

        m_uiCrushTimer = 8000;
        m_uiPoisonedSpearTimer = 10000;
        m_uiWhirlwindTimer = 17000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);

        m_creature->ExitVehicle();
//        m_creature->NearTeleportTo(SKADI_X, SKADI_Y, SKADI_Z, SKADI_O);
        if (m_pInstance)
        {
            if (Creature* pGrauf = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
            {
                pGrauf->GetMotionMaster()->MoveTargetedHome();
                pGrauf->RemoveSplineFlag(SPLINEFLAG_FLYING);
                pGrauf->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
            }
            m_pInstance->SetData(TYPE_SKADI, NOT_STARTED);
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, IN_PROGRESS);
    }
    
    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, DONE);
    }

    void JustSummoned(Creature* pCreature)
    {
        uint32 uiEntry = pCreature->GetEntry();
        if (uiEntry == NPC_YMIRJAR_WARRIOR || uiEntry == NPC_YMIRJAR_WITCH_DOCTOR || uiEntry == NPC_YMIRJAR_HARPOONER)
        {
            pCreature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            pCreature->GetMotionMaster()->MovePoint(0, SUMMON_X+urand(0, 6), SUMMON_Y-urand(25, 30), SUMMON_Z);
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
            pSummoned->SetInCombatWithZone();
    }

    void SummonedCreatureJustDied(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_YMIRJAR_HARPOONER)
            if (Creature* pDummyCaster = pCreature->SummonCreature(1921, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 2000))
            {
                pDummyCaster->SetDisplayId(MODEL_ID_INVISIBLE);
                pDummyCaster->setFaction(35);
                pDummyCaster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SUMMON_HARPOON);
                if (pTempSpell)
                {
                    pTempSpell->Effect[EFFECT_INDEX_1] = 0;
                    pTempSpell->Effect[EFFECT_INDEX_2] = 0;
                    pDummyCaster->CastSpell(pDummyCaster, pTempSpell, true);
                }
            }
    }

    void MoveGrauf(float fX, float fY, float fZ, uint32 uiTime)
    {
        if (m_pInstance)
        {
            if (Creature* pGrauf = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
            {
                m_uiMoveTimer = uiTime;
                pGrauf->GetMap()->CreatureRelocation(pGrauf, fX, fY, fZ, pGrauf->GetAngle(fX, fY));
                pGrauf->SendMonsterMove(fX, fY, fZ, SPLINETYPE_NORMAL, pGrauf->GetSplineFlags(), uiTime);
                m_creature->Relocate(fX, fY, fZ, m_creature->GetAngle(pGrauf));
            }
        }
    }

    void NextWp()
    {
        switch (m_uiWpCount)
        {
            case 0: 
                MoveGrauf(SKADI_X, SKADI_Y-10.0f, 137.451f, 3000); 
                break;
            case 1:
                MoveGrauf(SKADI_X, SKADI_Y-45.0f, 137.451f, 3000);
                break;
            case 2:
                MoveGrauf(484.186f, -553.961f, 114.544f, 15000);
                break;
            case 3:
                MoveGrauf(521.438f, -542.188f, 120.972f, 3000);
                break;
            case 4:
                MoveGrauf(510.603f, -530.988f, 120.586f, 3000);
                DoScriptText(EMOTE_HARPOON_RANGE, m_creature);
                m_uiMoveTimer = 10000;
                m_bCanLaunchHarpoon = true;
                break;
            case 5:
                MoveGrauf(479.678f, -513.855f, 116.717f, 2000);
                m_bCanLaunchHarpoon = false;
                m_creature->MonsterTextEmote("Grauf takes a deep breath.", 0, true);
                break;
            case 6:
                switch (urand(0, 2))
                {
                    case 0: DoScriptText(SAY_DRAKEBREATH_1, m_creature); break;
                    case 1: DoScriptText(SAY_DRAKEBREATH_2, m_creature); break;
                    case 2: DoScriptText(SAY_DRAKEBREATH_3, m_creature); break;
                }
                if (m_pInstance)
                    if (Creature* pGrauf = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
                        pGrauf->CastSpell(pGrauf, SPELL_GRAUF_BREATH_R, false); 
                m_uiBreathSide = urand(1, 2);
                m_bIsSkadiMove = false;
                m_uiFireStack = 0;
                m_bIsBreathAttack = true;
                break;
        }

        ++m_uiWpCount;
        
        if (m_uiWpCount > 6)
           m_uiWpCount = 0;

    }

    void BurnTrigger(uint8 uiSide)
    {
        std::list<Creature*> lCreatures;
        GetCreatureListWithEntryInGrid(lCreatures, m_creature, NPC_FLAME_TRIGGER, 20.0f);

        if (lCreatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = lCreatures.begin(); iter != lCreatures.end(); ++iter)
        {
            if (!(*iter)->HasAura(SPELL_TRIGGER_BREATH_FIRE_N) && !(*iter)->HasAura(SPELL_TRIGGER_BREATH_FIRE_H))
                if (((*iter)->GetPositionY() > -512.0f && uiSide == 1) || ((*iter)->GetPositionY() < -512.0f && uiSide == 2))
                    (*iter)->CastSpell((*iter), m_bIsRegularMode ? SPELL_TRIGGER_BREATH_FIRE_N : SPELL_TRIGGER_BREATH_FIRE_H, false);
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bIsFirstPhase)
        {
            if (m_uiIntroCount < 2)
            {
                if (m_uiIntroTimer <= uiDiff)
                {
                    if (m_pInstance)
                        if (Creature* pGrauf = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
                        {
                            switch (m_uiIntroCount)
                            {
                                case 0:
                                    m_creature->EnterVehicle(pGrauf->GetVehicleKit(), 0);
                                    for (uint8 i = 0; i < 7; ++i)
                                        m_creature->SummonCreature(NPC_YMIRJAR_WARRIOR, SUMMON_X+urand(0, 6), SUMMON_Y, SUMMON_Z, SUMMON_O, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                    for (uint8 i = 0; i < 3; ++i)
                                        m_creature->SummonCreature(NPC_YMIRJAR_HARPOONER, SUMMON_X+urand(0, 6), SUMMON_Y, SUMMON_Z, SUMMON_O, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                    m_uiIntroTimer = 3000;
                                    break;
                                case 1:
                                    m_bIsSkadiMove = true;
                                    pGrauf->AddSplineFlag(SPLINEFLAG_FLYING);
                                    pGrauf->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
                                    break;
                            }
                        }
                    ++m_uiIntroCount;
                }
                else
                    m_uiIntroTimer -= uiDiff;
            }

            if (m_uiSummonAddsTimer <= uiDiff)
            {
                m_creature->SummonCreature(NPC_YMIRJAR_WARRIOR, SUMMON_X+urand(0, 6), SUMMON_Y, SUMMON_Z, SUMMON_O, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                m_creature->SummonCreature((m_bSummonHarpooner ? NPC_YMIRJAR_HARPOONER : NPC_YMIRJAR_WITCH_DOCTOR), SUMMON_X+urand(0, 6), SUMMON_Y, SUMMON_Z, SUMMON_O, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                m_bSummonHarpooner = !m_bSummonHarpooner;
                m_uiSummonAddsTimer = (m_bIsRegularMode ? 30000 : 25000);
            }
            else
                m_uiSummonAddsTimer -= uiDiff;

            if (m_bIsSkadiMove)
            {
                if (m_uiMoveTimer <= uiDiff)
                    NextWp();
                else
                    m_uiMoveTimer -= uiDiff;
            }

            if (m_bIsBreathAttack)
            {
                if (m_uiMoveTimer <= uiDiff)
                {
                    BurnTrigger(m_uiBreathSide);
                    if (m_pInstance)
                        if (Creature* pGrauf = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_GRAUF)))
                        {
                            MoveGrauf(pGrauf->GetPositionX()-10.0f, pGrauf->GetPositionY(), 113.717f, 700);
                            if (m_uiFireStack >= 12)
                            {
                                m_bIsBreathAttack = false;
                                m_bIsSkadiMove = true;
                                pGrauf->RemoveAllAuras();
                            }
                        }
                    ++m_uiFireStack;
                }
                else
                    m_uiMoveTimer -= uiDiff;
            }
        }
        else
        {
            if (m_uiCrushTimer <= uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CRUSH : SPELL_CRUSH_H);
                m_uiCrushTimer = 8000;
            }
            else
                m_uiCrushTimer -= uiDiff;

            if (m_uiPoisonedSpearTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_POISONED_SPEAR : SPELL_POISONED_SPEAR_H);
                    pTarget->CastSpell(pTarget, m_bIsRegularMode ? SPELL_POISONED_SPEAR_PERIODIC : SPELL_POISONED_SPEAR_PERIODIC_H, true);
                }
                m_uiPoisonedSpearTimer = 10000;
            }
            else
                m_uiPoisonedSpearTimer -= uiDiff;

            if (m_uiWhirlwindTimer <= uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_WHIRLWIND : SPELL_WHIRLWIND_H);
                m_uiWhirlwindTimer = 20000;
            }
            else
                m_uiWhirlwindTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

/*######
## boss_grauf
######*/

struct MANGOS_DLL_DECL boss_graufAI : public ScriptedAI
{
    boss_graufAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 uiSpeechPauseTimer;
    bool m_bIsSpeechPause;

    void Reset()
    {
        uiSpeechPauseTimer = 3000;
        m_bIsSpeechPause = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetActiveObjectState(true);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LAUNCH_HARPOON)
        {
            pCaster->DealDamage(m_creature, m_creature->GetMaxHealth()/4.9, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            if (m_pInstance)
                if (Creature* pSkadi = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SKADI)))
                {
                    if (!m_bIsSpeechPause && m_creature->isAlive())
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                DoScriptText(SAY_DRAKE_HARPOON_1, pSkadi);
                                break;
                            case 1:
                                DoScriptText(SAY_DRAKE_HARPOON_2, pSkadi);
                                break;
                        }
                        uiSpeechPauseTimer = 8000;
                        m_bIsSpeechPause = true;
                    }
                }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
        {
            if (Creature* pSkadi = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SKADI)))
            {
                pSkadi->ExitVehicle();
                DoScriptText(SAY_DRAKE_DEATH, pSkadi);
                ((boss_skadiAI*)pSkadi->AI())->m_bIsFirstPhase = false;
                pSkadi->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                ((boss_skadiAI*)pSkadi->AI())->SetCombatMovement(true);
                ((boss_skadiAI*)pSkadi->AI())->m_bCanLaunchHarpoon = false;
                if (Unit* pTarget = pSkadi->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    pSkadi->GetMotionMaster()->MovementExpired();
                    pSkadi->GetMotionMaster()->Clear();
                    pSkadi->GetMotionMaster()->MoveChase(pTarget);
                }
            }
        }
        m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-400.0f, 0);
        m_creature->SendMonsterMove(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-400.0f, SPLINETYPE_NORMAL , m_creature->GetSplineFlags(), 10000);
        m_creature->ForcedDespawn(10000);
    }

    void AttackStart(Unit* who)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsSpeechPause)
        {
            if (uiSpeechPauseTimer <= uiDiff)
                m_bIsSpeechPause = false;
            else
                uiSpeechPauseTimer -= uiDiff;
        }
    }
};

bool GOHello_skadi_harpoon(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer)
       if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_HARPOON, 1)) 
       {
           pPlayer->SendNewItem(pItem, 1, true, false);
           pGo->RemoveFromWorld();
       }

    return true;
}

bool GOHello_skadi_harpoon_launcher(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData())
        if (Creature* pSkadi = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SKADI)))
            if (Creature* pDummyCaster = pGo->SummonCreature(1921, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 3000))
            {
                pDummyCaster->SetDisplayId(MODEL_ID_INVISIBLE);
                pDummyCaster->setFaction(35);
                pDummyCaster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if (((boss_skadiAI*)pSkadi->AI())->m_bCanLaunchHarpoon)
                    pDummyCaster->CastSpell(pDummyCaster, SPELL_LAUNCH_HARPOON, true);
                else
                    if (Creature* pTempGrauf = pGo->SummonCreature(NPC_GRAUF, 510.603f, -530.988f, 120.586f, 0, TEMPSUMMON_TIMED_DESPAWN, 500))
                    {
                        pTempGrauf->SetDisplayId(MODEL_ID_INVISIBLE);
                        pDummyCaster->CastSpell(pTempGrauf, SPELL_LAUNCH_HARPOON, true);
                    }
            }
    return true;
}

CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI(pCreature);
}

CreatureAI* GetAI_boss_grauf(Creature* pCreature)
{
    return new boss_graufAI(pCreature);
}

void AddSC_boss_skadi()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_skadi";
    pNewScript->GetAI = &GetAI_boss_skadi;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_grauf";
    pNewScript->GetAI = &GetAI_boss_grauf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_skadi_harpoon";
    pNewScript->pGOHello=&GOHello_skadi_harpoon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_skadi_harpoon_launcher";
    pNewScript->pGOHello=&GOHello_skadi_harpoon_launcher;
    pNewScript->RegisterSelf();
}