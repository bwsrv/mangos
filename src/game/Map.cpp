/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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

#include "Map.h"
#include "MapManager.h"
#include "Player.h"
#include "GridNotifiers.h"
#include "Log.h"
#include "GridStates.h"
#include "CellImpl.h"
#include "InstanceData.h"
#include "GridNotifiersImpl.h"
#include "Transports.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "World.h"
#include "ScriptMgr.h"
#include "Group.h"
#include "MapRefManager.h"
#include "DBCEnums.h"
#include "MapPersistentStateMgr.h"
#include "VMapFactory.h"
#include "MoveMap.h"
#include "BattleGroundMgr.h"

Map::~Map()
{
    UnloadAll(true);

    if(!m_scriptSchedule.empty())
        sScriptMgr.DecreaseScheduledScriptCount(m_scriptSchedule.size());

    if (GetPersistentState())
        GetPersistentState()->SetUsedByMapState(NULL);         // field pointer can be deleted after this

    if(i_data)
    {
        delete i_data;
        i_data = NULL;
    }

    // unload instance specific navigation data
    MMAP::MMapFactory::createOrGetMMapManager()->unloadMapInstance(m_TerrainData->GetMapId(), GetInstanceId());

    //release reference count
    if(m_TerrainData->Release())
        sTerrainMgr.UnloadTerrain(m_TerrainData->GetMapId());
}

void Map::LoadMapAndVMap(int gx,int gy)
{
    if(m_bLoadedGrids[gx][gx])
        return;

    GridMap * pInfo = m_TerrainData->Load(gx, gy);
    if(pInfo)
        m_bLoadedGrids[gx][gy] = true;
}

Map::Map(uint32 id, time_t expiry, uint32 InstanceId, uint8 SpawnMode)
  : i_mapEntry (sMapStore.LookupEntry(id)), i_spawnMode(SpawnMode),
  i_id(id), i_InstanceId(InstanceId), m_unloadTimer(0),
  m_VisibleDistance(DEFAULT_VISIBILITY_DISTANCE),
  m_activeNonPlayersIter(m_activeNonPlayers.end()),
  i_gridExpiry(expiry), m_TerrainData(sTerrainMgr.LoadTerrain(id)),
  i_data(NULL), i_script_id(0)
{
    m_CreatureGuids.Set(sObjectMgr.GetFirstTemporaryCreatureLowGuid());
    m_GameObjectGuids.Set(sObjectMgr.GetFirstTemporaryGameObjectLowGuid());

    for(unsigned int j=0; j < MAX_NUMBER_OF_GRIDS; ++j)
    {
        for(unsigned int idx=0; idx < MAX_NUMBER_OF_GRIDS; ++idx)
        {
            //z code
            m_bLoadedGrids[idx][j] = false;
            setNGrid(NULL, idx, j);
        }
    }

    //lets initialize visibility distance for map
    Map::InitVisibilityDistance();

    //add reference for TerrainData object
    m_TerrainData->AddRef();

    MapPersistentState* persistentState = sMapPersistentStateMgr.AddPersistentState(i_mapEntry, GetInstanceId(), GetDifficulty(), 0, IsDungeon());
    persistentState->SetUsedByMapState(this);
    SetBroken(false);
}

MapPersistentState* Map::GetPersistentState() const
{
    MapPersistentState* state = sMapPersistentStateMgr.GetPersistentState(GetId(), GetInstanceId());
    if (!state)
    {
        sLog.outError("Map::GetPersistentState requested, but map ( id %u, instance %u, difficulty %u ) not have this!", GetId(), GetInstanceId(), GetDifficulty());
        state = sMapPersistentStateMgr.AddPersistentState(i_mapEntry, GetInstanceId(), GetDifficulty(), 0, IsDungeon());
        state->SetUsedByMapState(const_cast<Map*>(this));
    }
    return state;
}

void Map::InitVisibilityDistance()
{
    //init visibility for continents
    m_VisibleDistance = World::GetMaxVisibleDistanceOnContinents();
}

// Template specialization of utility methods
template<class T>
void Map::AddToGrid(T* obj, NGridType *grid, Cell const& cell)
{
    (*grid)(cell.CellX(), cell.CellY()).template AddGridObject<T>(obj);
}

template<>
void Map::AddToGrid(Player* obj, NGridType *grid, Cell const& cell)
{
    (*grid)(cell.CellX(), cell.CellY()).AddWorldObject(obj);
}

template<>
void Map::AddToGrid(Corpse *obj, NGridType *grid, Cell const& cell)
{
    // add to world object registry in grid
    if(obj->GetType()!=CORPSE_BONES)
    {
        (*grid)(cell.CellX(), cell.CellY()).AddWorldObject(obj);
    }
    // add to grid object store
    else
    {
        (*grid)(cell.CellX(), cell.CellY()).AddGridObject(obj);
    }
}

template<>
void Map::AddToGrid(Creature* obj, NGridType *grid, Cell const& cell)
{
    // add to world object registry in grid
    if (obj->IsPet())
    {
        (*grid)(cell.CellX(), cell.CellY()).AddWorldObject<Creature>(obj);
        obj->SetCurrentCell(cell);
    }
    // add to grid object store
    else
    {
        (*grid)(cell.CellX(), cell.CellY()).AddGridObject<Creature>(obj);
        obj->SetCurrentCell(cell);
    }
}

template<class T>
void Map::RemoveFromGrid(T* obj, NGridType *grid, Cell const& cell)
{
    (*grid)(cell.CellX(), cell.CellY()).template RemoveGridObject<T>(obj);
}

template<>
void Map::RemoveFromGrid(Player* obj, NGridType *grid, Cell const& cell)
{
    (*grid)(cell.CellX(), cell.CellY()).RemoveWorldObject(obj);
}

template<>
void Map::RemoveFromGrid(Corpse *obj, NGridType *grid, Cell const& cell)
{
    // remove from world object registry in grid
    if(obj->GetType()!=CORPSE_BONES)
    {
        (*grid)(cell.CellX(), cell.CellY()).RemoveWorldObject(obj);
    }
    // remove from grid object store
    else
    {
        (*grid)(cell.CellX(), cell.CellY()).RemoveGridObject(obj);
    }
}

template<>
void Map::RemoveFromGrid(Creature* obj, NGridType *grid, Cell const& cell)
{
    // remove from world object registry in grid
    if (obj->IsPet())
    {
        (*grid)(cell.CellX(), cell.CellY()).RemoveWorldObject<Creature>(obj);
    }
    // remove from grid object store
    else
    {
        (*grid)(cell.CellX(), cell.CellY()).RemoveGridObject<Creature>(obj);
    }
}

void Map::DeleteFromWorld(Player* pl)
{
    sObjectAccessor.RemoveObject(pl);

    WriteGuard Guard(GetLock(MAP_LOCK_TYPE_AURAS));

    delete pl;
}

void
Map::EnsureGridCreated(const GridPair &p)
{
    if(!getNGrid(p.x_coord, p.y_coord))
    {
        setNGrid(new NGridType(p.x_coord*MAX_NUMBER_OF_GRIDS + p.y_coord, p.x_coord, p.y_coord, i_gridExpiry, sWorld.getConfig(CONFIG_BOOL_GRID_UNLOAD)),
            p.x_coord, p.y_coord);

        // build a linkage between this map and NGridType
        buildNGridLinkage(getNGrid(p.x_coord, p.y_coord));

        getNGrid(p.x_coord, p.y_coord)->SetGridState(GRID_STATE_IDLE);

        //z coord
        int gx = (MAX_NUMBER_OF_GRIDS - 1) - p.x_coord;
        int gy = (MAX_NUMBER_OF_GRIDS - 1) - p.y_coord;

        if(!m_bLoadedGrids[gx][gy])
            LoadMapAndVMap(gx,gy);
    }
}

void
Map::EnsureGridLoadedAtEnter(const Cell &cell, Player *player)
{
    NGridType *grid;

    if(EnsureGridLoaded(cell))
    {
        grid = getNGrid(cell.GridX(), cell.GridY());

        if (player)
        {
            DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Player %s enter cell[%u,%u] triggers of loading grid[%u,%u] on map %u", player->GetName(), cell.CellX(), cell.CellY(), cell.GridX(), cell.GridY(), i_id);
        }
        else
        {
            DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Active object nearby triggers of loading grid [%u,%u] on map %u", cell.GridX(), cell.GridY(), i_id);
        }

        ResetGridExpiry(*getNGrid(cell.GridX(), cell.GridY()), 0.1f);
        grid->SetGridState(GRID_STATE_ACTIVE);
    }
    else
        grid = getNGrid(cell.GridX(), cell.GridY());

    if (player)
        AddToGrid(player,grid,cell);
}

bool Map::EnsureGridLoaded(const Cell &cell)
{
    EnsureGridCreated(GridPair(cell.GridX(), cell.GridY()));
    NGridType *grid = getNGrid(cell.GridX(), cell.GridY());

    MANGOS_ASSERT(grid != NULL);
    if( !isGridObjectDataLoaded(cell.GridX(), cell.GridY()) )
    {
        //it's important to set it loaded before loading!
        //otherwise there is a possibility of infinity chain (grid loading will be called many times for the same grid)
        //possible scenario:
        //active object A(loaded with loader.LoadN call and added to the  map)
        //summons some active object B, while B added to map grid loading called again and so on..
        setGridObjectDataLoaded(true,cell.GridX(), cell.GridY());
        ObjectGridLoader loader(*grid, this, cell);
        loader.LoadN();

        // Add resurrectable corpses to world object list in grid
        sObjectAccessor.AddCorpsesToGrid(GridPair(cell.GridX(),cell.GridY()),(*grid)(cell.CellX(), cell.CellY()), this);
        return true;
    }

    return false;
}

void Map::LoadGrid(const Cell& cell, bool no_unload)
{
    EnsureGridLoaded(cell);

    if(no_unload)
        getNGrid(cell.GridX(), cell.GridY())->setUnloadExplicitLock(true);
}

bool Map::Add(Player *player)
{
    player->GetMapRef().link(this, player);
    player->SetMap(this);
    CreateAttackersStorageFor(player->GetObjectGuid());

    // update player state for other player and visa-versa
    CellPair p = MaNGOS::ComputeCellPair(player->GetPositionX(), player->GetPositionY());
    Cell cell(p);
    EnsureGridLoadedAtEnter(cell, player);
    player->AddToWorld();

    SendInitSelf(player);
    SendInitTransports(player);

    NGridType* grid = getNGrid(cell.GridX(), cell.GridY());
    player->GetViewPoint().Event_AddedToWorld(&(*grid)(cell.CellX(), cell.CellY()));
    UpdateObjectVisibility(player,cell,p);

    if (i_data)
        i_data->OnPlayerEnter(player);

    sLFGMgr.OnPlayerEnterMap(player, this);

    return true;
}

template<class T>
void
Map::Add(T *obj)
{
    MANGOS_ASSERT(obj);

    CellPair p = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());
    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP )
    {
        sLog.outError("Map::Add: Object (GUID: %u TypeId: %u) have invalid coordinates X:%f Y:%f grid cell [%u:%u]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), p.x_coord, p.y_coord);
        return;
    }

    obj->SetMap(this);
    if (obj->GetTypeId() == TYPEID_UNIT)
        CreateAttackersStorageFor(obj->GetObjectGuid());

    Cell cell(p);
    if(obj->isActiveObject())
        EnsureGridLoadedAtEnter(cell);
    else
        EnsureGridCreated(GridPair(cell.GridX(), cell.GridY()));

    NGridType *grid = getNGrid(cell.GridX(), cell.GridY());
    MANGOS_ASSERT( grid != NULL );

    AddToGrid(obj,grid,cell);
    obj->AddToWorld();

    if(obj->isActiveObject())
        AddToActive(obj);

    DEBUG_LOG("%s enters grid[%u,%u]", obj->GetGuidStr().c_str(), cell.GridX(), cell.GridY());

    obj->GetViewPoint().Event_AddedToWorld(&(*grid)(cell.CellX(), cell.CellY()));
    UpdateObjectVisibility(obj,cell,p);
}

void Map::MessageBroadcast(Player *player, WorldPacket *msg, bool to_self)
{
    CellPair p = MaNGOS::ComputeCellPair(player->GetPositionX(), player->GetPositionY());

    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP )
    {
        sLog.outError("Map::MessageBroadcast: Player (GUID: %u) have invalid coordinates X:%f Y:%f grid cell [%u:%u]", player->GetGUIDLow(), player->GetPositionX(), player->GetPositionY(), p.x_coord, p.y_coord);
        return;
    }

    Cell cell(p);
    cell.SetNoCreate();

    if( !loaded(GridPair(cell.data.Part.grid_x, cell.data.Part.grid_y)) )
        return;

    MaNGOS::MessageDeliverer post_man(*player, msg, to_self);
    TypeContainerVisitor<MaNGOS::MessageDeliverer, WorldTypeMapContainer > message(post_man);
    cell.Visit(p, message, *this, *player, GetVisibilityDistance());
}

void Map::MessageBroadcast(WorldObject *obj, WorldPacket *msg)
{
    CellPair p = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());

    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP )
    {
        sLog.outError("Map::MessageBroadcast: Object (GUID: %u TypeId: %u) have invalid coordinates X:%f Y:%f grid cell [%u:%u]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), p.x_coord, p.y_coord);
        return;
    }

    Cell cell(p);
    cell.SetNoCreate();

    if( !loaded(GridPair(cell.data.Part.grid_x, cell.data.Part.grid_y)) )
        return;

    //TODO: currently on continents when Visibility.Distance.InFlight > Visibility.Distance.Continents
    //we have alot of blinking mobs because monster move packet send is broken...
    MaNGOS::ObjectMessageDeliverer post_man(*obj,msg);
    TypeContainerVisitor<MaNGOS::ObjectMessageDeliverer, WorldTypeMapContainer > message(post_man);
    cell.Visit(p, message, *this, *obj, GetVisibilityDistance(obj));
}

void Map::MessageDistBroadcast(Player *player, WorldPacket *msg, float dist, bool to_self, bool own_team_only)
{
    CellPair p = MaNGOS::ComputeCellPair(player->GetPositionX(), player->GetPositionY());

    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP )
    {
        sLog.outError("Map::MessageBroadcast: Player (GUID: %u) have invalid coordinates X:%f Y:%f grid cell [%u:%u]", player->GetGUIDLow(), player->GetPositionX(), player->GetPositionY(), p.x_coord, p.y_coord);
        return;
    }

    Cell cell(p);
    cell.SetNoCreate();

    if( !loaded(GridPair(cell.data.Part.grid_x, cell.data.Part.grid_y)) )
        return;

    MaNGOS::MessageDistDeliverer post_man(*player, msg, dist, to_self, own_team_only);
    TypeContainerVisitor<MaNGOS::MessageDistDeliverer , WorldTypeMapContainer > message(post_man);
    cell.Visit(p, message, *this, *player, dist);
}

