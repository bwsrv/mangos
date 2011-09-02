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

struct LFGDungeonEntry;
class Player;
class Group;

enum LFGRoles
{
    ROLE_LEADER = 0,
    ROLE_TANK   = 1,
    ROLE_HEALER = 2,
    ROLE_DAMAGE = 3,
    ROLE_MAX,
};

enum LFGRoleMask
{
    LFG_ROLE_MASK_NONE    = 0x00,
    LFG_ROLE_MASK_LEADER  = (1 << ROLE_LEADER),
    LFG_ROLE_MASK_TANK    = (1 << ROLE_TANK),
    LFG_ROLE_MASK_HEALER  = (1 << ROLE_HEALER),
    LFG_ROLE_MASK_DAMAGE  = (1 << ROLE_DAMAGE),

    // unions
    LFG_ROLE_MASK_THD     = ( LFG_ROLE_MASK_TANK   |
                               LFG_ROLE_MASK_HEALER |
                               LFG_ROLE_MASK_DAMAGE ),

    LFG_ROLE_MASK_TH      = ( LFG_ROLE_MASK_TANK |
                               LFG_ROLE_MASK_HEALER ),

    LFG_ROLE_MASK_TD      = ( LFG_ROLE_MASK_TANK |
                               LFG_ROLE_MASK_DAMAGE ),

    LFG_ROLE_MASK_HD      = ( LFG_ROLE_MASK_DAMAGE |
                               LFG_ROLE_MASK_HEALER ),

    LFG_ROLE_MASK_LTHD     = ( LFG_ROLE_MASK_THD |
                               LFG_ROLE_MASK_LEADER ),
    LFG_ROLE_MASK_LTH      = ( LFG_ROLE_MASK_TH |
                               LFG_ROLE_MASK_LEADER ),
    LFG_ROLE_MASK_LTD      = ( LFG_ROLE_MASK_TD |
                               LFG_ROLE_MASK_LEADER ),
    LFG_ROLE_MASK_LHD      = ( LFG_ROLE_MASK_HD |
                               LFG_ROLE_MASK_LEADER ),
};

enum LFGMemberFlags
{
    LFG_MEMBER_FLAG_NONE         = 0x00,
    LFG_MEMBER_FLAG_CHARINFO     = 0x01,      // have charinfo
    LFG_MEMBER_FLAG_COMMENT      = 0x02,      // have comment
    LFG_MEMBER_FLAG_GROUPLEADER  = 0x04,      // IsInGroup
    LFG_MEMBER_FLAG_GROUPGUID    = 0x08,      // Have group guid
    LFG_MEMBER_FLAG_ROLES        = 0x10,      // have roles
    LFG_MEMBER_FLAG_AREA         = 0x20,      // have areaid
    LFG_MEMBER_FLAG_STATUS       = 0x40,      // have status (unknown bool)
    LFG_MEMBER_FLAG_BIND         = 0x80,      // have bind and completed encounter on this dungeon
    LFG_MEMBER_FLAG_UPDATE       = 0x1000,    // special custom flag for clear client cache
};

