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
    DEBUG_LOG("CMSG_LFG_JOIN");

    if (!GetPlayer())
        return;

    if (!sWorld.getConfig(CONFIG_BOOL_LFG_ENABLE))
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

    DEBUG_LOG("CMSG_LFG_JOIN %u as group: %u - Dungeons: %u real: %u", GetPlayer()->GetObjectGuid().GetCounter(), GetPlayer()->GetGroup() ? 1 : 0, numDungeons, uint8(newDungeons->size()));

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

void WorldSession::HandleLfrJoinOpcode( WorldPacket & recv_data )
{
    uint32 entry;                                           // Raid id to search
    recv_data >> entry;
    DEBUG_LOG("CMSG_SEARCH_LFG_JOIN %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), entry);
    sLFGMgr.JoinToLFRList(GetPlayer(),entry);
}

void WorldSession::HandleLfrLeaveOpcode( WorldPacket & recv_data )
{
    uint32 entry;                                          // Raid id queue to leave
    recv_data >> entry;

    DEBUG_LOG("CMSG_SEARCH_LFG_LEAVE %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), entry);
    sLFGMgr.RemoveFromLFRList(GetPlayer(),entry);
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

void WorldSession::HandleLookingForGroup(WorldPacket& recv_data)
{
    DEBUG_LOG("MSG_LOOKING_FOR_GROUP");
    //recv_data.hexlike();
    uint32 type, entry, unk;

    recv_data >> type >> entry >> unk;
    DEBUG_LOG("MSG_LOOKING_FOR_GROUP: type %u, entry %u, unk %u", type, entry, unk);

}


void WorldSession::HandleSetLfgOpcode( WorldPacket & recv_data )
{
    DEBUG_LOG("CMSG_SET_LOOKING_FOR_GROUP");
    recv_data.hexlike();
    uint32 slot, temp, entry, type;
    uint8 roles, unk1;

    recv_data >> slot >> temp >> roles >> unk1;

    entry = ( temp & 0x00FFFFFF);
    type = ( (temp >> 24) & 0x000000FF);

    if(slot >= MAX_LOOKING_FOR_GROUP_SLOT)
        return;

    DEBUG_LOG("LFG set: looknumber %u, temp %X, type %u, entry %u", slot, temp, type, entry);

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

        SendPacket(&data);
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

    SendPacket(&data);
}

void WorldSession::SendLfgUpdateParty(LFGUpdateType updateType)
{
    if (!GetPlayer()->GetLFGState()->NeedUpdate())
    {
        DEBUG_LOG("SMSG_LFG_UPDATE_PARTY %u updatetype: %u not sent by player flag", GetPlayer()->GetObjectGuid().GetCounter(), updateType);
        return;
    }

    bool join = false;
    bool extrainfo = false;
    bool queued = false;

    switch(updateType)
    {
        case LFG_UPDATETYPE_JOIN_PROPOSAL:
            extrainfo = true;
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
            break;
        case LFG_UPDATETYPE_PROPOSAL_BEGIN:
            extrainfo = true;
            join = true;
            break;
        default:
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

        data << comment;
    }
    SendPacket(&data);
}

void WorldSession::SendLfgUpdatePlayer(LFGUpdateType updateType)
{
    if (!GetPlayer()->GetLFGState()->NeedUpdate())
    {
        DEBUG_LOG("SMSG_LFG_UPDATE_PLAYER %u updatetype: %u not sent! player flag: false", GetPlayer()->GetObjectGuid().GetCounter(), updateType);
//        return;
    }

    bool queued = false;
    bool extrainfo = false;

    switch(updateType)
    {
        case LFG_UPDATETYPE_JOIN_PROPOSAL:
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            queued = true;
            extrainfo = true;
            break;
        //case LFG_UPDATETYPE_CLEAR_LOCK_LIST: // TODO: Sometimes has extrainfo - Check ocurrences...
        case LFG_UPDATETYPE_PROPOSAL_BEGIN:
            extrainfo = true;
            break;
    }
    LFGDungeonSet* dungeons = GetPlayer()->GetLFGState()->GetDungeons();
    uint8 size = dungeons->size();
    std::string comment = GetPlayer()->GetLFGState()->GetComment();

    sLog.outDebug("SMSG_LFG_UPDATE_PLAYER %u updatetype: %u", GetPlayer()->GetObjectGuid().GetCounter(), updateType);

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
        data << comment;
    }
    SendPacket(&data);
}


void WorldSession::SendLfgUpdateList(uint32 dungeonEntry, LFGType type)
{
    DEBUG_LOG("SMSG_LFG_UPDATE_LIST %u dungeonentry: %u ", GetPlayer()->GetObjectGuid().GetCounter(), dungeonEntry);

    uint16 size = 2;
    LFGDungeonEntry const* dungeon = sLFGMgr.GetDungeon(dungeonEntry);

    WorldPacket data(SMSG_UPDATE_LFG_LIST);
    data << uint32(type);                                    // type
    data << uint32(dungeon->Entry());                        // entry from LFGDungeons.dbc
/*
    uint8 guids1 = 0;                                        // additional guids

    data << uint8(guids1 > 0);
    if (guids1)
    {
        for(uint8 count1 = 0; count1 != guids1: ++count1)
        {
            data << uint64(0);                               // player guid
        }
    }

    uint32 groups1 = 0;                                      // groups count
    data << uint32(groups1);
    data << uint32(0);                                       // groups count2

    for(uint8 count2 = 0; count2 != groups1: ++count2)
    {
        data << uint64(leaderGuid);
        data << uint32(flags);

        if (flags & LFG_MEMBER_FLAG_COMMENT)
        {
            data << comment; // string
        }

        if (flags & LFG_MEMBER_FLAG_ROLES)
        {
            for (int i = 0; i < 3; ++i)
            {
                data << uint8(roles[i]);
            }
        }
    }

    size_t count3_pos = data.wpos();

    data << uint32(0);                                      // players count
    data << uint32(0);                                      // unk

    for(const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        Player *plr = iter->second;

        data << plr->GetObjectGuid();                       // guid

        data << uint32(flags);                              // flags

        if(flags &  LFG_MEMBER_FLAG_CHARINFO)
        {
            data << uint8(plr->getLevel());
            data << uint8(plr->getClass());
            data << uint8(plr->getRace());

            for(int i = 0; i < 3; ++i)
                data << uint8(0);                           // spent talents count in specific tab

            data << uint32(0);                              // armor
            data << uint32(0);                              // spd
            data << uint32(0);                              // heal
            data << uint32(0);                              // haste rating melee
            data << uint32(0);                              // haste rating ranged
            data << uint32(0);                              // haste rating spell
            data << float(0);                               // mp5
            data << float(0);                               // unk
            data << uint32(0);                              // attack power
            data << uint32(0);                              // agility
            data << uint32(0);                              // health
            data << uint32(0);                              // power
            data << uint32(0);                              // unk
            data << float(0);                               // unk
            data << uint32(0);                              // defence rating
            data << uint32(0);                              // dodge rating
            data << uint32(0);                              // block rating
            data << uint32(0);                              // parry rating
            data << uint32(0);                              // crit rating
            data << uint32(0);                              // expertize
        }

        if(flags & LFG_MEMBER_FLAG_COMMENT)
            data << comment;         // comment

        if(flags & LFG_MEMBER_FLAG_UNK1)
            data << uint8(0);                               // unk

        if(flags & LFG_MEMBER_FLAG_GROUP)
            data << uint64(0);                              // guid from count2 block, not player guid

        if(flags & LFG_MEMBER_FLAG_ROLES)
            data << uint8(roles);                               // 

        if(flags & LFG_MEMBER_FLAG_UNK2)
            data << uint32(0);

        if(flags & LFG_MEMBER_FLAG_ZONE)
            data << uint8(0);                                // areaid?

        if(flags & LFG_MEMBER_FLAG_UNK4)
        {
            data << uint64(0);                              // guid
            data << int32(0);                              // unk
        }
    }

    data.put<uint32>(count3_pos, number);                   // fill count placeholder
*/

    SendPacket(&data);
}