void Map::MessageDistBroadcast(WorldObject *obj, WorldPacket *msg, float dist)
{
    CellPair p = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());

    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP )
    {
        sLog.outError("Map::MessageBroadcast: Object (GUID: %u TypeId: %u) have invalid coordinates X:%f Y:%f grid cell [%u:%u]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), p.x_coord, p.y_coord);
        return;
    }

    Cell cell(p);
    cell.SetNoCreate();

    if( !loaded(GridPair(cell.data.Part.grid_x, cell.data.Part.grid_y)) )
        return;

    MaNGOS::ObjectMessageDistDeliverer post_man(*obj, msg, dist);
    TypeContainerVisitor<MaNGOS::ObjectMessageDistDeliverer, WorldTypeMapContainer > message(post_man);
    cell.Visit(p, message, *this, *obj, dist);
}

bool Map::loaded(const GridPair &p) const
{
    return ( getNGrid(p.x_coord, p.y_coord) && isGridObjectDataLoaded(p.x_coord, p.y_coord) );
}

void Map::Update(const uint32 &t_diff)
{
    /// update worldsessions for existing players
    for(m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* plr = m_mapRefIter->getSource();
        if(plr && plr->IsInWorld())
        {
            WorldSession * pSession = plr->GetSession();
            MapSessionFilter updater(pSession);

            pSession->Update(updater);
        }
    }

    /// update players at tick
    for(m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* plr = m_mapRefIter->getSource();
        if(plr && plr->IsInWorld())
        {
            WorldObject::UpdateHelper helper(plr);
            helper.Update(t_diff);
        }
    }

    /// update active cells around players and active objects
    resetMarkedCells();

    MaNGOS::ObjectUpdater updater(t_diff);
    // for creature
    TypeContainerVisitor<MaNGOS::ObjectUpdater, GridTypeMapContainer  > grid_object_update(updater);
    // for pets
    TypeContainerVisitor<MaNGOS::ObjectUpdater, WorldTypeMapContainer > world_object_update(updater);

    // the player iterator is stored in the map object
    // to make sure calls to Map::Remove don't invalidate it
    for(m_mapRefIter = m_mapRefManager.begin(); m_mapRefIter != m_mapRefManager.end(); ++m_mapRefIter)
    {
        Player* plr = m_mapRefIter->getSource();

        if (!plr || !plr->IsInWorld() || !plr->IsPositionValid())
            continue;

        //lets update mobs/objects in ALL visible cells around player!
        CellArea area = Cell::CalculateCellArea(plr->GetPositionX(), plr->GetPositionY(), GetVisibilityDistance());

        for(uint32 x = area.low_bound.x_coord; x <= area.high_bound.x_coord; ++x)
        {
            for(uint32 y = area.low_bound.y_coord; y <= area.high_bound.y_coord; ++y)
            {
                // marked cells are those that have been visited
                // don't visit the same cell twice
                uint32 cell_id = (y * TOTAL_NUMBER_OF_CELLS_PER_MAP) + x;
                if(!isCellMarked(cell_id))
                {
                    markCell(cell_id);
                    CellPair pair(x,y);
                    Cell cell(pair);
                    cell.SetNoCreate();
                    Visit(cell, grid_object_update);
                    Visit(cell, world_object_update);
                }
            }
        }
    }

    // non-player active objects
    if(!m_activeNonPlayers.empty())
    {
        for(m_activeNonPlayersIter = m_activeNonPlayers.begin(); m_activeNonPlayersIter != m_activeNonPlayers.end(); )
        {
            // skip not in world
            WorldObject* obj = *m_activeNonPlayersIter;

            // step before processing, in this case if Map::Remove remove next object we correctly
            // step to next-next, and if we step to end() then newly added objects can wait next update.
            ++m_activeNonPlayersIter;

            if (!obj->IsInWorld() || !obj->IsPositionValid())
                continue;

            //lets update mobs/objects in ALL visible cells around player!
            CellArea area = Cell::CalculateCellArea(obj->GetPositionX(), obj->GetPositionY(), GetVisibilityDistance());

            for(uint32 x = area.low_bound.x_coord; x <= area.high_bound.x_coord; ++x)
            {
                for(uint32 y = area.low_bound.y_coord; y <= area.high_bound.y_coord; ++y)
                {
                    // marked cells are those that have been visited
                    // don't visit the same cell twice
                    uint32 cell_id = (y * TOTAL_NUMBER_OF_CELLS_PER_MAP) + x;
                    if(!isCellMarked(cell_id))
                    {
                        markCell(cell_id);
                        CellPair pair(x,y);
                        Cell cell(pair);
                        cell.SetNoCreate();
                        Visit(cell, grid_object_update);
                        Visit(cell, world_object_update);
                    }
                }
            }
        }
    }

    // Send world objects and item update field changes
    SendObjectUpdates();

    // Don't unload grids if it's battleground, since we may have manually added GOs,creatures, those doesn't load from DB at grid re-load !
    // This isn't really bother us, since as soon as we have instanced BG-s, the whole map unloads as the BG gets ended
    if (!IsBattleGroundOrArena())
    {
        for (GridRefManager<NGridType>::iterator i = GridRefManager<NGridType>::begin(); i != GridRefManager<NGridType>::end(); )
        {
            NGridType *grid = i->getSource();
            GridInfo *info = i->getSource()->getGridInfoRef();
            ++i;                                                // The update might delete the map and we need the next map before the iterator gets invalid
            MANGOS_ASSERT(grid->GetGridState() >= 0 && grid->GetGridState() < MAX_GRID_STATE);
            sMapMgr.UpdateGridState(grid->GetGridState(), *this, *grid, *info, grid->getX(), grid->getY(), t_diff);
        }
    }

    ///- Process necessary scripts
    if (!m_scriptSchedule.empty())
        ScriptsProcess();

    if(i_data)
        i_data->Update(t_diff);
}

void Map::Remove(Player *player, bool remove)
{
    if (i_data)
        i_data->OnPlayerLeave(player);

    sLFGMgr.OnPlayerLeaveMap(player, this);

    if(remove)
        player->CleanupsBeforeDelete();
    else
        player->RemoveFromWorld();

    RemoveAttackersStorageFor(player->GetObjectGuid());
    // this may be called during Map::Update
    // after decrement+unlink, ++m_mapRefIter will continue correctly
    // when the first element of the list is being removed
    // nocheck_prev will return the padding element of the RefManager
    // instead of NULL in the case of prev
    if(m_mapRefIter == player->GetMapRef())
        m_mapRefIter = m_mapRefIter->nocheck_prev();
    player->GetMapRef().unlink();
    CellPair p = MaNGOS::ComputeCellPair(player->GetPositionX(), player->GetPositionY());
    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP)
    {
        // invalid coordinates
        if( remove )
            DeleteFromWorld(player);
        else
            player->TeleportToHomebind();

        return;
    }

    Cell cell(p);

    if( !getNGrid(cell.data.Part.grid_x, cell.data.Part.grid_y) )
    {
        sLog.outError("Map::Remove() i_grids was NULL x:%d, y:%d",cell.data.Part.grid_x,cell.data.Part.grid_y);
        return;
    }

    DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Remove player %s from grid[%u,%u]", player->GetName(), cell.GridX(), cell.GridY());
    NGridType *grid = getNGrid(cell.GridX(), cell.GridY());
    MANGOS_ASSERT(grid != NULL);

    RemoveFromGrid(player,grid,cell);

    SendRemoveTransports(player);
    UpdateObjectVisibility(player,cell,p);

    if (!remove && !player->GetPlayerbotAI())
        player->ResetMap();

    if (remove)
        DeleteFromWorld(player);
}

template<class T>
void
Map::Remove(T *obj, bool remove)
{
    CellPair p = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());
    if(p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP )
    {
        sLog.outError("Map::Remove: Object (GUID: %u TypeId:%u) have invalid coordinates X:%f Y:%f grid cell [%u:%u]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), p.x_coord, p.y_coord);
        return;
    }

    Cell cell(p);
    if( !loaded(GridPair(cell.data.Part.grid_x, cell.data.Part.grid_y)) )
        return;

    DEBUG_LOG("Remove object (GUID: %u TypeId:%u) from grid[%u,%u]", obj->GetGUIDLow(), obj->GetTypeId(), cell.data.Part.grid_x, cell.data.Part.grid_y);
    NGridType *grid = getNGrid(cell.GridX(), cell.GridY());
    MANGOS_ASSERT( grid != NULL );

    if(obj->isActiveObject())
        RemoveFromActive(obj);

    if(remove)
        obj->CleanupsBeforeDelete();
    else
        obj->RemoveFromWorld();

    UpdateObjectVisibility(obj,cell,p);                     // i think will be better to call this function while object still in grid, this changes nothing but logically is better(as for me)
    RemoveFromGrid(obj,grid,cell);

    if (obj->GetTypeId() == TYPEID_UNIT)
        RemoveAttackersStorageFor(obj->GetObjectGuid());

    if( remove )
    {
        // if option set then object already saved at this moment
        if(!sWorld.getConfig(CONFIG_BOOL_SAVE_RESPAWN_TIME_IMMEDIATELY))
            obj->SaveRespawnTime();

        // Note: In case resurrectable corpse and pet its removed from global lists in own destructor
        delete obj;
    }
}

void
Map::PlayerRelocation(Player *player, float x, float y, float z, float orientation)
{
    MANGOS_ASSERT(player);

    CellPair old_val = MaNGOS::ComputeCellPair(player->GetPositionX(), player->GetPositionY());
    CellPair new_val = MaNGOS::ComputeCellPair(x, y);

    Cell old_cell(old_val);
    Cell new_cell(new_val);
    bool same_cell = (new_cell == old_cell);

    player->Relocate(x, y, z, orientation);

    if( old_cell.DiffGrid(new_cell) || old_cell.DiffCell(new_cell) )
    {
        DEBUG_FILTER_LOG(LOG_FILTER_PLAYER_MOVES, "Player %s relocation grid[%u,%u]cell[%u,%u]->grid[%u,%u]cell[%u,%u]", player->GetName(), old_cell.GridX(), old_cell.GridY(), old_cell.CellX(), old_cell.CellY(), new_cell.GridX(), new_cell.GridY(), new_cell.CellX(), new_cell.CellY());

        NGridType* oldGrid = getNGrid(old_cell.GridX(), old_cell.GridY());
        RemoveFromGrid(player, oldGrid,old_cell);
        if( !old_cell.DiffGrid(new_cell) )
            AddToGrid(player, oldGrid,new_cell);
        else
            EnsureGridLoadedAtEnter(new_cell, player);

        NGridType* newGrid = getNGrid(new_cell.GridX(), new_cell.GridY());
        player->GetViewPoint().Event_GridChanged(&(*newGrid)(new_cell.CellX(),new_cell.CellY()));
    }

    player->OnRelocated();

    NGridType* newGrid = getNGrid(new_cell.GridX(), new_cell.GridY());
    if( !same_cell && newGrid->GetGridState()!= GRID_STATE_ACTIVE )
    {
        ResetGridExpiry(*newGrid, 0.1f);
        newGrid->SetGridState(GRID_STATE_ACTIVE);
    }
}

void Map::CreatureRelocation(Creature *creature, float x, float y, float z, float ang)
{
    MANGOS_ASSERT(CheckGridIntegrity(creature,false));

    Cell old_cell = creature->GetCurrentCell();
    Cell new_cell(MaNGOS::ComputeCellPair(x, y));

    // do move or do move to respawn or remove creature if previous all fail
    if (CreatureCellRelocation(creature,new_cell))
    {
        // update pos
        creature->Relocate(x, y, z, ang);
        creature->OnRelocated();
    }
    // if creature can't be move in new cell/grid (not loaded) move it to repawn cell/grid
    // creature coordinates will be updated and notifiers send
    else if (!CreatureRespawnRelocation(creature))
    {
        // ... or unload (if respawn grid also not loaded)
        DEBUG_FILTER_LOG(LOG_FILTER_CREATURE_MOVES, "Creature (GUID: %u Entry: %u ) can't be move to unloaded respawn grid.",creature->GetGUIDLow(),creature->GetEntry());
    }

    MANGOS_ASSERT(CheckGridIntegrity(creature,true));
}

bool Map::CreatureCellRelocation(Creature *c, Cell new_cell)
{
    Cell const& old_cell = c->GetCurrentCell();
    if (old_cell.DiffGrid(new_cell))
    {
        if (!c->isActiveObject() && !loaded(new_cell.gridPair()))
        {
            DEBUG_FILTER_LOG(LOG_FILTER_CREATURE_MOVES, "Creature (GUID: %u Entry: %u) attempt move from grid[%u,%u]cell[%u,%u] to unloaded grid[%u,%u]cell[%u,%u].", c->GetGUIDLow(), c->GetEntry(), old_cell.GridX(), old_cell.GridY(), old_cell.CellX(), old_cell.CellY(), new_cell.GridX(), new_cell.GridY(), new_cell.CellX(), new_cell.CellY());
            return false;
        }
        EnsureGridLoadedAtEnter(new_cell);
    }

    if (old_cell != new_cell)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_CREATURE_MOVES, "Creature (GUID: %u Entry: %u) moved in grid[%u,%u] from cell[%u,%u] to cell[%u,%u].", c->GetGUIDLow(), c->GetEntry(), old_cell.GridX(), old_cell.GridY(), old_cell.CellX(), old_cell.CellY(), new_cell.CellX(), new_cell.CellY());
        NGridType* oldGrid = getNGrid(old_cell.GridX(), old_cell.GridY());
        NGridType* newGrid = getNGrid(new_cell.GridX(), new_cell.GridY());
        RemoveFromGrid(c, oldGrid, old_cell);
        AddToGrid(c, newGrid, new_cell);
        c->GetViewPoint().Event_GridChanged(&(*newGrid)(new_cell.CellX(),new_cell.CellY()));
    }
    return true;
}

bool Map::CreatureRespawnRelocation(Creature *c)
{
    float resp_x, resp_y, resp_z, resp_o;
    c->GetRespawnCoord(resp_x, resp_y, resp_z, &resp_o);

    CellPair resp_val = MaNGOS::ComputeCellPair(resp_x, resp_y);
    Cell resp_cell(resp_val);

    c->CombatStop();
    c->GetMotionMaster()->Clear();

    DEBUG_FILTER_LOG(LOG_FILTER_CREATURE_MOVES, "Creature (GUID: %u Entry: %u) will moved from grid[%u,%u]cell[%u,%u] to respawn grid[%u,%u]cell[%u,%u].", c->GetGUIDLow(), c->GetEntry(), c->GetCurrentCell().GridX(), c->GetCurrentCell().GridY(), c->GetCurrentCell().CellX(), c->GetCurrentCell().CellY(), resp_cell.GridX(), resp_cell.GridY(), resp_cell.CellX(), resp_cell.CellY());

    // teleport it to respawn point (like normal respawn if player see)
    if(CreatureCellRelocation(c,resp_cell))
    {
        c->Relocate(resp_x, resp_y, resp_z, resp_o);
        c->GetMotionMaster()->Initialize();                 // prevent possible problems with default move generators
        c->OnRelocated();
        return true;
    }
    else
        return false;
}

