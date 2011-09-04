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
 * @addtogroup npc_linking
 * @{
 *
 * @file CreatureLinkingMgr.cpp
 * This file contains the the code needed for MaNGOS to link npcs together
 * Currently implemented
 * - Aggro on boss aggro, also reversed
 * - Despawning/ Selfkill on death of mob if the NPC it is linked to dies
 * - Respawning on leaving combat if the linked to NPC evades, also reversed
 * - Respawning on death of the linked to NPC
 * TODO
 * - (Re)Spawning dependend on boss Alive/ Dead
 * - Following NPCs
 *
 */

#include "CreatureLinkingMgr.h"
#include "Policies/SingletonImp.h"
#include "ProgressBar.h"
#include "SharedDefines.h"
#include "CreatureAI.h"

INSTANTIATE_SINGLETON_1(CreatureLinkingMgr);

/* *********************************************************
 * Method to Load From DB
 * DB Format:   entry, map, master_entry, flag
 *              0      1    2             3
 * **************************************
 * entry:       creature_template.entry
 * map:         Map on which the NPC has to be
 * master_entry creature_template.entry of the npc, that shall trigger the actions
 * flag:        flag value, of type CreatureLinkingFlags
 *
 * ***************************************************** */

void CreatureLinkingMgr::LoadFromDB()
{
    // Clear maps
    m_creatureLinkingMap.clear();
    m_eventTriggers.clear();                              // master

    QueryResult* result = WorldDatabase.Query("SELECT entry, map, master_entry, flag FROM creature_linking_template");

    uint32 count = 0;

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString(">> Table creature_linking_template is empty.");
        sLog.outString();

        return;
    }

    BarGoLink bar((int)result->GetRowCount());

    do
    {
        bar.step();

        Field* fields = result->Fetch();
        CreatureLinkingInfo tmp;

        uint32 entry            = fields[0].GetUInt32();
        tmp.mapId               = fields[1].GetUInt32();
        tmp.masterId            = fields[2].GetUInt32();
        tmp.linkingFlag         = fields[3].GetUInt16();
        tmp.masterDBGuid        = 0;                        // Will be initialized for unique mobs later (only for spawning dependend)

        if (!IsLinkingEntryValid(entry, &tmp))
            continue;

        // Store db-guid for master of whom pTmp is spawn dependend
        if (tmp.linkingFlag & (FLAG_CANT_SPAWN_IF_BOSS_DEAD | FLAG_CANT_SPAWN_IF_BOSS_ALIVE))
        {
            if (QueryResult* guid_result = WorldDatabase.PQuery("SELECT guid FROM creature WHERE id=%u AND map=%u LIMIT 1", tmp.masterId, tmp.mapId))
            {
                tmp.masterDBGuid = (*guid_result)[0].GetUInt32();

                delete guid_result;
            }
        }

        ++count;

        // Add it to the map
        m_creatureLinkingMap.insert(std::pair<uint32, CreatureLinkingInfo>(entry, tmp));

        // Store master_entry
        m_eventTriggers.insert(tmp.masterId);
    }
    while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded creature linking for %u creature-IDs", count);
    delete result;
}

// This function is used to check if a DB-Entry is valid
bool CreatureLinkingMgr::IsLinkingEntryValid(uint32 entry, CreatureLinkingInfo* pTmp)
{
    // Basic checks first
    CreatureInfo const* pInfo = ObjectMgr::GetCreatureTemplate(entry);
    if (!pInfo)
    {
        sLog.outErrorDb("`creature_linking_template` has a non existing creature (ID: %u) defined, skipped.", entry);
        return false;
    }

    pInfo = ObjectMgr::GetCreatureTemplate(pTmp->masterId);
    if (!pInfo)
    {
        sLog.outErrorDb("`creature_linking_template` has a non existing master_entry (ID: %u), skipped", pTmp->masterId);
        return false;
    }

    if (pTmp->linkingFlag & ~(LINKING_FLAG_INVALID - 1)  || pTmp->linkingFlag == 0)
    {
        sLog.outErrorDb("`creature_linking_template` has invalid flag, (entry: %u, map: %u, flags: %u), skipped", entry, pTmp->mapId, pTmp->linkingFlag);
        return false;
    }

    // Additional checks, depending on flags
    if (pTmp->linkingFlag & FLAG_DESPAWN_ON_RESPAWN && entry == pTmp->masterId)
    {
        sLog.outErrorDb("`creature_linking_template` has pointless FLAG_DESPAWN_ON_RESPAWN for self, (entry: %u, map: %u), skipped", entry, pTmp->mapId);
        return false;
    }

    // Check for uniqueness of mob whom is followed, on whom spawning is dependend
    if (pTmp->linkingFlag & (FLAG_FOLLOW | FLAG_CANT_SPAWN_IF_BOSS_DEAD | FLAG_CANT_SPAWN_IF_BOSS_ALIVE))
    {
        // Painfully slow, needs better idea
        QueryResult *result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM creature WHERE id=%u AND map=%u", pTmp->masterId, pTmp->mapId);
        if (result)
        {
            if ((*result)[0].GetUInt32() > 1)
                sLog.outErrorDb("`creature_linking_template` has FLAG_FOLLOW, but non unique master, (entry: %u, map: %u, master: %u)", entry, pTmp->mapId, pTmp->masterId);
            delete result;
        }
    }

    // All checks are passed, entry is valid
    return true;
}

