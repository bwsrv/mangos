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

    for (uint8 i = LFG_TYPE_NONE; i < LFG_TYPE_MAX; ++i)
    {
        m_queueInfoMap[i].clear();
        m_groupQueueInfoMap[i].clear();
    }

    m_dungeonMap.clear();
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(i))
        {
            m_dungeonMap.insert(std::make_pair(dungeon->ID, dungeon));
            m_queueStatus.insert(std::make_pair(dungeon->ID, LFGQueueStatus()));
        }
    }
    m_proposalMap.clear();
}

LFGMgr::~LFGMgr()
{
    for (LFGRewardMap::iterator itr = m_RewardMap.begin(); itr != m_RewardMap.end(); ++itr)
        delete itr->second;
    m_RewardMap.clear();

    for (uint8 i = LFG_TYPE_NONE; i < LFG_TYPE_MAX; ++i)
    {
    // TODO - delete ->second from maps
        m_queueInfoMap[i].clear();
        m_groupQueueInfoMap[i].clear();
    }
    m_dungeonMap.clear();
    m_proposalMap.clear();
    m_queueStatus.clear();
}

void LFGMgr::Update(uint32 diff)
{
    for (uint8 i = LFG_TYPE_NONE; i < LFG_TYPE_MAX; ++i)
    {
        // TODO - main operations
    }
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

    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
        return;

    ObjectGuid guid;
    Group* group = player->GetGroup();

    if (group)
    {
        if (player->GetObjectGuid() != group->GetLeaderGuid())
        {
            player->GetSession()->SendLfgJoinResult(ERR_LFG_NO_SLOTS_PLAYER);
            return;
        }
        else
            guid = group->GetObjectGuid();
    }
    else
        guid = player->GetObjectGuid();

    if (guid.IsEmpty())
        return;

    LFGType type = player->GetLFGState()->GetType();

    if (type == LFG_TYPE_NONE)
    {
        DEBUG_LOG("LFGMgr::Join: %u trying to join without dungeon type. Aborting.", guid.GetCounter());
        player->GetSession()->SendLfgJoinResult(ERR_LFG_INVALID_SLOT);
        return;
    }
    else if (group && type == LFG_TYPE_RAID && !group->isRaidGroup())
    {
        DEBUG_LOG("LFGMgr::Join: %u trying to join to raid finder, but group is not raid. Aborting.", guid.GetCounter());
        player->GetSession()->SendLfgJoinResult(ERR_LFG_MISMATCHED_SLOTS);
        return;
    }
    else if (group && type != LFG_TYPE_RAID && group->isRaidGroup())
    {
        DEBUG_LOG("LFGMgr::Join: %u trying to join to dungeon finder, but group is raid. Aborting.", guid.GetCounter());
        player->GetSession()->SendLfgJoinResult(ERR_LFG_MISMATCHED_SLOTS);
        return;
    }

    LFGQueueInfoMap::iterator queue = (guid.IsGroup() ? m_groupQueueInfoMap[type].find(guid) : m_queueInfoMap[type].find(guid));
    LFGJoinResult result            = ERR_LFG_OK;

    if (queue != (guid.IsGroup() ? m_groupQueueInfoMap[type].end() : m_queueInfoMap[type].end()))
    {
        DEBUG_LOG("LFGMgr::Join: %u trying to join but is already in queue!", guid.GetCounter());
        result = ERR_LFG_NO_LFG_OBJECT;
        player->GetSession()->SendLfgJoinResult(result);
        _Leave(guid);
        _LeaveGroup(guid);
        return;
    }

    result = guid.IsGroup() ? GetGroupJoinResult(group) : GetPlayerJoinResult(player);

    if (result != ERR_LFG_OK)                              // Someone can't join. Clear all stuf
    {
        DEBUG_LOG("LFGMgr::Join: %u joining with %u members. result: %u", guid.GetCounter(), group ? group->GetMembersCount() : 1, result);
        player->GetLFGState()->Clear();
        player->GetSession()->SendLfgJoinResult(result);
        player->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_ROLECHECK_FAILED, type);
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
        _LeaveGroup(guid, type);
        _JoinGroup(guid, type);
    }
    else
    {
        _Leave(guid, type);
        _Join(guid, type);
    }

    player->GetLFGState()->SetState((type == LFG_TYPE_RAID) ? LFG_STATE_LFR : LFG_STATE_LFG);

    player->GetSession()->SendLfgJoinResult(ERR_LFG_OK, 0);

    if (group)
        player->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_JOIN_PROPOSAL, type);
    else
        player->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_JOIN_PROPOSAL, type);

    if(sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL))
        player->JoinLFGChannel();

}

