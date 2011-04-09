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

#include "Common.h"
#include "Policies/SingletonImp.h"
#include "SharedDefines.h"
#include "ObjectMgr.h"
#include "ProgressBar.h"
#include "SocialMgr.h"
#include "LFGMgr.h"
#include "World.h"
#include "Group.h"
#include "Player.h"

#include <limits>

INSTANTIATE_SINGLETON_1(LFGMgr);

LFGMgr::LFGMgr()
{
    m_RewardMap.clear();
    m_queueInfoMap.clear();
    m_dungeonMap.clear();

    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i))
            m_dungeonMap.insert(std::make_pair(dungeon->ID, dungeon));
    }
}

LFGMgr::~LFGMgr()
{
    for (LFGRewardMap::iterator itr = m_RewardMap.begin(); itr != m_RewardMap.end(); ++itr)
        delete itr->second;
    m_RewardMap.clear();
    m_queueInfoMap.clear();
    m_dungeonMap.clear();

}

void LFGMgr::Update(uint32 diff)
{
}

void LFGMgr::LoadRewards()
{
   // (c) TrinityCore, 2010. Rewrited for MaNGOS by /dev/rsa
    for (LFGRewardMap::iterator itr = m_RewardMap.begin(); itr != m_RewardMap.end(); ++itr)
        delete itr->second;
    m_RewardMap.clear();

    uint32 count = 0;
    // ORDER BY is very important for GetRandomDungeonReward!
    QueryResult* result = WorldDatabase.Query("SELECT dungeonId, maxLevel, firstQuestId, firstMoneyVar, firstXPVar, otherQuestId, otherMoneyVar, otherXPVar FROM lfg_dungeon_rewards ORDER BY dungeonId, maxLevel ASC");

    if (!result)
    {
        barGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outErrorDb(">> Loaded 0 LFG dungeon rewards. DB table `lfg_dungeon_rewards` is empty!");
        return;
    }

    barGoLink bar(result->GetRowCount());

    Field* fields = NULL;
    do
    {
        bar.step();
        fields = result->Fetch();
        uint32 dungeonId = fields[0].GetUInt32();
        uint32 maxLevel = fields[1].GetUInt8();
        uint32 firstQuestId = fields[2].GetUInt32();
        uint32 firstMoneyVar = fields[3].GetUInt32();
        uint32 firstXPVar = fields[4].GetUInt32();
        uint32 otherQuestId = fields[5].GetUInt32();
        uint32 otherMoneyVar = fields[6].GetUInt32();
        uint32 otherXPVar = fields[7].GetUInt32();

        if (!sLFGDungeonStore.LookupEntry(dungeonId))
        {
            sLog.outErrorDb("LFGMgr: Dungeon %u specified in table `lfg_dungeon_rewards` does not exist!", dungeonId);
            continue;
        }

        if (!maxLevel || maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        {
            sLog.outErrorDb("LFGMgr: Level %u specified for dungeon %u in table `lfg_dungeon_rewards` can never be reached!", maxLevel, dungeonId);
            maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);
        }

        if (firstQuestId && !sObjectMgr.GetQuestTemplate(firstQuestId))
        {
            sLog.outErrorDb("LFGMgr: First quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", firstQuestId, dungeonId);
            firstQuestId = 0;
        }

        if (otherQuestId && !sObjectMgr.GetQuestTemplate(otherQuestId))
        {
            sLog.outErrorDb("LFGMgr: Other quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", otherQuestId, dungeonId);
            otherQuestId = 0;
        }

        m_RewardMap.insert(LFGRewardMap::value_type(dungeonId, new LFGReward(maxLevel, firstQuestId, firstMoneyVar, firstXPVar, otherQuestId, otherMoneyVar, otherXPVar)));
        ++count;
    }
    while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u LFG dungeon rewards.", count);
}