// Linked actions and corresponding flags
enum EventMask
{
    EVENT_MASK_ON_AGGRO     = FLAG_AGGRO_ON_AGGRO,
    EVENT_MASK_ON_EVADE     = FLAG_RESPAWN_ON_EVADE,
    EVENT_MASK_ON_DIE       = FLAG_DESPAWN_ON_DEATH | FLAG_SELFKILL_ON_DEATH | FLAG_RESPAWN_ON_DEATH | FLAG_FOLLOW,
    EVENT_MASK_ON_RESPAWN   = FLAG_RESPAWN_ON_RESPAWN | FLAG_DESPAWN_ON_RESPAWN | FLAG_FOLLOW,
    EVENT_MASK_TRIGGER_TO   = FLAG_TO_AGGRO_ON_AGGRO | FLAG_TO_RESPAWN_ON_EVADE | FLAG_FOLLOW,
};

// This functions checks if the NPC has linked NPCs for dynamic action
bool CreatureLinkingMgr::IsLinkedEventTrigger(Creature* pCreature)
{
    // TODO could actually be improved to also check for the map
    // Depends if we want to cache this bool into Creature or not
    if (m_eventTriggers.find(pCreature->GetEntry()) != m_eventTriggers.end())
        return true;

    // Also return true for npcs that trigger reverse actions, or for followers(needed in respawn)
    if (CreatureLinkingInfo const* pInfo = GetLinkedTriggerInformation(pCreature))
        return pInfo->linkingFlag & EVENT_MASK_TRIGGER_TO;

    return false;
}

// This function check if the NPC is a master to other NPCs
bool CreatureLinkingMgr::IsLinkedMaster(Creature* pCreature)
{
    return m_eventTriggers.find(pCreature->GetEntry()) != m_eventTriggers.end();
}

// This function checks if the spawning of this NPC is dependend on other NPCs
bool CreatureLinkingMgr::IsSpawnedByLinkedMob(Creature* pCreature)
{
    CreatureLinkingInfo const* pInfo = CreatureLinkingMgr::GetLinkedTriggerInformation(pCreature);

    return pInfo && pInfo->linkingFlag & (FLAG_CANT_SPAWN_IF_BOSS_DEAD | FLAG_CANT_SPAWN_IF_BOSS_ALIVE) && pInfo->masterDBGuid;
}

// This gives the information of a linked NPC (describes action when its ActionTrigger triggers)
// Depends of the map
CreatureLinkingInfo const* CreatureLinkingMgr::GetLinkedTriggerInformation(Creature* pCreature)
{
    CreatureLinkingMapBounds bounds = m_creatureLinkingMap.equal_range(pCreature->GetEntry());
    for (CreatureLinkingMap::const_iterator iter = bounds.first; iter != bounds.second; ++iter)
    {
        if (iter->second.mapId == pCreature->GetMapId())
            return &(iter->second);
    }

    return NULL;
}

// Function to add slave-NPCs to the holder
void CreatureLinkingHolder::AddSlaveToHolder(Creature* pCreature)
{
    CreatureLinkingInfo const* pInfo = sCreatureLinkingMgr.GetLinkedTriggerInformation(pCreature);
    if (!pInfo)
        return;

    // First try to find holder with same flag
    HolderMapBounds bounds = m_holderMap.equal_range(pInfo->masterId);
    for (HolderMap::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second.linkingFlag == pInfo->linkingFlag)
        {
            itr->second.linkedGuids.push_back(pCreature->GetObjectGuid());
            pCreature = NULL;                               // Store that is was handled
            break;
        }
    }

    // If this is a new flag, insert new entry
    if (pCreature)
    {
        FlagAndGuids tmp;
        tmp.linkedGuids.push_back(pCreature->GetObjectGuid());
        tmp.linkingFlag = pInfo->linkingFlag;
        m_holderMap.insert(std::pair<uint32, FlagAndGuids>(pInfo->masterId, tmp));
    }
}