void LFGMgr::_Join(ObjectGuid guid, LFGType type)
{
    if (guid.IsEmpty())
        return;

    LFGQueueInfo* pqInfo = new LFGQueueInfo();

    // Joining process
    DEBUG_LOG("LFGMgr::AddToQueue: player %u joined", guid.GetCounter());
    m_queueInfoMap[type][guid] = pqInfo;

}

void LFGMgr::_JoinGroup(ObjectGuid guid, LFGType type)
{
    if (guid.IsEmpty() || !guid.IsGroup())
        return;

    LFGQueueInfo* pqInfo = new LFGQueueInfo();

    // Joining process
    DEBUG_LOG("LFGMgr::AddToQueue: group %u joined", guid.GetCounter());
    m_groupQueueInfoMap[type][guid] = pqInfo;

}

void LFGMgr::Leave(Group* group)
{
    if (!group)
        return;

    Player* leader = sObjectMgr.GetPlayer(group->GetLeaderGuid());

    if (!leader)
        return;

    Leave(leader);
}

void LFGMgr::Leave(Player* player)
{

    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
        return;

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

    LFGType type = player->GetLFGState()->GetType();

    guid.IsGroup() ? _LeaveGroup(guid) : _Leave(guid);

    player->GetLFGState()->Clear();
    if (group)
    {
        group->GetLFGState()->Clear();
        player->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, type);
    }
    player->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_REMOVED_FROM_QUEUE, type);

    if(sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && player->GetSession()->GetSecurity() == SEC_PLAYER )
        player->LeaveLFGChannel();
}

void LFGMgr::_Leave(ObjectGuid guid, LFGType excludeType)
{
    // leaving process
    for (uint8 i = LFG_TYPE_NONE; i < LFG_TYPE_MAX; ++i)
    {
        if (i == excludeType)
            continue;

        LFGQueueInfoMap::iterator queue = m_queueInfoMap[i].find(guid);
        if (queue != m_queueInfoMap[i].end())
        {
            delete queue->second;
            m_queueInfoMap[i].erase(guid);
        }
    }
}

void LFGMgr::_LeaveGroup(ObjectGuid guid, LFGType excludeType)
{
    // leaving process
    for (uint8 i = LFG_TYPE_NONE; i < LFG_TYPE_MAX; ++i)
    {
        if (i == excludeType)
            continue;

        LFGQueueInfoMap::iterator queue = m_groupQueueInfoMap[i].find(guid);
        if (queue != m_groupQueueInfoMap[i].end())
        {
            delete queue->second;
            m_groupQueueInfoMap[i].erase(guid);
        }
    }
}

LFGJoinResult LFGMgr::GetPlayerJoinResult(Player* player)
{

   if (player->InBattleGround() || player->InArena() || player->InBattleGroundQueue())
        return ERR_LFG_CANT_USE_DUNGEONS;

   if (player->HasAura(LFG_SPELL_DUNGEON_DESERTER))
        return  ERR_LFG_DESERTER_PLAYER;

    if (player->HasAura(LFG_SPELL_DUNGEON_COOLDOWN))
        return ERR_LFG_RANDOM_COOLDOWN_PLAYER;

    LFGDungeonSet* dungeons = player->GetLFGState()->GetDungeons();

    if (!dungeons || !dungeons->size())
        return ERR_LFG_INVALID_SLOT;

    // TODO - Check if all dungeons are valid

    return ERR_LFG_OK;
}

