/* Copyright (C) 2008 - 2010 TrinityCore <http://www.trinitycore.org>
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

#include "precompiled.h"
#include "oculus.h"

#define MAX_ENCOUNTER 7

/* The Occulus encounters:
0 - Drakos the Interrogator
1 - Varos Cloudstrider
2 - Mage-Lord Urom
3 - Ley-Guardian Eregos */

struct MANGOS_DLL_DECL instance_oculus : public ScriptedInstance
{
    instance_oculus(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint64 uiDrakos;
    uint64 m_uiVarosGUID;
    uint64 m_uiUromGUID;
    uint64 m_uiEregosGUID;
    uint64 uiProect;
    uint64 uiCacheEregosGUID;

    uint8 m_auiEncounter[MAX_ENCOUNTER];
    std::string str_data;

    void Initialize()
    {
      uiCacheEregosGUID = 0;
      uiProect = 0;
      m_uiVarosGUID = 0;
      m_uiUromGUID = 0;
      m_uiEregosGUID = 0;
      m_auiEncounter[4] = 10;
      m_auiEncounter[5] = 0;
    }
    
    void OnGameObjectCreate(GameObject* pGO, bool bAdd)
    {
        switch(pGO->GetEntry())
        {
	case CACHE_OF_EREGOS:
	case CACHE_OF_EREGOS_H:
	uiCacheEregosGUID = pGO->GetGUID();
	break;
	}
    }
    
    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case CREATURE_DRAKOS:
                uiDrakos = pCreature->GetGUID();
                break;
            case NPC_VAROS:
                m_uiVarosGUID = pCreature->GetGUID();
                break;
            case NPC_UROM:
                m_uiUromGUID = pCreature->GetGUID();
                break;
            case NPC_EREGOS:
                m_uiEregosGUID = pCreature->GetGUID();
                break;
	      case CREATURE_PROEC:
	          uiProect = pCreature->GetGUID();
		    break;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case TYPE_DRAKOS:
                m_auiEncounter[0] = data;
                break;
            case TYPE_VAROS:
                m_auiEncounter[1] = data;
                if(data == DONE)
                  if(Creature* pUrom = instance->GetCreature(m_uiUromGUID))
                  {
                    pUrom->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pUrom->InterruptNonMeleeSpells(false); 
                    pUrom->RemoveAurasDueToSpell(53813);
                  }
                break;
            case TYPE_UROM:
                m_auiEncounter[2] = data;
                if(data == DONE)
                  if(Creature* pEregos = instance->GetCreature(m_uiEregosGUID))
                  {
                     pEregos->SetVisibility(VISIBILITY_ON);
                     pEregos->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                  }
                break;
            case TYPE_EREGOS:
                m_auiEncounter[3] = data;
                break;
            case TYPE_ROBOTS:
                m_auiEncounter[4] = m_auiEncounter[4] - data;
                if(m_auiEncounter[4] == 0)
                  if(Creature* pVaros = instance->GetCreature(m_uiVarosGUID))
                  {
                    DoScriptText(SAY_VAROS_SPAWN, pVaros);
                    pVaros->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pVaros->InterruptNonMeleeSpells(false);
                    pVaros->RemoveAurasDueToSpell(50053);
                  }
                break;
            case TYPE_UROM_PHASE:
                m_auiEncounter[5] = data;
                break;
        }

        if (data == DONE)
            SaveToDB();
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case TYPE_DRAKOS:                return m_auiEncounter[0];
            case TYPE_VAROS:                 return m_auiEncounter[1];
            case TYPE_UROM:                  return m_auiEncounter[2];
            case TYPE_EREGOS:                return m_auiEncounter[3];
            case TYPE_ROBOTS:                return m_auiEncounter[4];
            case TYPE_UROM_PHASE:            return m_auiEncounter[5];
        }

        return 0;
    }

    uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
            case DATA_DRAKOS:                 return uiDrakos;
            case NPC_VAROS:                   return m_uiVarosGUID;
            case NPC_UROM:                    return m_uiUromGUID;
            case NPC_EREGOS:                  return m_uiEregosGUID;
        }

        return 0;
    }


    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << "T O " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        str_data = saveStream.str();

        OUT_SAVE_INST_DATA_COMPLETE;
        return str_data;
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        char dataHead1, dataHead2;
        uint16 data0, data1, data2, data3;

        std::istringstream loadStream(in);
        loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

        if (dataHead1 == 'T' && dataHead2 == 'O')
        {
            m_auiEncounter[0] = data0;
            m_auiEncounter[1] = data1;
            m_auiEncounter[2] = data2;
            m_auiEncounter[3] = data3;

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED;

        } else OUT_LOAD_INST_DATA_FAIL;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_oculus(Map* pMap)
{
    return new instance_oculus(pMap);
}

void AddSC_instance_oculus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_oculus";
    newscript->GetInstanceData = &GetInstanceData_instance_oculus;
    newscript->RegisterSelf();
}
