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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundDS.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Language.h"

BattleGroundDS::BattleGroundDS()
{
    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

BattleGroundDS::~BattleGroundDS()
{

}

void BattleGroundDS::Update(uint32 diff)
{
    BattleGround::Update(diff);
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // push people from the tubes
        //if (pushbackCheck)

        // knockback
        if (m_uiKnockback < diff)
        {
            for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            {
                Player *plr = sObjectMgr.GetPlayer(itr->first);
                if (!plr)
                    continue;

                if (GameObject* obj = plr->GetGameObject(48018))                         // Remove Demonic Circle
                    obj->Delete();

                if (plr->GetPositionZ() < 11.0f)
                    continue;

                float angle = (plr->GetBGTeam() == ALLIANCE /*gold*/) ? plr->GetAngle(1259.58f, 764.43f) : plr->GetAngle(1325.84f, 817.304f);

                plr->KnockBackPlayerWithAngle(angle, 45, 7);

                if (plr->IsWithinDist2d(1214, 765, 50) && plr->IsWithinLOS(1214, 765, 14))
                    plr->KnockBackPlayerWithAngle(6.40f,55,7);

                if (plr->IsWithinDist2d(1369, 817, 50) && plr->IsWithinLOS(1369, 817, 14))
                    plr->KnockBackPlayerWithAngle(3.03f,55,7);
            }
            pushbackCheck = false;
            m_uiKnockback = 1000;
        }
        else
            m_uiKnockback -= diff;

        // in case pushback failed
        if (teleportCheck)
        {
            if (m_uiTeleport < diff)
            {
                for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                {
                    if (Player *plr = sObjectMgr.GetPlayer(itr->first))
                    {
                        if (plr->GetPositionZ() < 11.0f)
                            continue;

                        float x, y;
                        if (plr->GetBGTeam() == ALLIANCE)
                        {
                            x = 1259.58f;
                            y = 764.43f;
                        }
                        else
                        {
                            x = 1325.84f;
                            y = 817.304f;
                        }
                        plr->TeleportTo(GetMapId(), x + urand(0,2), y + urand(0,2), 3.15f, plr->GetOrientation());
                    }
                }

                teleportCheck = false;

                // close the gate
                OpenDoorEvent(BG_EVENT_DOOR);
            }
            else
                m_uiTeleport -= diff;
        }

        // Waterfall
        if (m_uiWaterfall < diff)
        {
            if (waterfallActivated)
            {
                SpawnEvent(WATERFALL_EVENT, 0, false);
                waterfallActivated = false;
            }
            else
            {
                SpawnEvent(WATERFALL_EVENT, 0, true);
                waterfallActivated = true;

                for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                {
                    Player * plr = sObjectMgr.GetPlayer(itr->first);
                    if (plr && plr->GetDistance2d(1291, 790) <= 6)
                        plr->KnockBackFrom(plr, -20.0f, 9.0f);
                }
            }
            m_uiWaterfall = urand(30,45)*IN_MILLISECONDS;
        }
        else
            m_uiWaterfall -= diff;
    }
}

void BattleGroundDS::StartingEventCloseDoors()
{
}

void BattleGroundDS::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void BattleGroundDS::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundDSScore* sc = new BattleGroundDSScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
}

void BattleGroundDS::RemovePlayer(Player* /*plr*/, ObjectGuid /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

void BattleGroundDS::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog.outError("BattleGroundDS: Killer player not found");
        return;
    }

    BattleGround::HandleKillPlayer(player,killer);

    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));

    CheckArenaWinConditions();
}

bool BattleGroundDS::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(),1299.046f,784.825f,9.338f,player->GetOrientation(),false);
    return true;
}

void BattleGroundDS::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch(Trigger)
    {
        case 5326:
            Source->TeleportTo(GetMapId(),1299.046f, 784.825f, 9.338f, Source->GetOrientation(),false);
            break;
        case 5347:
        case 5348:
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

void BattleGroundDS::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    FillInitialWorldState(data, count, 0xe10, GetAlivePlayersCountByTeam(HORDE));
    FillInitialWorldState(data, count, 0xe1a, 1);
}

void BattleGroundDS::Reset()
{
    //call parent's class reset
    BattleGround::Reset();
    m_uiTeleport = 10000;
    teleportCheck = true;

    m_uiKnockback = 5000;
    pushbackCheck = true;

    m_uiWaterfall = 2000;
    waterfallActivated = false;
}

bool BattleGroundDS::SetupBattleGround()
{
    return true;
}
