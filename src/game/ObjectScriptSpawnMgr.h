/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
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

#ifndef OBJECT_SCRIPT_SPAWN_MGR_H
#define OBJECT_SCRIPT_SPAWN_MGR_H

#include "Common.h"
#include "Policies/Singleton.h"
#include "ObjectGuid.h"
#include <functional>
#include "Map.h"

class Unit;
class Creature;

// Structure holding the information for an entry
struct CreatureScriptSpawnInfo
{
    uint32 team_control;
    uint32 add_control;
    uint32 remove_control;
    //uint32 command;
};

struct GameObjectScriptSpawnInfo
{
    uint32 team_control;
    uint32 add_control;
    uint32 remove_control;
};

/*######## Team Mask #########
#### HORDE               = 67,
#### ALLIANCE            = 469,
##############################*/

// TODO: spawn command
struct CSScommand
{
    uint16 command;
};

class ObjectScriptSpawnMgr
{
    public:                                                 // Constructors
        ObjectScriptSpawnMgr() {}

    public:                                                 // Initialisation
        void LoadFromDB();

    public:                                                 // Accessors
        // This functions checks NPC on data event
        bool CreatureCanSpawn(uint32 guidlow, Map* map);
        bool GameObjectCanSpawn(uint32 guidlow, Map* map);
        bool CalculateSpawnPossible(uint32 object_team, uint32 object_add_event, uint32 object_remove_event, uint32 map_team, uint32 map_event);

        const CreatureScriptSpawnInfo* GetCreatureSpawnInfo(uint32 guidlow);
        const GameObjectScriptSpawnInfo* GetGameObjectSpawnInfo(uint32 guid_low);

        void SpawnNpc(uint32 guidlow, Map* map, CreatureData const* data);
        void SpawnGameObject(uint32 guidlow, Map* map, GameObjectData const* data);
        void UnSpawnNpc(uint32 guidlow, Map* map, CreatureData const* data);
        void UnSpawnGameObject(uint32 guidlow, Map* map, GameObjectData const* data);

        void ProcessEvent(Map* map);

    private:
        typedef std::map<uint32 /*low_guid*/, CreatureScriptSpawnInfo> CreatureScriptSpawnList;
        CreatureScriptSpawnList m_creatureScriptSpawnList;

        typedef std::map<uint32 /*low_guid*/, GameObjectScriptSpawnInfo> GameObjectScriptSpawnList;
        GameObjectScriptSpawnList m_gameobjectScriptSpawnList;
};

#define sObjectScriptSpawnMgr MaNGOS::Singleton<ObjectScriptSpawnMgr>::Instance()

#endif