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
SDName: Boss_Svala
SD%Complete: 95%
SDComment:
SDComment:
SDAuthor: MaxXx2021 Aka Mioka.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_INTRO_1                 = -1575000,
    SAY_INTRO_2_ARTHAS          = -1575001,
    SAY_INTRO_3                 = -1575002,
    SAY_INTRO_4_ARTHAS          = -1575003,
    SAY_INTRO_5                 = -1575004,

    SAY_AGGRO                   = -1575005,
    SAY_SLAY_1                  = -1575006,
    SAY_SLAY_2                  = -1575007,
    SAY_SLAY_3                  = -1575008,
    SAY_SACRIFICE_1             = -1575009,
    SAY_SACRIFICE_2             = -1575010,
    SAY_SACRIFICE_3             = -1575011,
    SAY_SACRIFICE_4             = -1575012,
    SAY_SACRIFICE_5             = -1575013,
    SAY_DEATH                   = -1575014,

    NPC_SVALA_SORROW            = 26668,
    NPC_ARTHAS_IMAGE            = 29280,
    NPC_RITUAL_CHANNELER        = 27281,
    NPC_RITUAL_TARGET           = 27327,
    NPC_FLAME_BRAZIER           = 27273,

    SPELL_ARTHAS_VISUAL         = 54134,

    // don't know how these should work in relation to each other
    SPELL_TRANSFORMING          = 54205,
    SPELL_TRANSFORMING_FLOATING = 54140,
    SPELL_TRANSFORMING_CHANNEL  = 54142,

    SPELL_RITUAL_OF_SWORD       = 48276,
    SPELL_CALL_FLAMES           = 48258,
    SPELL_SINISTER_STRIKE       = 15667,
    SPELL_SINISTER_STRIKE_H     = 59409,
    SPELL_BALL_OF_FLAME         = 48246,

    //Other's
    SPELL_RITUAL_OF_SWORD_RS    = 54159, //RS - Remove Boss Equipment
    SPELL_RITUAL_OF_SWORD_UNK   = 54148, //Unknown Effect.
    SPELL_RITUAL_STRIKE         = 48277, //When Sword on ground, svala cast this spell on invisible trigger. Need Spell Script Target.
    SPELL_SWIRL_SWORD           = 48331, //Visual Effect, need script target 27327

    //Summons
    SPELL_PARALYZE              = 48278,
    SPELL_SHADOWS_IN_THE_DARK   = 59407,

    MODEL_ID_INVISIBLE          = 11686,

    NORMAL                      = 1,
    SACRED                      = 2
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations RitualChannelerPos[]=
{
    {296.42f, -355.01f, 90.94f},
    {302.36f, -352.01f, 90.54f},
    {291.39f, -350.89f, 90.54f}
};

/*######
## npc_ritual_channeler
######*/

struct MANGOS_DLL_DECL npc_ritual_channelerAI : public Scripted_NoMovementAI
{
    npc_ritual_channelerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    Unit* m_pTarget;
    bool m_bCasted;

    void Reset()
    {
        m_bCasted = false;
        if (!m_bIsRegularMode)
            DoCast(m_creature, SPELL_SHADOWS_IN_THE_DARK);
    }

    void SetVictim(Unit* pVictim)
    {
         if (pVictim)
            m_pTarget = pVictim;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pTarget && (!m_bCasted || !m_pTarget->HasAura(SPELL_PARALYZE)))
        {
            SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_PARALYZE);
            if (pTempSpell)
            {
                pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                pTempSpell->EffectImplicitTargetB[0] = 0;
                pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
                pTempSpell->EffectImplicitTargetB[1] = 0;
                pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
                pTempSpell->EffectImplicitTargetB[2] = 0;
                m_creature->CastSpell(m_pTarget, pTempSpell, true);
            }
            m_bCasted = true;
        }
    }
};

/*######
## boss_svala
######*/

struct MANGOS_DLL_DECL boss_svalaAI : public ScriptedAI
{
    boss_svalaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bIsIntroDone = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Creature* pArthas;

    bool m_bIsIntroDone;
    uint32 m_uiIntroTimer;
    uint32 m_uiIntroCount;
    uint32 m_uiSinsterStrikeTimer;
    uint32 m_uiCallFlamesTimer;
    uint32 m_uiRitualOfSwordTimer;
    uint32 m_uiSacrificeTimer;
    uint32 m_uiRitualStage;

    uint8 m_uiPhase;

    Unit* pVictim;

    void Reset()
    {
        pArthas = NULL;

        m_uiIntroTimer = 2500;
        m_uiIntroCount = 0;
        m_uiRitualStage = 1;
        m_uiSinsterStrikeTimer = 4000;
        m_uiCallFlamesTimer = 10000;
        m_uiRitualOfSwordTimer = 20000;
        m_uiSacrificeTimer = 25000;

        m_uiPhase = NORMAL;

        m_creature->NearTeleportTo(296.632f, -346.075f, 90.630f, 0);
        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
        m_creature->clearUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);

