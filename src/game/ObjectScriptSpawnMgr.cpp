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

/**
 *
 * @file ObjectScriptSpawnMgr.cpp
 * This file contains the code needed for MaNGOS to spawn npcs from scripts
 * Currently implemented
 * - Spawn Only Horde (Alliance) object for Horde (Alliance) player team in Instance
 * - Spawn All object from base when InstanceEvent >= add_control
 * - Remove All object from world when InstanceEvent >= remove_control
 * TODO: Need implemend dynamic spawn place.
 */

#include "ObjectScriptSpawnMgr.h"
#include "Policies/SingletonImp.h"
#include "ProgressBar.h"
#include "Database/DatabaseEnv.h"
#include "ObjectMgr.h"
#include "SharedDefines.h"
#include "Creature.h"
#include "GameObject.h"
#include "InstanceData.h"

/* *********************************************************
 * Method to Load From DB
 * DB Format:      spawn_guid, team_control, add_control, remove_control
 *                 0           1             2            3
 * **************************************
 * spawn_guid:     creature.guid
 * team_control:   Team control
 * add_control:    Add Action
 * remove_control: Remove Action
 *
 * ***************************************************** */


INSTANTIATE_SINGLETON_1(ObjectScriptSpawnMgr);

void ObjectScriptSpawnMgr::LoadFromDB()
{
    m_creatureScriptSpawnList.clear();

    QueryResult* result = WorldDatabase.Query("SELECT spawn_guid, team_control, add_control, remove_control /*,command*/ FROM creature_script_spawn");

    uint32 count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Table creature_script_spawn is empty.");
        sLog.outString();

        return;
    }

    BarGoLink bar((int)result->GetRowCount());

    do
    {
        bar.step();

        Field* fields = result->Fetch();
        CreatureScriptSpawnInfo sInfo;

        uint32 guid               = fields[0].GetUInt32();
        sInfo.team_control        = fields[1].GetUInt32();
        sInfo.add_control         = fields[2].GetUInt32();
        sInfo.remove_control      = fields[3].GetUInt32();

        CreatureData const* data = sObjectMgr.GetCreatureData(guid);
        if (!data)
            continue;

        ++count;

        m_creatureScriptSpawnList.insert(CreatureScriptSpawnList::value_type(guid, sInfo));
    }
    while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded creature script spawn for %u creatures.", count);
    delete result;

    //GameObject
    m_gameobjectScriptSpawnList.clear();

    result = WorldDatabase.Query("SELECT spawn_guid, team_control, add_control, remove_control FROM gameobject_script_spawn");

    count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Table gameobject_script_spawn is empty.");
        sLog.outString();

        return;
    }

    BarGoLink go_bar((int)result->GetRowCount());

    do
    {
        go_bar.step();

        Field* fields = result->Fetch();
        GameObjectScriptSpawnInfo sInfo;

        uint32 guid               = fields[0].GetUInt32();
        sInfo.team_control        = fields[1].GetUInt32();
        sInfo.add_control         = fields[2].GetUInt32();
        sInfo.remove_control      = fields[3].GetUInt32();

        GameObjectData const* data = sObjectMgr.GetGOData(guid);
        if (!data)
            continue;

        ++count;

        m_gameobjectScriptSpawnList.insert(GameObjectScriptSpawnList::value_type(guid, sInfo));
    }
    while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded gameobject script spawn for %u gameobjects.", count);
    delete result;
}

bool ObjectScriptSpawnMgr::CreatureCanSpawn(uint32 guidlow, Map* map)
{
    if (!map)
    {
        error_log("ObjectScriptSpawnMgr: Script spawn not working becouse map not found for creature with guid: %u ", guidlow);
        return false;
    }

    const CreatureScriptSpawnInfo* sInfo = GetCreatureSpawnInfo(guidlow);

    if (!sInfo)
        return true;

    return CalculateSpawnPossible(sInfo->team_control, sInfo->add_control, sInfo->remove_control, map->GetMapTeam(), map->GetMapProcess());
}

bool ObjectScriptSpawnMgr::GameObjectCanSpawn(uint32 guidlow, Map* map)
{
    if (!map)
    {
        error_log("ObjectScriptSpawnMgr: Script spawn not working becouse map not found for gameobject with guid: %u ", guidlow);
        return false;
    }

    const GameObjectScriptSpawnInfo* sInfo = GetGameObjectSpawnInfo(guidlow);

    if (!sInfo)
        return true;

    return CalculateSpawnPossible(sInfo->team_control, sInfo->add_control, sInfo->remove_control, map->GetMapTeam(), map->GetMapProcess());
}