enum LFGState
{
    LFG_STATE_NONE               = 0,                  // Not using LFG / LFR
    LFG_STATE_LFR                = 1,                  // Using Raid finder
    LFG_STATE_LFG                = 2,                  // Using Dungeon finder
    LFG_STATE_ROLECHECK          = 3,                  // Rolecheck active
    LFG_STATE_QUEUED             = 4,                  // Queued
    LFG_STATE_PROPOSAL           = 5,                  // Proposal active
    LFG_STATE_BOOT               = 6,                  // Vote kick active
    LFG_STATE_DUNGEON            = 7,                  // In LFG Group, in a Dungeon
    LFG_STATE_FINISHED_DUNGEON   = 8,                  // In LFG Group, in a finished Dungeon
    LFG_STATE_MAX
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

enum LFGType
{
    LFG_TYPE_NONE                 = 0,
    LFG_TYPE_DUNGEON              = 1,
    LFG_TYPE_RAID                 = 2,
    LFG_TYPE_QUEST                = 3,
    LFG_TYPE_ZONE                 = 4,
    LFG_TYPE_HEROIC_DUNGEON       = 5,
    LFG_TYPE_RANDOM_DUNGEON       = 6,
    LFG_TYPE_MAX
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
    ERR_LFG_OK                                  = 0x00,                    // Joined (no client msg)
    ERR_LFG_ROLE_CHECK_FAILED                   = 0x01,                    // RoleCheck Failed
    ERR_LFG_GROUP_FULL                          = 0x02,                    // Your group is full
    ERR_LFG_UNK3                                = 0x03,                    // No client reaction
    ERR_LFG_NO_LFG_OBJECT                       = 0x04,                    // Internal LFG Error
    ERR_LFG_NO_SLOTS_PLAYER                     = 0x05,                    // You do not meet the requirements for the chosen dungeons
    ERR_LFG_NO_SLOTS_PARTY                      = 0x06,                    // One or more party members do not meet the requirements for the chosen dungeons
    ERR_LFG_MISMATCHED_SLOTS                    = 0x07,                    // You cannot mix dungeons, raids, and random when picking dungeons
    ERR_LFG_PARTY_PLAYERS_FROM_DIFFERENT_REALMS = 0x08,                    // The dungeon you chose does not support players from multiple realms
    ERR_LFG_MEMBERS_NOT_PRESENT                 = 0x09,                    // One or more party members are pending invites or disconnected
    ERR_LFG_GET_INFO_TIMEOUT                    = 0x0A,                    // Could not retrieve information about some party members
    ERR_LFG_INVALID_SLOT                        = 0x0B,                    // One or more dungeons was not valid
    ERR_LFG_DESERTER_PLAYER                     = 0x0C,                    // You can not queue for dungeons until your deserter debuff wears off
    ERR_LFG_DESERTER_PARTY                      = 0x0D,                    // One or more party members has a deserter debuff
    ERR_LFG_RANDOM_COOLDOWN_PLAYER              = 0x0E,                    // You can not queue for random dungeons while on random dungeon cooldown
    ERR_LFG_RANDOM_COOLDOWN_PARTY               = 0x0F,                    // One or more party members are on random dungeon cooldown
    ERR_LFG_TOO_MANY_MEMBERS                    = 0x10,                    // You can not enter dungeons with more that 5 party members
    ERR_LFG_CANT_USE_DUNGEONS                   = 0x11,                    // You can not use the dungeon system while in BG or arenas
    ERR_LFG_ROLE_CHECK_FAILED2                  = 0x12,                    // RoleCheck Failed
};

enum LFGRoleCheckState
{
    LFG_ROLECHECK_NONE                           = 0,      // Internal use = Not initialized.
    LFG_ROLECHECK_FINISHED                       = 1,      // Role check finished
    LFG_ROLECHECK_INITIALITING                   = 2,      // Role check begins
    LFG_ROLECHECK_MISSING_ROLE                   = 3,      // Someone didn't selected a role after 2 mins
    LFG_ROLECHECK_WRONG_ROLES                    = 4,      // Can't form a group with that role selection
    LFG_ROLECHECK_ABORTED                        = 5,      // Someone leave the group
    LFG_ROLECHECK_NO_ROLE                        = 6       // Someone selected no role
};

enum LFGAnswer
{
    LFG_ANSWER_PENDING         = -1,
    LFG_ANSWER_DENY            = 0,
    LFG_ANSWER_AGREE           = 1,
};

enum LFGDungeonStatus
{
    LFG_STATUS_SAVED                 = 0,
    LFG_STATUS_NOT_SAVED             = 1,
    LFG_STATUS_OFFER_CONTINUE        = 2,
    LFG_STATUS_COMPLETE              = 3,
};

typedef std::set<LFGDungeonEntry const*> LFGDungeonSet;
typedef std::map<LFGDungeonEntry const*, LFGLockStatusType> LFGLockStatusMap;

struct LFGProposal;

struct LFGStateStructure
{
public:
    virtual ~LFGStateStructure() {};
    virtual void Clear() {};

    void Update(bool _update = true) { update = _update; };
    LFGDungeonSet const* GetDungeons() const  { return &m_DungeonsList; };
    void SetDungeons(LFGDungeonSet* dungeons);
    void RemoveDungeon(LFGDungeonEntry const* dungeon);
    void AddDungeon(LFGDungeonEntry const* dungeon);


    LFGType const& GetType() const { return m_type;};

    void SetState(LFGState _state) { m_state = _state; };
    LFGState GetState() { return m_state; };

    LFGProposal*   GetProposal()   { return m_proposal; };
    void           SetProposal(LFGProposal* proposal)   { m_proposal = proposal; };

    uint32         GetFlags()                { return m_flags;};
    void           AddFlags(uint32 flags)    { m_flags = m_flags | flags;};
    void           RemoveFlags(uint32 flags) { m_flags = m_flags & ~flags;};

protected:
    LFGStateStructure()
        : m_type(LFG_TYPE_NONE), m_flags(0), update(false), m_state(LFG_STATE_NONE), m_proposal(NULL) {};
    LFGType          m_type;
    uint32           m_flags;
    bool             update;
    LFGState         m_state;
    LFGDungeonSet    m_DungeonsList;                   // Dungeons the player have applied for
    LFGLockStatusMap m_LockMap;                        // Dungeons lock map
    LFGProposal*     m_proposal;

};


struct LFGPlayerState : public LFGStateStructure
{
public:
    explicit LFGPlayerState(Player* player) : m_player(player)
    {
        Clear();
    };
    ~LFGPlayerState() {};

