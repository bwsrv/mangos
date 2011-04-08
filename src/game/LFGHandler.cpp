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

static void AttemptJoin(Player* _player)
{
    // skip not can autojoin cases and player group case
    if (!_player->m_lookingForGroup.canAutoJoin() || _player->GetGroup())
        return;

    //TODO: Guard Player Map
    HashMapHolder<Player>::MapType const& players = sObjectAccessor.GetPlayers();
    for(HashMapHolder<Player>::MapType::const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        Player *plr = iter->second;

        // skip enemies and self
        if (!plr || plr==_player || plr->GetTeam() != _player->GetTeam())
            continue;

        //skip players not in world
        if (!plr->IsInWorld())
            continue;

        // skip not auto add, not group leader cases
        if (!plr->GetSession()->LookingForGroup_auto_add || (plr->GetGroup() && plr->GetGroup()->GetLeaderGuid() != plr->GetObjectGuid()))
            continue;

        // skip non auto-join or empty slots, or non compatible slots
        if (!plr->m_lookingForGroup.more.canAutoJoin() || !_player->m_lookingForGroup.HaveInSlot(plr->m_lookingForGroup.more))
            continue;

        // attempt create group, or skip
        if (!plr->GetGroup())
        {
            Group* group = new Group;
            if (!group->Create(plr->GetObjectGuid(), plr->GetName()))
            {
                delete group;
                continue;
            }

            sObjectMgr.AddGroup(group);
        }

        // stop at success join
        if(plr->GetGroup()->AddMember(_player->GetObjectGuid(), _player->GetName()))
        {
            if( sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && _player->GetSession()->GetSecurity() == SEC_PLAYER )
                _player->LeaveLFGChannel();
            break;
        }
        // full
        else
        {
            if( sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && plr->GetSession()->GetSecurity() == SEC_PLAYER )
                plr->LeaveLFGChannel();
        }
    }
}

static void AttemptAddMore(Player* _player)
{
    // skip not group leader case
    if (_player->GetGroup() && _player->GetGroup()->GetLeaderGuid() != _player->GetObjectGuid())
        return;

    if(!_player->m_lookingForGroup.more.canAutoJoin())
        return;

    //TODO: Guard Player map
    HashMapHolder<Player>::MapType const& players = sObjectAccessor.GetPlayers();
    for(HashMapHolder<Player>::MapType::const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        Player *plr = iter->second;

        // skip enemies and self
        if (!plr || plr==_player || plr->GetTeam() != _player->GetTeam())
            continue;

        if(!plr->IsInWorld())
            continue;

        // skip not auto join or in group
        if(!plr->GetSession()->LookingForGroup_auto_join || plr->GetGroup() )
            continue;

        if(!plr->m_lookingForGroup.HaveInSlot(_player->m_lookingForGroup.more))
            continue;

        // attempt create group if need, or stop attempts
        if(!_player->GetGroup())
        {
            Group* group = new Group;
            if(!group->Create(_player->GetObjectGuid(), _player->GetName()))
            {
                delete group;
                return;                                     // can't create group (??)
            }

            sObjectMgr.AddGroup(group);
        }

        // stop at join fail (full)
        if(!_player->GetGroup()->AddMember(plr->GetObjectGuid(), plr->GetName()) )
        {
            if( sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && _player->GetSession()->GetSecurity() == SEC_PLAYER )
                _player->LeaveLFGChannel();

            break;
        }

        // joined
        if( sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && plr->GetSession()->GetSecurity() == SEC_PLAYER )
            plr->LeaveLFGChannel();

        // and group full
        if(_player->GetGroup()->IsFull() )
        {
            if( sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && _player->GetSession()->GetSecurity() == SEC_PLAYER )
                _player->LeaveLFGChannel();

            break;
        }
    }
}

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

    GetPlayer()->GetLFGState()->comment = comment;

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

