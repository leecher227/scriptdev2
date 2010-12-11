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
SDName: Boss_Tharonja
SD%Complete: 99.9% :P
SDComment: Timers
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

enum
{
    SAY_AGGRO                       = -1600012,
    SAY_KILL_1                      = -1600013,
    SAY_KILL_2                      = -1600014,
    SAY_FLESH_1                     = -1600015,
    SAY_FLESH_2                     = -1600016,
    SAY_SKELETON_1                  = -1600017,
    SAY_SKELETON_2                  = -1600018,
    SAY_DEATH                       = -1600019,

    //Phase 1 (Skeleton) Spells
    SPELL_CURSE_OF_LIFE_N              = 49527,
    SPELL_CURSE_OF_LIFE_H              = 59972,
    SPELL_SHADOW_VOLLEY_N              = 49528,
    SPELL_SHADOW_VOLLEY_H              = 59973,
    SPELL_RAIN_OF_FIRE_N               = 49518,
    SPELL_RAIN_OF_FIRE_H               = 59971,
    
    SPELL_DECAY_FLESH                  = 49356,            //not working
    SPELL_FLESH_VISUAL                 = 52582,

    //Phase 2 (Flesh) Spells
    SPELL_GIFT_OF_THARONJA             = 52509,

    SPELL_LIGHTNING_BREATH_N           = 49537,
    SPELL_LIGHTNING_BREATH_H           = 59963,
    SPELL_EYE_BEAM_N                   = 49544,
    SPELL_EYE_BEAM_H                   = 59965,
    SPELL_POISON_CLOUD_N               = 49548,
    SPELL_POISON_CLOUD_H               = 59969,
    
    SPELL_RETURN_FLESH                 = 53463,                //not working

    NPC_DRAKURU                        = 28016,

    MODEL_ID_INVISIBLE                 = 11686,

    //achie hacks
    ACHIEVEMENT_NORMAL                 = 482,
    ACHIEVEMENT_HEROIC                 = 493
};

/*######
## boss_tharonja
######*/

struct MANGOS_DLL_DECL boss_tharonjaAI : public ScriptedAI
{
    boss_tharonjaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPhase;
    uint32 m_uiCurseOfLifeTimer;
    uint32 m_uiTransformTimer;
    uint32 m_uiRandomSpellsTimer;
    uint32 m_uiChangePhase;
    uint32 m_uiPoisonCloudTimer;
    uint32 m_uiFleshSpellsTimer;
    bool m_bTransform;

