/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LFG_H
#define _LFG_H

enum LFGRoles
{
    ROLE_LEADER = 0,
    ROLE_TANK   = 1,
    ROLE_HEALER = 2,
    ROLE_DAMAGE = 3,
};

enum LFGRoleMask
{
    LFG_ROLE_MASK_NONE    = 0x00,
    LFG_ROLE_MASK_LEADER  = (1 << ROLE_LEADER),
    LFG_ROLE_MASK_TANK    = (1 << ROLE_TANK),
    LFG_ROLE_MASK_HEALER  = (1 << ROLE_HEALER),
    LFG_ROLE_MASK_DAMAGE  = (1 << ROLE_DAMAGE),

    // unions
    LFG_ROLE_MASK_MEMBER   = ( LFG_ROLE_MASK_TANK   |
                               LFG_ROLE_MASK_HEALER |
                               LFG_ROLE_MASK_DAMAGE ),

    LFG_ROLE_MASK_TH      = ( LFG_ROLE_MASK_TANK |
                               LFG_ROLE_MASK_HEALER ),

    LFG_ROLE_MASK_TD      = ( LFG_ROLE_MASK_TANK |
                               LFG_ROLE_MASK_DAMAGE ),

    LFG_ROLE_MASK_HD      = ( LFG_ROLE_MASK_DAMAGE |
                               LFG_ROLE_MASK_HEALER ),
};

enum LFGState
{
    LFG_STATE_NONE = 0,                                     // Not using LFG / LFR
    LFG_STATE_LFG  = 1,                                     // Using Dungeon finder
    LFG_STATE_LFR  = 2,                                     // Using Raid finder
};

enum LFGUpdateType
{
    LFG_UPDATETYPE_LEADER               = 1,
    LFG_UPDATETYPE_ROLECHECK_ABORTED    = 4,
    LFG_UPDATETYPE_JOIN_PROPOSAL        = 5,
    LFG_UPDATETYPE_ROLECHECK_FAILED     = 6,
    LFG_UPDATETYPE_REMOVED_FROM_QUEUE   = 7,
    LFG_UPDATETYPE_PROPOSAL_FAILED      = 8,
    LFG_UPDATETYPE_PROPOSAL_DECLINED    = 9,
    LFG_UPDATETYPE_GROUP_FOUND          = 10,
    LFG_UPDATETYPE_ADDED_TO_QUEUE       = 12,
    LFG_UPDATETYPE_PROPOSAL_BEGIN       = 13,
    LFG_UPDATETYPE_CLEAR_LOCK_LIST      = 14,
    LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE = 15,
    LFG_UPDATETYPE_GROUP_DISBAND        = 16,
};

enum LFGenum
{
    LFG_TIME_ROLECHECK         = 2*MINUTE,
    LFG_TIME_BOOT              = 2*MINUTE,
    LFG_TIME_PROPOSAL          = 2*MINUTE,
    LFG_TIME_JOIN_WARNING      = 1*IN_MILLISECONDS,
    LFG_TANKS_NEEDED           = 1,
    LFG_HEALERS_NEEDED         = 1,
    LFG_DPS_NEEDED             = 3,
    LFG_QUEUEUPDATE_INTERVAL   = 15*IN_MILLISECONDS,
    LFG_SPELL_DUNGEON_COOLDOWN = 71328,
    LFG_SPELL_DUNGEON_DESERTER = 71041,
    LFG_SPELL_LUCK_OF_THE_DRAW = 72221,
};

enum LFGType
{
    LFG_TYPE_NONE                 = 0,
    LFG_TYPE_DUNGEON              = 1,
    LFG_TYPE_RAID                 = 2,
    LFG_TYPE_QUEST                = 3,
    LFG_TYPE_ZONE                 = 4,
    LFG_TYPE_HEROIC_DUNGEON       = 5,
    LFG_TYPE_RANDOM_DUNGEON       = 6
};

enum LFGProposalState
{
    LFG_PROPOSAL_INITIATING = 0,
    LFG_PROPOSAL_FAILED     = 1,
    LFG_PROPOSAL_SUCCESS    = 2,
};