LFGJoinResult LFGMgr::GetGroupJoinResult(Group* group)
{
    if (!group)
        return ERR_LFG_GET_INFO_TIMEOUT;

    if (group->GetMembersCount() > MAX_GROUP_SIZE)
        return ERR_LFG_TOO_MANY_MEMBERS;

    for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->getSource();

        if (!player->IsInWorld())
            return ERR_LFG_MEMBERS_NOT_PRESENT;

        LFGJoinResult result = GetPlayerJoinResult(player);

        if (result != ERR_LFG_OK)
            return result;
    }

    return ERR_LFG_OK;
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

    if (dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL)
    {
        if (AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(dungeon->map))
        {
            uint32 gs = player->GetEquipGearScore(true,true);

            if (at->minGS > 0 && gs < at->minGS)
                return LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE;
            else if (at->maxGS > 0 && gs > at->maxGS)
                return LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE;
        }
        else
            return LFG_LOCKSTATUS_RAID_LOCKED;
    }

    if (InstancePlayerBind* bind = player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
    {
        if (DungeonPersistentState* state = bind->state)
            if (state->IsCompleted())
                return LFG_LOCKSTATUS_RAID_LOCKED;
    }

        /* TODO
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

void LFGMgr::ClearLFRList(Player* player)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
        return;

    if (!player)
        return;

    LFGDungeonSet* dungeons = player->GetLFGState()->GetDungeons();
    dungeons->clear();
    DEBUG_LOG("LFGMgr::LFR List cleared, player %u leaving LFG queue", player->GetObjectGuid().GetCounter());
    _Leave(player->GetObjectGuid());

}

LFGQueueSet LFGMgr::GetDungeonPlayerQueue(LFGDungeonEntry const* dungeon, Team team)
{
    LFGQueueSet tmpSet;
    tmpSet.clear();
    LFGType type = LFG_TYPE_NONE;
    uint32 dungeonID = 0;
    uint8 searchEnd = LFG_TYPE_MAX;
    if (dungeon)
    {
        type = LFGType(dungeon->type);
        dungeonID = dungeon->ID;
        searchEnd = type+1;
    }

    for (uint8 i = type; i < searchEnd; ++i)
    {
        for (LFGQueueInfoMap::iterator itr = m_queueInfoMap[i].begin(); itr != m_queueInfoMap[i].end(); ++itr)
        {
            ObjectGuid guid = itr->first;
            if (!guid.IsPlayer())
                continue;

            Player* player = sObjectMgr.GetPlayer(guid);
            if (!player)
                continue;

            if (team && player->GetTeam() != team)
                continue;

            if (player->GetLFGState()->GetState() < LFG_STATE_LFR ||
                player->GetLFGState()->GetState() > LFG_STATE_PROPOSAL)
                continue;

            if (player->GetLFGState()->GetDungeons()->find(dungeon) == player->GetLFGState()->GetDungeons()->end())
                continue;

            tmpSet.insert(player->GetObjectGuid());
        }
    }
    return tmpSet;
}

LFGQueueSet LFGMgr::GetDungeonGroupQueue(LFGDungeonEntry const* dungeon, Team team)
{
    LFGQueueSet tmpSet;
    tmpSet.clear();
    LFGType type = LFG_TYPE_NONE;
    uint32 dungeonID = 0;
    uint8 searchEnd = LFG_TYPE_MAX;
    if (dungeon)
    {
        type = LFGType(dungeon->type);
        dungeonID = dungeon->ID;
        searchEnd = type+1;
    }

    for (uint8 i = type; i < searchEnd; ++i)
    {
        for (LFGQueueInfoMap::iterator itr = m_groupQueueInfoMap[i].begin(); itr != m_groupQueueInfoMap[i].end(); ++itr)
        {
            ObjectGuid guid = itr->first;
            if (!guid.IsGroup())
                continue;

            Group* group = sObjectMgr.GetGroup(guid);
            if (!group)
                continue;

            Player* player = sObjectMgr.GetPlayer(group->GetLeaderGuid());
            if (!player)
                continue;

            if (team && player->GetTeam() != team)
                continue;

            if (player->GetLFGState()->GetState() < LFG_STATE_LFR ||
                player->GetLFGState()->GetState() > LFG_STATE_PROPOSAL)
                continue;

            if (player->GetLFGState()->GetDungeons()->find(dungeon) == player->GetLFGState()->GetDungeons()->end())
                continue;

            tmpSet.insert(group->GetObjectGuid());
        }
    }
    return tmpSet;
}

void LFGMgr::SendLFGRewards(Player* player)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
        return;

    Group* group = player->GetGroup();
    if (!group || !group->isLFDGroup())
    {
        DEBUG_LOG("LFGMgr::SendLFGReward: %u is not in a group or not a LFGGroup. Ignoring", player->GetObjectGuid().GetCounter());
        return;
    }

    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        if (Player* member = itr->getSource())
            if (member->IsInWorld())
                SendLFGReward(member);
    }
}

void LFGMgr::SendLFGReward(Player* player)
{
    LFGDungeonEntry const* dungeon = *player->GetLFGState()->GetDungeons()->begin();

    if (!dungeon || dungeon->type != LFG_TYPE_RANDOM_DUNGEON)
    {
        DEBUG_LOG("LFGMgr::RewardDungeonDoneFor: %u dungeon is not random", player->GetObjectGuid().GetCounter());
        return;
    }

    // Update achievements
//    if (dungeon->difficulty == DUNGEON_DIFFICULTY_HEROIC)
//        player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS, 1);

    LFGReward const* reward = GetRandomDungeonReward(dungeon, player);

    if (!reward)
        return;

    uint8 index = 0;
    Quest const* qReward = sObjectMgr.GetQuestTemplate(reward->reward[index].questId);
    if (!qReward)
        return;

    // if we can take the quest, means that we haven't done this kind of "run", IE: First Heroic Random of Day.
    if (player->CanRewardQuest(qReward,false))
        player->RewardQuest(qReward,0,NULL,false);
    else
    {
        index = 1;
        qReward = sObjectMgr.GetQuestTemplate(reward->reward[index].questId);
        if (!qReward)
            return;
        // we give reward without informing client (retail does this)
        player->RewardQuest(qReward,0,NULL,false);
    }

    // Give rewards
    DEBUG_LOG("LFGMgr::RewardDungeonDoneFor: %u done dungeon %u, %s previously done.", player->GetObjectGuid().GetCounter(), dungeon->ID, index > 0 ? " " : " not");
    player->GetSession()->SendLfgPlayerReward(dungeon, reward, qReward, index == 0);
}

bool LFGMgr::CreateProposal(LFGDungeonEntry const* dungeon, Group* group)
{
    if (!dungeon)
        return false;

    uint32 ID = GenerateProposalID();
    LFGProposal proposal = LFGProposal(dungeon);
    proposal.cancelTime = time_t(time(NULL)) + LFG_TIME_PROPOSAL;
    proposal.state = LFG_PROPOSAL_INITIATING;
    proposal.group = group;
    m_proposalMap.insert(std::make_pair(ID, proposal));

    if (group)
    {
        group->GetLFGState()->SetProposal(GetProposal(ID));
    }

    DEBUG_LOG("LFGMgr::CreateProposal: %u, dungeon %u, %s", ID, dungeon->ID, group ? " in group" : " not in group");
}

LFGProposal* LFGMgr::GetProposal(uint32 ID)
{
    LFGProposalMap::iterator itr = m_proposalMap.find(ID);
    return itr != m_proposalMap.end() ? &itr->second : NULL;
}

void LFGMgr::RemoveProposal(uint32 ID)
{
    LFGProposalMap::iterator itr = m_proposalMap.find(ID);
    if (itr == m_proposalMap.end())
        return;

    if (Group* group = (*itr).second.group)
    {
        group->GetLFGState()->SetProposal(NULL);
        m_proposalMap.erase(itr);
    }
}

uint32 LFGMgr::GenerateProposalID()
{
    for (int32 i = 1; i != -1  ; ++i)
    {
        if (m_proposalMap.find(i) == m_proposalMap.end())
            return uint32(i);
    }
    return 0;
}

void LFGMgr::OfferContinue(Group* group)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
        return;

    if (group)
    {
        LFGDungeonEntry const* dungeon = *group->GetLFGState()->GetDungeons()->begin();

        if (!dungeon)
        {
            DEBUG_LOG("LFGMgr::OfferContinue: group %u not have selected dungeon!", group->GetObjectGuid().GetCounter());
            return;
        }
        if (Player* leader = sObjectMgr.GetPlayer(group->GetLeaderGuid()))
            leader->GetSession()->SendLfgOfferContinue(dungeon);
    }
    else
        sLog.outError("LFGMgr::OfferContinue: no group!");
}

void LFGMgr::InitBoot(Player* kicker, ObjectGuid victimGuid, std::string reason)
{
    Group* group = kicker->GetGroup();
    Player* victim = sObjectMgr.GetPlayer(victimGuid);

    if (!group || !victim)
        return;

    group->GetLFGState()->SetState(LFG_STATE_BOOT);

    LFGPlayerBoot boot;
    boot.inProgress = true;
    boot.cancelTime = time_t(time(NULL)) + LFG_TIME_BOOT;
    boot.reason     = reason;
    boot.victim     = victimGuid;
    boot.votedNeeded = group->GetLFGState()->GetVotesNeeded();

    // Set votes
    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        if (Player* player = itr->getSource())
        {
            player->GetLFGState()->SetState(LFG_STATE_BOOT);;
            if (player == victim)
                boot.votes[victimGuid] = LFG_ANSWER_DENY;    // Victim auto vote NO
            else if (player == kicker)
                boot.votes[kicker->GetObjectGuid()] = LFG_ANSWER_AGREE;   // Kicker auto vote YES
            else
            {
                boot.votes[player->GetObjectGuid()] = LFG_ANSWER_PENDING;   // Other members need to vote
            }
        }
    }
    m_bootMap.insert(std::make_pair(group->GetObjectGuid(), boot));

    LFGPlayerBoot* pBoot = GetBoot(group->GetObjectGuid());

    if (!pBoot)
        return;

    // Notify players
    for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->getSource();

        if (player && player->IsInWorld())
            player->GetSession()->SendLfgBootPlayer(pBoot);
    }
}

LFGPlayerBoot* LFGMgr::GetBoot(ObjectGuid guid)
{
    LFGBootMap::iterator itr = m_bootMap.find(guid);
    return itr != m_bootMap.end() ? &itr->second : NULL;
}

void LFGMgr::DeleteBoot(ObjectGuid guid)
{
    LFGBootMap::iterator itr = m_bootMap.find(guid);
    if (itr != m_bootMap.end())
        m_bootMap.erase(itr);
}

void LFGMgr::UpdateBoot(Player* player, bool accept)
{
    Group* group = player ? player->GetGroup() : NULL;

    if (!group)
        return;

    LFGPlayerBoot* pBoot = GetBoot(group->GetObjectGuid());

    if (!pBoot)
        return;

    if (pBoot->votes[player->GetObjectGuid()] != LFG_ANSWER_PENDING)          // Cheat check: Player can't vote twice
        return;

    Player* victim = sObjectMgr.GetPlayer(pBoot->victim);
    if (!victim)
        return;

    pBoot->votes[player->GetObjectGuid()] = LFGAnswer(accept);

    uint8 votesNum = 0;
    uint8 agreeNum = 0;

    for (LFGAnswerMap::const_iterator itVotes = pBoot->votes.begin(); itVotes != pBoot->votes.end(); ++itVotes)
    {
        if (itVotes->second != LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (itVotes->second == LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }

    if (agreeNum >= pBoot->votedNeeded ||                  // Vote passed
        votesNum >= pBoot->votes.size() ||                 // All voted but not passed
        (pBoot->votes.size() - votesNum + agreeNum) < pBoot->votedNeeded) // Vote didnt passed
    {
        // Send update info to all players
        pBoot->inProgress = false;
        for (LFGAnswerMap::const_iterator itVotes = pBoot->votes.begin(); itVotes != pBoot->votes.end(); ++itVotes)
        {
            Player* pPlayer = sObjectMgr.GetPlayer(itVotes->first);
            if (pPlayer && (pPlayer != victim))
            {
                pPlayer->GetLFGState()->SetState(LFG_STATE_DUNGEON);
                pPlayer->GetSession()->SendLfgBootPlayer(pBoot);
            }
        }

        group->GetLFGState()->SetState(LFG_STATE_DUNGEON);

        if (agreeNum == pBoot->votedNeeded)                // Vote passed - Kick player
        {
            Player::RemoveFromGroup(group, victim->GetObjectGuid());
            TeleportPlayer(victim, true, false);
            victim->GetLFGState()->Clear();
            OfferContinue(group);
            group->GetLFGState()->DecreaseKicksLeft();
        }
        DeleteBoot(group->GetObjectGuid());
    }
}

void LFGMgr::TeleportPlayer(Player* player, bool out, bool fromOpcode /*= false*/)
{
    DEBUG_LOG("LFGMgr::TeleportPlayer: %u is being teleported %s", player->GetObjectGuid().GetCounter(), out ? "out" : "in");

    if (out)
    {
        player->RemoveAurasDueToSpell(LFG_SPELL_LUCK_OF_THE_DRAW);
        player->TeleportToBGEntryPoint();
        return;
    }

    // TODO Add support for LFG_TELEPORTERROR_FATIGUE
    LFGTeleportError error = LFG_TELEPORTERROR_OK;

    Group* group = player->GetGroup();

    if (!group || !group->isLFDGroup())                        // should never happen, but just in case...
        error = LFG_TELEPORTERROR_INVALID_LOCATION;
    else if (!player->isAlive())
        error = LFG_TELEPORTERROR_PLAYER_DEAD;
//    else if (player->IsFalling())
//        error = LFG_TELEPORTERROR_FALLING;

    uint32 mapid = 0;
    Difficulty difficulty = DUNGEON_DIFFICULTY_NORMAL;
    float x = 0;
    float y = 0;
    float z = 0;
    float orientation = 0;

    if (error == LFG_TELEPORTERROR_OK)
    {
        LFGDungeonEntry const* dungeon = *group->GetLFGState()->GetDungeons()->begin();

        if (!dungeon)
        {
            error = LFG_TELEPORTERROR_INVALID_LOCATION;
        }
        else if (group->GetDungeonDifficulty() != dungeon->difficulty)
        {
//    group->SetDungeonDifficulty(Difficulty(dungeon->difficulty));
            error = LFG_TELEPORTERROR_UNK4;
        }
        else if (AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(dungeon->map))
        {
            difficulty = Difficulty(dungeon->difficulty);
            mapid = at->target_mapId;
            x = at->target_X;
            y = at->target_Y;
            z = at->target_Z;
            orientation = at->target_Orientation;
        }
    }

    if (error == LFG_TELEPORTERROR_OK)
    {

        if (player->GetMap() && !player->GetMap()->IsDungeon() && !player->GetMap()->IsRaid() && !player->InBattleGround())
            player->SetBattleGroundEntryPoint();

        // stop taxi flight at port
        if (player->IsTaxiFlying())
        {
            player->GetMotionMaster()->MovementExpired();
            player->m_taxi.ClearTaxiDestinations();
        }

        player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        player->RemoveSpellsCausingAura(SPELL_AURA_FLY);

        DETAIL_LOG("LFGMgr: Sending %s to map %u, difficulty %u X %f, Y %f, Z %f, O %f", player->GetName(), uint8(difficulty), mapid, x, y, z, orientation);

        player->TeleportTo(mapid, x, y, z, orientation);
    }
    else
        player->GetSession()->SendLfgTeleportError(error);
}