    void Clear();
    LFGLockStatusMap const* GetLockMap();
    std::string    GetComment()    { return m_comment; };
    void           SetComment(std::string comment);

    LFGRoleMask    GetRoles();
    void           SetRoles(uint8 roles);
    void           AddRole(LFGRoles role) { rolesMask = LFGRoleMask( rolesMask | (1 << role)); };
    void           RemoveRole(LFGRoles role) { rolesMask = LFGRoleMask( rolesMask & ~(1 << role)); };
    bool           IsSingleRole();

    void           SetJoined();
    time_t         GetJoinTime() { return m_jointime;};

    void           SetTeleported() {m_teleported = true;};
    bool           IsTeleported() { return m_teleported;};

    void           SetAnswer(LFGAnswer _accept) { accept = _accept;};
    LFGAnswer      GetAnswer() { return accept;};


private:
    LFGRoleMask    rolesMask;
    bool           m_teleported;
    Player*        m_player;
    time_t         m_jointime;
    std::string    m_comment;
    LFGAnswer      accept;                           ///< Accept status (-1 not answer | 0 Not agree | 1 agree)
};

typedef std::map<ObjectGuid, LFGAnswer> LFGAnswerMap;

struct LFGGroupState : public LFGStateStructure
{

public:
    explicit LFGGroupState(Group* group) : m_group(group)
    {
        Clear();
    };
    ~LFGGroupState() {};

    void Clear();
    LFGDungeonEntry const* GetDungeon()   { return m_realdungeon; };
    void SetDungeon(LFGDungeonEntry const* _dungeon)   { m_realdungeon = _dungeon; };

    // Current state operations
    void          SaveState() { m_savedstate = m_state; };
    void          RestoreState() { m_state = m_savedstate; };

    // Group status
    void                  SetStatus(LFGDungeonStatus _status) { m_status = _status; };
    LFGDungeonStatus      GetStatus() { return m_status; };

    // Boot system
    LFGAnswerMap*  GetBootMap() { return &m_bootVotes; };
    void  StartBoot(ObjectGuid kicker, ObjectGuid victim, std::string reason);
    void  StopBoot();
    ObjectGuid GetBootVictim() { return m_bootVictim; };
    void  UpdateBoot(ObjectGuid kicker, LFGAnswer vote);
    LFGAnswer  GetBootResult();
    std::string GetBootReason() { return m_bootReason; };
    bool  IsBootActive();
    time_t GetBootCancelTime() { return m_bootCancelTime; };
    uint8 GetVotesNeeded() const;
    void  SetVotesNeeded(uint8 votes);
    uint8 const GetKicksLeft() const;
    void  DecreaseKicksLeft();

    // Role checks
    LFGRoleCheckState GetRoleCheckState() const { return m_roleCheckState;};
    void SetRoleCheckState( LFGRoleCheckState _state) { m_roleCheckState = _state; };
    void StartRoleCheck();
    bool QueryRoleCheckTime() {return (time_t(time(NULL)) < m_roleCheckCancelTime);};
    bool IsRoleCheckActive();

    uint8 GetRandomPlayersCount() const { return m_randomPlayersCount; };
    void  SetRandomPlayersCount(uint8 _count) { m_randomPlayersCount = _count; };

private:
    bool           queued;
    Group*         m_group;
    LFGState       m_savedstate;
    LFGDungeonStatus     m_status;
    LFGDungeonEntry const* m_realdungeon;                       // real dungeon entry (if random or list)
    // Role check
    time_t         m_roleCheckCancelTime;                       // Time when the rolecheck will fail
    LFGRoleCheckState m_roleCheckState;                         // State of the rolecheck
    // Boot
    time_t         m_bootCancelTime;                            // Time left to vote
    uint8          m_votesNeeded;                               // Votes need to kick success
    uint8          m_kicksLeft;                                 // Number of kicks left
    uint8          m_randomPlayersCount;                        // Count of random players in this group
    LFGAnswerMap   m_bootVotes;                                 // Player votes (-1 not answer | 0 Not agree | 1 agree)
    ObjectGuid     m_bootVictim;                                // Player guid to be kicked (can't vote)
    std::string    m_bootReason;                                // kick reason

};

#endif
