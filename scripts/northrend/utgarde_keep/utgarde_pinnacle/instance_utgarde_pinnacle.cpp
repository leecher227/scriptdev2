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
SDName: instance_pinnacle
SD%Complete: 25%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

struct MANGOS_DLL_DECL instance_pinnacle : public ScriptedInstance
{
    instance_pinnacle(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiGortokGUID;

    uint64 m_uiSkadiDoorGUID;
    uint64 m_uiStasisGeneratorGUID;
    uint64 m_uiOrbGUID;
    uint64 m_uiRhinoGUID;
    uint64 m_uiWorgenGUID;
    uint64 m_uiFurlborgGUID;
    uint64 m_uiJormungarGUID;

    uint64 m_uiRanulfGUID;
    uint64 m_uiHaldorGUID;
    uint64 m_uiBjornGUID;
    uint64 m_uiTorgynGUID;
    uint64 m_uiYmironGUID;

    uint64 m_uiSkadiGUID;
    uint64 m_uiTriggerGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiGortokGUID = 0;

        m_uiSkadiDoorGUID = 0;
        m_auiEncounter[1] = NOT_STARTED;
        m_uiStasisGeneratorGUID = 0;
        m_uiOrbGUID = 0;
        m_uiRhinoGUID = 0;
        m_uiWorgenGUID = 0;
        m_uiFurlborgGUID = 0;
        m_uiJormungarGUID = 0;

        m_uiRanulfGUID = 0;
        m_uiHaldorGUID = 0;
        m_uiBjornGUID = 0;
        m_uiTorgynGUID = 0;
        m_uiYmironGUID = 0;

        m_uiSkadiGUID = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_RANULF:
                m_uiRanulfGUID = pCreature->GetGUID();
                break;
            case NPC_HALDOR:
                m_uiHaldorGUID = pCreature->GetGUID();
                break;
            case NPC_BJORN:
                m_uiBjornGUID = pCreature->GetGUID();
                break;
            case NPC_TORGYN:
                m_uiTorgynGUID = pCreature->GetGUID();
                break;
            case NPC_YMIRON:
                m_uiYmironGUID = pCreature->GetGUID();
                break;
            case NPC_GORTOK:
                m_uiGortokGUID = pCreature->GetGUID();
                break;
            case NPC_WORGEN:
                m_uiWorgenGUID = pCreature->GetGUID();
                break;
            case NPC_JORMUNGAR:
                m_uiJormungarGUID = pCreature->GetGUID();
                break;
            case NPC_FURLBORG:
                m_uiFurlborgGUID = pCreature->GetGUID(); 
               break;
            case NPC_RHINO:
                m_uiRhinoGUID = pCreature->GetGUID();
                break;
            case NPC_SKADI:
                m_uiSkadiGUID = pCreature->GetGUID();
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_DOOR_SKADI:
                m_uiSkadiDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_STASIS_GENERATOR:
                m_uiStasisGeneratorGUID = pGo->GetGUID();
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Pinnacle: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_SVALA:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_GORTOK:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_SKADI:
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiSkadiDoorGUID);
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_YMIRON:
                m_auiEncounter[3] = uiData;
                break;
            default:
                error_log("SD2: Instance Pinnacle: SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
                break;
        }

        //saving also SPECIAL for this instance
        if (uiData == DONE || uiData == SPECIAL)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_SVALA:
                return m_auiEncounter[0];
            case TYPE_GORTOK:
                return m_auiEncounter[1];
            case TYPE_SKADI:
                return m_auiEncounter[2];
            case TYPE_YMIRON:
                return m_auiEncounter[3];
        }

        return 0;
    }

    void SetData64(uint32 uiData, uint64 uiGuid)
    {
        switch(uiData)
        {
            case NPC_STASIS_CONTROLLER:
                m_uiOrbGUID = uiGuid;
                break;
        }
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
            case NPC_RANULF:
                return m_uiRanulfGUID;
            case NPC_HALDOR:
                return m_uiHaldorGUID;
            case NPC_BJORN:
                return m_uiBjornGUID;
            case NPC_TORGYN:
                return m_uiTorgynGUID;
            case NPC_YMIRON:
                return m_uiYmironGUID;
            case NPC_STASIS_CONTROLLER:
                return m_uiOrbGUID;
            case NPC_GORTOK:
                return m_uiGortokGUID;
            case NPC_WORGEN:
                return m_uiWorgenGUID;
            case NPC_FURLBORG:
                return m_uiFurlborgGUID;
            case NPC_RHINO:
                return m_uiRhinoGUID;
            case NPC_JORMUNGAR:
                return m_uiJormungarGUID;
            case GO_STASIS_GENERATOR:
                return m_uiStasisGeneratorGUID;
            case NPC_SKADI:
                return m_uiSkadiGUID;
        }
        return 0;
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

InstanceData* GetInstanceData_instance_pinnacle(Map* pMap)
{
    return new instance_pinnacle(pMap);
}

void AddSC_instance_pinnacle()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_pinnacle";
    newscript->GetInstanceData = &GetInstanceData_instance_pinnacle;
    newscript->RegisterSelf();
}