enum LFGLockStatusType
{
    LFG_LOCKSTATUS_OK                        = 0,           // Internal use only
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION    = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL             = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL            = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE        = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE       = 5,
    LFG_LOCKSTATUS_RAID_LOCKED               = 6,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL  = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED       = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM              = 1025,
    LFG_LOCKSTATUS_NOT_IN_SEASON             = 1031,
};

enum LFGTeleportError
{
    LFG_TELEPORTERROR_OK               = 0,                 // Internal use
    LFG_TELEPORTERROR_PLAYER_DEAD      = 1,
    LFG_TELEPORTERROR_FALLING          = 2,
    LFG_TELEPORTERROR_UNK2             = 3,                 // You can't do that right now
    LFG_TELEPORTERROR_FATIGUE          = 4,
    LFG_TELEPORTERROR_UNK3             = 5,                 // No reaction
    LFG_TELEPORTERROR_INVALID_LOCATION = 6,
    LFG_TELEPORTERROR_UNK4             = 7,                 // You can't do that right now
    LFG_TELEPORTERROR_UNK5             = 8,                 // You can't do that right now
};

enum LFGJoinResult
{
    LFG_JOIN_OK                    = 0,                     // Joined (no client msg)
    LFG_JOIN_FAILED                = 1,                     // RoleCheck Failed
    LFG_JOIN_GROUPFULL             = 2,                     // Your group is full
    LFG_JOIN_UNK3                  = 3,                     // No client reaction
    LFG_JOIN_INTERNAL_ERROR        = 4,                     // Internal LFG Error
    LFG_JOIN_NOT_MEET_REQS         = 5,                     // You do not meet the requirements for the chosen dungeons
    LFG_JOIN_PARTY_NOT_MEET_REQS   = 6,                     // One or more party members do not meet the requirements for the chosen dungeons
    LFG_JOIN_MIXED_RAID_DUNGEON    = 7,                     // You cannot mix dungeons, raids, and random when picking dungeons
    LFG_JOIN_MULTI_REALM           = 8,                     // The dungeon you chose does not support players from multiple realms
    LFG_JOIN_DISCONNECTED          = 9,                     // One or more party members are pending invites or disconnected
    LFG_JOIN_PARTY_INFO_FAILED     = 10,                    // Could not retrieve information about some party members
    LFG_JOIN_DUNGEON_INVALID       = 11,                    // One or more dungeons was not valid
    LFG_JOIN_DESERTER              = 12,                    // You can not queue for dungeons until your deserter debuff wears off
    LFG_JOIN_PARTY_DESERTER        = 13,                    // One or more party members has a deserter debuff
    LFG_JOIN_RANDOM_COOLDOWN       = 14,                    // You can not queue for random dungeons while on random dungeon cooldown
    LFG_JOIN_PARTY_RANDOM_COOLDOWN = 15,                    // One or more party members are on random dungeon cooldown
    LFG_JOIN_TOO_MUCH_MEMBERS      = 16,                    // You can not enter dungeons with more that 5 party members
    LFG_JOIN_USING_BG_SYSTEM       = 17,                    // You can not use the dungeon system while in BG or arenas
    LFG_JOIN_FAILED2               = 18,                    // RoleCheck Failed
};

enum LFGRoleCheckResult
{
    LFG_ROLECHECK_FINISHED     = 1,                         // Role check finished
    LFG_ROLECHECK_INITIALITING = 2,                         // Role check begins
    LFG_ROLECHECK_MISSING_ROLE = 3,                         // Someone didn't selected a role after 2 mins
    LFG_ROLECHECK_WRONG_ROLES  = 4,                         // Can't form a group with that role selection
    LFG_ROLECHECK_ABORTED      = 5,                         // Someone leave the group
    LFG_ROLECHECK_NO_ROLE      = 6,                         // Someone selected no role
};

enum LFGAnswer
{
    LFG_ANSWER_PENDING         = -1,
    LFG_ANSWER_DENY            = 0,
    LFG_ANSWER_AGREE           = 1,
};

enum LFGDungeonStatus
{
    LFG_STATUS_SAVED           = 0,
    LFG_STATUS_NOT_SAVED       = 1,
    LFG_STATUS_COMPLETE        = 2,
};

#endif
