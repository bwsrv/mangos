/*
* Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
* Copyright (C) 2010 Blueboy
* Copyright (C) 2011 MangosR2 
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

#ifndef _PLAYERBOTMGR_H
#define _PLAYERBOTMGR_H

#include "Common.h"

class WorldPacket;
class Player;
class Unit;
class Object;
class Item;
class PlayerbotClassAI;

typedef UNORDERED_MAP<ObjectGuid, Player*> PlayerBotMap;

class MANGOS_DLL_SPEC PlayerbotMgr
{
public:
    PlayerbotMgr(Player * const master);
    virtual ~PlayerbotMgr();

    // This is called from Unit.cpp and is called every second (I think)
    void UpdateAI(const uint32 p_time);

    // This is called whenever the master sends a packet to the server.
    // These packets can be viewed, but not edited.
    // It allows bot creators to craft AI in response to a master's actions.
    // For a list of opcodes that can be caught see Opcodes.cpp (CMSG_* opcodes only)
    // Notice: this is static - it is called once for all bots of the master.
    void HandleMasterIncomingPacket(const WorldPacket& packet);
    void HandleMasterOutgoingPacket(const WorldPacket& packet);

    void AddPlayerBot(ObjectGuid guid);
    void LogoutPlayerBot(ObjectGuid guid);
    Player* GetPlayerBot (ObjectGuid guid) const;
    Player* GetMaster() const { return m_master; };
    PlayerBotMap::const_iterator GetPlayerBotsBegin() const { return m_playerBots.begin(); }
    PlayerBotMap::const_iterator GetPlayerBotsEnd()   const { return m_playerBots.end();   }
    int GetBotCount() const { return m_botCount; }

    void LogoutAllBots();
    void RemoveAllBotsFromGroup();
    void OnBotLogin(Player * const bot);
    void Stay();

public:
    // config variables
    uint32 m_confRestrictBotLevel;
    uint32 m_confDisableBotsInRealm;
    uint32 m_confMaxNumBots;
    bool m_confDisableBots;
    bool m_confSharedBots;
    bool m_confDebugWhisper;
    float m_confFollowDistance[2];
    bool m_confCollectCombat;
    bool m_confCollectQuest;
    bool m_confCollectProfession;
    bool m_confCollectLoot;
    bool m_confCollectSkin;
    bool m_confCollectObjects;

private:
    Player* const m_master;
    PlayerBotMap m_playerBots;
    int m_botCount;
};

#endif