void WorldSession::HandleSearchLfgJoinOpcode( WorldPacket & recv_data )
{

    LookingForGroup_auto_add = true;

    uint32 entry;                                           // Raid id to search
    recv_data >> entry;
    DEBUG_LOG("CMSG_SEARCH_LFG_JOIN %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), entry);
    //sLFGMgr(entry);

    AttemptAddMore(_player);
}

void WorldSession::HandleSearchLfgLeaveOpcode( WorldPacket & recv_data )
{
    LookingForGroup_auto_add = false;

    uint32 entry;                                          // Raid id queue to leave
    recv_data >> entry;

    DEBUG_LOG("CMSG_SEARCH_LFG_LEAVE %u dungeon entry: %u", GetPlayer()->GetObjectGuid().GetCounter(), entry);
    //sLFGMgr(GetPlayer(), entry);
}

void WorldSession::HandleLfgClearOpcode( WorldPacket & /*recv_data */ )
{
    // empty packet
    DEBUG_LOG("CMSG_CLEAR_LOOKING_FOR_GROUP");

    for(int i = 0; i < MAX_LOOKING_FOR_GROUP_SLOT; ++i)
        _player->m_lookingForGroup.slots[i].Clear();

    if( sWorld.getConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL) && _player->GetSession()->GetSecurity() == SEC_PLAYER )
        _player->LeaveLFGChannel();

    SendLfgUpdate(0, 0, 0);
}

void WorldSession::HandleLfmClearOpcode( WorldPacket & /*recv_data */)
{
    // empty packet
    DEBUG_LOG("CMSG_CLEAR_LOOKING_FOR_MORE");

    _player->m_lookingForGroup.more.Clear();
}

void WorldSession::HandleSetLfmOpcode( WorldPacket & recv_data )
{
    DEBUG_LOG("CMSG_SET_LOOKING_FOR_MORE");
    //recv_data.hexlike();
    uint32 temp, entry, type;
    uint8 unk1;
    uint8 unk2[3];

    recv_data >> temp >> unk1 >> unk2[0] >> unk2[1] >> unk2[2];

    entry = ( temp & 0x00FFFFFF);
    type = ( (temp >> 24) & 0x000000FF);

    _player->m_lookingForGroup.more.Set(entry,type);
    DEBUG_LOG("LFM set: temp %u, zone %u, type %u", temp, entry, type);

    if(LookingForGroup_auto_add)
        AttemptAddMore(_player);

    SendLfgResult(type, entry, 1);
}

void WorldSession::HandleSetLfgCommentOpcode( WorldPacket & recv_data )
{
    std::string comment;
    recv_data >> comment;

    DEBUG_LOG("CMSG_SET_LFG_COMMENT: [%s]", comment.c_str());

    GetPlayer()->GetLFGState()->comment = comment;
}

void WorldSession::HandleLookingForGroup(WorldPacket& recv_data)
{
    DEBUG_LOG("MSG_LOOKING_FOR_GROUP");
    //recv_data.hexlike();
    uint32 type, entry, unk;

    recv_data >> type >> entry >> unk;
    DEBUG_LOG("MSG_LOOKING_FOR_GROUP: type %u, entry %u, unk %u", type, entry, unk);

    if(LookingForGroup_auto_add)
        AttemptAddMore(_player);

    if(LookingForGroup_auto_join)
        AttemptJoin(_player);

    SendLfgResult(type, entry, 0);
    SendLfgUpdate(0, 1, 0);
}

