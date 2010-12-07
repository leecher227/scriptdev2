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
SDName: instance_draktharon_keep
SD%Complete: 75%
SDComment:
SDCategory: draktharon_keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

struct MANGOS_DLL_DECL instance_draktharon_keep : public ScriptedInstance
{
    instance_draktharon_keep(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiHeartGUID;
    uint64 m_uiEyeGUID;
    uint64 m_uiRuneGUID;

    uint64 m_uiTrollGUID;

    uint64 m_uiCrystal01GUID;
    uint64 m_uiCrystal02GUID;
    uint64 m_uiCrystal03GUID;
    uint64 m_uiCrystal04GUID;

    uint8 m_uiCrystalActiveCount;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiHeartGUID = 0;
        m_uiEyeGUID = 0;
        m_uiRuneGUID = 0;

        m_uiTrollGUID = 0;

        m_uiCrystal01GUID = 0;
        m_uiCrystal02GUID = 0;
        m_uiCrystal03GUID = 0;
        m_uiCrystal04GUID = 0;
        m_uiCrystalActiveCount = 0;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_CRYSTAL_01:
                m_uiCrystal01GUID = pGo->GetGUID();
                break;
            case GO_CRYSTAL_02:
                m_uiCrystal02GUID = pGo->GetGUID();
                break;
            case GO_CRYSTAL_03:
                m_uiCrystal03GUID = pGo->GetGUID();
                break;
            case GO_CRYSTAL_04:
                m_uiCrystal04GUID = pGo->GetGUID();
                break;
            case GO_HEART_OF_ASCIENT:
                m_uiHeartGUID = pGo->GetGUID();
                pGo->SetPhaseMask(0, true);
                break;
            case GO_EYE_OF_PROPHET:
                m_uiEyeGUID = pGo->GetGUID();
                pGo->SetPhaseMask(0, true);
                break;
            case GO_RUNE_OF_DRAKKARI:
                m_uiRuneGUID = pGo->GetGUID();
                pGo->SetPhaseMask(0, true);
                break;
        }
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_TROLLGORE:
                m_uiTrollGUID = pCreature->GetGUID();
                break;
        }
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
            case GO_HEART_OF_ASCIENT:
                return m_uiHeartGUID;
            case GO_EYE_OF_PROPHET:
                return m_uiEyeGUID;
            case GO_RUNE_OF_DRAKKARI:
                return m_uiRuneGUID;
            case NPC_TROLLGORE:
                return m_uiTrollGUID;
            case GO_CRYSTAL_01:
                return m_uiCrystal01GUID;
            case GO_CRYSTAL_02:
                return m_uiCrystal02GUID;
            case GO_CRYSTAL_03:
                return m_uiCrystal03GUID;
            case GO_CRYSTAL_04:
                return m_uiCrystal04GUID;
        }

        return 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_THARONJA:
                return m_auiEncounter[0];
            case TYPE_TROLLGORE:
                return m_auiEncounter[1];
            case TYPE_CRYSTALS:
                return m_auiEncounter[2];
        }

        return 0;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance DK: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_THARONJA:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_TROLLGORE:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_CRYSTALS:
                if(uiData != 0)
                   m_auiEncounter[2] = m_auiEncounter[2] + 1;
                else
                {
                   m_auiEncounter[2] = 0;
                   if(GameObject* pGo = instance->GetGameObject(m_uiCrystal01GUID))
                   {
                      pGo->SetGoState(GO_STATE_READY);
                      pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                   }
                   if(GameObject* pGo = instance->GetGameObject(m_uiCrystal02GUID))
                   {
                      pGo->SetGoState(GO_STATE_READY);
                      pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                   }
                   if(GameObject* pGo = instance->GetGameObject(m_uiCrystal03GUID))
                   {
                      pGo->SetGoState(GO_STATE_READY);
                      pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                   }
                   if(GameObject* pGo = instance->GetGameObject(m_uiCrystal04GUID))
                   {
                      pGo->SetGoState(GO_STATE_READY);
                      pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                   }
                }
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_draktharon_keep(Map* pMap)
{
    return new instance_draktharon_keep(pMap);
}

void AddSC_instance_draktharon_keep()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_draktharon_keep";
    newscript->GetInstanceData = &GetInstanceData_instance_draktharon_keep;
    newscript->RegisterSelf();
}