LFGReward const* LFGMgr::GetRandomDungeonReward(LFGDungeonEntry const* dungeon, Player* player)
{
    LFGReward const* rew = NULL;
    if (player)
    {
        LFGRewardMapBounds bounds = m_RewardMap.equal_range(dungeon->ID);
        for (LFGRewardMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
        {
            // Difficulty check TODO
            rew = itr->second;
            // ordered properly at loading
            if (itr->second->maxLevel >= player->getLevel())
                break;
        }
    }
    return rew;
}

bool LFGMgr::IsRandomDungeon(LFGDungeonEntry const*  dungeon)
{
    if (!dungeon)
        return false;

    return dungeon->type == LFG_TYPE_RANDOM_DUNGEON;
}

void LFGMgr::Join(Player* player)
{
//    LfgDungeonSet* dungeons = NULL;

    ObjectGuid guid;
    Group* group = player->GetGroup();

    if (group)
    {
        if (player->GetObjectGuid() != group->GetLeaderGuid())
            return;
        else
            guid = group->GetObjectGuid();
    }
    else
        guid = player->GetObjectGuid();

    if (guid.IsEmpty())
        return;

    LFGQueueInfoMap::iterator queue = (guid.IsGroup() ? m_groupQueueInfoMap.find(guid) : m_queueInfoMap.find(guid));
    LFGJoinResult result            = LFG_JOIN_OK;

    if (queue != (guid.IsGroup() ? m_groupQueueInfoMap.end() : m_queueInfoMap.end()))
    {
        DEBUG_LOG("LFGMgr::Join: %u trying to join but is already in queue!", guid.GetCounter());
        result = LFG_JOIN_INTERNAL_ERROR;
        player->GetSession()->SendLfgJoinResult(result);
        return;
    }

    result = guid.IsGroup() ? GetGroupJoinResult(group) : GetPlayerJoinResult(player);

    if (result != LFG_JOIN_OK)                              // Someone can't join. Clear all stuf
    {
        DEBUG_LOG("LFGMgr::Join: %u joining with %u members. result: %u", guid.GetCounter(), group ? group->GetMembersCount() : 1, result);
        player->GetLFGState()->Clear();
        player->GetSession()->SendLfgJoinResult(result);
        player->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_ROLECHECK_FAILED);
        return;
    }

    if (!guid.IsGroup() && player->GetLFGState()->GetRoles() == LFG_ROLE_MASK_NONE)
    {
        sLog.outError("LFGMgr::Join: %u has no roles", guid.GetCounter());
    }


    // Joining process
    if (guid.IsGroup())
    {
        for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            if (Player* player = itr->getSource())
                _Leave(player->GetObjectGuid());
        }
        _JoinGroup(guid);
    }
    else
        _Join(guid);
}

void LFGMgr::_Join(ObjectGuid guid)
{
    if (guid.IsEmpty())
        return;

    LFGQueueInfo* pqInfo = new LFGQueueInfo();

    pqInfo->joinTime = time_t(time(NULL));

    // Joining process
    DEBUG_LOG("LFGMgr::AddToQueue: player %u joined", guid.GetCounter());
    m_queueInfoMap[guid] = pqInfo;

}

void LFGMgr::_JoinGroup(ObjectGuid guid)
{
    if (guid.IsEmpty() || !guid.IsGroup())
        return;

    LFGQueueInfo* pqInfo = new LFGQueueInfo();

    pqInfo->joinTime = time_t(time(NULL));

    // Joining process
    DEBUG_LOG("LFGMgr::AddToQueue: group %u joined", guid.GetCounter());
    m_groupQueueInfoMap[guid] = pqInfo;

}

void LFGMgr::Leave(Player* player)
{

    ObjectGuid guid;
    Group* group = player->GetGroup();

    if (group)
    {
        if (player->GetObjectGuid() != group->GetLeaderGuid())
            return;
        else
            guid = group->GetObjectGuid();
    }
    else
        guid = player->GetObjectGuid();

    if (guid.IsEmpty())
        return;

    player->GetLFGState()->SetState(LFG_STATE_NONE);

    _Leave(guid);
}

void LFGMgr::_Leave(ObjectGuid guid)
{
    // leaving process
    LFGQueueInfoMap::iterator queue = m_queueInfoMap.find(guid);
    if (queue != m_queueInfoMap.end())
    {
        delete queue->second;
        m_queueInfoMap.erase(guid);
    }
}

