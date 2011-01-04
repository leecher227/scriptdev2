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
SDName: instance_vault_of_archavon
SD%Complete: 0
SDComment: Placeholder
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

struct MANGOS_DLL_DECL instance_vault_of_archavon : public ScriptedInstance
{
    instance_vault_of_archavon(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiArchavonGUID;
    uint64 m_uiEmalonGUID;
    uint64 m_uiTempestMinion1GUID;
    uint64 m_uiTempestMinion2GUID;
    uint64 m_uiTempestMinion3GUID;
    uint64 m_uiTempestMinion4GUID;
    uint8 m_uiMinion;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
        m_uiArchavonGUID = 0;
        m_uiEmalonGUID = 0;
        m_uiTempestMinion1GUID = 0;
        m_uiTempestMinion2GUID = 0;
        m_uiTempestMinion3GUID = 0;
        m_uiTempestMinion4GUID = 0;
        m_uiMinion = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_ARCHAVON:
                m_uiArchavonGUID = pCreature->GetGUID();
                break;
            case NPC_EMALON:
                m_uiEmalonGUID = pCreature->GetGUID();
                break;
            case NPC_TEMPEST_MINION:
                ++m_uiMinion;
                switch (m_uiMinion)
                {
                    case 1:
                        m_uiTempestMinion1GUID = pCreature->GetGUID();
                        break;
                    case 2:
                        m_uiTempestMinion2GUID = pCreature->GetGUID();
                        break;
                    case 3:
                        m_uiTempestMinion3GUID = pCreature->GetGUID();
                        break;
                    case 4:
                        m_uiTempestMinion4GUID = pCreature->GetGUID();
                        break;
                    case 5:
                        m_uiMinion = 0;
                        break;
                }
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch (uiType)
        {
            case TYPE_ARCHAVON:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_EMALON:
                m_auiEncounter[1] = uiData;
                break;
        }
        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch (uiType)
        {
            case TYPE_ARCHAVON:
                return m_auiEncounter[0];
            case TYPE_EMALON:
                return m_auiEncounter[1];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch (uiData)
        {
            case DATA_ARCHAVON:
                return m_uiArchavonGUID;
            case DATA_EMALON:
                return m_uiEmalonGUID;
            case DATA_TEMPEST_MINION_1:
                return m_uiTempestMinion1GUID;
            case DATA_TEMPEST_MINION_2:
                return m_uiTempestMinion2GUID;
            case DATA_TEMPEST_MINION_3:
                return m_uiTempestMinion3GUID;
            case DATA_TEMPEST_MINION_4:
                return m_uiTempestMinion4GUID;
        }
        return 0;
    }
    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1];

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_vault_of_archavon(Map* pMap)
{
    return new instance_vault_of_archavon(pMap);
}

void AddSC_instance_vault_of_archavon()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_vault_of_archavon";
    pNewScript->GetInstanceData = &GetInstanceData_instance_vault_of_archavon;
    pNewScript->RegisterSelf();
}