// Function to add master-NPCs to the holder
void CreatureLinkingHolder::AddMasterToHolder(Creature* pCreature)
{
    if (pCreature->IsPet())
        return;

    // Only add master NPCs
    if (!sCreatureLinkingMgr.IsLinkedMaster(pCreature))
        return;

    // TODO More checks needed?
    m_masterGuid[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

// Function to process actions for linked NPCs
void CreatureLinkingHolder::DoCreatureLinkingEvent(CreatureLinkingEvent eventType, Creature* pSource, Unit* pEnemy /* = NULL*/)
{
    // This check will be needed in reload case
    if (!sCreatureLinkingMgr.IsLinkedEventTrigger(pSource))
        return;

    // Ignore atypic behaviour
    if (pSource->IsControlledByPlayer())
        return;

    if (eventType == LINKING_EVENT_AGGRO && !pEnemy)
        return;

    // Process Slaves
    HolderMapBounds bounds = m_holderMap.equal_range(pSource->GetEntry());
    // Get all holders for this boss
    for (HolderMap::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        switch (eventType)
        {
            case LINKING_EVENT_AGGRO:
                if (itr->second.linkingFlag & EVENT_MASK_ON_AGGRO)
                {
                    for (GuidList::iterator slave_itr = itr->second.linkedGuids.begin(); slave_itr != itr->second.linkedGuids.end();)
                    {
                        Creature* pSlave = pSource->GetMap()->GetCreature(*slave_itr);
                        if (!pSlave)
                        {
                            // Remove old guid first
                            itr->second.linkedGuids.erase(slave_itr++);
                            continue;
                        }

                        if (pSlave->IsControlledByPlayer())
                        {
                            ++slave_itr;
                            continue;
                        }

                        if (pSlave->isInCombat())
                            pSlave->SetInCombatWith(pEnemy);
                        else
                            pSlave->AI()->AttackStart(pEnemy);

                        ++slave_itr;
                    }
                }
                break;
            case LINKING_EVENT_EVADE:
                if (itr->second.linkingFlag & EVENT_MASK_ON_EVADE)
                {
                    for (GuidList::iterator slave_itr = itr->second.linkedGuids.begin(); slave_itr != itr->second.linkedGuids.end();)
                    {
                        Creature* pSlave = pSource->GetMap()->GetCreature(*slave_itr);
                        if (!pSlave)
                        {
                            // Remove old guid first
                            itr->second.linkedGuids.erase(slave_itr++);
                            continue;
                        }

                        if (!pSlave->isAlive())
                            pSlave->Respawn();

                        ++slave_itr;
                    }
                }
                break;
            case LINKING_EVENT_DIE:
                if (itr->second.linkingFlag & EVENT_MASK_ON_DIE)
                {
                    for (GuidList::iterator slave_itr = itr->second.linkedGuids.begin(); slave_itr != itr->second.linkedGuids.end();)
                    {
                        Creature* pSlave = pSource->GetMap()->GetCreature(*slave_itr);
                        if (!pSlave)
                        {
                            // Remove old guid first
                            itr->second.linkedGuids.erase(slave_itr++);
                            continue;
                        }

                        // Don't play with pets
                        if (pSlave->IsPet())
                        {
                            ++slave_itr;
                            continue;
                        }

                        if (itr->second.linkingFlag & FLAG_SELFKILL_ON_DEATH && pSlave->isAlive())
                            pSlave->DealDamage(pSlave, pSlave->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        if (itr->second.linkingFlag & FLAG_DESPAWN_ON_DEATH && pSlave->isAlive())
                            pSlave->ForcedDespawn();
                        if (itr->second.linkingFlag & FLAG_RESPAWN_ON_DEATH && !pSlave->isAlive())
                                pSlave->Respawn();

                        ++slave_itr;
                    }
                }
                break;
            case LINKING_EVENT_RESPAWN:
                if (itr->second.linkingFlag & EVENT_MASK_ON_RESPAWN)
                {
                    for (GuidList::iterator slave_itr = itr->second.linkedGuids.begin(); slave_itr != itr->second.linkedGuids.end();)
                    {
                        Creature* pSlave = pSource->GetMap()->GetCreature(*slave_itr);
                        if (!pSlave)
                        {
                            // Remove old guid first
                            itr->second.linkedGuids.erase(slave_itr++);
                            continue;
                        }

                        // Don't play with pets
                        if (pSlave->IsPet())
                        {
                            ++slave_itr;
                            continue;
                        }

                        if (itr->second.linkingFlag & FLAG_RESPAWN_ON_RESPAWN)
                        {
                            // Additional check to prevent endless loops (in case whole group respawns on first respawn)
                            if (!pSlave->isAlive() && pSlave->GetRespawnTime() > time(NULL))
                                pSlave->Respawn();
                        }
                        else if (itr->second.linkingFlag & FLAG_DESPAWN_ON_RESPAWN && pSlave->isAlive())
                            pSlave->ForcedDespawn();

                        if (itr->second.linkingFlag & FLAG_FOLLOW && pSlave->isAlive() && !pSlave->isInCombat())
                            SetFollowing(pSlave, pSource);

                        ++slave_itr;
                    }
                }
                break;
        }
    }

    // Process Master
    if (CreatureLinkingInfo const* pInfo = sCreatureLinkingMgr.GetLinkedTriggerInformation(pSource))
    {
        if ((eventType == LINKING_EVENT_AGGRO && pInfo->linkingFlag & FLAG_TO_AGGRO_ON_AGGRO)
         || (eventType == LINKING_EVENT_EVADE && pInfo->linkingFlag & FLAG_TO_RESPAWN_ON_EVADE)
         || (eventType == LINKING_EVENT_RESPAWN && pInfo->linkingFlag & FLAG_FOLLOW))
        {
            BossGuidMap::const_iterator find = m_masterGuid.find(pInfo->masterId);
            if (find != m_masterGuid.end())
            {
                if (Creature* pMaster = pSource->GetMap()->GetCreature(find->second))
                {
                    switch (eventType)
                    {
                        case LINKING_EVENT_AGGRO:
                            if (pMaster->IsControlledByPlayer())
                                return;

                            if (pMaster->isInCombat())
                                pMaster->SetInCombatWith(pEnemy);
                            else
                                pMaster->AI()->AttackStart(pEnemy);
                            break;
                        case LINKING_EVENT_EVADE:
                            if (!pMaster->isAlive())
                                pMaster->Respawn();
                            break;
                        case LINKING_EVENT_RESPAWN:
                            if (pMaster->isAlive())
                                SetFollowing(pSource, pMaster);
                    }
                }
            }
        }
    }
}

// Helper function to set following
void CreatureLinkingHolder::SetFollowing(Creature* pWho, Creature* pWhom)
{
    // Do some calculations
    float sX, sY, sZ, mX, mY, mZ, mO;
    pWho->GetRespawnCoord(sX, sY, sZ);
    pWhom->GetRespawnCoord(mX, mY, mZ, &mO);

    float dx, dy, dz;
    dx = sX - mX;
    dy = sY - mY;
    dz = sZ - mZ;

    float dist = sqrt(dx*dx + dy*dy + dz*dz);
    // TODO this code needs the same distance calculation that is used for following
    // Atm this means we have to subtract the bounding radiuses
    dist = dist - pWho->GetObjectBoundingRadius() - pWhom->GetObjectBoundingRadius();
    if (dist < 0.0f)
        dist = 0.0f;

    // Need to pass the relative angle to following
    float angle = atan2(dy, dx) - mO;
    angle = (angle >= 0) ? angle : 2 * M_PI_F + angle;

    pWho->GetMotionMaster()->MoveFollow(pWhom, dist, angle);
}

// Function to check if a passive spawning condition is met
bool CreatureLinkingHolder::CanSpawn(Creature* pCreature)
{
    CreatureLinkingInfo const*  pInfo = sCreatureLinkingMgr.GetLinkedTriggerInformation(pCreature);

    if (!pInfo || !pInfo->masterDBGuid)
        return true;

    if (pInfo->linkingFlag & FLAG_CANT_SPAWN_IF_BOSS_DEAD)
        return pCreature->GetMap()->GetPersistentState()->GetCreatureRespawnTime(pInfo->masterDBGuid) == 0;
    else if (pInfo->linkingFlag & FLAG_CANT_SPAWN_IF_BOSS_ALIVE)
        return pCreature->GetMap()->GetPersistentState()->GetCreatureRespawnTime(pInfo->masterDBGuid) > 0;

    return true;
}

/*! @} */