void LFGMgr::_LeaveGroup(ObjectGuid guid)
{
    // leaving process
    LFGQueueInfoMap::iterator queue = m_groupQueueInfoMap.find(guid);
    if (queue == m_groupQueueInfoMap.end())
    {
        sLog.outError("LFGMgr::Leave: group %u trying to leave, but not in in queue!", guid.GetCounter());
    }
    else
    {
        delete queue->second;
        m_groupQueueInfoMap.erase(guid);
    }
}

LFGJoinResult LFGMgr::GetPlayerJoinResult(Player* player)
{

   if (player->InBattleGround() || player->InArena() || player->InBattleGroundQueue())
        return LFG_JOIN_USING_BG_SYSTEM;

   if (player->HasAura(LFG_SPELL_DUNGEON_DESERTER))
        return LFG_JOIN_DESERTER;

    if (player->HasAura(LFG_SPELL_DUNGEON_COOLDOWN))
        return LFG_JOIN_RANDOM_COOLDOWN;

    LFGDungeonSet* dungeons = player->GetLFGState()->GetDungeons();

    if (!dungeons || !dungeons->size())
        return LFG_JOIN_NOT_MEET_REQS;
    // TODO - Check if all dungeons are valid

    return LFG_JOIN_OK;
}

LFGJoinResult LFGMgr::GetGroupJoinResult(Group* group)
{
    if (!group)
        return LFG_JOIN_PARTY_INFO_FAILED;

    if (group->GetMembersCount() > MAX_GROUP_SIZE)
        return LFG_JOIN_TOO_MUCH_MEMBERS;

    for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->getSource();

        if (!player->IsInWorld())
            return LFG_JOIN_DISCONNECTED;

        LFGJoinResult result = GetPlayerJoinResult(player);

        if (result != LFG_JOIN_OK)
            return result;
    }

    return LFG_JOIN_OK;
}

LFGLockStatusMap LFGMgr::GetGroupLockMap(Group* group)
{
    LFGLockStatusMap tmpMap;
    tmpMap.clear();

    if (!group)
        return tmpMap;

//    if (!dungeons)
//        dungeons = GetAllDungeons();
    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->getSource();
        if (!player)
            continue;
        LFGLockStatusMap playerMap = GetPlayerLockMap(player);
        // Concatenate maps
    }
    return tmpMap;
}

LFGLockStatusMap LFGMgr::GetPlayerLockMap(Player* player)
{
    LFGLockStatusMap tmpMap;
    tmpMap.clear();

    if (!player)
        return tmpMap;

    for (uint32 i = 1; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        if (LFGDungeonEntry const* entry = sLFGDungeonStore.LookupEntry(i))
            if (LFGLockStatusType status = GetPlayerLockStatus(player, entry))
                if (status != LFG_LOCKSTATUS_OK)
                    tmpMap.insert(std::make_pair(entry,status));
    }

    return tmpMap;
}