    void Reset()
    {
        m_creature->SetDisplayId(27072);
        m_uiPhase = 1;
        m_uiCurseOfLifeTimer = urand (5000, 10000);
        m_uiRandomSpellsTimer = urand(7000, 13000);
        m_uiPoisonCloudTimer = urand(2000, 5000);
        m_uiChangePhase = 25000;
        m_uiTransformTimer = 3000;
        m_bTransform = false;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THARONJA, DONE);
        m_creature->SummonCreature(NPC_DRAKURU, -236.739f, -618.786f, 116.474f, 4.68f, TEMPSUMMON_CORPSE_TIMED_DESPAWN,500); //hack
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiPhase)
        {
            case 1:
            {
                if (m_bTransform)
                {
                    if (m_uiTransformTimer < uiDiff)
                    {
                        m_creature->SetDisplayId(27073);
                        m_creature->CastSpell(m_creature, SPELL_GIFT_OF_THARONJA, true);
                        m_creature->CastSpell(m_creature, SPELL_FLESH_VISUAL, false);
                        m_bTransform = false;
                        m_uiPhase = 2;
                    }
                    else
                        m_uiTransformTimer -= uiDiff;

                    return;
                }

                if (m_uiChangePhase < uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoScriptText(urand(0, 1) ? SAY_FLESH_1 : SAY_FLESH_2, m_creature);
                    m_creature->CastSpell(m_creature, SPELL_DECAY_FLESH, false);
                    m_uiChangePhase = 25000;
                    m_bTransform = true;
                    m_uiTransformTimer = 3000;
                    return;
                }
                else
                    m_uiChangePhase -= uiDiff;
            
                if (m_uiCurseOfLifeTimer < uiDiff)
                {
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CURSE_OF_LIFE_N : SPELL_CURSE_OF_LIFE_H);
                        m_uiCurseOfLifeTimer = urand (5000, 10000);
                    }
                }
                else
                    m_uiCurseOfLifeTimer -= uiDiff;

                if (m_uiRandomSpellsTimer < uiDiff)
                {     
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        switch(urand(0, 2))
                        {
                            case 0: 
                            case 1: 
                                DoCast(m_creature->getVictim(),  m_bIsRegularMode ? SPELL_SHADOW_VOLLEY_N : SPELL_SHADOW_VOLLEY_H);
                                break;
                            case 2: 
                                DoCast(m_creature->getVictim(),  m_bIsRegularMode ? SPELL_RAIN_OF_FIRE_N : SPELL_RAIN_OF_FIRE_H);
                                break;
                        }
                        m_uiRandomSpellsTimer = urand (5000, 10000);
                    }
                }
                else
                    m_uiRandomSpellsTimer -= uiDiff;

                break;
            }
            case 2:
            {
                if (m_bTransform)
                {
                    if (m_uiTransformTimer < uiDiff)
                    {
                        m_creature->SetDisplayId(27072);
                        m_creature->CastSpell(m_creature, SPELL_FLESH_VISUAL, false);
                        m_bTransform = false;
                        m_uiPhase = 1;
                    }
                    else
                        m_uiTransformTimer -= uiDiff;

                    return;
                }

                if (m_uiChangePhase < uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoScriptText(urand(0, 1) ? SAY_SKELETON_1 : SAY_SKELETON_2, m_creature);
                    m_creature->CastSpell(m_creature, SPELL_RETURN_FLESH, false);
                    m_uiChangePhase = 28000;
                    m_bTransform = true;
                    m_uiTransformTimer = 3000;
                    return;
                }
                else
                    m_uiChangePhase -= uiDiff;
            
                if (m_uiPoisonCloudTimer < uiDiff)
                {
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget,  m_bIsRegularMode ? SPELL_POISON_CLOUD_N : SPELL_POISON_CLOUD_H);
                        m_uiPoisonCloudTimer = 10000;
                    }
                }
                else
                    m_uiPoisonCloudTimer -= uiDiff;

                if (m_uiRandomSpellsTimer < uiDiff)
                {     
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        switch (urand(0, 1))
                        {
                            case 0:
                                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    DoCast(pTarget,  m_bIsRegularMode ? SPELL_LIGHTNING_BREATH_N : SPELL_LIGHTNING_BREATH_H);
                                break;
                            case 1: 
                                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    DoCast(pTarget,  m_bIsRegularMode ? SPELL_EYE_BEAM_N : SPELL_EYE_BEAM_H);
                                break;
                        }
                        m_uiRandomSpellsTimer = 4500;
                    }
                }
                else
                    m_uiRandomSpellsTimer -= uiDiff;

                break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

/*######
## npc_drakuru_dt
######*/

enum 
{
   //SAYS
   SAY_DRAKURU_01                    = -1568270,
   SAY_DRAKURU_02                    = -1568271,
   SAY_DRAKURU_03                    = -1568272,
   SAY_DRAKURU_04                    = -1568273,
   SAY_DRAKURU_05                    = -1568274,
   SAY_DRAKURU_06                    = -1568275,
   SAY_DRAKURU_07                    = -1568276,
   SAY_DRAKURU_08                    = -1568277,
   SAY_LICHKING_09                   = -1568278,
   SAY_LICHKING_10                   = -1568279,
   SAY_LICHKING_11                   = -1568280,
   SAY_LICHKING_12                   = -1568281,
   SAY_LICHKING_13                   = -1568282,
   SAY_DRAKURU_14                    = -1568283,
   SAY_LICHKING_15                   = -1568284,
   SAY_LICHKING_16                   = -1568285,
   SAY_DRAKURU_17                    = -1568286,
   SAY_DRAKURU_18                    = -1568287,
   SAY_DRAKURU_19                    = -1568288,
   SAY_DRAKURU_20                    = -1568289,
   SAY_DRAKURU_21                    = -1568290,

