/*
 * Copyright (C) 2005-2011 /dev/rsa for MaNGOS <http://getmangos.com/>
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

#ifndef _LFGMGR_H
#define _LFGMGR_H

#include "Common.h"
#include "ObjectGuid.h"
#include "Policies/Singleton.h"
#include "LFG.h"

class Group;
class Player;

// forward struct declarations
struct LFGReward;
struct LFGQueueInfo;
struct LFGProposal;

typedef std::multimap<uint32, LFGReward const*> LFGRewardMap;
typedef std::pair<LFGRewardMap::const_iterator, LFGRewardMap::const_iterator> LFGRewardMapBounds;
typedef std::map<ObjectGuid, LFGQueueInfo*> LFGQueueInfoMap;
typedef std::map<uint32/*ID*/, LFGDungeonEntry const*> LFGDungeonMap;
typedef std::set<Player*> LFGQueuePlayerSet;
typedef std::set<Group*>  LFGQueueGroupSet;
typedef std::map<ObjectGuid, LFGRoleMask> LFGRolesMap;
typedef std::map<Player*, LFGAnswer> LFGAnswerMap;
typedef std::map<uint32/*ID*/, LFGProposal> LFGProposalMap;

// Reward info
struct LFGReward
{
    uint32 maxLevel;
    struct
    {
        uint32 questId;
        uint32 variableMoney;
        uint32 variableXP;
    } reward[2];

    LFGReward(uint32 _maxLevel, uint32 firstQuest, uint32 firstVarMoney, uint32 firstVarXp, uint32 otherQuest, uint32 otherVarMoney, uint32 otherVarXp)
        : maxLevel(_maxLevel)
    {
        reward[0].questId = firstQuest;
        reward[0].variableMoney = firstVarMoney;
        reward[0].variableXP = firstVarXp;
        reward[1].questId = otherQuest;
        reward[1].variableMoney = otherVarMoney;
        reward[1].variableXP = otherVarXp;
    }
};

// Stores player or group queue info
struct LFGQueueInfo
{
    LFGQueueInfo(): tanks(LFG_TANKS_NEEDED), healers(LFG_HEALERS_NEEDED), dps(LFG_DPS_NEEDED) 
    {
        joinTime = time_t(time(NULL));
    };
    time_t joinTime;                                        // Player queue join time (to calculate wait times)
    uint8 tanks;                                            // Tanks needed
    uint8 healers;                                          // Healers needed
    uint8 dps;                                              // Dps needed
};

struct LFGQueueStatus
{
    LFGDungeonEntry const* dungeon;                          // Dungeon
    time_t                 avgWaitTime;                      // Average Wait time
    time_t                 waitTime;                         // Wait Time
    time_t                 waitTimeTanks;                    // Wait Tanks
    time_t                 waitTimeHealer;                   // Wait Healers
    time_t                 waitTimeDps;                      // Wait Dps
    uint8                  tanks;                            // Tanks needed
    uint8                  healers;                          // Healers needed
    uint8                  dps;                              // Dps needed
    time_t                 queuedTime;                       // Player wait time in queue
};

/// Stores group data related to proposal to join
struct LFGProposal
{
    LFGProposal(LFGDungeonEntry const* _dungeon): dungeon(_dungeon), state(LFG_PROPOSAL_INITIATING), group(NULL), cancelTime(0) {};

    LFGDungeonEntry const* dungeon;                        // Dungeon
    LFGQueuePlayerSet players;                             // Players in this proposal
    LFGProposalState state;                                // State of the proposal
    Group* group;                                          // Proposal group (NULL if not created)
    time_t cancelTime;                                     // Time when we will cancel this proposal
    uint32 ID;                                             // Proposal id
};

/// Stores all rolecheck info of a group that wants to join
struct LFGRoleCheck
{
    time_t cancelTime;                                     ///< Time when the rolecheck will fail
    LFGRolesMap roles;                                     ///< Player selected roles
    LFGRoleCheckState state;                               ///< State of the rolecheck
    LFGDungeonSet dungeons;                                ///< Dungeons group is applying for (expanded random dungeons)
    uint32 randomID;                                       ///< Random Dungeon Id.
    ObjectGuid leaderGuid;                                 ///< Leader of the group
};