bool Map::UnloadGrid(const uint32 &x, const uint32 &y, bool pForce)
{
    NGridType *grid = getNGrid(x, y);
    MANGOS_ASSERT( grid != NULL);

    {
        if(!pForce && ActiveObjectsNearGrid(x, y) )
            return false;

        DEBUG_LOG("Unloading grid[%u,%u] for map %u", x,y, i_id);
        ObjectGridUnloader unloader(*grid);

        // Finish remove and delete all creatures with delayed remove before moving to respawn grids
        // Must know real mob position before move
        RemoveAllObjectsInRemoveList();

        // move creatures to respawn grids if this is diff.grid or to remove list
        unloader.MoveToRespawnN();

        // Finish remove and delete all creatures with delayed remove before unload
        RemoveAllObjectsInRemoveList();

        unloader.UnloadN();
        delete getNGrid(x, y);
        setNGrid(NULL, x, y);
    }

    int gx = (MAX_NUMBER_OF_GRIDS - 1) - x;
    int gy = (MAX_NUMBER_OF_GRIDS - 1) - y;

    // unload GridMap - it is reference-countable so will be deleted safely when lockCount < 1
    // also simply set Map's pointer to corresponding GridMap object to NULL
    if(m_bLoadedGrids[gx][gy])
    {
        m_bLoadedGrids[gx][gy] = false;
        m_TerrainData->Unload(gx, gy);
    }

    DEBUG_LOG("Unloading grid[%u,%u] for map %u finished", x,y, i_id);
    return true;
}

void Map::UnloadAll(bool pForce)
{
    for (GridRefManager<NGridType>::iterator i = GridRefManager<NGridType>::begin(); i != GridRefManager<NGridType>::end(); )
    {
        NGridType &grid(*i->getSource());
        ++i;
        UnloadGrid(grid.getX(), grid.getY(), pForce);       // deletes the grid and removes it from the GridRefManager
    }
}

MapDifficultyEntry const* Map::GetMapDifficulty() const
{
    return GetMapDifficultyData(GetId(),GetDifficulty());
}

uint32 Map::GetMaxPlayers() const
{
    if(MapDifficultyEntry const* mapDiff = GetMapDifficulty())
    {
        if(mapDiff->maxPlayers || IsRegularDifficulty())    // Normal case (expect that regular difficulty always have correct maxplayers)
            return mapDiff->maxPlayers;
        else                                                // DBC have 0 maxplayers for heroic instances with expansion < 2
        {                                                   // The heroic entry exists, so we don't have to check anything, simply return normal max players
            MapDifficultyEntry const* normalDiff = GetMapDifficultyData(i_id, REGULAR_DIFFICULTY);
            return normalDiff ? normalDiff->maxPlayers : 0;
        }
    }
    else                                                    // I'd rather ASSERT(false);
        return 0;
}

uint32 Map::GetMaxResetDelay() const
{
    return DungeonResetScheduler::GetMaxResetTimeFor(GetMapDifficulty());
}

bool Map::CheckGridIntegrity(Creature* c, bool moved) const
{
    Cell const& cur_cell = c->GetCurrentCell();

    CellPair xy_val = MaNGOS::ComputeCellPair(c->GetPositionX(), c->GetPositionY());
    Cell xy_cell(xy_val);
    if(xy_cell != cur_cell)
    {
        sLog.outError("Creature (GUIDLow: %u) X: %f Y: %f (%s) in grid[%u,%u]cell[%u,%u] instead grid[%u,%u]cell[%u,%u]",
            c->GetGUIDLow(),
            c->GetPositionX(),c->GetPositionY(),(moved ? "final" : "original"),
            cur_cell.GridX(), cur_cell.GridY(), cur_cell.CellX(), cur_cell.CellY(),
            xy_cell.GridX(),  xy_cell.GridY(),  xy_cell.CellX(),  xy_cell.CellY());
        return true;                                        // not crash at error, just output error in debug mode
    }

    return true;
}

const char* Map::GetMapName() const
{
    return i_mapEntry ? i_mapEntry->name[sWorld.GetDefaultDbcLocale()] : "UNNAMEDMAP\x0";
}

void Map::UpdateObjectVisibility( WorldObject* obj, Cell cell, CellPair cellpair)
{
    cell.SetNoCreate();
    MaNGOS::VisibleChangesNotifier notifier(*obj);
    TypeContainerVisitor<MaNGOS::VisibleChangesNotifier, WorldTypeMapContainer > player_notifier(notifier);
    cell.Visit(cellpair, player_notifier, *this, *obj, GetVisibilityDistance(obj));
}