bool ObjectScriptSpawnMgr::CalculateSpawnPossible(uint32 object_team, uint32 object_add_event, uint32 object_remove_event, uint32 map_team, uint32 map_event)
{
    if (map_team && object_team && map_team != object_team)
        return false;

    if ((object_add_event || object_remove_event) && (map_event < object_add_event || map_event >= object_remove_event))
        return false;

    return true;
}

const CreatureScriptSpawnInfo* ObjectScriptSpawnMgr::GetCreatureSpawnInfo(uint32 guid_low)
{
    CreatureScriptSpawnList::const_iterator itr = m_creatureScriptSpawnList.find(guid_low);
    if (itr != m_creatureScriptSpawnList.end())
        return &itr->second;

    return NULL;
}

const GameObjectScriptSpawnInfo* ObjectScriptSpawnMgr::GetGameObjectSpawnInfo(uint32 guid_low)
{
    GameObjectScriptSpawnList::const_iterator itr = m_gameobjectScriptSpawnList.find(guid_low);
    if (itr != m_gameobjectScriptSpawnList.end())
        return &itr->second;

    return NULL;
}

void ObjectScriptSpawnMgr::SpawnNpc(uint32 guidlow, Map* map, CreatureData const* data)
{
    // We use spawn coords to spawn
    if (map->IsLoaded(data->posX, data->posY))
    {
        Creature* pCreature = new Creature;
        if (!pCreature->LoadFromDB(guidlow, map))
            delete pCreature;
        else
            map->Add(pCreature);
    }
}

void ObjectScriptSpawnMgr::SpawnGameObject(uint32 guidlow, Map* map, GameObjectData const* data)
{
    // We use spawn coords to spawn
    if (map->IsLoaded(data->posX, data->posY))
    {
        GameObject* pGameObject = new GameObject;
        if (!pGameObject->LoadFromDB(guidlow, map))
            delete pGameObject;
        else
            map->Add(pGameObject);
    }
}

void ObjectScriptSpawnMgr::UnSpawnNpc(uint32 guidlow, Map* map, CreatureData const* data)
{
    CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(data->id);
    if (!cinfo)
        return;

    if (Creature* pCreature = map->GetCreature(cinfo->GetObjectGuid(guidlow)))
        map->Remove(pCreature, false);
}

void ObjectScriptSpawnMgr::UnSpawnGameObject(uint32 guidlow, Map* map, GameObjectData const* data)
{
    GameObjectInfo const *cinfo = sObjectMgr.GetGameObjectInfo(data->id);
    if (!cinfo)
        return;

    if (GameObject* pGameObject = map->GetGameObject(cinfo->GetObjectGuid(guidlow)))
        map->Remove(pGameObject, false);
}

void ObjectScriptSpawnMgr::ProcessEvent(Map* map)
{
    if (!map)
    {
        error_log("CreatureScriptSpawnMgr: Script spawn not working becouse map not found.");
        return;
    }

    if (!m_creatureScriptSpawnList.empty())
    {
        for (CreatureScriptSpawnList::const_iterator itr = m_creatureScriptSpawnList.begin(); itr != m_creatureScriptSpawnList.end(); itr++)
        {
            CreatureData const* data = sObjectMgr.GetCreatureData(itr->first);

            if (!data || data->mapid != map->GetId())
                continue;

            if (!CalculateSpawnPossible(itr->second.team_control, itr->second.add_control, itr->second.remove_control, map->GetMapTeam(), map->GetMapProcess()))
                UnSpawnNpc(itr->first, map, data);
            else
                SpawnNpc(itr->first, map, data);
        }
    }

    if (!m_gameobjectScriptSpawnList.empty())
    {
        for (GameObjectScriptSpawnList::const_iterator itr = m_gameobjectScriptSpawnList.begin(); itr != m_gameobjectScriptSpawnList.end(); itr++)
        {
            GameObjectData const* data = sObjectMgr.GetGOData(itr->first);

            if (!data || data->mapid != map->GetId())
                continue;

            if (!CalculateSpawnPossible(itr->second.team_control, itr->second.add_control, itr->second.remove_control, map->GetMapTeam(), map->GetMapProcess()))
                UnSpawnGameObject(itr->first, map, data);
            else
                SpawnGameObject(itr->first, map, data);
        }
    }
}

void Map::ProcessMapEvent(uint32 event, bool process)
{
    i_eventId = event;

    if (process)
        sObjectScriptSpawnMgr.ProcessEvent(this);
}