LFGQueueStatus* LFGMgr::GetDungeonQueueStatus(uint32 dungeonID)
{
    LFGQueueStatusMap::iterator itr = m_queueStatus.find(dungeonID);
    return itr != m_queueStatus.end() ? &itr->second : NULL;
}

void LFGMgr::UpdateRoleCheck(Group* group)
{
    if (!group)
        return;

//        roleCheck->state = LFG_ROLECHECK_NO_ROLE;
    if (!group->GetLFGState()->IsRoleCheckActive())
        return;

    LFGRoleCheckState state = group->GetLFGState()->GetRoleCheckState();
    LFGRoleCheckState newstate = LFG_ROLECHECK_NONE;

    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        if (Player* member = itr->getSource())
        {
            if (member->IsInWorld())
            {
                if (uint8(member->GetLFGState()->GetRoles()) < LFG_ROLE_MASK_TANK)
                    newstate = LFG_ROLECHECK_MISSING_ROLE;
            }
        }
    }

    if (newstate == LFG_ROLECHECK_MISSING_ROLE)
        return;

    if (!CheckRoles(group))
        newstate == LFG_ROLECHECK_WRONG_ROLES;
    else 
        newstate = LFG_ROLECHECK_FINISHED;

    if (newstate == LFG_ROLECHECK_WRONG_ROLES)
        return;