void WorldSession::SendLfgResult(uint32 type, uint32 entry, uint8 lfg_type)
{
    /*uint32 number = 0;

    WorldPacket data(MSG_LOOKING_FOR_GROUP);
    data << uint32(type);                                   // type
    data << uint32(entry);                                  // entry from LFGDungeons.dbc

    data << uint8(0);
    if(uint8)
    {
        uint32 count1;
        for(count1)
        {
            uint64; // player guid
        }
    }

    data << uint32(0);                                      // count2
    data << uint32(0);
    for(count2)
    {
        uint64 // not player guid
        uint32 flags;
        if(flags & 0x2)
        {
            data << uint8(0); // string
        }
        if(flags & 0x10)
        {
            data << uint8(0);
        }
        if(flags & 0x20)
        {
            for(int i = 0; i < 3; ++i)
            {
                data << uint8(0);
            }
        }
    }

    size_t count3_pos = data.wpos();
    data << uint32(0);                                      // count3
    data << uint32(0);                                      // unk

    //TODO: Guard Player map
    HashMapHolder<Player>::MapType const& players = sObjectAccessor.GetPlayers();
    for(HashMapHolder<Player>::MapType::const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        Player *plr = iter->second;

        if(!plr || plr->GetTeam() != _player->GetTeam())
            continue;

        if(!plr->IsInWorld())
            continue;

        if(!plr->m_lookingForGroup.HaveInSlot(entry, type))
            continue;

        ++number;

        data << plr->GetObjectGuid();                       // guid

        uint32 flags = 0x1FF;
        data << uint32(flags);                              // flags

        if(flags & 0x1)
        {
            data << uint8(plr->getLevel());
            data << uint8(plr->getClass());
            data << uint8(plr->getRace());

            for(int i = 0; i < 3; ++i)
                data << uint8(0);                           // spent talents count in specific tab

            data << uint32(0);                              // resistances1
            data << uint32(0);                              // spd/heal
            data << uint32(0);                              // spd/heal
            data << uint32(0);                              // combat_rating9
            data << uint32(0);                              // combat_rating10
            data << uint32(0);                              // combat_rating11
            data << float(0);                               // mp5
            data << float(0);                               // unk
            data << uint32(0);                              // attack power
            data << uint32(0);                              // stat1
            data << uint32(0);                              // maxhealth
            data << uint32(0);                              // maxpower1
            data << uint32(0);                              // unk
            data << float(0);                               // unk
            data << uint32(0);                              // unk
            data << uint32(0);                              // unk
            data << uint32(0);                              // unk
            data << uint32(0);                              // unk
            data << uint32(0);                              // combat_rating20
            data << uint32(0);                              // unk
        }

        if(flags & 0x2)
            data << plr->m_lookingForGroup.comment;         // comment

        if(flags & 0x4)
            data << uint8(0);                               // unk

        if(flags & 0x8)
            data << uint64(0);                              // guid from count2 block, not player guid

        if(flags & 0x10)
            data << uint8(0);                               // unk

        if(flags & 0x20)
            data << uint8(plr->m_lookingForGroup.roles);    // roles

        if(flags & 0x40)
            data << uint32(plr->GetZoneId());               // areaid

        if(flags & 0x100)
            data << uint8(0);                               // LFG/LFM flag?

        if(flags & 0x80)
        {
            for(uint8 j = 0; j < MAX_LOOKING_FOR_GROUP_SLOT; ++j)
            {
                data << uint32(plr->m_lookingForGroup.slots[j].entry | (plr->m_lookingForGroup.slots[j].type << 24));
            }
        }
    }

    data.put<uint32>(count3_pos, number);                   // fill count placeholder

    SendPacket(&data);*/
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

    _player->m_lookingForGroup.slots[slot].Set(entry, type);
    _player->m_lookingForGroup.roles = roles;
    DEBUG_LOG("LFG set: looknumber %u, temp %X, type %u, entry %u", slot, temp, type, entry);

    if(LookingForGroup_auto_join)
        AttemptJoin(_player);

    //SendLfgResult(type, entry, 0);
    SendLfgUpdate(0, 1, 0);
}

void WorldSession::HandleLfgSetRolesOpcode(WorldPacket &recv_data)
{
    DEBUG_LOG("CMSG_LFG_SET_ROLES");

    uint8 roles;
    recv_data >> roles;

    _player->m_lookingForGroup.roles = roles;
}

void WorldSession::SendLfgUpdate(uint8 /*unk1*/, uint8 /*unk2*/, uint8 /*unk3*/)
{
    // disabled
    /*WorldPacket data(SMSG_LFG_UPDATE, 3);
    data << uint8(unk1);
    data << uint8(unk2);
    data << uint8(unk3);
    SendPacket(&data);*/
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

void WorldSession::SendLfgJoinResult(LFGJoinResult checkResult, uint8 checkValue /* = 0 */)
{
    uint32 size = 0;

    sLog.outDebug("SMSG_LFG_JOIN_RESULT checkResult: %u checkValue: %u", checkResult, checkValue);
    WorldPacket data(SMSG_LFG_JOIN_RESULT, 4 + 4 + size);
    data << uint32(checkResult);                            // Check Result
    data << uint32(checkValue);                             // Check Value
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
        sLog.outDebug("SMSG_LFG_UPDATE_PLAYER [" UI64FMTD "] updatetype: %u not sent! player flag: false", GetPlayer()->GetObjectGuid().GetRawValue(), updateType);
        return;
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

    sLog.outDebug("SMSG_LFG_UPDATE_PLAYER [" UI64FMTD "] updatetype: %u", GetPlayer()->GetObjectGuid().GetRawValue(), updateType);
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
