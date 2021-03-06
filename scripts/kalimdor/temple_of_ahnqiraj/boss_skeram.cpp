/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Skeram
SD%Complete: 75
SDComment: Mind Control buggy.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"
#include "Group.h"

#define SAY_AGGRO1                  -1531000
#define SAY_AGGRO2                  -1531001
#define SAY_AGGRO3                  -1531002
#define SAY_SLAY1                   -1531003
#define SAY_SLAY2                   -1531004
#define SAY_SLAY3                   -1531005
#define SAY_SPLIT                   -1531006
#define SAY_DEATH                   -1531007

#define SPELL_ARCANE_EXPLOSION      25679
#define SPELL_EARTH_SHOCK           26194
#define SPELL_TRUE_FULFILLMENT4     26526
#define SPELL_BLINK                 28391

class ov_mycoordinates
{
    public:
        float x,y,z,r;
        ov_mycoordinates(float cx, float cy, float cz, float cr)
        {
            x = cx; y = cy; z = cz; r = cr;
        }
};

struct MANGOS_DLL_DECL boss_skeramAI : public ScriptedAI
{
    boss_skeramAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        IsImage = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 ArcaneExplosion_Timer;
    uint32 EarthShock_Timer;
    uint32 FullFillment_Timer;
    uint32 Blink_Timer;
    uint32 Invisible_Timer;

    Creature *Image1, *Image2;

    bool Images75;
    bool Images50;
    bool Images25;
    bool IsImage;
    bool Invisible;

    void Reset()
    {
        ArcaneExplosion_Timer = urand(6000, 12000);
        EarthShock_Timer = 2000;
        FullFillment_Timer = 15000;
        Blink_Timer = urand(8000, 20000);
        Invisible_Timer = 500;

        Images75 = false;
        Images50 = false;
        Images25 = false;
        Invisible = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_ON);

        if (IsImage)
            m_creature->SetDeathState(JUST_DIED);
    }

    void KilledUnit(Unit* victim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY3, m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        if (!IsImage)
            DoScriptText(SAY_DEATH, m_creature);
    }

    void Aggro(Unit *who)
    {
        if (IsImage || Images75)
            return;
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //ArcaneExplosion_Timer
        if (ArcaneExplosion_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION);
            ArcaneExplosion_Timer = urand(8000, 18000);
        }else ArcaneExplosion_Timer -= diff;

        //If we are within range melee the target
        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
        {
            //Make sure our attack is ready and we arn't currently casting
            if (m_creature->isAttackReady() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                m_creature->AttackerStateUpdate(m_creature->getVictim());
                m_creature->resetAttackTimer();
            }
        }else
        {
            //EarthShock_Timer
            if (EarthShock_Timer < diff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_EARTH_SHOCK);
                EarthShock_Timer = 1000;
            }else EarthShock_Timer -= diff;
        }

        //Blink_Timer
        if (Blink_Timer < diff)
        {
            //DoCastSpellIfCan(m_creature, SPELL_BLINK);
            switch(urand(0, 2))
            {
                case 0:
                    m_creature->GetMap()->CreatureRelocation(m_creature, -8340.782227f, 2083.814453f, 125.648788f, 0.0f);
                    DoResetThreat();
                    break;
                case 1:
                    m_creature->GetMap()->CreatureRelocation(m_creature, -8341.546875f, 2118.504639f, 133.058151f, 0.0f);
                    DoResetThreat();
                    break;
                case 2:
                    m_creature->GetMap()->CreatureRelocation(m_creature, -8318.822266f, 2058.231201f, 133.058151f, 0.0f);
                    DoResetThreat();
                    break;
            }
            DoStopAttack();

            Blink_Timer = urand(20000, 40000);
        }else Blink_Timer -= diff;

        float procent = m_creature->GetHealthPercent();

        //Summoning 2 Images and teleporting to a random position on 75% health
        if (!Images75 && !IsImage && procent <= 75.0f && procent > 70.0f)
            DoSplit(75);

        //Summoning 2 Images and teleporting to a random position on 50% health
        if (!Images50 && !IsImage && procent <= 50.0f && procent > 45.0f)
            DoSplit(50);

        //Summoning 2 Images and teleporting to a random position on 25% health
        if (!Images25 && !IsImage && procent <= 25.0f && procent > 20.0f)
            DoSplit(25);

        //Invisible_Timer
        if (Invisible)
        {
            if (Invisible_Timer < diff)
            {
                //Making Skeram visible after telporting
                m_creature->SetVisibility(VISIBILITY_ON);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                Invisible_Timer = 2500;
                Invisible = false;
            }else Invisible_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }

    void DoSplit(int atPercent /* 75 50 25 */)
    {
        DoScriptText(SAY_SPLIT, m_creature);

        ov_mycoordinates *place1 = new ov_mycoordinates(-8340.782227f, 2083.814453f, 125.648788f, 0.0f);
        ov_mycoordinates *place2 = new ov_mycoordinates(-8341.546875f, 2118.504639f, 133.058151f, 0.0f);
        ov_mycoordinates *place3 = new ov_mycoordinates(-8318.822266f, 2058.231201f, 133.058151f, 0.0f);

        ov_mycoordinates *bossc=place1, *i1=place2, *i2=place3;

        switch(urand(0, 2))
        {
            case 0:
                bossc = place1;
                i1 = place2;
                i2 = place3;
                break;
            case 1:
                bossc = place2;
                i1 = place1;
                i2 = place3;
                break;
            case 2:
                bossc = place3;
                i1 = place1;
                i2 = place2;
                break;
        }

        m_creature->RemoveAllAuras();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_OFF);

        m_creature->GetMap()->CreatureRelocation(m_creature, bossc->x, bossc->y, bossc->z, bossc->r);

        Invisible = true;
        DoResetThreat();
        DoStopAttack();

        switch (atPercent)
        {
            case 75: Images75 = true; break;
            case 50: Images50 = true; break;
            case 25: Images25 = true; break;
        }

        Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);

        Image1 = m_creature->SummonCreature(15263, i1->x, i1->y, i1->z, i1->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (Image1)
        {
            Image1->SetMaxHealth(m_creature->GetMaxHealth() / 5);
            Image1->SetHealth(m_creature->GetHealth() / 5);

            if (target)
                Image1->AI()->AttackStart(target);

            if (boss_skeramAI* pImageAI = dynamic_cast<boss_skeramAI*>(Image1->AI()))
                pImageAI->IsImage = true;
        }

        Image2 = m_creature->SummonCreature(15263,i2->x, i2->y, i2->z, i2->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (Image2)
        {
            Image2->SetMaxHealth(m_creature->GetMaxHealth() / 5);
            Image2->SetHealth(m_creature->GetHealth() / 5);

            if (target)
                Image2->AI()->AttackStart(target);

            if (boss_skeramAI* pImageAI = dynamic_cast<boss_skeramAI*>(Image2->AI()))
                pImageAI->IsImage = true;
        }


        Invisible = true;
        delete place1;
        delete place2;
        delete place3;
    }
};

CreatureAI* GetAI_boss_skeram(Creature* pCreature)
{
    return new boss_skeramAI(pCreature);
}

void AddSC_boss_skeram()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_skeram";
    newscript->GetAI = &GetAI_boss_skeram;
    newscript->RegisterSelf();
}