/// Stores information of a current vote to kick someone from a group
struct LFGPlayerBoot
{
    time_t cancelTime;                                     ///< Time left to vote
    bool inProgress;                                       ///< Vote in progress
    LFGAnswerMap votes;                                    ///< Player votes (-1 not answer | 0 Not agree | 1 agree)
    ObjectGuid victim;                                     ///< Player guid to be kicked (can't vote)
    uint8 votedNeeded;                                     ///< Votes needed to kick the player
    std::string reason;                                    ///< kick reason
};

typedef std::map<Group*, LFGPlayerBoot>  LFGBootMap;

class LFGMgr
{
    public:
        LFGMgr();
        ~LFGMgr();

        void Update(uint32 diff);

        void Join(Player* player);
        void Leave(Player* player);
        void Leave(Group* group);

        LFGQueuePlayerSet GetDungeonPlayerQueue(LFGDungeonEntry const* dungeon, Team team = TEAM_NONE);
        LFGQueueGroupSet  GetDungeonGroupQueue(LFGDungeonEntry const* dungeon, Team team = TEAM_NONE);

        void ClearLFRList(Player* player);

        // reward system
        void LoadRewards();
        LFGReward const* GetRandomDungeonReward(LFGDungeonEntry const* dungeon, Player* player);
        void SendLFGRewards(Player* player);
        void SendLFGReward(Player* player);

        // Proposal system
        bool CreateProposal(LFGDungeonEntry const* dungeon, Group* group = NULL);
        LFGProposal* GetProposal(uint32 ID);
        void RemoveProposal(uint32 ID);

        // boot vote system
        void OfferContinue(Group* group);
        void InitBoot(Player* kicker, ObjectGuid victimGuid, std::string reason);
        LFGPlayerBoot* GetBoot(Group* group);
        void DeleteBoot(Group* group);
        void UpdateBoot(Player* player, bool accept);
        void TeleportPlayer(Player* player, bool out, bool fromOpcode = false);

        // Dungeon operations
        LFGDungeonEntry const* GetDungeon(uint32 dungeonID);
        bool IsRandomDungeon(LFGDungeonEntry const* dungeon);
        LFGDungeonSet GetRandomDungeonsForPlayer(Player* player);

        // Checks
        LFGJoinResult GetPlayerJoinResult(Player* player);
        LFGJoinResult GetGroupJoinResult(Group* group);

        LFGLockStatusType GetPlayerLockStatus(Player* player, LFGDungeonEntry const* dungeon);
        LFGLockStatusType GetGroupLockStatus(Group* group, LFGDungeonEntry const* dungeon);

        LFGLockStatusMap GetPlayerLockMap(Player* player);

    private:
        void _Join(ObjectGuid guid, LFGType type);
        void _Leave(ObjectGuid guid, LFGType excludeType = LFG_TYPE_NONE);
        void _JoinGroup(ObjectGuid guid, LFGType type);
        void _LeaveGroup(ObjectGuid guid, LFGType excludeType = LFG_TYPE_NONE);
        uint32 GenerateProposalID();
        LFGRewardMap    m_RewardMap;                        // Stores rewards for random dungeons
        LFGQueueInfoMap m_queueInfoMap[LFG_TYPE_MAX];       // Queued players
        LFGQueueInfoMap m_groupQueueInfoMap[LFG_TYPE_MAX];  // Queued groups
        LFGDungeonMap   m_dungeonMap;                       // sorted dungeon map
        LFGQueueStatus  m_queueStatus[LFG_TYPE_MAX];        // Queue statisic
        LFGProposalMap  m_proposalMap;                      // Proposal store
        LFGBootMap      m_bootMap;                          // boot store

};

#define sLFGMgr MaNGOS::Singleton<LFGMgr>::Instance()
#endif
