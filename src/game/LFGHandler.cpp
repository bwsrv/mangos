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

#include "WorldSession.h"
#include "Log.h"
#include "Database/DatabaseEnv.h"
#include "Player.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "World.h"
#include "LFGMgr.h"

void WorldSession::HandleLfgJoinOpcode( WorldPacket & recv_data )
{
    if (!GetPlayer())
        return;

    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
    {
        recv_data.rpos(recv_data.wpos());
        DEBUG_LOG("CMSG_LFG_JOIN %u failed - Dungeon finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }
    else
        DEBUG_LOG("CMSG_LFG_JOIN %u processing...", GetPlayer()->GetObjectGuid().GetCounter());

    uint8  numDungeons;
    uint32 dungeon;
    uint32 roles;
    std::string comment;
    LFGDungeonSet* newDungeons = GetPlayer()->GetLFGState()->GetDungeons();


    recv_data >> roles;                                     // lfg roles
    recv_data >> Unused<uint8>();                           // unk1 (unused?)
    recv_data >> Unused<uint8>();                           // unk2 (unused?)

    recv_data >> numDungeons;
    if (!numDungeons)
    {
        DEBUG_LOG("CMSG_LFG_JOIN %u no dungeons selected", GetPlayer()->GetObjectGuid().GetCounter());
        recv_data.rpos(recv_data.wpos());
        return;
    }

    newDungeons->clear();

    for (int8 i = 0 ; i < numDungeons; ++i)
    {
        recv_data >> dungeon;
        newDungeons->insert(sLFGMgr.GetDungeon(dungeon & 0x00FFFFFF));         // remove the type from the dungeon entry
    }

    uint8 counter2;                                         // unk - always 3
    recv_data >> counter2;
    for (uint8 i = 0; i < counter2; i++)
        recv_data >> Unused<uint8>();                       // unk (unused?)

    recv_data >> comment;                                   // lfg comment

    GetPlayer()->GetLFGState()->SetRoles(roles);

    GetPlayer()->GetLFGState()->SetComment(comment);

    DEBUG_LOG("CMSG_LFG_JOIN %u as group: %u  Dungeons: %u", GetPlayer()->GetObjectGuid().GetCounter(), GetPlayer()->GetGroup() ? 1 : 0, uint8(newDungeons->size()));

    sLFGMgr.Join(GetPlayer());
}

void WorldSession::HandleLfgLeaveOpcode( WorldPacket & /*recv_data*/ )
{
    Group* group = GetPlayer()->GetGroup();

    DEBUG_LOG("CMSG_LFG_LEAVE %u in group: %u", GetPlayer()->GetObjectGuid().GetCounter(), group ? 1 : 0);

    // Check cheating - only leader can leave the queue
    if (!group || group->GetLeaderGuid() == GetPlayer()->GetObjectGuid())
        sLFGMgr.Leave(GetPlayer());

}

void WorldSession::HandleLfrSearchOpcode( WorldPacket & recv_data )
{
    uint32 entry;                                           // Raid id to search
    recv_data >> entry;
    DEBUG_LOG("CMSG_SEARCH_LFG_JOIN %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), entry);
    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
    {
        DEBUG_LOG("CMSG_SEARCH_LFG_JOIN %u failed - Raid finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }
    SendLfgUpdateList(entry & 0x00FFFFFF);
}

void WorldSession::HandleLfrLeaveOpcode( WorldPacket & recv_data )
{
    uint32 entry;                                          // Raid id queue to leave
    recv_data >> entry;

    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
    {
        DEBUG_LOG("CMSG_SEARCH_LFG_LEAVE %u failed - Raid finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    DEBUG_LOG("CMSG_SEARCH_LFG_LEAVE %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), entry);

    Group* group = GetPlayer()->GetGroup();

//    if (!group || group->GetLeaderGuid() == GetPlayer()->GetObjectGuid())
    if (!group)
    {
        GetPlayer()->GetLFGState()->GetDungeons()->erase(sLFGMgr.GetDungeon(entry & 0x00FFFFFF));

        if (GetPlayer()->GetLFGState()->GetDungeons()->empty())
            sLFGMgr.Leave(GetPlayer());
    }
}

void WorldSession::HandleLfgClearOpcode( WorldPacket & /*recv_data */ )
{
    // empty packet
    DEBUG_LOG("CMSG_CLEAR_LOOKING_FOR_GROUP %u ", GetPlayer()->GetObjectGuid().GetCounter());

    sLFGMgr.ClearLFRList(GetPlayer());

    if(sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && _player->GetSession()->GetSecurity() == SEC_PLAYER )
        GetPlayer()->LeaveLFGChannel();

}

void WorldSession::HandleSetLfgCommentOpcode( WorldPacket & recv_data )
{
    std::string comment;
    recv_data >> comment;

    DEBUG_LOG("CMSG_SET_LFG_COMMENT: [%s]", comment.c_str());

    GetPlayer()->GetLFGState()->SetComment(comment);
}

void WorldSession::HandleLfgSetRolesOpcode(WorldPacket &recv_data)
{
    uint8 roles;
    recv_data >> roles;

    DEBUG_LOG("CMSG_LFG_SET_ROLES %u roles %u", GetPlayer()->GetObjectGuid().GetCounter(), roles);

    GetPlayer()->GetLFGState()->SetRoles(roles);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPacket &recv_data)
{
    bool agree;                                             // Agree to kick player
    recv_data >> agree;

    DEBUG_LOG("CMSG_LFG_SET_BOOT_VOTE %u agree: %u", GetPlayer()->GetObjectGuid().GetCounter(), agree ? 1 : 0);
    //sLFGMgr.UpdateBoot(GetPlayer(), agree);
}

void WorldSession::HandleLfgTeleportOpcode(WorldPacket &recv_data)
{
    bool agree;
    recv_data >> agree;

    DEBUG_LOG("CMSG_LFG_TELEPORT  %u teleport: %u", GetPlayer()->GetObjectGuid().GetCounter(), agree ? 1 : 0);
    //sLFGMgr.TeleportPlayer(GetPlayer(), agree);
}

void WorldSession::HandleLfgPlayerLockInfoRequestOpcode(WorldPacket &/*recv_data*/)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
    {
        DEBUG_LOG("CMSG_LFD_PLAYER_LOCK_INFO_REQUEST %u failed - Dungeon finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    DEBUG_LOG("CMSG_LFD_PLAYER_LOCK_INFO_REQUEST %u ", GetPlayer()->GetObjectGuid().GetCounter());

    uint32 rsize = 0;
    uint32 lsize = 0;
    LFGDungeonSet    randomlist = sLFGMgr.GetRandomDungeonsForPlayer(GetPlayer());
    LFGLockStatusMap*   lockSet = GetPlayer()->GetLFGState()->GetLockMap();

    rsize = randomlist.size();

    if (lockSet)
        lsize = lockSet->size();

    WorldPacket data(SMSG_LFG_PLAYER_INFO, 1 + rsize * (4 + 1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4) + 4 + lsize * (1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4));

    if (rsize == 0)
    {
        data << uint8(0);
    }
    else
    {
        uint8 done;
        data << uint8(rsize);                               // Random Dungeon count
        for (LFGDungeonSet::const_iterator itr = randomlist.begin(); itr != randomlist.end(); ++itr)
        {
            data << uint32((*itr)->Entry());                     // Entry
            LFGReward const* reward = sLFGMgr.GetRandomDungeonReward(*itr,GetPlayer());
            Quest const* qRew = NULL;
            qRew = NULL;
            if (reward)
            {
                qRew = sObjectMgr.GetQuestTemplate(reward->reward[0].questId);
                if (qRew)
                {
                    done = !GetPlayer()->CanRewardQuest(qRew,false);
                    if (done)
                        qRew = sObjectMgr.GetQuestTemplate(reward->reward[1].questId);
                }

            }
            if (qRew)
            {
                data << uint8(done);
                data << uint32(qRew->GetRewOrReqMoney());
                data << uint32(qRew->XPValue(GetPlayer()));
                data << uint32(reward->reward[done].variableMoney);
                data << uint32(reward->reward[done].variableXP);
                data << uint8(qRew->GetRewItemsCount());
                if (qRew->GetRewItemsCount())
                {
                    ItemPrototype const* iProto = NULL;
                    for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
                    {
                        if (!qRew->RewItemId[i])
                            continue;

                        iProto = ObjectMgr::GetItemPrototype(qRew->RewItemId[i]);

                        data << uint32(qRew->RewItemId[i]);
                        data << uint32(iProto ? iProto->DisplayInfoID : 0);
                        data << uint32(qRew->RewItemCount[i]);
                    }
                }
            }
            else
            {
                data << uint8(0);
                data << uint32(0);
                data << uint32(0);
                data << uint32(0);
                data << uint32(0);
                data << uint8(0);
            }
        }
    }

    if (!lockSet || lsize == 0)
    {
        data << uint8(0);
    }
    else
    {
        data << uint32(lockSet->size());                             // Size of lock dungeons
        for (LFGLockStatusMap::iterator itr = lockSet->begin(); itr != lockSet->end(); ++itr)
        {
            data << uint32((*itr->first).Entry());                   // Dungeon entry + type
            data << uint32(itr->second);                             // Lock status
        }
    }
    SendPacket(&data);
}

void WorldSession::HandleLfgPartyLockInfoRequestOpcode(WorldPacket & /*recv_data*/)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
    {
        DEBUG_LOG("CMSG_LFD_PARTY_LOCK_INFO_REQUEST %u failed - Dungeon finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    DEBUG_LOG("CMSG_LFD_PARTY_LOCK_INFO_REQUEST %u", GetPlayer()->GetObjectGuid().GetCounter());
    uint32 size = 0;

    Group* group = GetPlayer()->GetGroup();
    if (!group)
    {
        WorldPacket data(SMSG_LFG_PARTY_INFO, 1 + size);
        data << uint8(0);
        SendPacket(&data);
    }
    else
    {
        std::map<ObjectGuid,LFGLockStatusMap*> lockMap;
        lockMap.clear();
        uint8 membersCount = 0;

        for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* player = itr->getSource();

            if (!player->IsInWorld())
                continue;

            LFGLockStatusMap* lockSet = player->GetLFGState()->GetLockMap();

            size += 8 + 4 + lockSet->size() * (4 + 4);

            lockMap.insert(std::make_pair(player->GetObjectGuid(), lockSet));
            membersCount++;
        }

        WorldPacket data(SMSG_LFG_PARTY_INFO, 1 + size);

        data << membersCount;

        for (std::map<ObjectGuid,LFGLockStatusMap*>::const_iterator  itr1 = lockMap.begin(); itr1 != lockMap.end(); ++itr1)
        {

            data << itr1->first;
            for (LFGLockStatusMap::iterator itr2 = itr1->second->begin(); itr2 != itr1->second->end(); ++itr2)
            {
                data << uint32((*itr2->first).Entry());                   // Dungeon entry + type
                data << uint32(itr2->second);                             // Lock status
            }
        }

        GetPlayer()->GetGroup()->BroadcastPacket(&data, false);
    }
}

void WorldSession::HandleLfgProposalResultOpcode(WorldPacket &recv_data)
{
    uint32 lfgGroupID;                                      // Internal lfgGroupID
    bool   accept;                                          // Accept to join?
    recv_data >> lfgGroupID;
    recv_data >> accept;

    DEBUG_LOG("CMSG_LFG_PROPOSAL_RESULT %u proposal: %u accept: %u", GetPlayer()->GetObjectGuid().GetCounter(), lfgGroupID, accept ? 1 : 0);
    //sLFGMgr.UpdateProposal(lfgGroupID, GetPlayer()->GetObjectGuid().GetCounter(), accept);
}

void WorldSession::SendLfgJoinResult(LFGJoinResult checkResult, uint8 checkValue, bool withLockMap)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
    {
        DEBUG_LOG("SendLfgJoinResult %u failed - Dungeon finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    uint32 size = 0;

    Group* group = GetPlayer()->GetGroup();
    uint8 membersCount = 0;
    std::map<ObjectGuid,LFGLockStatusMap*> lockMap;
    lockMap.clear();

    if (group && withLockMap)
    {
        size +=1;
        for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* player = itr->getSource();

            if (!player->IsInWorld())
                continue;

            LFGLockStatusMap* lockSet = player->GetLFGState()->GetLockMap();

            size += 8 + 4 + lockSet->size() * (4 + 4);

            lockMap.insert(std::make_pair(player->GetObjectGuid(), lockSet));
            membersCount++;
        }
    }

    DEBUG_LOG("SMSG_LFG_JOIN_RESULT checkResult: %u checkValue: %u", checkResult, checkValue);

    WorldPacket data(SMSG_LFG_JOIN_RESULT, 4 + 4 + size);

    data << uint32(checkResult);                            // Check Result
    data << uint32(checkValue);                             // Check Value

    if (group && withLockMap)
    {
        data << membersCount;

        for (std::map<ObjectGuid,LFGLockStatusMap*>::const_iterator  itr1 = lockMap.begin(); itr1 != lockMap.end(); ++itr1)
        {

            data << itr1->first;
            for (LFGLockStatusMap::iterator itr2 = itr1->second->begin(); itr2 != itr1->second->end(); ++itr2)
            {
                data << uint32((*itr2->first).Entry());                   // Dungeon entry + type
                data << uint32(itr2->second);                             // Lock status
            }
        }
    }

    if(GetPlayer()->GetGroup())
        GetPlayer()->GetGroup()->BroadcastPacket(&data, false);
    else
        SendPacket(&data);
}

void WorldSession::SendLfgUpdateParty(LFGUpdateType updateType, LFGType type)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
    {
        DEBUG_LOG("SendLfgUpdateParty %u failed - Dungeon finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    bool join = false;
    bool extrainfo = false;
    bool queued = false;

    switch(updateType)
    {
        case LFG_UPDATETYPE_JOIN_PROPOSAL:
            extrainfo = true;
            if (type != LFG_TYPE_RAID)
                queued = true;
            else
                join = true;
            break;
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            extrainfo = true;
            join = true;
            queued = true;
            break;
        case LFG_UPDATETYPE_CLEAR_LOCK_LIST:
            // join = true;  
            // TODO: Sometimes queued and extrainfo - Check ocurrences...
            queued = true;
            extrainfo = false;
            break;
        case LFG_UPDATETYPE_PROPOSAL_BEGIN:
            extrainfo = true;
            join = true;
            break;
    }

    LFGDungeonSet* dungeons = GetPlayer()->GetLFGState()->GetDungeons();
    uint8 size = dungeons->size();
    std::string comment = GetPlayer()->GetLFGState()->GetComment();

    DEBUG_LOG("SMSG_LFG_UPDATE_PARTY updatetype: %u", updateType);

    WorldPacket data(SMSG_LFG_UPDATE_PARTY, 1 + 1 + (extrainfo ? 1 : 0) * (1 + 1 + 1 + 1 + 1 + size * 4 + comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(join);                                // LFG Join
        data << uint8(queued);                              // Join the queue
        for (uint8 i = 0; i < 5; ++i)
            data << uint8(0);                               // unk - Always 0

        data << uint8(size);

        for (LFGDungeonSet::const_iterator itr = dungeons->begin(); itr != dungeons->end(); ++itr)
            data << uint32((*itr)->Entry());

        data << comment.c_str();
    }

    if(GetPlayer()->GetGroup())
        GetPlayer()->GetGroup()->BroadcastPacket(&data, false);
    else
        SendPacket(&data);
}

void WorldSession::SendLfgUpdatePlayer(LFGUpdateType updateType, LFGType type)
{

    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE) && !sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
    {
        DEBUG_LOG("SendLfgUpdatePlayer %u failed - Dungeon finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    bool queued = false;
    bool extrainfo = false;

    switch(updateType)
    {
        case LFG_UPDATETYPE_JOIN_PROPOSAL:
            extrainfo = true;
            if (type != LFG_TYPE_RAID)
                queued = true;
            break;
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            queued = true;
            break;
        //case LFG_UPDATETYPE_CLEAR_LOCK_LIST: // TODO: Sometimes has extrainfo - Check ocurrences...
        case LFG_UPDATETYPE_PROPOSAL_BEGIN:
            extrainfo = true;
            break;
    }
    LFGDungeonSet* dungeons = GetPlayer()->GetLFGState()->GetDungeons();
    uint8 size = dungeons->size();
    std::string comment = GetPlayer()->GetLFGState()->GetComment();

    DEBUG_LOG("SMSG_LFG_UPDATE_PLAYER %u updatetype: %u", GetPlayer()->GetObjectGuid().GetCounter(), updateType);

    WorldPacket data(SMSG_LFG_UPDATE_PLAYER, 1 + 1 + (extrainfo ? 1 : 0) * (1 + 1 + 1 + 1 + size * 4 + comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(queued);                              // Join the queue
        data << uint8(0);                                   // unk - Always 0
        data << uint8(0);                                   // unk - Always 0

        data << uint8(size);

        for (LFGDungeonSet::const_iterator itr = dungeons->begin(); itr != dungeons->end(); ++itr)
            data << uint32((*itr)->Entry());
        data << comment.c_str();
    }
    SendPacket(&data);
}


void WorldSession::SendLfgUpdateList(uint32 dungeonEntry)
{
    if (!sWorld.getConfig(CONFIG_BOOL_LFR_ENABLE))
    {
        DEBUG_LOG("SendLfgUpdateList %u failed - Raid finder disabled", GetPlayer()->GetObjectGuid().GetCounter());
        return;
    }

    DEBUG_LOG("SMSG_LFG_SEARCH_RESULTS %u dungeonentry: %u ", GetPlayer()->GetObjectGuid().GetCounter(), dungeonEntry);

    LFGDungeonEntry const* dungeon = sLFGMgr.GetDungeon(dungeonEntry);

    if (!dungeonEntry)
        return;

    Team team = sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP) ? TEAM_NONE : GetPlayer()->GetTeam();

    LFGQueueGroupSet   groups = sLFGMgr.GetDungeonGroupQueue(dungeon, team);
    LFGQueuePlayerSet players = sLFGMgr.GetDungeonPlayerQueue(dungeon, team);

    uint32 groupCount = groups.size();
    uint32 groupSize = 4+4;

    for (LFGQueueGroupSet::const_iterator itr = groups.begin(); itr != groups.end(); ++itr)
    {
        Player* leader = sObjectMgr.GetPlayer((*itr)->GetLeaderGuid());

        if (!leader)
            continue;
        leader->GetLFGState()->AddFlags(LFG_MEMBER_FLAG_CHARINFO |
                                        LFG_MEMBER_FLAG_COMMENT |
                                        LFG_MEMBER_FLAG_GROUPLEADER |
                                        LFG_MEMBER_FLAG_GROUPGUID |
                                        LFG_MEMBER_FLAG_ROLES |
                                        LFG_MEMBER_FLAG_AREA  |
                                        LFG_MEMBER_FLAG_STATUS  |
                                        LFG_MEMBER_FLAG_BIND);

        uint32 flags = leader->GetLFGState()->GetFlags();

        if (flags & LFG_MEMBER_FLAG_COMMENT)
            groupSize += leader->GetLFGState()->GetComment().size()+1;
        if (flags & LFG_MEMBER_FLAG_ROLES)
            groupSize +=3;
        if (flags & LFG_MEMBER_FLAG_BIND)
            groupSize += (8+4);

        players.insert(leader);

        for (GroupReference *itr2 = (*itr)->GetFirstMember(); itr2 != NULL; itr2 = itr2->next())
        {
            Player* player = itr2->getSource();

            if (!player || player == leader)
                continue;

            player->GetLFGState()->AddFlags(LFG_MEMBER_FLAG_GROUPLEADER |
                                        LFG_MEMBER_FLAG_GROUPGUID |
                                        LFG_MEMBER_FLAG_STATUS);

            player->GetLFGState()->RemoveFlags(LFG_MEMBER_FLAG_CHARINFO |
                                        LFG_MEMBER_FLAG_COMMENT |
                                        LFG_MEMBER_FLAG_ROLES |
                                        LFG_MEMBER_FLAG_AREA  |
                                        LFG_MEMBER_FLAG_BIND);
            players.insert(player);
        }

    }

    uint32 playerCount = players.size();
    uint32 playerSize = 4+4;

    uint32 guidsSize = 0;
    LFGQueuePlayerSet playersUpdated;
    playersUpdated.clear();

    for(LFGQueuePlayerSet::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        uint32 flags = (*itr)->GetLFGState()->GetFlags();

        playerSize += (8+4);

        if (flags &  LFG_MEMBER_FLAG_CHARINFO)
            playerSize += (1+1+1+3+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+4);

        if (flags & LFG_MEMBER_FLAG_COMMENT)
            playerSize += (*itr)->GetLFGState()->GetComment().size()+1;         // comment

        if (flags & LFG_MEMBER_FLAG_GROUPLEADER)
            playerSize += 1;

        if (flags & LFG_MEMBER_FLAG_GROUPGUID)
            playerSize += 8;

        if (flags & LFG_MEMBER_FLAG_ROLES)
            playerSize += 1;

        if (flags & LFG_MEMBER_FLAG_AREA)
            playerSize += 4;

        if (flags & LFG_MEMBER_FLAG_STATUS)
            playerSize += 1;

        if (flags & LFG_MEMBER_FLAG_BIND)
            playerSize += (8+4);

        if (flags & LFG_MEMBER_FLAG_UPDATE)
        {
            (*itr)->GetLFGState()->RemoveFlags(LFG_MEMBER_FLAG_UPDATE);
            playersUpdated.insert(*itr);
            guidsSize += 8;
        }

    }

    WorldPacket data(SMSG_LFG_SEARCH_RESULTS, 4 + 4 + 1 + groupSize + playerSize + guidsSize);

    data << uint32(dungeon->type);                           // type
    data << uint32(dungeon->Entry());                        // entry from LFGDungeons.dbc

    if (!playersUpdated.empty())
    {
        data << uint8(playersUpdated.size());
        for (LFGQueuePlayerSet::const_iterator itr = playersUpdated.begin(); itr != playersUpdated.end(); ++itr)
        {
            data << (*itr)->GetObjectGuid();                 // player guid
        }
    }
    else
        data << uint8(0);


    if (!groups.empty())
    {
        data << uint32(groupCount);                          // groups count
        data << uint32(groupCount);                          // groups count2

        for (LFGQueueGroupSet::const_iterator itr = groups.begin(); itr != groups.end(); ++itr)
        {
            Group* group = *itr;

            Player* leader = sObjectMgr.GetPlayer(group->GetLeaderGuid());

            uint32 flags = leader->GetLFGState()->GetFlags();

            data << group->GetObjectGuid();

            data << flags;

            if (flags & LFG_MEMBER_FLAG_COMMENT)
            {
                data << leader->GetLFGState()->GetComment().c_str();
            }

            if (flags & LFG_MEMBER_FLAG_ROLES)
            {
                for (int i = 0; i < 3; ++i)
                {
                    data << uint8(group->GetLFGState()->GetRoles(LFGRoles(i+1)));
                }
            }

            if (flags & LFG_MEMBER_FLAG_BIND)
            {
                ObjectGuid instanceGuid;
                uint32 encounters = 0;
                if (InstancePlayerBind* bind = leader->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
                {
                    if (DungeonPersistentState* state = bind->state)
                    {
                        instanceGuid = state->GetInstanceGuid();
                        encounters = state->GetCompletedEncountersMask();
                    }
                }
                data << instanceGuid;
                data << encounters;
            }
        }
    }
    else
    {
        data << uint32(0);
        data << uint32(0);                                       // groups count2
    }


    if (!players.empty())
    {
        data << uint32(playerCount);                           // players count
        data << uint32(playerCount);                           // players count 2

        for(LFGQueuePlayerSet::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = *itr;

            uint32 flags = player->GetLFGState()->GetFlags();

            data << player->GetObjectGuid();                      // guid

            data << flags;                                        // flags

            if (flags &  LFG_MEMBER_FLAG_CHARINFO)                // charinfo
            {
                data << uint8(player->getLevel());
                data << uint8(player->getClass());
                data << uint8(player->getRace());

                for(int i = 0; i < 3; ++i)
                    data << uint8(player->GetTalentsCount(i));                                    // spent talents count in specific tab

                data << uint32(player->GetArmor());                                                 // armor
                data << uint32(player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SPELL));          // spd
                data << uint32(player->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_SPELL));         // heal
                data << uint32(player->GetRatingBonusValue(CR_CRIT_MELEE));                        // crit rating melee
                data << uint32(player->GetRatingBonusValue(CR_CRIT_RANGED));                       // crit rating ranged
                data << uint32(player->GetRatingBonusValue(CR_CRIT_SPELL));                        // crit rating spell
                data << float(player->GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER)*5);                             // mp5
                data << float(player->GetFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER)*5);                 // unk
                data << uint32(player->GetTotalAttackPowerValue(BASE_ATTACK));                      // attack power
                data << uint32(player->GetTotalStatValue(STAT_AGILITY));                            // agility
                data << uint32(player->GetMaxHealth());                                             // health
                data << uint32(player->GetMaxPower(player->getPowerType()));                        // power
                data << uint32(0);                              // unk
                data << float(0);                               // unk
                data << uint32(player->GetRatingBonusValue(CR_DEFENSE_SKILL));                      // defence rating
                data << uint32(player->GetRatingBonusValue(CR_DODGE));                              // dodge rating
                data << uint32(player->GetRatingBonusValue(CR_BLOCK));                              // block rating
                data << uint32(player->GetRatingBonusValue(CR_PARRY));                              // parry rating
                data << uint32(player->GetRatingBonusValue(CR_HASTE_MELEE));                         // crit rating
                data << uint32(player->GetRatingBonusValue(CR_EXPERTISE));                          // expertize
            }

            if (flags & LFG_MEMBER_FLAG_COMMENT)
                data << player->GetLFGState()->GetComment().c_str();         // comment

            if (flags & LFG_MEMBER_FLAG_GROUPLEADER)                         // Group leader flag
            {
                bool isLeader = false;
                if (Group* group = player->GetGroup())
                    if (player->GetObjectGuid() == group->GetLeaderGuid())
                        isLeader = true;

                data << uint8(isLeader);
            }

            if (flags & LFG_MEMBER_FLAG_GROUPGUID)                          // Group guid
            {
                ObjectGuid groupGuid = ObjectGuid();
                if (Group* group = player->GetGroup())
                    groupGuid = group->GetObjectGuid();

                data << groupGuid;
            }

            if (flags & LFG_MEMBER_FLAG_ROLES)                              // rolesMask
                data << uint8(player->GetLFGState()->GetRoles());

            if (flags & LFG_MEMBER_FLAG_AREA)                               // Area
                data << uint32(player->GetAreaId());

            if (flags & LFG_MEMBER_FLAG_STATUS)                             // status
                data << uint8(0);

            if (flags & LFG_MEMBER_FLAG_BIND)
            {
                ObjectGuid instanceGuid;
                uint32 encounters = 0;
                if (InstancePlayerBind* bind = player->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
                {
                    if (DungeonPersistentState* state = bind->state)
                    {
                        instanceGuid = state->GetInstanceGuid();
                        encounters = state->GetCompletedEncountersMask();
                    }
                }
                data << instanceGuid;
                data << encounters;
            }

        }
    }
    else
    {
        data << uint32(0);                                          // players count
        data << uint32(0);                                          // unk
    }
    SendPacket(&data);
}

void WorldSession::SendLfgUpdateSearch(bool update)
{
    DEBUG_LOG("SMSG_LFG_UPDATE_SEARCH %u update: %u",GetPlayer()->GetObjectGuid().GetCounter(), update ? 1 : 0);
    WorldPacket data(SMSG_LFG_UPDATE_SEARCH, 1);
    data << uint8(update);                                  // In Lfg Queue?
    SendPacket(&data);
}

void WorldSession::SendLfgDisabled()
{
    DEBUG_LOG("SMSG_LFG_DISABLED %u", GetPlayer()->GetObjectGuid().GetCounter());
    WorldPacket data(SMSG_LFG_DISABLED, 0);
    SendPacket(&data);
}

void WorldSession::SendLfgOfferContinue(uint32 dungeonEntry)
{
    DEBUG_LOG("SMSG_LFG_OFFER_CONTINUE %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), dungeonEntry);
    WorldPacket data(SMSG_LFG_OFFER_CONTINUE, 4);
    data << uint32(dungeonEntry);
    SendPacket(&data);
}

void WorldSession::SendLfgTeleportError(uint8 err)
{
    DEBUG_LOG("SMSG_LFG_TELEPORT_DENIED %u reason: %u", GetPlayer()->GetObjectGuid().GetCounter(), err);
    WorldPacket data(SMSG_LFG_TELEPORT_DENIED, 4);
    data << uint32(err);
    SendPacket(&data);
}

void WorldSession::SendLfgPlayerReward(LFGDungeonEntry const* dungeon, const LFGReward* reward, const Quest* qRew, bool isSecond)
{
    if (!dungeon || !reward || !qRew)
        return;

    uint8 itemNum = uint8(qRew ? qRew->GetRewItemsCount() : 0);
    uint8 done = uint8(isSecond);

    DEBUG_LOG("SMSG_LFG_PLAYER_REWARD %u dungeonEntry: %u ", GetPlayer()->GetObjectGuid().GetCounter(), dungeon->ID);

    WorldPacket data(SMSG_LFG_PLAYER_REWARD, 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 1 + itemNum * (4 + 4 + 4));
    data << uint32(dungeon->Entry());                         // Random Dungeon Finished
    data << uint32(0);                                        // Dungeon Finished
    data << uint8(done);
    data << uint32(1);
    data << uint32(qRew->GetRewOrReqMoney());
    data << uint32(qRew->XPValue(GetPlayer()));
    data << uint32(reward->reward[done].variableMoney);
    data << uint32(reward->reward[done].variableXP);
    data << uint8(itemNum);
    if (itemNum)
    {
        ItemPrototype const* iProto = NULL;
        for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            if (!qRew->RewItemId[i])
                continue;

            iProto = ObjectMgr::GetItemPrototype(qRew->RewItemId[i]);

            data << uint32(qRew->RewItemId[i]);
            data << uint32(iProto ? iProto->DisplayInfoID : 0);
            data << uint32(qRew->RewItemCount[i]);
        }
    }
    SendPacket(&data);
}
