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
#include "Policies/Singleton.h"
#include "LFG.h"

class Group;
class Player;


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

typedef std::multimap<uint32, LFGReward const*> LFGRewardMap;
typedef std::pair<LFGRewardMap::const_iterator, LFGRewardMap::const_iterator> LFGRewardMapBounds;

class LFGMgr
{
    public:
        LFGMgr();
        ~LFGMgr();

        void Update(uint32 diff);

        void LoadRewards();
        LFGReward const* GetRandomDungeonReward(uint32 dungeon, uint32 level, Difficulty difficulty);

        bool IsRandomDungeon(uint32 dungeonId);

    private:
        LFGRewardMap m_RewardMap;                           // Stores rewards for random dungeons

};

#define sLFGMgr MaNGOS::Singleton<LFGMgr>::Instance()
#endif