        pVictim = NULL;
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(37146));
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_creature->isAlive() && m_pInstance && m_pInstance->GetData(TYPE_SVALA) > IN_PROGRESS)
        {
            if (m_creature->GetEntry() != NPC_SVALA_SORROW)
                m_creature->UpdateEntry(NPC_SVALA_SORROW);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_bIsIntroDone = true;
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bIsIntroDone && pWho && pWho->GetTypeId() == TYPEID_PLAYER && !((Player*)pWho)->isGameMaster())
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_SVALA) == IN_PROGRESS)
            {
                m_pInstance->SetData(TYPE_SVALA, SPECIAL);
                m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(37146));

                float fX, fY, fZ;
                m_creature->GetClosePoint(fX, fY, fZ, m_creature->GetObjectBoundingRadius(), 16.0f, 0.0f);

                // we assume m_creature is spawned in proper location
                m_creature->SummonCreature(NPC_ARTHAS_IMAGE, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 50000);
            }

            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_uiPhase != NORMAL) return;
 
        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ARTHAS_IMAGE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ARTHAS_VISUAL, true);
            pArthas = pSummoned;
            pSummoned->SetFacingToObject(m_creature);
        }
    }

    void SummonedCreatureDespawn(Creature* pDespawned)
    {
        if (pDespawned->GetEntry() == NPC_ARTHAS_IMAGE)
            pArthas = NULL;
    }

    void DoMoveToPosition()
    {
        float fX, fZ, fY;
        m_creature->GetRespawnCoord(fX, fY, fZ);

        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);

        m_creature->SendMonsterMoveWithSpeed(fX, fY, fZ + 5.0f, m_uiIntroTimer);
        m_creature->GetMap()->CreatureRelocation(m_creature, fX, fY, fZ + 5.0f, m_creature->GetOrientation());
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_SWIRL_SWORD)
            if(target->GetTypeId() != TYPEID_PLAYER && target->GetEntry() == NPC_RITUAL_TARGET)
                m_creature->CastSpell(target, SPELL_RITUAL_STRIKE, true);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_TRANSFORMING)
        {
            if (pArthas)
                pArthas->InterruptNonMeleeSpells(true);

            m_creature->UpdateEntry(NPC_SVALA_SORROW);
            m_creature->RemoveAurasDueToSpell(SPELL_TRANSFORMING_FLOATING);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(37146));
        }
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

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SVALA, DONE);
    }

    void SacredText(uint8 SoundId)
    {
        switch(SoundId)
        {
           case 1:
              DoScriptText(SAY_SACRIFICE_1, m_creature);
              break;
           case 2:
              DoScriptText(SAY_SACRIFICE_2, m_creature);
              break;
           case 3:
              DoScriptText(SAY_SACRIFICE_3, m_creature);
              break;
           case 4:
              DoScriptText(SAY_SACRIFICE_4, m_creature);
              break;
           case 5:
              DoScriptText(SAY_SACRIFICE_5, m_creature);
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_bIsIntroDone || (!pArthas && m_uiIntroCount < 5))
                return;

            if (m_uiIntroTimer < uiDiff)
            {
                switch (m_uiIntroCount)
                {
                    case 0:
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                        DoScriptText(SAY_INTRO_1, m_creature);
                        m_uiIntroTimer = 9000;
                        break;
                    case 1:
                        DoScriptText(SAY_INTRO_2_ARTHAS, pArthas);
                        m_uiIntroTimer = 12000;
                        break;
                    case 2:
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pArthas->CastSpell(m_creature, SPELL_TRANSFORMING_CHANNEL, false);
                        m_creature->CastSpell(m_creature, SPELL_TRANSFORMING_FLOATING, false);
                        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
                        m_uiIntroTimer = 6000;
                        DoMoveToPosition();
                        break;
                    case 3:
                        m_creature->CastSpell(m_creature, SPELL_TRANSFORMING, false);
                        DoScriptText(SAY_INTRO_3, m_creature);
                        m_uiIntroTimer = 12000;
                        break;
                    case 4:
                        DoScriptText(SAY_INTRO_4_ARTHAS, pArthas);
                        m_uiIntroTimer = 7000;
                        break;
                    case 5:
                        DoScriptText(SAY_INTRO_5, m_creature);
                        m_uiIntroTimer = 13000;
                        break;
                    case 6:
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_bIsIntroDone = true;
                        break;
                }
                ++m_uiIntroCount;
            }
            else
                m_uiIntroTimer -= uiDiff;

            return;
        }
        else
        {
            if (m_uiPhase == NORMAL)
            {
                if (m_uiSinsterStrikeTimer <= uiDiff)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SINISTER_STRIKE : SPELL_SINISTER_STRIKE_H);
                    m_uiSinsterStrikeTimer = urand(5000, 9000);
                }
                else
                    m_uiSinsterStrikeTimer -= uiDiff;

                if (m_uiCallFlamesTimer <= uiDiff)
                {
                    DoCast(m_creature, SPELL_CALL_FLAMES);
                    int32 dmg = urand(1000, 1500);
                    if (!m_bIsRegularMode)
                        dmg += 1500;
                    Creature* pBrazier = NULL;
                    if (pBrazier = m_creature->SummonCreature(NPC_FLAME_BRAZIER, 285.64f, -355.16f, 92.99f, 1.65f, TEMPSUMMON_TIMED_DESPAWN, 5000))
                    {
                        pBrazier->SetDisplayId(MODEL_ID_INVISIBLE);
                        pBrazier->setFaction(14);
                        pBrazier->CastCustomSpell(pBrazier, SPELL_BALL_OF_FLAME, &dmg, 0, 0, true);
                    }
                    if (pBrazier = m_creature->SummonCreature(NPC_FLAME_BRAZIER, 306.94f, -355.77f, 92.96f, 1.65f, TEMPSUMMON_TIMED_DESPAWN, 5000))
                    {
                        pBrazier->SetDisplayId(MODEL_ID_INVISIBLE);
                        pBrazier->setFaction(14);
                        pBrazier->CastCustomSpell(pBrazier, SPELL_BALL_OF_FLAME, &dmg, 0, 0, true);
                    }
                    m_uiCallFlamesTimer = urand(8000,12000);
                }
                else
                    m_uiCallFlamesTimer -= uiDiff;

                if (m_uiRitualOfSwordTimer <= uiDiff)
                { 
                    if (Unit* pSacrificeTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        m_creature->InterruptNonMeleeSpells(false);
                        SacredText(urand(1, 5));
                        m_creature->CastSpell(pSacrificeTarget, SPELL_RITUAL_OF_SWORD, true); //spell ports caster but not target
                        pVictim = pSacrificeTarget;
                        m_uiSacrificeTimer = 750;
                        m_uiPhase = SACRED;
                        m_uiRitualStage = 1;
                    }
                }
                else
                    m_uiRitualOfSwordTimer -= uiDiff;

                DoMeleeAttackIfReady();
            }
            else  //SACRIFICING
            {
                if (m_uiSacrificeTimer <= uiDiff)
                {
                    switch(m_uiRitualStage)
                    {
                        case 1:
                            m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
                            m_creature->NearTeleportTo(296.632f, -346.075f, 90.63f + 17.0f, 0);
                            if (pVictim)
                            {
                                DoTeleportPlayer(pVictim, 296.632f, -346.075f, 90.63f, 4.6f);
                                for (uint8 i = 0; i < 3; ++i)
                                    if (Creature* pSummon = m_creature->SummonCreature(NPC_RITUAL_CHANNELER, RitualChannelerPos[i].x,RitualChannelerPos[i].y,RitualChannelerPos[i].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 27000))
                                        ((npc_ritual_channelerAI*)pSummon->AI())->SetVictim(pVictim);
                                m_creature->SetFacingToObject(pVictim);
                            }
                            m_creature->addUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
                            m_uiSacrificeTimer = 100;
                            ++m_uiRitualStage;
                            break;
                        case 2:
                            m_creature->NearTeleportTo(296.632f, -346.075f, 90.63f + 17.0f, 0);
                            m_uiSacrificeTimer = 100;
                            ++m_uiRitualStage;
                            break;
                        case 3:
                            if (Creature* pRitualTarget = m_creature->SummonCreature(NPC_RITUAL_TARGET, 296.632f, -346.075f, 90.63f, 1.568f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            {
                                pRitualTarget->SetDisplayId(MODEL_ID_INVISIBLE);
                                m_creature->CastSpell(pRitualTarget, SPELL_SWIRL_SWORD, true);
                            }
                            m_uiSacrificeTimer = 100;
                            ++m_uiRitualStage;
                            break;
                        case 4:
                            m_creature->CastSpell(m_creature, SPELL_RITUAL_OF_SWORD_RS, true);
                            m_uiSacrificeTimer = 26000;
                            ++m_uiRitualStage;
                            break;
                        case 5:
                            m_uiPhase = NORMAL;
                            pVictim = NULL;
                            m_creature->clearUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT);
                            m_creature->InterruptNonMeleeSpells(false);
                            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                            m_uiRitualOfSwordTimer = 20000;
                            m_uiRitualStage = 0;
                            break;
                    }
                }
                else
                    m_uiSacrificeTimer -= uiDiff;
            }
        }
    }
};

CreatureAI* GetAI_boss_svala(Creature* pCreature)
{
    return new boss_svalaAI(pCreature);
}

CreatureAI* GetAI_npc_ritual_channeler(Creature* pCreature)
{
    return new npc_ritual_channelerAI(pCreature);
}

bool AreaTrigger_at_svala_intro(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SVALA) == NOT_STARTED)
            pInstance->SetData(TYPE_SVALA, IN_PROGRESS);
    }

    return false;
}

void AddSC_boss_svala()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_svala";
    newscript->GetAI = &GetAI_boss_svala;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ritual_channeler";
    newscript->GetAI = &GetAI_npc_ritual_channeler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_svala_intro";
    newscript->pAreaTrigger = &AreaTrigger_at_svala_intro;
    newscript->RegisterSelf();
}