LFGLockStatusType LFGMgr::GetPlayerLockStatus(Player* player, LFGDungeonEntry const* dungeon)
{
    if (!player || !player->IsInWorld())
        return LFG_LOCKSTATUS_RAID_LOCKED;

    if (dungeon->expansion > player->GetSession()->Expansion())
        return LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;

    if (dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL
        && player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
        return  LFG_LOCKSTATUS_RAID_LOCKED;

    if (dungeon->minlevel > player->getLevel())
        return  LFG_LOCKSTATUS_TOO_LOW_LEVEL;

    if (dungeon->maxlevel < player->getLevel())
        return LFG_LOCKSTATUS_TOO_HIGH_LEVEL;

    switch (player->GetAreaLockStatus(dungeon->map, Difficulty(dungeon->difficulty)))
    {
        case AREA_LOCKSTATUS_OK:
            break;
        case AREA_LOCKSTATUS_TOO_LOW_LEVEL:
            return  LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        case AREA_LOCKSTATUS_QUEST_NOT_COMPLETED:
            return LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
        case AREA_LOCKSTATUS_MISSING_ITEM:
            return LFG_LOCKSTATUS_MISSING_ITEM;
        case AREA_LOCKSTATUS_MISSING_DIFFICULTY:
            return LFG_LOCKSTATUS_RAID_LOCKED;
        case AREA_LOCKSTATUS_INSUFFICIENT_EXPANSION:
            return LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        case AREA_LOCKSTATUS_NOT_ALLOWED:
            return LFG_LOCKSTATUS_RAID_LOCKED;
        case AREA_LOCKSTATUS_RAID_LOCKED:
        case AREA_LOCKSTATUS_UNKNOWN_ERROR:
        default:
            return LFG_LOCKSTATUS_RAID_LOCKED;
    }

        /* TODO
            LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE;
            LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE;
            LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL;
            LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL;
            LFG_LOCKSTATUS_NOT_IN_SEASON;
        */

    return LFG_LOCKSTATUS_OK;
}

LFGLockStatusType LFGMgr::GetGroupLockStatus(Group* group, LFGDungeonEntry const* dungeon)
{
    if (!group)
        return LFG_LOCKSTATUS_RAID_LOCKED;

    for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->getSource();

        LFGLockStatusType result = GetPlayerLockStatus(player, dungeon);

        if (result != LFG_LOCKSTATUS_OK)
            return result;
    }
    return LFG_LOCKSTATUS_OK;
}

LFGDungeonSet LFGMgr::GetRandomDungeonsForPlayer(Player* player)
{
    LFGDungeonSet list;
    list.clear();

    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i))
        {
            if (dungeon && 
                dungeon->type == LFG_TYPE_RANDOM_DUNGEON &&
                GetPlayerLockStatus(player, dungeon) == LFG_LOCKSTATUS_OK)
                list.insert(dungeon);
        }
    }
    return list;
}

LFGDungeonEntry const* LFGMgr::GetDungeon(uint32 dungeonID)
{
    LFGDungeonMap::const_iterator itr = m_dungeonMap.find(dungeonID);
    return itr != m_dungeonMap.end() ? itr->second : NULL;
}

void LFGMgr::JoinToLFRList(Player* player, uint32 dungeon)
{
    if (!player)
        return;

    LFGDungeonEntry const* dungeonEntry = GetDungeon(dungeon & 0x00FFFFFF);         // remove the type from the dungeon entry
    if (!dungeonEntry)
        return;

    LFGDungeonSet* dungeons = player->GetLFGState()->GetDungeons();

    if (dungeons)
        dungeons->insert(dungeonEntry);

    // Joining process
    DEBUG_LOG("LFGMgr::LFR List: player %u joined to dungeon %u", player->GetObjectGuid().GetCounter(), dungeonEntry->ID);

}

void LFGMgr::RemoveFromLFRList(Player* player, uint32 dungeon)
{
    if (!player)
        return;

    LFGDungeonEntry const* dungeonEntry = GetDungeon(dungeon & 0x00FFFFFF);         // remove the type from the dungeon entry
    if (!dungeonEntry)
        return;

    LFGDungeonSet* dungeons = player->GetLFGState()->GetDungeons();

    if (!dungeons)
        return;

    dungeons->erase(dungeonEntry);

    // Joining process
    DEBUG_LOG("LFGMgr::LFR List: player %u erase dungeon %u from list", player->GetObjectGuid().GetCounter(), dungeonEntry->ID);

    if (dungeons->size() == 0)
    {
        DEBUG_LOG("LFGMgr::LFR List cleared, player %u leaving LFG queue", player->GetObjectGuid().GetCounter());
        _Leave(player->GetObjectGuid());
    }

}

void LFGMgr::ClearLFRList(Player* player)
{
    if (!player)
        return;

    LFGDungeonSet* dungeons = player->GetLFGState()->GetDungeons();
    dungeons->clear();
    DEBUG_LOG("LFGMgr::LFR List cleared, player %u leaving LFG queue", player->GetObjectGuid().GetCounter());
    _Leave(player->GetObjectGuid());

}