   //NPCS
   NPC_INVISMAN                      = 28492,  //Portal and lightning effect
   NPC_OVERLORD_DRAKURU              = 28503,
   NPC_LICH_KING                     = 28498,

   //SPELLS
   SPELL_DRAKURU_TRANSFORM           = 51834,
   SPELL_ARTHAS_GIFT                 = 51825,  //need spell script on 28016
   SPELL_ARTHAS_PORTAL               = 51807,  //cast this npc 28492
   SPELL_DRAKURU_RED_LIGHTNING       = 51802,  //cast this npc 28492
   SPELL_SUMMON_DRAKURU              = 50446,  //summon this npc 28016, Casted by flame brazier...
   SPELL_SUMMON_PORTAL               = 58071,  //not right but on off use this effect
   //SPELL_SUMMON_RUNE_01              = ?????,  //Hmmmmm.... Drakuru summoned three random rune around center on kneel...
   //SPELL_SUMMON_RUNE_02              = ?????,  //Hmmmmm....
   //SPELL_SUMMON_RUNE_03              = ?????,  //Hmmmmm....
};

//-240.753f, -629.795f, 116.497f cheer point 4 sec stand
//-237.562f, -653.363f, 131.169f
//-259.288f, -668.062f, 131.186f 
//-265.817f, -668.179f, 131.177f pause 9 sec pause and move
//-242.563f, -673.616f, 131.841f install eye of prophet 4 sec
//-237.919f, -679.445f, 131.854f install heart of ascient 4 sec 
//-234.348f, -675.218f, 131.854f install rune 4 sec
//-237.063f, -672.480f, 131.850f
//-237.076f, -676.245f, 131.866f start ritual Revelation
//-237.215f, -698.836f, 131.170f summon portal
//-237.304f, -686.504f, 132.174f lich king move

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

struct Locations DrakuruPoint[]=
{
   {0, 0, 0},
   {-240.753f, -629.795f, 116.497f}, //1
   {-237.562f, -653.363f, 131.169f}, //2
   {-259.288f, -668.062f, 131.186f}, //3
   {-265.817f, -668.179f, 131.177f}, //4
   {-242.563f, -673.616f, 131.841f}, //5
   {-237.919f, -679.445f, 131.854f}, //6
   {-234.348f, -675.218f, 131.854f}, //7
   {-237.063f, -672.480f, 131.850f}, //8
   {-237.076f, -676.245f, 131.866f}, //9
};

struct MANGOS_DLL_DECL npc_drakuru_dtAI : public ScriptedAI
{
    npc_drakuru_dtAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
  
    uint64 m_uiLichKingGUID;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPoint; 
    uint32 m_uiPauseTimer;

    bool m_bIsTalk;
    bool m_bIsPause;

    void Reset()
    {
        if (m_pInstance->GetData(TYPE_THARONJA) != DONE)
            m_creature->ForcedDespawn();
        m_uiStep = 0;
        m_uiLichKingGUID = 0;
        m_uiStepTimer = 100;
        m_uiPoint = 1;
        m_bIsPause = true;
        m_uiPauseTimer = 2000;
        m_bIsTalk = false;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }    