/*
    uint8 team = 0;
    LfgDungeonSet dungeons;
    if (roleCheck->rDungeonId)
        dungeons.insert(roleCheck->rDungeonId);
    else
        dungeons = roleCheck->dungeons;

//    LfgJoinResultData joinData = LfgJoinResultData(LFG_JOIN_FAILED, roleCheck->state);
    for (LfgRolesMap::const_iterator it = roleCheck->roles.begin(); it != roleCheck->roles.end(); ++it)
    {
        uint64 pguid = it->first;
        Player* plrg = sObjectMgr->GetPlayer(pguid);
        if (!plrg)
        {
            if (roleCheck->state == LFG_ROLECHECK_FINISHED)
                SetState(pguid, LFG_STATE_QUEUED);
            else if (roleCheck->state != LFG_ROLECHECK_INITIALITING)
                ClearState(pguid);
            continue;
        }

        team = uint8(plrg->GetTeam());
        if (!sendRoleChosen)
            plrg->GetSession()->SendLfgRoleChosen(guid, roles);
        plrg->GetSession()->SendLfgRoleCheckUpdate(roleCheck);
        switch (roleCheck->state)
        {
            case LFG_ROLECHECK_INITIALITING:
                continue;
            case LFG_ROLECHECK_FINISHED:
                SetState(pguid, LFG_STATE_QUEUED);
                plrg->GetSession()->SendLfgUpdateParty(LfgUpdateData(LFG_UPDATETYPE_ADDED_TO_QUEUE, dungeons, GetComment(pguid)));
                break;
            default:
                if (roleCheck->leader == pguid)
                    plrg->GetSession()->SendLfgJoinResult(joinData);
                plrg->GetSession()->SendLfgUpdateParty(LfgUpdateData(LFG_UPDATETYPE_ROLECHECK_FAILED));
                ClearState(pguid);
                break;
        }
    }

    if (roleCheck->state == LFG_ROLECHECK_FINISHED)
    {
        SetState(gguid, LFG_STATE_QUEUED);
        LfgQueueInfo* pqInfo = new LfgQueueInfo();
        pqInfo->joinTime = time_t(time(NULL));
        pqInfo->roles = roleCheck->roles;
        pqInfo->dungeons = roleCheck->dungeons;

        // Set queue roles needed - As we are using check_roles will not have more that 1 tank, 1 healer, 3 dps
        for (LfgRolesMap::const_iterator it = check_roles.begin(); it != check_roles.end(); ++it)
        {
            uint8 roles = it->second;
            if (roles & ROLE_TANK)
                --pqInfo->tanks;
            else if (roles & ROLE_HEALER)
                --pqInfo->healers;
            else
                --pqInfo->dps;
        }

        m_QueueInfoMap[gguid] = pqInfo;
        AddToQueue(gguid, team);
    }
*/
}

bool LFGMgr::CheckRoles(Group* group)
{
    return true;
}