void Map::SendInitSelf( Player * player )
{
    DETAIL_LOG("Creating player data for himself %u", player->GetGUIDLow());

    UpdateData data;

    // attach to player data current transport data
    if(Transport* transport = player->GetTransport())
    {
        transport->BuildCreateUpdateBlockForPlayer(&data, player);
    }

    // build data for self presence in world at own client (one time for map)
    player->BuildCreateUpdateBlockForPlayer(&data, player);

    // build other passengers at transport also (they always visible and marked as visible and will not send at visibility update at add to map
    if(Transport* transport = player->GetTransport())
    {
        for(Transport::PlayerSet::const_iterator itr = transport->GetPassengers().begin();itr!=transport->GetPassengers().end();++itr)
        {
            if(player!=(*itr) && player->HaveAtClient(*itr))
            {
                (*itr)->BuildCreateUpdateBlockForPlayer(&data, player);
            }
        }
    }

    WorldPacket packet;
    data.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

void Map::SendInitTransports( Player * player )
{
    // Hack to send out transports
    MapManager::TransportMap& tmap = sMapMgr.m_TransportsByMap;

    // no transports at map
    if (tmap.find(player->GetMapId()) == tmap.end())
        return;

    UpdateData transData;

    MapManager::TransportSet& tset = tmap[player->GetMapId()];

    for (MapManager::TransportSet::const_iterator i = tset.begin(); i != tset.end(); ++i)
    {
        // send data for current transport in other place
        if((*i) != player->GetTransport() && (*i)->GetMapId()==i_id)
        {
            (*i)->BuildCreateUpdateBlockForPlayer(&transData, player);
        }
    }

    WorldPacket packet;
    transData.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

void Map::SendRemoveTransports( Player * player )
{
    // Hack to send out transports
    MapManager::TransportMap& tmap = sMapMgr.m_TransportsByMap;

    // no transports at map
    if (tmap.find(player->GetMapId()) == tmap.end())
        return;

    UpdateData transData;

    MapManager::TransportSet& tset = tmap[player->GetMapId()];

    // except used transport
    for (MapManager::TransportSet::const_iterator i = tset.begin(); i != tset.end(); ++i)
        if((*i) != player->GetTransport() && (*i)->GetMapId()!=i_id)
            (*i)->BuildOutOfRangeUpdateBlock(&transData);

    WorldPacket packet;
    transData.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

inline void Map::setNGrid(NGridType *grid, uint32 x, uint32 y)
{
    if(x >= MAX_NUMBER_OF_GRIDS || y >= MAX_NUMBER_OF_GRIDS)
    {
        sLog.outError("map::setNGrid() Invalid grid coordinates found: %d, %d!",x,y);
        MANGOS_ASSERT(false);
    }
    i_grids[x][y] = grid;
}

void Map::AddObjectToRemoveList(WorldObject *obj)
{
    MANGOS_ASSERT(obj->GetMapId()==GetId() && obj->GetInstanceId()==GetInstanceId());

    obj->CleanupsBeforeDelete();                            // remove or simplify at least cross referenced links

    i_objectsToRemove.insert(obj);
    //DEBUG_LOG("Object (GUID: %u TypeId: %u ) added to removing list.",obj->GetGUIDLow(),obj->GetTypeId());
}

void Map::RemoveAllObjectsInRemoveList()
{
    if(i_objectsToRemove.empty())
        return;

    //DEBUG_LOG("Object remover 1 check.");
    while(!i_objectsToRemove.empty())
    {
        WorldObject* obj = *i_objectsToRemove.begin();
        i_objectsToRemove.erase(i_objectsToRemove.begin());

        if (!obj)
            continue;

        switch(obj->GetTypeId())
        {
            case TYPEID_CORPSE:
            {
                // ??? WTF
                ObjectGuid guid = obj->GetObjectGuid();
                if (guid && guid.IsUnit())
                {
                    Corpse* corpse = GetCorpse(guid);
                    if (!corpse)
                        sLog.outError("Try delete corpse/bones, but corpse of %s not exists!", guid.GetString().c_str());
                    else
                        Remove(corpse,true);
                }
                break;
            }
            case TYPEID_DYNAMICOBJECT:
                Remove((DynamicObject*)obj,true);
                break;
            case TYPEID_GAMEOBJECT:
                Remove((GameObject*)obj,true);
                break;
            case TYPEID_UNIT:
                Remove((Creature*)obj,true);
                break;
            default:
                sLog.outError("Non-grid object (TypeId: %u) in grid object removing list, ignored.",obj->GetTypeId());
                break;
        }
    }
    //DEBUG_LOG("Object remover 2 check.");
}

uint32 Map::GetPlayersCountExceptGMs() const
{
    uint32 count = 0;
    for(MapRefManager::const_iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
        if(!itr->getSource()->isGameMaster())
            ++count;
    return count;
}

void Map::SendToPlayers(WorldPacket const* data) const
{
    for(MapRefManager::const_iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
        itr->getSource()->GetSession()->SendPacket(data);
}

bool Map::ActiveObjectsNearGrid(uint32 x, uint32 y) const
{
    MANGOS_ASSERT(x < MAX_NUMBER_OF_GRIDS);
    MANGOS_ASSERT(y < MAX_NUMBER_OF_GRIDS);

    CellPair cell_min(x*MAX_NUMBER_OF_CELLS, y*MAX_NUMBER_OF_CELLS);
    CellPair cell_max(cell_min.x_coord + MAX_NUMBER_OF_CELLS, cell_min.y_coord+MAX_NUMBER_OF_CELLS);

    //we must find visible range in cells so we unload only non-visible cells...
    float viewDist = GetVisibilityDistance();
    int cell_range = (int)ceilf(viewDist / SIZE_OF_GRID_CELL) + 1;

    cell_min << cell_range;
    cell_min -= cell_range;
    cell_max >> cell_range;
    cell_max += cell_range;

    for(MapRefManager::const_iterator iter = m_mapRefManager.begin(); iter != m_mapRefManager.end(); ++iter)
    {
        Player* plr = iter->getSource();

        CellPair p = MaNGOS::ComputeCellPair(plr->GetPositionX(), plr->GetPositionY());
        if( (cell_min.x_coord <= p.x_coord && p.x_coord <= cell_max.x_coord) &&
            (cell_min.y_coord <= p.y_coord && p.y_coord <= cell_max.y_coord) )
            return true;
    }

    for(ActiveNonPlayers::const_iterator iter = m_activeNonPlayers.begin(); iter != m_activeNonPlayers.end(); ++iter)
    {
        WorldObject* obj = *iter;

        CellPair p = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());
        if( (cell_min.x_coord <= p.x_coord && p.x_coord <= cell_max.x_coord) &&
            (cell_min.y_coord <= p.y_coord && p.y_coord <= cell_max.y_coord) )
            return true;
    }

    return false;
}

void Map::AddToActive( WorldObject* obj )
{
    m_activeNonPlayers.insert(obj);
    Cell cell = Cell(MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY()));
    EnsureGridLoaded(cell);

    // also not allow unloading spawn grid to prevent creating creature clone at load
    if (obj->GetTypeId() == TYPEID_UNIT)
    {
        Creature* c= (Creature*)obj;

        if (!c->IsPet() && c->HasStaticDBSpawnData())
        {
            float x,y,z;
            c->GetRespawnCoord(x,y,z);
            GridPair p = MaNGOS::ComputeGridPair(x, y);
            if(getNGrid(p.x_coord, p.y_coord))
                getNGrid(p.x_coord, p.y_coord)->incUnloadActiveLock();
            else
            {
                GridPair p2 = MaNGOS::ComputeGridPair(c->GetPositionX(), c->GetPositionY());
                sLog.outError("Active creature (GUID: %u Entry: %u) added to grid[%u,%u] but spawn grid[%u,%u] not loaded.",
                    c->GetGUIDLow(), c->GetEntry(), p.x_coord, p.y_coord, p2.x_coord, p2.y_coord);
            }
        }
    }
}

void Map::RemoveFromActive( WorldObject* obj )
{
    // Map::Update for active object in proccess
    if(m_activeNonPlayersIter != m_activeNonPlayers.end())
    {
        ActiveNonPlayers::iterator itr = m_activeNonPlayers.find(obj);
        if(itr==m_activeNonPlayersIter)
            ++m_activeNonPlayersIter;
        m_activeNonPlayers.erase(itr);
    }
    else
        m_activeNonPlayers.erase(obj);

    // also allow unloading spawn grid
    if (obj->GetTypeId()==TYPEID_UNIT)
    {
        Creature* c= (Creature*)obj;

        if(!c->IsPet() && c->HasStaticDBSpawnData())
        {
            float x,y,z;
            c->GetRespawnCoord(x,y,z);
            GridPair p = MaNGOS::ComputeGridPair(x, y);
            if(getNGrid(p.x_coord, p.y_coord))
                getNGrid(p.x_coord, p.y_coord)->decUnloadActiveLock();
            else
            {
                GridPair p2 = MaNGOS::ComputeGridPair(c->GetPositionX(), c->GetPositionY());
                sLog.outError("Active creature (GUID: %u Entry: %u) removed from grid[%u,%u] but spawn grid[%u,%u] not loaded.",
                    c->GetGUIDLow(), c->GetEntry(), p.x_coord, p.y_coord, p2.x_coord, p2.y_coord);
            }
        }
    }
}

void Map::CreateInstanceData(bool load)
{
    if(i_data != NULL)
        return;

    if (Instanceable())
    {
        if (InstanceTemplate const* mInstance = ObjectMgr::GetInstanceTemplate(GetId()))
            i_script_id = mInstance->script_id;
    }
    else
    {
        if (WorldTemplate const* mInstance = ObjectMgr::GetWorldTemplate(GetId()))
            i_script_id = mInstance->script_id;
    }

    if (!i_script_id)
        return;

    i_data = sScriptMgr.CreateInstanceData(this);
    if(!i_data)
        return;

    if (load)
    {
        // TODO: make a global storage for this
        QueryResult* result;

        if (Instanceable())
            result = CharacterDatabase.PQuery("SELECT data FROM instance WHERE id = '%u'", i_InstanceId);
        else
            result = CharacterDatabase.PQuery("SELECT data FROM world WHERE map = '%u'", GetId());

        if (result)
        {
            Field* fields = result->Fetch();
            const char* data = fields[0].GetString();
            if (data)
            {
                DEBUG_LOG("Loading instance data for `%s` (Map: %u Instance: %u)", sScriptMgr.GetScriptName(i_script_id), GetId(), i_InstanceId);
                i_data->Load(data);
            }
            delete result;
        }
        else
        {
            // for non-instanceable map always add data to table if not found, later code expected that for map in `word` exist always after load
            if (!Instanceable())
                CharacterDatabase.PExecute("INSERT INTO world VALUES ('%u', '')", GetId());
        }
    }
    else
    {
        DEBUG_LOG("New instance data, \"%s\" ,initialized!", sScriptMgr.GetScriptName(i_script_id));
        i_data->Initialize();
    }
}

template void Map::Add(Corpse *);
template void Map::Add(Creature *);
template void Map::Add(GameObject *);
template void Map::Add(DynamicObject *);

template void Map::Remove(Corpse *,bool);
template void Map::Remove(Creature *,bool);
template void Map::Remove(GameObject *, bool);
template void Map::Remove(DynamicObject *, bool);

/* ******* World Maps ******* */

WorldPersistentState* WorldMap::GetPersistanceState() const
{
    return (WorldPersistentState*)Map::GetPersistentState();
}

/* ******* Dungeon Instance Maps ******* */

DungeonMap::DungeonMap(uint32 id, time_t expiry, uint32 InstanceId, uint8 SpawnMode)
  : Map(id, expiry, InstanceId, SpawnMode),
    m_resetAfterUnload(false), m_unloadWhenEmpty(false)
{
    MANGOS_ASSERT(i_mapEntry->IsDungeon());

    //lets initialize visibility distance for dungeons
    DungeonMap::InitVisibilityDistance();

    // the timer is started by default, and stopped when the first player joins
    // this make sure it gets unloaded if for some reason no player joins
    m_unloadTimer = std::max(sWorld.getConfig(CONFIG_UINT32_INSTANCE_UNLOAD_DELAY), (uint32)MIN_UNLOAD_DELAY);
}

DungeonMap::~DungeonMap()
{
}

void DungeonMap::InitVisibilityDistance()
{
    //init visibility distance for instances
    m_VisibleDistance = World::GetMaxVisibleDistanceInInstances();
}

/*
    Do map specific checks to see if the player can enter
*/
bool DungeonMap::CanEnter(Player *player)
{
    if(player->GetMapRef().getTarget() == this)
    {
        sLog.outError("DungeonMap::CanEnter - player %s(%u) already in map %d,%d,%d!", player->GetName(), player->GetGUIDLow(), GetId(), GetInstanceId(), GetSpawnMode());
        MANGOS_ASSERT(false);
        return false;
    }

    return Map::CanEnter(player);
}

/*
    Do map specific checks and add the player to the map if successful.
*/
bool DungeonMap::Add(Player *player)
{
    // TODO: Not sure about checking player level: already done in HandleAreaTriggerOpcode
    // GMs still can teleport player in instance.
    // Is it needed?

    if (!CanEnter(player))
        return false;

    // check for existing instance binds
    InstancePlayerBind *playerBind = player->GetBoundInstance(GetId(), GetDifficulty());
    if (playerBind && playerBind->perm)
    {
        // cannot enter other instances if bound permanently
        if (playerBind->state != GetPersistanceState())
        {
            sLog.outError("DungeonMap::Add: player %s(%d) is permanently bound to instance %d,%d,%d,%d,%d,%d but he is being put in instance %d,%d,%d,%d,%d,%d",
                player->GetName(), player->GetGUIDLow(), playerBind->state->GetMapId(),
                playerBind->state->GetInstanceId(), playerBind->state->GetDifficulty(),
                playerBind->state->GetPlayerCount(), playerBind->state->GetGroupCount(),
                playerBind->state->CanReset(),
                GetPersistanceState()->GetMapId(), GetPersistanceState()->GetInstanceId(),
                GetPersistanceState()->GetDifficulty(), GetPersistanceState()->GetPlayerCount(),
                GetPersistanceState()->GetGroupCount(), GetPersistanceState()->CanReset());
            //MANGOS_ASSERT(false);
            player->RemoveFromGroup();
            player->RepopAtGraveyard();
            player->GetSession()->KickPlayer();
            return false;
        }
    }
    else
    {
        Group *pGroup = player->GetGroup();
        if (pGroup)
        {
            // solo saves should be reset when entering a group
            InstanceGroupBind* groupBind = pGroup->GetBoundInstance(this,GetDifficulty());
            if (playerBind)
            {
                DEBUG_LOG("DungeonMap::Add: %s enter to instance %d,%d,%d,%d,%d,%d but he is in group (Id: %d) and has non-permanent bind to instance %d,%d,%d,%d,%d,%d!",
                    player->GetGuidStr().c_str(), GetPersistentState()->GetMapId(), GetPersistentState()->GetInstanceId(),
                    GetPersistanceState()->GetDifficulty(), GetPersistanceState()->GetPlayerCount(), GetPersistanceState()->GetGroupCount(),
                    GetPersistanceState()->CanReset(), pGroup->GetId(),
                    playerBind->state->GetMapId(), playerBind->state->GetInstanceId(), playerBind->state->GetDifficulty(),
                    playerBind->state->GetPlayerCount(), playerBind->state->GetGroupCount(), playerBind->state->CanReset());

                if (groupBind)
                    DEBUG_LOG("DungeonMap::Add: the group (Id: %d) is bound to instance %d,%d,%d,%d,%d,%d.",
                    pGroup->GetId(),
                    groupBind->state->GetMapId(), groupBind->state->GetInstanceId(), groupBind->state->GetDifficulty(),
                    groupBind->state->GetPlayerCount(), groupBind->state->GetGroupCount(), groupBind->state->CanReset());

                // no reason crash if we can fix state
                player->UnbindInstance(GetId(), GetDifficulty());
            }

            // bind to the group or keep using the group save
            if (!groupBind)
                pGroup->BindToInstance(GetPersistanceState(), false);
            else
            {
                // cannot jump to a different instance without resetting it
                if (groupBind->state != GetPersistentState())
                {
                    DEBUG_LOG("DungeonMap::Add: %s is being put in instance %d,%d,%d but he is in group (Id: %d) which has non-permanent bind to instance %d,%d,%d!",
                        player->GetGuidStr().c_str(), GetPersistentState()->GetMapId(),
                        GetPersistentState()->GetInstanceId(), GetPersistentState()->GetDifficulty(),
                        pGroup->GetId(), groupBind->state->GetMapId(),
                        groupBind->state->GetInstanceId(), groupBind->state->GetDifficulty());

                    sLog.outError("MapSave players: %d, group count: %d",
                        GetPersistanceState()->GetPlayerCount(), GetPersistanceState()->GetGroupCount());

                    if (groupBind->state)
                        sLog.outError("GroupBind save players: %d, group count: %d", groupBind->state->GetPlayerCount(), groupBind->state->GetGroupCount());
                    else
                        sLog.outError("GroupBind save NULL");
                    //MANGOS_ASSERT(false);
                    player->RemoveFromGroup();
                    player->RepopAtGraveyard();
                    player->GetSession()->KickPlayer();
                    return false;
                }
                // if the group/leader is permanently bound to the instance
                // players also become permanently bound when they enter
                if (groupBind->perm && IsDungeon())
                {
                    WorldPacket data(SMSG_PENDING_RAID_LOCK, 9);
                    data << uint32(60000);
                    data << groupBind->state->GetCompletedEncountersMask();
                    data << uint8(0);
                    player->GetSession()->SendPacket(&data);
                    player->SetPendingBind(GetPersistanceState(), 60000);
                }
            }
        }
        else
        {
            // set up a solo bind or continue using it
            if(!playerBind)
                player->BindToInstance(GetPersistanceState(), false);
            else
            {
                // cannot jump to a different instance without resetting it
                MANGOS_ASSERT(playerBind->state == GetPersistentState());
            }
        }
    }

    // for normal instances cancel the reset schedule when the
    // first player enters (no players yet)
    SetResetSchedule(false);

    DETAIL_LOG("MAP: Player '%s' is entering instance '%u' of map '%s'", player->GetName(), GetInstanceId(), GetMapName());
    // initialize unload state
    m_unloadTimer = 0;
    m_resetAfterUnload = false;
    m_unloadWhenEmpty = false;

    // this will acquire the same mutex so it cannot be in the previous block
    Map::Add(player);

    return true;
}

void DungeonMap::Update(const uint32& t_diff)
{
    Map::Update(t_diff);
}

void DungeonMap::Remove(Player *player, bool remove)
{
    DETAIL_LOG("MAP: Removing player '%s' from instance '%u' of map '%s' before relocating to other map", player->GetName(), GetInstanceId(), GetMapName());

    //if last player set unload timer
    if(!m_unloadTimer && m_mapRefManager.getSize() == 1)
        m_unloadTimer = m_unloadWhenEmpty ? MIN_UNLOAD_DELAY : std::max(sWorld.getConfig(CONFIG_UINT32_INSTANCE_UNLOAD_DELAY), (uint32)MIN_UNLOAD_DELAY);

    Map::Remove(player, remove);

    // for normal instances schedule the reset after all players have left
    SetResetSchedule(true);
}

/*
    Returns true if there are no players in the instance
*/
bool DungeonMap::Reset(InstanceResetMethod method)
{
    // note: since the map may not be loaded when the instance needs to be reset
    // the instance must be deleted from the DB by InstanceSaveManager

    if(HavePlayers())
    {
        if (method == INSTANCE_RESET_ALL)
        {
            // notify the players to leave the instance so it can be reset
            for(MapRefManager::iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
                itr->getSource()->SendResetFailedNotify(GetId());
        }
        else if (method == INSTANCE_RESET_CHANGE_DIFFICULTY)
        {
            // notify the players to leave the instance so it can be reset
            for(MapRefManager::iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
                itr->getSource()->SendResetFailedNotify(GetId());

            m_unloadWhenEmpty = true;
            m_resetAfterUnload = true;
        }
        else
        {
            if(method == INSTANCE_RESET_GLOBAL)
            {
                // set the homebind timer for players inside (1 minute)
                for(MapRefManager::iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
                    itr->getSource()->m_InstanceValid = false;
            }

            // the unload timer is not started
            // instead the map will unload immediately after the players have left
            m_unloadWhenEmpty = true;
            m_resetAfterUnload = true;
        }
    }
    else
    {
        // unloaded at next update
        m_unloadTimer = MIN_UNLOAD_DELAY;
        m_resetAfterUnload = true;
    }

    return m_mapRefManager.isEmpty();
}

void DungeonMap::PermBindAllPlayers(Player *player, bool permanent)
{
    Group *group = player->GetGroup();
    // group members outside the instance group don't get bound
    for(MapRefManager::iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
    {
        Player* plr = itr->getSource();
        // players inside an instance cannot be bound to other instances
        // some players may already be permanently bound, in this case nothing happens
        InstancePlayerBind *bind = plr->GetBoundInstance(GetId(), GetDifficulty());
        if (!bind || !bind->perm)
        {
            plr->BindToInstance(GetPersistanceState(), permanent);
            WorldPacket data(SMSG_INSTANCE_SAVE_CREATED, 4);
            data << uint32(0);
            plr->GetSession()->SendPacket(&data);
        }

        // if the leader is not in the instance the group will not get a perm bind
        if (group && group->GetLeaderGuid() == plr->GetObjectGuid())
            group->BindToInstance(GetPersistanceState(), permanent);
    }
}

void DungeonMap::UnloadAll(bool pForce)
{
    if(HavePlayers())
    {
        sLog.outError("DungeonMap::UnloadAll: there are still players in the instance at unload, should not happen!");
        for(MapRefManager::iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
        {
            Player* plr = itr->getSource();
            plr->TeleportToHomebind();
        }
    }

    if(m_resetAfterUnload == true)
    {
        if (DungeonPersistentState* state = GetPersistanceState())
            state->DeleteRespawnTimes();
    }

    Map::UnloadAll(pForce);
}

void DungeonMap::SendResetWarnings(uint32 timeLeft) const
{
    for(MapRefManager::const_iterator itr = m_mapRefManager.begin(); itr != m_mapRefManager.end(); ++itr)
        itr->getSource()->SendInstanceResetWarning(GetId(), itr->getSource()->GetDifficulty(IsRaid()), timeLeft);
}

void DungeonMap::SetResetSchedule(bool on)
{
    // only for normal instances
    // the reset time is only scheduled when there are no payers inside
    // it is assumed that the reset time will rarely (if ever) change while the reset is scheduled
    if(!HavePlayers() && !IsRaidOrHeroicDungeon())
        sMapPersistentStateMgr.GetScheduler().ScheduleReset(on, GetPersistanceState()->GetResetTime(), DungeonResetEvent(RESET_EVENT_NORMAL_DUNGEON, GetId(), Difficulty(GetSpawnMode()), GetInstanceId()));
}

DungeonPersistentState* DungeonMap::GetPersistanceState() const
{
    return (DungeonPersistentState*)Map::GetPersistentState();
}


/* ******* Battleground Instance Maps ******* */

BattleGroundMap::BattleGroundMap(uint32 id, time_t expiry, uint32 InstanceId, uint8 spawnMode)
  : Map(id, expiry, InstanceId, spawnMode)
{
    //lets initialize visibility distance for BG/Arenas
    BattleGroundMap::InitVisibilityDistance();
}

BattleGroundMap::~BattleGroundMap()
{
}

void BattleGroundMap::Update(const uint32& diff)
{
    Map::Update(diff);

    GetBG()->Update(diff);
}

BattleGroundPersistentState* BattleGroundMap::GetPersistanceState() const
{
    return (BattleGroundPersistentState*)Map::GetPersistentState();
}


void BattleGroundMap::InitVisibilityDistance()
{
    //init visibility distance for BG/Arenas
    m_VisibleDistance = World::GetMaxVisibleDistanceInBGArenas();
}

bool BattleGroundMap::CanEnter(Player * player)
{
    if(player->GetMapRef().getTarget() == this)
    {
        sLog.outError("BGMap::CanEnter - player %u already in map!", player->GetGUIDLow());
        MANGOS_ASSERT(false);
        return false;
    }

    if(player->GetBattleGroundId() != GetInstanceId())
        return false;

    // player number limit is checked in bgmgr, no need to do it here

    return Map::CanEnter(player);
}

bool BattleGroundMap::Add(Player * player)
{
    if(!CanEnter(player))
        return false;

    // reset instance validity, battleground maps do not homebind
    player->m_InstanceValid = true;

    return Map::Add(player);
}

void BattleGroundMap::Remove(Player *player, bool remove)
{
    DETAIL_LOG("MAP: Removing player '%s' from bg '%u' of map '%s' before relocating to other map", player->GetName(), GetInstanceId(), GetMapName());
    Map::Remove(player, remove);
}

void BattleGroundMap::SetUnload()
{
    m_unloadTimer = MIN_UNLOAD_DELAY;
}

void BattleGroundMap::UnloadAll(bool pForce)
{
    while(HavePlayers())
    {
        if(Player * plr = m_mapRefManager.getFirst()->getSource())
        {
            plr->TeleportTo(plr->GetBattleGroundEntryPoint());
            // TeleportTo removes the player from this map (if the map exists) -> calls BattleGroundMap::Remove -> invalidates the iterator.
            // just in case, remove the player from the list explicitly here as well to prevent a possible infinite loop
            // note that this remove is not needed if the code works well in other places
            plr->GetMapRef().unlink();
        }
    }

    Map::UnloadAll(pForce);
}

/// Put scripts in the execution queue
void Map::ScriptsStart(ScriptMapMap const& scripts, uint32 id, Object* source, Object* target)
{
    ///- Find the script map
    ScriptMapMap::const_iterator s = scripts.find(id);
    if (s == scripts.end())
        return;

    // prepare static data
    ObjectGuid sourceGuid = source->GetObjectGuid();
    ObjectGuid targetGuid = target ? target->GetObjectGuid() : ObjectGuid();
    ObjectGuid ownerGuid  = source->isType(TYPEMASK_ITEM) ? ((Item*)source)->GetOwnerGuid() : ObjectGuid();

    ///- Schedule script execution for all scripts in the script map
    ScriptMap const *s2 = &(s->second);
    bool immedScript = false;
    for (ScriptMap::const_iterator iter = s2->begin(); iter != s2->end(); ++iter)
    {
        ScriptAction sa;
        sa.sourceGuid = sourceGuid;
        sa.targetGuid = targetGuid;
        sa.ownerGuid  = ownerGuid;

        sa.script = &iter->second;
        m_scriptSchedule.insert(ScriptScheduleMap::value_type(time_t(sWorld.GetGameTime() + iter->first), sa));
        if (iter->first == 0)
            immedScript = true;

        sScriptMgr.IncreaseScheduledScriptsCount();
    }
    ///- If one of the effects should be immediate, launch the script execution
    if (immedScript)
        ScriptsProcess();
}

void Map::ScriptCommandStart(ScriptInfo const& script, uint32 delay, Object* source, Object* target)
{
    // NOTE: script record _must_ exist until command executed

    // prepare static data
    ObjectGuid sourceGuid = source->GetObjectGuid();
    ObjectGuid targetGuid = target ? target->GetObjectGuid() : ObjectGuid();
    ObjectGuid ownerGuid  = source->isType(TYPEMASK_ITEM) ? ((Item*)source)->GetOwnerGuid() : ObjectGuid();

    ScriptAction sa;
    sa.sourceGuid = sourceGuid;
    sa.targetGuid = targetGuid;
    sa.ownerGuid  = ownerGuid;

    sa.script = &script;
    m_scriptSchedule.insert(ScriptScheduleMap::value_type(time_t(sWorld.GetGameTime() + delay), sa));

    sScriptMgr.IncreaseScheduledScriptsCount();

    ///- If effects should be immediate, launch the script execution
    if(delay == 0)
        ScriptsProcess();
}

/// Process queued scripts
void Map::ScriptsProcess()
{
    if (m_scriptSchedule.empty())
        return;

    ///- Process overdue queued scripts
    ScriptScheduleMap::iterator iter = m_scriptSchedule.begin();
    // ok as multimap is a *sorted* associative container
    while (!m_scriptSchedule.empty() && (iter->first <= sWorld.GetGameTime()))
    {
        ScriptAction const& step = iter->second;

        Object* source = NULL;

        if (step.sourceGuid)
        {
            switch(step.sourceGuid.GetHigh())
            {
                case HIGHGUID_ITEM:
                // case HIGHGUID_CONTAINER: ==HIGHGUID_ITEM
                {
                    if (Player* player = HashMapHolder<Player>::Find(step.ownerGuid))
                        source = player->GetItemByGuid(step.sourceGuid);
                    break;
                }
                case HIGHGUID_UNIT:
                case HIGHGUID_VEHICLE:
                    source = GetCreature(step.sourceGuid);
                    break;
                case HIGHGUID_PET:
                    source = GetPet(step.sourceGuid);
                    break;
                case HIGHGUID_PLAYER:
                    source = HashMapHolder<Player>::Find(step.sourceGuid);
                    break;
                case HIGHGUID_GAMEOBJECT:
                    source = GetGameObject(step.sourceGuid);
                    break;
                case HIGHGUID_CORPSE:
                    source = HashMapHolder<Corpse>::Find(step.sourceGuid);
                    break;
                default:
                    sLog.outError("*_script source with unsupported guid %s", step.sourceGuid.GetString().c_str());
                    break;
            }
        }

        if (source && !source->IsInWorld())
            source = NULL;

        Object* target = NULL;

        if (step.targetGuid)
        {
            switch(step.targetGuid.GetHigh())
            {
                case HIGHGUID_UNIT:
                case HIGHGUID_VEHICLE:
                    target = GetCreature(step.targetGuid);
                    break;
                case HIGHGUID_PET:
                    target = GetPet(step.targetGuid);
                    break;
                case HIGHGUID_PLAYER:
                    target = HashMapHolder<Player>::Find(step.targetGuid);
                    break;
                case HIGHGUID_GAMEOBJECT:
                    target = GetGameObject(step.targetGuid);
                    break;
                case HIGHGUID_CORPSE:
                    target = HashMapHolder<Corpse>::Find(step.targetGuid);
                    break;
                default:
                    sLog.outError("*_script target with unsupported guid %s", step.targetGuid.GetString().c_str());
                    break;
            }
        }

        if (target && !target->IsInWorld())
            target = NULL;

        switch(step.script->command)
        {
            case SCRIPT_COMMAND_TALK:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_TALK (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_TALK (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pBuddy = NULL;

                // flag_target_player_as_source     0x01
                // flag_original_source_as_target   0x02
                // flag_buddy_as_target             0x04

                // If target is player (and not already the source) but should be the source
                if (target && target->GetTypeId() == TYPEID_PLAYER && step.script->talk.flags & 0x01)
                {
                    if (source->GetTypeId() != TYPEID_PLAYER)
                        pSource = (WorldObject*)target;
                }

                // If step has a buddy entry defined, search for it.
                if (step.script->talk.creatureEntry)
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->talk.creatureEntry, true, step.script->talk.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pBuddy, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->talk.searchRadius);
                }

                // If buddy found, then use it
                if (pBuddy)
                {
                    // pBuddy can be target of talk
                    if (step.script->talk.flags & 0x04)
                    {
                        target = (Object*)pBuddy;
                    }
                    else
                    {
                        // If not target of talk, then set pBuddy as source
                        // Useless when source is already flagged to be player, and should maybe produce error.
                        if (!(step.script->talk.flags & 0x01))
                            pSource = (WorldObject*)pBuddy;
                    }
                }

                // If we should talk to the original source instead of target
                if (step.script->talk.flags & 0x02)
                    target = source;

                Unit* unitTarget = target && target->isType(TYPEMASK_UNIT) ? static_cast<Unit*>(target) : NULL;
                int32 textId = step.script->talk.textId[0];

                // May have text for random
                if (step.script->talk.textId[1])
                {
                    int i = 2;
                    for(; i < MAX_TEXT_ID; ++i)
                    {
                        if (!step.script->talk.textId[i])
                            break;
                    }

                    // Use one random
                    textId = step.script->talk.textId[rand() % i];
                }

                switch(step.script->talk.chatType)
                {
                    case CHAT_TYPE_SAY:
                        pSource->MonsterSay(textId, step.script->talk.language, unitTarget);
                        break;
                    case CHAT_TYPE_YELL:
                        pSource->MonsterYell(textId, step.script->talk.language, unitTarget);
                        break;
                    case CHAT_TYPE_TEXT_EMOTE:
                        pSource->MonsterTextEmote(textId, unitTarget);
                        break;
                    case CHAT_TYPE_BOSS_EMOTE:
                        pSource->MonsterTextEmote(textId, unitTarget, true);
                        break;
                    case CHAT_TYPE_WHISPER:
                        if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        {
                            sLog.outError("SCRIPT_COMMAND_TALK (script id %u) attempt to whisper (%u) to %s, skipping.", step.script->id, step.script->talk.chatType, unitTarget ? unitTarget->GetGuidStr().c_str() : "<no target>");
                            break;
                        }
                        pSource->MonsterWhisper(textId, unitTarget);
                        break;
                    case CHAT_TYPE_BOSS_WHISPER:
                        if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        {
                            sLog.outError("SCRIPT_COMMAND_TALK (script id %u) attempt to whisper (%u) to %s, skipping.", step.script->id, step.script->talk.chatType, unitTarget ? unitTarget->GetGuidStr().c_str() : "<no target>");
                            break;
                        }
                        pSource->MonsterWhisper(textId, unitTarget, true);
                        break;
                    case CHAT_TYPE_ZONE_YELL:
                        pSource->MonsterYellToZone(textId, step.script->talk.language, unitTarget);
                        break;
                    default:
                        break;                              // must be already checked at load
                }
                break;
            }
            case SCRIPT_COMMAND_EMOTE:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_EMOTE (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_EMOTE (script id %u) call for non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }
                // When creatureEntry is not defined, GameObject can not be source
                else if (!step.script->emote.creatureEntry)
                {
                    if (!source->isType(TYPEMASK_UNIT))
                    {
                        sLog.outError("SCRIPT_COMMAND_EMOTE (script id %u) are missing datalong2 (creature entry). Unsupported call for non-unit (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                        break;
                    }
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pBuddy = NULL;

                // flag_target_as_source            0x01

                // If target is Unit* and should do the emote (or should be source of searcher below)
                if (target && target->isType(TYPEMASK_UNIT) && step.script->emote.flags & 0x01)
                    pSource = (WorldObject*)target;

                // If step has a buddy entry defined, search for it.
                if (step.script->emote.creatureEntry)
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->emote.creatureEntry, true, step.script->emote.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pBuddy, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->emote.searchRadius);

                    // If buddy found, then use it or break (break since we must assume pBuddy was defined for a reason)
                    if (pBuddy)
                        pSource = (WorldObject*)pBuddy;
                    else
                        break;
                }

                // Must be safe cast to Unit*
                ((Unit*)pSource)->HandleEmote(step.script->emote.emoteId);
                break;
            }
            case SCRIPT_COMMAND_FIELD_SET:
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_FIELD_SET (script id %u) call for NULL object.", step.script->id);
                    break;
                }

                if (step.script->setField.fieldId <= OBJECT_FIELD_ENTRY || step.script->setField.fieldId >= source->GetValuesCount())
                {
                    sLog.outError("SCRIPT_COMMAND_FIELD_SET (script id %u) call for wrong field %u (max count: %u) in object (TypeId: %u).",
                        step.script->id, step.script->setField.fieldId, source->GetValuesCount(), source->GetTypeId());
                    break;
                }

                source->SetUInt32Value(step.script->setField.fieldId, step.script->setField.fieldValue);
                break;
            case SCRIPT_COMMAND_MOVE_TO:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_MOVE_TO (script id %u) call for NULL creature.", step.script->id);
                    break;
                }

                if (source->GetTypeId() != TYPEID_UNIT)
                {
                    sLog.outError("SCRIPT_COMMAND_MOVE_TO (script id %u) call for non-creature (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                Unit* unit = (Unit*)source;

                // Just turn around
                if ((fabs(step.script->x) < M_NULL_F && fabs(step.script->y) < M_NULL_F && fabs(step.script->z) < M_NULL_F) ||
                    // Check point-to-point distance, hence revert effect of bounding radius
                    unit->IsWithinDist3d(step.script->x, step.script->y, step.script->z, 0.01f - unit->GetObjectBoundingRadius()))
                {
                    unit->SetFacingTo(step.script->o);
                    break;
                }


                if (step.script->moveTo.travelTime != 0)
                {
                    float speed = unit->GetDistance(step.script->x, step.script->y, step.script->z) / ((float)step.script->moveTo.travelTime * 0.001f);
                    unit->MonsterMoveWithSpeed(step.script->x, step.script->y, step.script->z, speed);
                }
                else
                    unit->NearTeleportTo(step.script->x, step.script->y, step.script->z, fabs(step.script->o) > M_NULL_F ? step.script->o : unit->GetOrientation());
                break;
            }
            case SCRIPT_COMMAND_FLAG_SET:
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_FLAG_SET (script id %u) call for NULL object.", step.script->id);
                    break;
                }
                if (step.script->setFlag.fieldId <= OBJECT_FIELD_ENTRY || step.script->setFlag.fieldId >= source->GetValuesCount())
                {
                    sLog.outError("SCRIPT_COMMAND_FLAG_SET (script id %u) call for wrong field %u (max count: %u) in object (TypeId: %u).",
                        step.script->id, step.script->setFlag.fieldId, source->GetValuesCount(), source->GetTypeId());
                    break;
                }

                source->SetFlag(step.script->setFlag.fieldId, step.script->setFlag.fieldValue);
                break;
            case SCRIPT_COMMAND_FLAG_REMOVE:
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_FLAG_REMOVE (script id %u) call for NULL object.", step.script->id);
                    break;
                }
                if (step.script->removeFlag.fieldId <= OBJECT_FIELD_ENTRY || step.script->removeFlag.fieldId >= source->GetValuesCount())
                {
                    sLog.outError("SCRIPT_COMMAND_FLAG_REMOVE (script id %u) call for wrong field %u (max count: %u) in object (TypeId: %u).",
                        step.script->id, step.script->removeFlag.fieldId, source->GetValuesCount(), source->GetTypeId());
                    break;
                }

                source->RemoveFlag(step.script->removeFlag.fieldId, step.script->removeFlag.fieldValue);
                break;
            case SCRIPT_COMMAND_TELEPORT_TO:
            {
                // accept player in any one from target/source arg
                if (!target && !source)
                {
                    sLog.outError("SCRIPT_COMMAND_TELEPORT_TO (script id %u) call for NULL object.", step.script->id);
                    break;
                }

                // must be only Player
                if ((!target || target->GetTypeId() != TYPEID_PLAYER) && (!source || source->GetTypeId() != TYPEID_PLAYER))
                {
                    sLog.outError("SCRIPT_COMMAND_TELEPORT_TO (script id %u) call for non-player (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                Player* pSource = target && target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : (Player*)source;

                pSource->TeleportTo(step.script->teleportTo.mapId, step.script->x, step.script->y, step.script->z, step.script->o);
                break;
            }
            case SCRIPT_COMMAND_QUEST_EXPLORED:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_QUEST_EXPLORED (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!target)
                {
                    sLog.outError("SCRIPT_COMMAND_QUEST_EXPLORED (script id %u) call for NULL target.", step.script->id);
                    break;
                }

                // when script called for item spell casting then target == (unit or GO) and source is player
                WorldObject* worldObject;
                Player* player;

                if (target->GetTypeId() == TYPEID_PLAYER)
                {
                    if (source->GetTypeId() != TYPEID_UNIT && source->GetTypeId() != TYPEID_GAMEOBJECT && source->GetTypeId() != TYPEID_PLAYER)
                    {
                        sLog.outError("SCRIPT_COMMAND_QUEST_EXPLORED (script id %u) call for non-creature, non-gameobject or non-player (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                        break;
                    }

                    worldObject = (WorldObject*)source;
                    player = (Player*)target;
                }
                else
                {
                    if (target->GetTypeId() != TYPEID_UNIT && target->GetTypeId() != TYPEID_GAMEOBJECT && target->GetTypeId() != TYPEID_PLAYER)
                    {
                        sLog.outError("SCRIPT_COMMAND_QUEST_EXPLORED (script id %u) call for non-creature, non-gameobject or non-player (TypeId: %u), skipping.", step.script->id, target->GetTypeId());
                        break;
                    }

                    if (source->GetTypeId() != TYPEID_PLAYER)
                    {
                        sLog.outError("SCRIPT_COMMAND_QUEST_EXPLORED (script id %u) call for non-player (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                        break;
                    }

                    worldObject = (WorldObject*)target;
                    player = (Player*)source;
                }

                // quest id and flags checked at script loading
                if ((worldObject->GetTypeId() != TYPEID_UNIT || ((Unit*)worldObject)->isAlive()) &&
                    (step.script->questExplored.distance == 0 || worldObject->IsWithinDistInMap(player, float(step.script->questExplored.distance))))
                    player->AreaExploredOrEventHappens(step.script->questExplored.questId);
                else
                    player->FailQuest(step.script->questExplored.questId);

                break;
            }
            case SCRIPT_COMMAND_KILL_CREDIT:
            {
                // accept player in any one from target/source arg
                if (!target && !source)
                {
                    sLog.outError("SCRIPT_COMMAND_KILL_CREDIT (script id %u) call for NULL object.", step.script->id);
                    break;
                }

                // must be only Player
                if ((!target || target->GetTypeId() != TYPEID_PLAYER) && (!source || source->GetTypeId() != TYPEID_PLAYER))
                {
                    sLog.outError("SCRIPT_COMMAND_KILL_CREDIT (script id %u) call for non-player (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                Player* pSource = target && target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : (Player*)source;

                if (step.script->killCredit.isGroupCredit)
                {
                    pSource->RewardPlayerAndGroupAtEvent(step.script->killCredit.creatureEntry, pSource);
                }
                else
                {
                    pSource->KilledMonsterCredit(step.script->killCredit.creatureEntry);
                }

                break;
            }
            case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
            {
                if (!step.script->respawnGo.goGuid)         // checked at load
                    break;

                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (script id %u) call for NULL world object.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (script id %u) call for non-WorldObject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* summoner = (WorldObject*)source;
                if (!summoner->IsInWorld())
                {
                    sLog.outError("SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (script id %u) call for non-in-world WorldObject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                int32 time_to_despawn = step.script->respawnGo.despawnDelay < 5 ? 5 : step.script->respawnGo.despawnDelay;

                uint32 guidlow = step.script->respawnGo.goGuid;
                GameObjectData const* goData = sObjectMgr.GetGOData(guidlow);
                if (!goData)
                    break;                                  // checked at load

                GameObject *go = summoner->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, goData->id, guidlow));
                if (!go)
                {
                    sLog.outError("SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (script id %u) failed for gameobject(guid: %u).", step.script->id, guidlow);
                    break;
                }

                if (go->GetGoType()==GAMEOBJECT_TYPE_FISHINGNODE ||
                    go->GetGoType()==GAMEOBJECT_TYPE_DOOR        ||
                    go->GetGoType()==GAMEOBJECT_TYPE_BUTTON      ||
                    go->GetGoType()==GAMEOBJECT_TYPE_TRAP)
                {
                    sLog.outError("SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (script id %u) can not be used with gameobject of type %u (guid: %u).", step.script->id, uint32(go->GetGoType()), step.script->respawnGo.goGuid);
                    break;
                }

                if (go->isSpawned())
                    break;                                  //gameobject already spawned

                go->SetLootState(GO_READY);
                go->SetRespawnTime(time_to_despawn);        //despawn object in ? seconds

                go->GetMap()->Add(go);
                break;
            }
            case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
            {
                if (!step.script->summonCreature.creatureEntry)
                {
                    sLog.outError("SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (script id %u) call for NULL creature.", step.script->id);
                    break;
                }

                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (script id %u) call for NULL world object.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (script id %u) call for non-WorldObject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* summoner = (WorldObject*)source;

                float x = step.script->x;
                float y = step.script->y;
                float z = step.script->z;
                float o = step.script->o;

                Creature* pCreature = summoner->SummonCreature(step.script->summonCreature.creatureEntry, x, y, z, o, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, step.script->summonCreature.despawnDelay, (step.script->summonCreature.flags & 0x01) ? true: false);
                if (!pCreature)
                {
                    sLog.outError("SCRIPT_COMMAND_TEMP_SUMMON (script id %u) failed for creature (entry: %u).", step.script->id, step.script->summonCreature.creatureEntry);
                    break;
                }

                break;
            }
            case SCRIPT_COMMAND_OPEN_DOOR:
            {
                if (!step.script->openDoor.goGuid)          // checked at load
                    break;

                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_OPEN_DOOR (script id %u) call for NULL unit.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_UNIT))         // must be any Unit (creature or player)
                {
                    sLog.outError("SCRIPT_COMMAND_OPEN_DOOR (script id %u) call for non-unit (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                Unit* caster = (Unit*)source;
                if (!caster->IsInWorld())
                {
                    sLog.outError("SCRIPT_COMMAND_OPEN_DOOR (script id %u) call for non-in-world unit (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                int32 time_to_close = step.script->openDoor.resetDelay < 15 ? 15 : step.script->openDoor.resetDelay;

                uint32 guidlow = step.script->openDoor.goGuid;
                GameObjectData const* goData = sObjectMgr.GetGOData(guidlow);
                if (!goData)                                // checked at load
                    break;

                GameObject *door = caster->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, goData->id, guidlow));
                if (!door)
                {
                    sLog.outError("SCRIPT_COMMAND_OPEN_DOOR (script id %u) failed for gameobject(guid: %u).", step.script->id, step.script->openDoor.goGuid);
                    break;
                }

                if (door->GetGoType() != GAMEOBJECT_TYPE_DOOR)
                {
                    sLog.outError("SCRIPT_COMMAND_OPEN_DOOR (script id %u) failed for non-door(GoType: %u).", step.script->id, door->GetGoType());
                    break;
                }

                if (door->GetGoState() != GO_STATE_READY)
                    break;                                  //door already  open

                door->UseDoorOrButton(time_to_close);

                if (target && target->isType(TYPEMASK_GAMEOBJECT) && ((GameObject*)target)->GetGoType()==GAMEOBJECT_TYPE_BUTTON)
                    ((GameObject*)target)->UseDoorOrButton(time_to_close);

                break;
            }
            case SCRIPT_COMMAND_CLOSE_DOOR:
            {
                if (!step.script->closeDoor.goGuid)         // checked at load
                    break;

                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_CLOSE_DOOR (script id %u) call for NULL unit.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_UNIT))         // must be any Unit (creature or player)
                {
                    sLog.outError("SCRIPT_COMMAND_CLOSE_DOOR (script id %u) call for non-unit (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                Unit* caster = (Unit*)source;
                if (!caster->IsInWorld())
                {
                    sLog.outError("SCRIPT_COMMAND_CLOSE_DOOR (script id %u) call for non-in-world unit (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                int32 time_to_open = step.script->closeDoor.resetDelay < 15 ? 15 : step.script->closeDoor.resetDelay;

                uint32 guidlow = step.script->closeDoor.goGuid;
                GameObjectData const* goData = sObjectMgr.GetGOData(guidlow);
                if (!goData)                                // checked at load
                    break;

                GameObject *door = caster->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, goData->id, guidlow));
                if (!door)
                {
                    sLog.outError("SCRIPT_COMMAND_CLOSE_DOOR (script id %u) failed for gameobject(guid: %u).", step.script->id, guidlow);
                    break;
                }
                if (door->GetGoType() != GAMEOBJECT_TYPE_DOOR)
                {
                    sLog.outError("SCRIPT_COMMAND_CLOSE_DOOR (script id %u) failed for non-door(GoType: %u).", step.script->id, door->GetGoType());
                    break;
                }

                if (door->GetGoState() == GO_STATE_READY)
                    break;                                  //door already closed

                door->UseDoorOrButton(time_to_open);

                if (target && target->isType(TYPEMASK_GAMEOBJECT) && ((GameObject*)target)->GetGoType()==GAMEOBJECT_TYPE_BUTTON)
                    ((GameObject*)target)->UseDoorOrButton(time_to_open);

                break;
            }
            case SCRIPT_COMMAND_ACTIVATE_OBJECT:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_ACTIVATE_OBJECT must have source caster.");
                    break;
                }

                if (!source->isType(TYPEMASK_UNIT))
                {
                    sLog.outError("SCRIPT_COMMAND_ACTIVATE_OBJECT source caster isn't unit (TypeId: %u), skipping.",source->GetTypeId());
                    break;
                }

                if (!target)
                {
                    sLog.outError("SCRIPT_COMMAND_ACTIVATE_OBJECT call for NULL gameobject.");
                    break;
                }

                if (target->GetTypeId()!=TYPEID_GAMEOBJECT)
                {
                    sLog.outError("SCRIPT_COMMAND_ACTIVATE_OBJECT call for non-gameobject (TypeId: %u), skipping.",target->GetTypeId());
                    break;
                }

                Unit* caster = (Unit*)source;

                GameObject *go = (GameObject*)target;

                go->Use(caster);
                break;
            }
            case SCRIPT_COMMAND_REMOVE_AURA:
            {
                Object* cmdTarget = step.script->removeAura.isSourceTarget ? source : target;

                if (!cmdTarget)
                {
                    sLog.outError("SCRIPT_COMMAND_REMOVE_AURA (script id %u) call for NULL %s.", step.script->id, step.script->removeAura.isSourceTarget ? "source" : "target");
                    break;
                }

                if (!cmdTarget->isType(TYPEMASK_UNIT))
                {
                    sLog.outError("SCRIPT_COMMAND_REMOVE_AURA (script id %u) %s isn't unit (TypeId: %u), skipping.", step.script->id, step.script->removeAura.isSourceTarget ? "source" : "target",cmdTarget->GetTypeId());
                    break;
                }

                ((Unit*)cmdTarget)->RemoveAurasDueToSpell(step.script->removeAura.spellId);
                break;
            }
            case SCRIPT_COMMAND_CAST_SPELL:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_CAST_SPELL (script id %u) must have source caster.", step.script->id);
                    break;
                }

                Object* cmdTarget = step.script->castSpell.flags & 0x01 ? source : target;

                if (!cmdTarget)
                {
                    sLog.outError("SCRIPT_COMMAND_CAST_SPELL (script id %u) call for NULL %s.", step.script->id, step.script->castSpell.flags & 0x01 ? "source" : "target");
                    break;
                }

                if (!cmdTarget->isType(TYPEMASK_UNIT))
                {
                    sLog.outError("SCRIPT_COMMAND_CAST_SPELL (script id %u) %s isn't unit (TypeId: %u), skipping.", step.script->id, step.script->castSpell.flags & 0x01 ? "source" : "target", cmdTarget->GetTypeId());
                    break;
                }

                Unit* spellTarget = (Unit*)cmdTarget;

                Object* cmdSource = step.script->castSpell.flags & 0x02 ? target : source;

                if (!cmdSource)
                {
                    sLog.outError("SCRIPT_COMMAND_CAST_SPELL (script id %u) call for NULL %s.", step.script->id, step.script->castSpell.flags & 0x02 ? "target" : "source");
                    break;
                }

                if (!cmdSource->isType(TYPEMASK_UNIT))
                {
                    sLog.outError("SCRIPT_COMMAND_CAST_SPELL (script id %u) %s isn't unit (TypeId: %u), skipping.", step.script->id, step.script->castSpell.flags & 0x02 ? "target" : "source", cmdSource->GetTypeId());
                    break;
                }

                Unit* spellSource = (Unit*)cmdSource;

                //TODO: when GO cast implemented, code below must be updated accordingly to also allow GO spell cast
                spellSource->CastSpell(spellTarget, step.script->castSpell.spellId, (step.script->castSpell.flags & 0x04) != 0);

                break;
            }
            case SCRIPT_COMMAND_PLAY_SOUND:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_PLAY_SOUND (script id %u) call for NULL creature.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_PLAY_SOUND (script id %u) call for non-world object (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;

                // bitmask: 0/1=anyone/target, 0/2=with distance dependent
                Player* pTarget = NULL;

                if (step.script->playSound.flags & 1)
                {
                    if (!target)
                    {
                        sLog.outError("SCRIPT_COMMAND_PLAY_SOUND (script id %u) in targeted mode call for NULL target.", step.script->id);
                        break;
                    }

                    if (target->GetTypeId() != TYPEID_PLAYER)
                    {
                        sLog.outError("SCRIPT_COMMAND_PLAY_SOUND (script id %u) in targeted mode call for non-player (TypeId: %u), skipping.", step.script->id, target->GetTypeId());
                        break;
                    }

                    pTarget = (Player*)target;
                }

                // bitmask: 0/1=anyone/target, 0/2=with distance dependent
                if (step.script->playSound.flags & 2)
                    pSource->PlayDistanceSound(step.script->playSound.soundId, pTarget);
                else
                    pSource->PlayDirectSound(step.script->playSound.soundId, pTarget);

                break;
            }
            case SCRIPT_COMMAND_CREATE_ITEM:
            {
                if (!target && !source)
                {
                    sLog.outError("SCRIPT_COMMAND_CREATE_ITEM (script id %u) call for NULL object.", step.script->id);
                    break;
                }

                // only Player
                if ((!target || target->GetTypeId() != TYPEID_PLAYER) && (!source || source->GetTypeId() != TYPEID_PLAYER))
                {
                    sLog.outError("SCRIPT_COMMAND_CREATE_ITEM (script id %u) call for non-player (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                Player* pReceiver = target && target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : (Player*)source;

                if (Item* pItem = pReceiver->StoreNewItemInInventorySlot(step.script->createItem.itemEntry, step.script->createItem.amount))
                    pReceiver->SendNewItem(pItem, step.script->createItem.amount, true, false);

                break;
            }
            case SCRIPT_COMMAND_DESPAWN_SELF:
            {
                if (!target && !source)
                {
                    sLog.outError("SCRIPT_COMMAND_DESPAWN_SELF (script id %u) call for NULL object.", step.script->id);
                    break;
                }

                // only creature
                if ((!target || target->GetTypeId() != TYPEID_UNIT) && (!source || source->GetTypeId() != TYPEID_UNIT))
                {
                    sLog.outError("SCRIPT_COMMAND_DESPAWN_SELF (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                Creature* pCreature = target && target->GetTypeId() == TYPEID_UNIT ? (Creature*)target : (Creature*)source;

                pCreature->ForcedDespawn(step.script->despawn.despawnDelay);

                break;
            }
            case SCRIPT_COMMAND_PLAY_MOVIE:
            {
                if (!target && !source)
                {
                    sLog.outError("SCRIPT_COMMAND_PLAY_MOVIE (script id %u) call for NULL object.", step.script->id);
                    break;
                }

                // only Player
                if ((!target || target->GetTypeId() != TYPEID_PLAYER) && (!source || source->GetTypeId() != TYPEID_PLAYER))
                {
                    sLog.outError("SCRIPT_COMMAND_PLAY_MOVIE (script id %u) call for non-player (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                Player* pReceiver = target && target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : (Player*)source;

                pReceiver->SendMovieStart(step.script->playMovie.movieId);

                break;
            }
            case SCRIPT_COMMAND_MOVEMENT:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_MOVEMENT (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_MOVEMENT (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pMover = NULL;

                if (!step.script->movement.creatureEntry)   // No buddy defined, so try use source (or target where source is player)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't move source being non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pMover = (Creature*)target;
                    }
                    else
                        pMover = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->movement.creatureEntry, true, step.script->movement.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pMover, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->movement.searchRadius);
                }

                if (!pMover)
                {
                    sLog.outError("SCRIPT_COMMAND_MOVEMENT (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                // Consider add additional checks for cases where creature should not change movementType
                // (pet? in combat? already using same MMgen as script try to apply?)

                switch(step.script->movement.movementType)
                {
                    case IDLE_MOTION_TYPE:
                        pMover->GetMotionMaster()->MoveIdle();
                        break;
                    case RANDOM_MOTION_TYPE:
                        pMover->GetMotionMaster()->MoveRandom();
                        break;
                    case WAYPOINT_MOTION_TYPE:
                        pMover->GetMotionMaster()->MoveWaypoint();
                        break;
                }

                break;
            }
            case SCRIPT_COMMAND_SET_ACTIVEOBJECT:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_SET_ACTIVEOBJECT (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_SET_ACTIVEOBJECT (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pOwner = NULL;

                // No buddy defined, so try use source (or target if source is not creature)
                if (!step.script->activeObject.creatureEntry)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't be non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pOwner = (Creature*)target;
                    }
                    else
                        pOwner = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->activeObject.creatureEntry, true, step.script->activeObject.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pOwner, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->activeObject.searchRadius);
                }

                if (!pOwner)
                {
                    sLog.outError("SCRIPT_COMMAND_SET_ACTIVEOBJECT (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                pOwner->SetActiveObjectState(step.script->activeObject.activate);
                break;
            }
            case SCRIPT_COMMAND_SET_FACTION:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_SET_FACTION (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_SET_FACTION (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pOwner = NULL;

                // No buddy defined, so try use source (or target if source is not creature)
                if (!step.script->faction.creatureEntry)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't be non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pOwner = (Creature*)target;
                    }
                    else
                        pOwner = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->faction.creatureEntry, true, step.script->faction.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pOwner, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->faction.searchRadius);
                }

                if (!pOwner)
                {
                    sLog.outError("SCRIPT_COMMAND_SET_FACTION (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                if (step.script->faction.factionId)
                    pOwner->SetFactionTemporary(step.script->faction.factionId, step.script->faction.flags);
                else
                    pOwner->ClearTemporaryFaction();

                break;
            }
            case SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pOwner = NULL;

                // No buddy defined, so try use source (or target if source is not creature)
                if (!step.script->morph.creatureEntry)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't be non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pOwner = (Creature*)target;
                    }
                    else
                        pOwner = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->morph.creatureEntry, true, step.script->morph.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pOwner, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->morph.searchRadius);
                }

                if (!pOwner)
                {
                    sLog.outError("SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                if (!step.script->morph.creatureOrModelEntry)
                    pOwner->DeMorph();
                else if (step.script->morph.flags & 0x01)
                    pOwner->SetDisplayId(step.script->morph.creatureOrModelEntry);
                else
                {
                    CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(step.script->morph.creatureOrModelEntry);
                    uint32 display_id = Creature::ChooseDisplayId(ci);

                    pOwner->SetDisplayId(display_id);
                }

                break;
            }
            case SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pOwner = NULL;

                // No buddy defined, so try use source (or target if source is not creature)
                if (!step.script->mount.creatureEntry)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't be non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pOwner = (Creature*)target;
                    }
                    else
                        pOwner = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->mount.creatureEntry, true, step.script->mount.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pOwner, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->mount.searchRadius);
                }

                if (!pOwner)
                {
                    sLog.outError("SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                if (!step.script->mount.creatureOrModelEntry)
                    pOwner->Unmount();
                else if (step.script->mount.flags & 0x01)
                    pOwner->Mount(step.script->mount.creatureOrModelEntry);
                else
                {
                    CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(step.script->mount.creatureOrModelEntry);
                    uint32 display_id = Creature::ChooseDisplayId(ci);

                    pOwner->Mount(display_id);
                }

                break;
            }
            case SCRIPT_COMMAND_SET_RUN:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_SET_RUN (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_SET_RUN (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pOwner = NULL;

                // No buddy defined, so try use source (or target if source is not creature)
                if (!step.script->run.creatureEntry)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't be non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pOwner = (Creature*)target;
                    }
                    else
                        pOwner = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->run.creatureEntry, true, step.script->run.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pOwner, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->run.searchRadius);
                }

                if (!pOwner)
                {
                    sLog.outError("SCRIPT_COMMAND_SET_RUN (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                pOwner->SetWalk(!step.script->run.run);

                break;
            }
            case SCRIPT_COMMAND_ATTACK_START:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_ATTACK_START (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_ATTACK_START (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pBuddy = NULL;

                // flag_original_source_as_target   0x02
                // flag_buddy_as_target             0x04

                // If step has a buddy entry defined, search for it.
                if (step.script->attack.creatureEntry)
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->attack.creatureEntry, true, step.script->attack.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pBuddy, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->attack.searchRadius);

                    // If buddy found, then use it
                    if (pBuddy)
                    {
                        if (step.script->attack.flags & 0x04)
                        {
                            // pBuddy is target of attack
                            target = (Object*)pBuddy;
                        }
                        else
                        {
                            // If not target of attack, then set pBuddy as source, the attacker
                            pSource = (WorldObject*)pBuddy;
                        }
                    }
                    else
                    {
                        // No buddy found, so don't do anything
                        break;
                    }
                }

                // If we should attack the original source instead of target
                if (step.script->attack.flags & 0x02)
                    target = source;

                Unit* unitTarget = target && target->isType(TYPEMASK_UNIT) ? static_cast<Unit*>(target) : NULL;
                Creature* pAttacker = pSource && pSource->GetTypeId() == TYPEID_UNIT ? static_cast<Creature*>(pSource) : NULL;

                if (pAttacker && unitTarget)
                {
                    if (pAttacker->IsFriendlyTo(unitTarget))
                    {
                        sLog.outError("SCRIPT_COMMAND_ATTACK_START (script id %u) attacker is friendly to target, can not attack.", step.script->id);
                        break;
                    }

                    pAttacker->AI()->AttackStart(unitTarget);
                    break;
                }

                sLog.outError("SCRIPT_COMMAND_ATTACK_START (script id %u) unexpected error, attacker or victim could not be found, no action.", step.script->id);
                break;
            }
            case SCRIPT_COMMAND_GO_LOCK_STATE:
            {
                if ((!source || !source->isType(TYPEMASK_WORLDOBJECT)) && (!target || !target->isType(TYPEMASK_WORLDOBJECT)))
                {
                    sLog.outError("SCRIPT_COMMAND_GO_LOCK_STATE (script id %u) call for non-worldobject (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                WorldObject* pSearcher = source && source->isType(TYPEMASK_WORLDOBJECT) ? (WorldObject*)source : (WorldObject*)target;
                GameObject* pGo = NULL;

                MaNGOS::NearestGameObjectEntryInObjectRangeCheck u_check(*pSearcher, step.script->goLockState.goEntry, step.script->goLockState.searchRadius);
                MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck> searcher(pGo, u_check);

                Cell::VisitGridObjects(pSearcher, searcher, step.script->goLockState.searchRadius);

                /* flag lockState
                 * go_lock          0x01
                 * go_unlock        0x02
                 * go_nonInteract   0x04
                 * go_Interact      0x08
                 */
                if (pGo)
                {
                    // Lock or Unlock
                    if (step.script->goLockState.lockState & 0x01)
                        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    else if (step.script->goLockState.lockState & 0x02)
                        pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    // Set Non Interactable or Set Interactable
                    if (step.script->goLockState.lockState & 0x04)
                        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                    else if (step.script->goLockState.lockState & 0x08)
                        pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                }
            }
            case SCRIPT_COMMAND_STAND_STATE:
            {
                if (!source)
                {
                    sLog.outError("SCRIPT_COMMAND_STAND_STATE (script id %u) call for NULL source.", step.script->id);
                    break;
                }

                if (!source->isType(TYPEMASK_WORLDOBJECT))
                {
                    sLog.outError("SCRIPT_COMMAND_STAND_STATE (script id %u) call for non-worldobject (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                    break;
                }
                // When creatureEntry is not defined, GameObject can not be source
                else if (!step.script->standState.creatureEntry)
                {
                    if (!source->isType(TYPEMASK_UNIT))
                    {
                        sLog.outError("SCRIPT_COMMAND_STAND_STATE (script id %u) are missing datalong2 (creature entry). Unsupported call for non-unit (TypeId: %u), skipping.", step.script->id, source->GetTypeId());
                        break;
                    }
                }

                WorldObject* pSource = (WorldObject*)source;
                Creature* pBuddy = NULL;

                // flag_target_as_source            0x01

                // If target is Unit* and should change it's stand state (or should be source of searcher below)
                if (target && target->isType(TYPEMASK_UNIT) && step.script->standState.flags & 0x01)
                    pSource = (WorldObject*)target;

                // If step has a buddy entry defined, search for it.
                if (step.script->standState.creatureEntry)
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->standState.creatureEntry, true, step.script->standState.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pBuddy, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->standState.searchRadius);

                    // If buddy found, then use it or break (break since we must assume pBuddy was defined for a reason)
                    if (pBuddy)
                        pSource = (WorldObject*)pBuddy;
                    else
                        break;
                }

                // Must be safe cast to Unit* here
                ((Unit*)pSource)->SetStandState(step.script->standState.stand_state);
                break;
            }
            case SCRIPT_COMMAND_MODIFY_NPC_FLAGS:
            {
                if (!source && !target)
                {
                    sLog.outError("SCRIPT_COMMAND_MODIFY_NPC_FLAGS (script id %u) call for NULL source and NULL target.", step.script->id);
                    break;
                }

                if ((!source || !source->isType(TYPEMASK_WORLDOBJECT)) && (!target || !target->isType(TYPEMASK_WORLDOBJECT)))
                {
                    sLog.outError("SCRIPT_COMMAND_MODIFY_NPC_FLAGS (script id %u) call for unsupported non-worldobject (TypeId: %u), skipping.", step.script->id, source ? source->GetTypeId() : target->GetTypeId());
                    break;
                }

                WorldObject* pSource = source && source->isType(TYPEMASK_WORLDOBJECT) ? (WorldObject*)source : (WorldObject*)target;
                Creature* pBuddy = NULL;

                // No buddy defined, so try use source (or target if source is not creature)
                if (!step.script->npcFlag.creatureEntry)
                {
                    if (pSource->GetTypeId() != TYPEID_UNIT)
                    {
                        // we can't be non-creature, so see if target is creature
                        if (target && target->GetTypeId() == TYPEID_UNIT)
                            pBuddy = (Creature*)target;
                    }
                    else
                        pBuddy = (Creature*)pSource;
                }
                else                                        // If step has a buddy entry defined, search for it
                {
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*pSource, step.script->npcFlag.creatureEntry, true, step.script->npcFlag.searchRadius);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pBuddy, u_check);

                    Cell::VisitGridObjects(pSource, searcher, step.script->npcFlag.searchRadius);
                }

                if (!pBuddy)
                {
                    sLog.outError("SCRIPT_COMMAND_MODIFY_NPC_FLAGS (script id %u) call for non-creature (TypeIdSource: %u)(TypeIdTarget: %u), skipping.", step.script->id, source ? source->GetTypeId() : 0, target ? target->GetTypeId() : 0);
                    break;
                }

                // Add Flags
                if (step.script->npcFlag.data_flags & 0x01)
                    pBuddy->SetFlag(UNIT_NPC_FLAGS, step.script->npcFlag.flag);
                // Remove Flags
                else if (step.script->npcFlag.data_flags & 0x02)
                    pBuddy->RemoveFlag(UNIT_NPC_FLAGS, step.script->npcFlag.flag);
                // Toggle Flags
                else
                {
                    if (pBuddy->HasFlag(UNIT_NPC_FLAGS, step.script->npcFlag.flag))
                        pBuddy->RemoveFlag(UNIT_NPC_FLAGS, step.script->npcFlag.flag);
                    else
                        pBuddy->SetFlag(UNIT_NPC_FLAGS, step.script->npcFlag.flag);
                }

                break;
            }
            default:
                sLog.outError("Unknown SCRIPT_COMMAND_ %u called for script id %u.", step.script->command, step.script->id);
                break;
        }

        m_scriptSchedule.erase(iter);
        iter = m_scriptSchedule.begin();

        sScriptMgr.DecreaseScheduledScriptCount();
    }
}

/**
 * Function return player that in world at CURRENT map
 *
 * Note: This is function preferred if you sure that need player only placed at specific map
 *       This is not true for some spell cast targeting and most packet handlers
 *
 * @param guid must be player guid (HIGHGUID_PLAYER)
 */
Player* Map::GetPlayer(ObjectGuid guid)
{
    Player* plr = ObjectAccessor::FindPlayer(guid);         // return only in world players
    return plr && plr->GetMap() == this ? plr : NULL;
}

/**
 * Function return creature (non-pet and then most summoned by spell creatures) that in world at CURRENT map
 *
 * @param guid must be creature or vehicle guid (HIGHGUID_UNIT HIGHGUID_VEHICLE)
 */
Creature* Map::GetCreature(ObjectGuid guid)
{
    return m_objectsStore.find<Creature>(guid, (Creature*)NULL);
}

/**
 * Function return pet that in world at CURRENT map
 *
 * @param guid must be pet guid (HIGHGUID_PET)
 */
Pet* Map::GetPet(ObjectGuid guid)
{
    return m_objectsStore.find<Pet>(guid, (Pet*)NULL);
}

/**
 * Function return corpse that at CURRENT map
 *
 * Note: corpse can be NOT IN WORLD, so can't be used corpse->GetMap() without pre-check corpse->isInWorld()
 *
 * @param guid must be corpse guid (HIGHGUID_CORPSE)
 */
Corpse* Map::GetCorpse(ObjectGuid guid)
{
    Corpse * ret = ObjectAccessor::GetCorpseInMap(guid,GetId());
    return ret && ret->GetInstanceId() == GetInstanceId() ? ret : NULL;
}

/**
 * Function return non-player unit object that in world at CURRENT map, so creature, or pet, or vehicle
 *
 * @param guid must be non-player unit guid (HIGHGUID_PET HIGHGUID_UNIT HIGHGUID_VEHICLE)
 */
Creature* Map::GetAnyTypeCreature(ObjectGuid guid)
{
    switch(guid.GetHigh())
    {
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:      return GetCreature(guid);
        case HIGHGUID_PET:          return GetPet(guid);
        default:                    break;
    }

    return NULL;
}

/**
 * Function return gameobject that in world at CURRENT map
 *
 * @param guid must be gameobject guid (HIGHGUID_GAMEOBJECT)
 */
GameObject* Map::GetGameObject(ObjectGuid guid)
{
    return m_objectsStore.find<GameObject>(guid, (GameObject*)NULL);
}

/**
 * Function return dynamic object that in world at CURRENT map
 *
 * @param guid must be dynamic object guid (HIGHGUID_DYNAMICOBJECT)
 */
DynamicObject* Map::GetDynamicObject(ObjectGuid guid)
{
    return m_objectsStore.find<DynamicObject>(guid, (DynamicObject*)NULL);
}

/**
 * Function return unit in world at CURRENT map
 *
 * Note: in case player guid not always expected need player at current map only.
 *       For example in spell casting can be expected any in world player targeting in some cases
 *
 * @param guid must be unit guid (HIGHGUID_PLAYER HIGHGUID_PET HIGHGUID_UNIT HIGHGUID_VEHICLE)
 */
Unit* Map::GetUnit(ObjectGuid guid)
{
    if (guid.IsPlayer())
        return GetPlayer(guid);

    return GetAnyTypeCreature(guid);
}

/**
 * Function return world object in world at CURRENT map, so any except transports
 */
WorldObject* Map::GetWorldObject(ObjectGuid guid)
{
    switch(guid.GetHigh())
    {
        case HIGHGUID_PLAYER:       return GetPlayer(guid);
        case HIGHGUID_GAMEOBJECT:   return GetGameObject(guid);
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:      return GetCreature(guid);
        case HIGHGUID_PET:          return GetPet(guid);
        case HIGHGUID_DYNAMICOBJECT:return GetDynamicObject(guid);
        case HIGHGUID_CORPSE:
        {
            // corpse special case, it can be not in world
            Corpse* corpse = GetCorpse(guid);
            return corpse && corpse->IsInWorld() ? corpse : NULL;
        }
        case HIGHGUID_MO_TRANSPORT:
        case HIGHGUID_TRANSPORT:
        default:                    break;
    }

    return NULL;
}

void Map::SendObjectUpdates()
{
    UpdateDataMapType update_players;

    while(!i_objectsToClientUpdateQueue.empty())
    {
        if (i_objectsToClientNotUpdate.find(i_objectsToClientUpdateQueue.front()) == i_objectsToClientNotUpdate.end())
            i_objectsToClientUpdate.insert(i_objectsToClientUpdateQueue.front());
        i_objectsToClientUpdateQueue.pop();
    }
    i_objectsToClientNotUpdate.clear();

    while(!i_objectsToClientUpdate.empty())
    {
        Object* obj = *i_objectsToClientUpdate.begin();
        i_objectsToClientUpdate.erase(i_objectsToClientUpdate.begin());
        if (obj && obj->IsInWorld())
            obj->BuildUpdateData(update_players);
    }

    if (!update_players.empty())
    {
        for (UpdateDataMapType::iterator iter = update_players.begin(); iter != update_players.end(); ++iter)
        {
            if (!iter->first || !iter->first->IsInWorld())
                continue;

            WorldPacket packet;                                     // here we allocate a std::vector with a size of 0x10000
            if (iter->second.BuildPacket(&packet))
                iter->first->GetSession()->SendPacket(&packet);
        }
    }
}

uint32 Map::GenerateLocalLowGuid(HighGuid guidhigh)
{
    // TODO: for map local guid counters possible force reload map instead shutdown server at guid counter overflow
    switch(guidhigh)
    {
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:
            return m_CreatureGuids.Generate();
        case HIGHGUID_GAMEOBJECT:
            return m_GameObjectGuids.Generate();
        case HIGHGUID_DYNAMICOBJECT:
            return m_DynObjectGuids.Generate();
        case HIGHGUID_PET:
            return m_PetGuids.Generate();
        default:
            MANGOS_ASSERT(false);
            return 0;
    }
}

/**
 * Helper structure for building static chat information
 *
 */
class StaticMonsterChatBuilder
{
    public:
        StaticMonsterChatBuilder(CreatureInfo const* cInfo, ChatMsg msgtype, int32 textId, uint32 language, Unit* target, uint32 senderLowGuid = 0)
            : i_cInfo(cInfo), i_msgtype(msgtype), i_textId(textId), i_language(language), i_target(target)
        {
            // 0 lowguid not used in core, but accepted fine in this case by client
            i_senderGuid = i_cInfo->GetObjectGuid(senderLowGuid);
        }
        void operator()(WorldPacket& data, int32 loc_idx)
        {
            char const* text = sObjectMgr.GetMangosString(i_textId, loc_idx);

            char const* nameForLocale = i_cInfo->Name;
            sObjectMgr.GetCreatureLocaleStrings(i_cInfo->Entry, loc_idx, &nameForLocale);

            WorldObject::BuildMonsterChat(&data, i_senderGuid, i_msgtype, text, i_language, nameForLocale, i_target ? i_target->GetObjectGuid() : ObjectGuid(), i_target ? i_target->GetNameForLocaleIdx(loc_idx) : "");
        }

    private:
        ObjectGuid i_senderGuid;
        CreatureInfo const* i_cInfo;
        ChatMsg i_msgtype;
        int32 i_textId;
        uint32 i_language;
        Unit* i_target;
};


/**
 * Function simulates yell of creature
 *
 * @param guid must be creature guid of whom to Simulate the yell, non-creature guids not supported at this moment
 * @param textId Id of the simulated text
 * @param language language of the text
 * @param target, can be NULL
 */
void Map::MonsterYellToMap(ObjectGuid guid, int32 textId, uint32 language, Unit* target)
{
    if (guid.IsAnyTypeCreature())
    {
        CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(guid.GetEntry());
        if (!cInfo)
        {
            sLog.outError("Map::MonsterYellToMap: Called for nonexistent creature entry in guid: %s", guid.GetString().c_str());
            return;
        }

        MonsterYellToMap(cInfo, textId, language, target, guid.GetCounter());
    }
    else
    {
        sLog.outError("Map::MonsterYellToMap: Called for non creature guid: %s", guid.GetString().c_str());
        return;
    }
}


/**
 * Function simulates yell of creature
 *
 * @param cinfo must be entry of Creature of whom to Simulate the yell
 * @param textId Id of the simulated text
 * @param language language of the text
 * @param target, can be NULL
 * @param senderLowGuid provide way proper show yell for near spawned creature with known lowguid,
 *        0 accepted by client else if this not important
 */
void Map::MonsterYellToMap(CreatureInfo const* cinfo, int32 textId, uint32 language, Unit* target, uint32 senderLowGuid /*= 0*/)
{
    StaticMonsterChatBuilder say_build(cinfo, CHAT_MSG_MONSTER_YELL, textId, language, target, senderLowGuid);
    MaNGOS::LocalizedPacketDo<StaticMonsterChatBuilder> say_do(say_build);

    Map::PlayerList const& pList = GetPlayers();
    for (PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        say_do(itr->getSource());
}

/**
 * Function to play sound to all players in map
 *
 * @param soundId Played Sound
 * @param zoneId Id of the Zone to which the sound should be restricted
 */
void Map::PlayDirectSoundToMap(uint32 soundId, uint32 zoneId /*=0*/)
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << uint32(soundId);

    Map::PlayerList const& pList = GetPlayers();
    for (PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        if (!zoneId || itr->getSource()->GetZoneId() == zoneId)
            itr->getSource()->SendDirectMessage(&data);
}

/**
* Function to change weather in zone and change weather on map from script. 
*
* need base support (With Values 0)
*/

void Map::SetMapWeather(WeatherState state, float grade)
{
    //Weather is OFF
    if (!sWorld.getConfig(CONFIG_BOOL_WEATHER))
        return;

    if (grade < 0.0f || grade > 1.0f)
        return;

    if (!IsDungeon())
        return;

    WorldPacket data(SMSG_WEATHER, (4+4+4));
    data << uint32(state) << (float)grade << uint8(0);

    ((DungeonMap*)this)->SendToPlayers(&data);
}

bool Map::SetZoneWeather(uint32 zoneId, WeatherType type, float grade)
{
    //Weather is OFF
    if (!sWorld.getConfig(CONFIG_BOOL_WEATHER))
        return false;

    if (grade < 0.0f || grade > 1.0f || MAX_WEATHER_TYPE <= type)
        return false;

    Weather* weather = sWorld.FindWeather(zoneId);

    if (!weather)
        weather = sWorld.AddWeather(zoneId);

    if (!weather)
        return false;

    weather->SetWeather(type, grade);

    return true;
}

/**
 * Function to operations with attackers per-map storage
 *
 * @param targetGuid (attackerGuid)
 */

void Map::AddAttackerFor(ObjectGuid targetGuid, ObjectGuid attackerGuid)
{
    if (targetGuid.IsEmpty() || attackerGuid.IsEmpty())
        return;

    WriteGuard Guard(GetLock());
    AttackersMap::iterator itr = m_attackersMap.find(targetGuid);
    if (itr != m_attackersMap.end())
    {
        itr->second.insert(attackerGuid);
    }
    else
    {
        CreateAttackersStorageFor(targetGuid);
        m_attackersMap[targetGuid].insert(attackerGuid);
    }
}

void Map::RemoveAttackerFor(ObjectGuid targetGuid, ObjectGuid attackerGuid)
{
    if (targetGuid.IsEmpty() || attackerGuid.IsEmpty())
        return;

    WriteGuard Guard(GetLock());
    AttackersMap::iterator itr = m_attackersMap.find(targetGuid);
    if (itr != m_attackersMap.end())
    {
        itr->second.erase(attackerGuid);
    }
}

void Map::RemoveAllAttackersFor(ObjectGuid targetGuid)
{
    if (targetGuid.IsEmpty())
        return;

    WriteGuard Guard(GetLock());
    AttackersMap::iterator itr = m_attackersMap.find(targetGuid);
    if (itr != m_attackersMap.end())
    {
        itr->second.clear();
    }
}

ObjectGuidSet Map::GetAttackersFor(ObjectGuid targetGuid)
{
    if (!targetGuid.IsEmpty())
    {
        ReadGuard Guard(GetLock());
        AttackersMap::const_iterator itr = m_attackersMap.find(targetGuid);
        if (itr != m_attackersMap.end())
            return itr->second;
    }

    return ObjectGuidSet();
}

void Map::CreateAttackersStorageFor(ObjectGuid targetGuid)
{
    if (targetGuid.IsEmpty())
        return;

    AttackersMap::iterator itr = m_attackersMap.find(targetGuid);
    if (itr == m_attackersMap.end())
    {
        m_attackersMap.insert(std::make_pair(targetGuid,ObjectGuidSet()));
    }

}

void Map::RemoveAttackersStorageFor(ObjectGuid targetGuid)
{
    if (targetGuid.IsEmpty())
        return;

    WriteGuard Guard(GetLock());
    AttackersMap::iterator itr = m_attackersMap.find(targetGuid);
    if (itr != m_attackersMap.end())
    {
        m_attackersMap.erase(itr);
    }
}

void Map::ForcedUnload()
{
    sLog.outError("Map::ForcedUnload called for map %u instance %u. Map crushed. Cleaning up...", GetId(), GetInstanceId());

    // Immediately cleanup update sets/queues
    i_objectsToClientUpdate.clear();
    i_objectsToClientNotUpdate.clear();
    while (!i_objectsToClientUpdateQueue.empty())
        i_objectsToClientUpdateQueue.pop();


    Map::PlayerList const pList = GetPlayers();
    for (PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        Player* player = itr->getSource();
        if (!player || !player->GetSession())
            continue;

        if (player->IsBeingTeleportedFar())
        {
            WorldLocation old_loc;
            player->GetPosition(old_loc);
            if (!player->TeleportTo(old_loc))
            {
                DETAIL_LOG("Map::ForcedUnload: %s is in teleport state, cannot be ported to his previous place, teleporting him to his homebind place...",
                    player->GetGuidStr().c_str());
                player->TeleportToHomebind();
            }
            player->SetSemaphoreTeleportFar(false);
        }

        switch (sWorld.getConfig(CONFIG_UINT32_VMSS_MAPFREEMETHOD))
        {
            case 0:
            {
                player->RemoveAllAurasOnDeath();
                if (Pet* pet = player->GetPet())
                    pet->RemoveAllAurasOnDeath();
                player->GetSession()->LogoutPlayer(true);
                break;
            }
            case 1:
            {
                player->GetSession()->KickPlayer();
                Remove(player, true);
                break;
            }
            case 2:
            {
                player->GetSession()->LogoutPlayer(false);
                Remove(player, true);
                break;
            }
            default:
                break;
        }
    }

    switch (sWorld.getConfig(CONFIG_UINT32_VMSS_MAPFREEMETHOD))
    {
        case 0:
            if (InstanceData* iData = GetInstanceData())
                iData->Save();
            break;
        case 1:
        case 2:
        default:
            break;
    }

    UnloadAll(true);

    RemoveAllObjectsInRemoveList();

    SetBroken(false);
}

float Map::GetVisibilityDistance(WorldObject* obj) const 
{
    if (obj && obj->GetObjectGuid().IsGameObject())
        return (m_VisibleDistance + ((GameObject*)obj)->GetDeterminativeSize());
    else
        return m_VisibleDistance; 
}