    void NextPoint(uint32 Timer)
    {
        m_uiPoint++;
        m_bIsPause = true;
        m_uiPauseTimer = Timer;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case 1:
                DoScriptText(SAY_DRAKURU_01, m_creature);
                NextPoint(4000);
                break;
            case 2:
                NextPoint(100);
                break;
            case 3:
                NextPoint(100);
                break;
            case 4:
                m_creature->HandleEmoteCommand(25);
                NextPoint(9000);
                break;
            case 5:
                DoScriptText(SAY_DRAKURU_03, m_creature);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                if (m_pInstance)
                {
                    if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_EYE_OF_PROPHET)))
                        pGo->SetPhaseMask(1, true);
                }
                NextPoint(4000);
                break;
            case 6:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                if (m_pInstance)
                {
                    if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_HEART_OF_ASCIENT)))
                        pGo->SetPhaseMask(1, true);
                }
                NextPoint(4000);
                break;
            case 7:
                DoScriptText(SAY_DRAKURU_04, m_creature);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                if (m_pInstance)
                {
                    if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_RUNE_OF_DRAKKARI)))
                    pGo->SetPhaseMask(1, true);
                }
                NextPoint(4000);
                break;
            case 8:
                NextPoint(100);
                break;
            case 9:
                DoScriptText(SAY_DRAKURU_05, m_creature);
                m_bIsTalk = true;
                m_uiStepTimer = 2000;
                m_uiStep = 1;
                break;
        }
    }

    void JumpNextStep(uint32 Timer)
    {
        m_uiStepTimer = Timer;
        m_uiStep++;
    }

    void Event()
    {
        if (!m_pInstance)
        {
            JumpNextStep(2000);
            return;
        }

        switch (m_uiStep)
        { 
            case 1:
                DoCast(m_creature, SPELL_SUMMON_PORTAL);
                JumpNextStep(5000);
                break;
            case 2:
                RedLightning();
                JumpNextStep(3000);
                break;
            case 3:
                if (Creature* pPortal = m_creature->SummonCreature(NPC_INVISMAN,-237.215f, -698.836f, 131.170f,1.55f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,84500))
                {
                    pPortal->SetDisplayId(MODEL_ID_INVISIBLE);
                    pPortal->CastSpell(pPortal, SPELL_ARTHAS_PORTAL, false);
                }
                JumpNextStep(2000);
                break;
            case 4:
                DoScriptText(SAY_DRAKURU_06, m_creature);
                m_creature->InterruptNonMeleeSpells(false);
                JumpNextStep(5000);
                break;
            case 5:
                if (Creature* pLich = m_creature->SummonCreature(NPC_LICH_KING,-237.215f, -698.836f, 131.170f,1.55f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,73500))
                {
                    m_uiLichKingGUID = pLich->GetGUID();
                    pLich->GetMotionMaster()->MovePoint(0, -237.304f, -686.504f, 132.174f);
                }
                JumpNextStep(5000);
                break;
            case 6:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                JumpNextStep(3000);
                break;
            case 7:
                DoScriptText(SAY_DRAKURU_07, m_creature);
                JumpNextStep(5000);
                break;
            case 8:
                DoScriptText(SAY_DRAKURU_08, m_creature);
                JumpNextStep(6000);
                break;
            case 9:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICHKING_09, pLich);
                JumpNextStep(5000);
                break;
            case 10:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICHKING_10, pLich);
                JumpNextStep(9000);
                break;
            case 11:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICHKING_11, pLich);
                JumpNextStep(3000);
                break;
            case 12:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICHKING_12, pLich);
                JumpNextStep(2500);
                break;
            case 13:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    pLich->CastSpell(m_creature, SPELL_ARTHAS_GIFT, false);
                JumpNextStep(5000);
                break;
            case 14:
                m_creature->HandleEmoteCommand(15);
                JumpNextStep(4000);
                break;
            case 15:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                DoScriptText(SAY_LICHKING_13, pLich);
                JumpNextStep(7000);
                break;
            case 16:
                DoScriptText(SAY_DRAKURU_14, m_creature);
                JumpNextStep(3000);
                break;
            case 17:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICHKING_15, pLich);
                JumpNextStep(4000);
                break;
            case 18:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    DoScriptText(SAY_LICHKING_16, pLich);
                JumpNextStep(7000);
                break;
            case 19:
                if (Creature* pLich = m_pInstance->instance->GetCreature(m_uiLichKingGUID))
                    pLich->GetMotionMaster()->MovePoint(0, -237.215f, -698.836f, 131.170f);
                JumpNextStep(7000);
                break;
            case 20:
                DoScriptText(SAY_DRAKURU_17, m_creature);
                JumpNextStep(6000);
                break;
            case 21:
                DoScriptText(SAY_DRAKURU_18, m_creature);
                JumpNextStep(4000);
                break;
            case 22:
                DoScriptText(SAY_DRAKURU_19, m_creature);
                JumpNextStep(8000);
                break;
            case 23:
                DoScriptText(SAY_DRAKURU_20, m_creature);
                JumpNextStep(7000);
                break;
            case 24:
                DoScriptText(SAY_DRAKURU_21, m_creature);
                JumpNextStep(6000);
                break;
            case 25:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                JumpNextStep(600);
                break;
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_ARTHAS_GIFT)
            DoCast(m_creature, SPELL_DRAKURU_TRANSFORM);
    }

    void RedLightning()
    {
        if (m_pInstance)
        {
            if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_HEART_OF_ASCIENT)))
            {
                if(Creature* pTrigger = pGo->SummonCreature(NPC_INVISMAN,pGo->GetPositionX(),pGo->GetPositionY(),pGo->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,3000))
                {
                    pTrigger->SetDisplayId(MODEL_ID_INVISIBLE);
                    pTrigger->CastSpell(pTrigger, SPELL_DRAKURU_RED_LIGHTNING, false);
                }
                pGo->SetPhaseMask(0, true);
            }
            if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_RUNE_OF_DRAKKARI)))
            {
                if(Creature* pTrigger = pGo->SummonCreature(NPC_INVISMAN,pGo->GetPositionX(),pGo->GetPositionY(),pGo->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,3000))
                {
                    pTrigger->SetDisplayId(MODEL_ID_INVISIBLE);
                    pTrigger->CastSpell(pTrigger, SPELL_DRAKURU_RED_LIGHTNING, false);
                }
                pGo->SetPhaseMask(0, true);
            }
            if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_EYE_OF_PROPHET)))
            {
                if(Creature* pTrigger = pGo->SummonCreature(NPC_INVISMAN,pGo->GetPositionX(),pGo->GetPositionY(),pGo->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,3000))
                {
                    pTrigger->SetDisplayId(MODEL_ID_INVISIBLE);
                    pTrigger->CastSpell(pTrigger, SPELL_DRAKURU_RED_LIGHTNING, false);
                }
                pGo->SetPhaseMask(0, true);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_THARONJA) == DONE)
        {
            if (m_bIsTalk)
            {
                if (m_uiStepTimer <= uiDiff)
                    Event();
                else
                    m_uiStepTimer -= uiDiff;
            }

            if (m_bIsPause && !m_bIsTalk)
            {
                if (m_uiPauseTimer <= uiDiff)
                {
                    m_bIsPause = false;
                    switch (m_uiPoint)
                    {
                        case 5:
                            DoScriptText(SAY_DRAKURU_02, m_creature);
                            break;
                        case 6:
                        case 7:
                        case 8:
                            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                            break;
                    }
                    m_creature->GetMotionMaster()->MovePoint(m_uiPoint, DrakuruPoint[m_uiPoint].x, DrakuruPoint[m_uiPoint].y, DrakuruPoint[m_uiPoint].z);
                }
                else
                    m_uiPauseTimer -= uiDiff;
            }
        }
    }
};

CreatureAI* GetAI_boss_tharonja(Creature* pCreature)
{
    return new boss_tharonjaAI(pCreature);
}

CreatureAI* GetAI_npc_drakuru_dt(Creature* pCreature)
{
    return new npc_drakuru_dtAI(pCreature);
}

void AddSC_boss_tharonja()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_tharonja";
    pNewScript->GetAI = &GetAI_boss_tharonja;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_drakuru_dt";
    pNewScript->GetAI = &GetAI_npc_drakuru_dt;
    pNewScript->RegisterSelf();
}