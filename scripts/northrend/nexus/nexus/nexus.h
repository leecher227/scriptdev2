/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_NEXUS_H
#define DEF_NEXUS_H

enum
{
    MAX_ENCOUNTER                  = 4,

    TYPE_TELESTRA                  = 0,
    TYPE_ANOMALUS                  = 1,
    TYPE_ORMOROK                   = 2,
    TYPE_KERISTRASZA               = 3,

    NPC_TELESTRA                   = 26731,
    NPC_ANOMALUS                   = 26763,
    NPC_ORMOROK                    = 26794,
    NPC_KERISTRASZA                = 26723,
    NPC_ALLIANCE_BERSERKER         = 26800,
    NPC_HORDE_BERSERKER            = 26799,
    NPC_ALLIANCE_RANGER            = 26802,
    NPC_HORDE_RANGER               = 26801,
    NPC_ALLIANCE_CLERIC            = 26805,
    NPC_HORDE_CLERIC               = 26803,
    NPC_ALLIANCE_COMMANDER         = 27949,
    NPC_HORDE_COMMANDER            = 27947,
    NPC_COMMANDER_STOUTBEARD       = 26796,
    NPC_COMMANDER_KOLURG           = 26798,
    NPC_BREATH_CASTER              = 27048,

    GO_CONTAINMENT_SPHERE_TELESTRA = 188526,
    GO_CONTAINMENT_SPHERE_ANOMALUS = 188527,
    GO_CONTAINMENT_SPHERE_ORMOROK  = 188528,

    SPELL_FROZEN_PRISON             = 47854,                // may not be correct spell
    SPELL_ICE_BREATH                = 47842,

    MODEL_ID_CASTER                 = 1126,  
};
#endif
