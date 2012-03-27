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

#include "MotionMaster.h"
#include "CreatureAISelector.h"
#include "Creature.h"
#include "ConfusedMovementGenerator.h"
#include "FleeingMovementGenerator.h"
#include "HomeMovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "movement/MoveSpline.h"
#include "movement/MoveSplineInit.h"
#include "CreatureLinkingMgr.h"

#include <cassert>

inline bool isStatic(MovementGenerator *mv)
{
    return (mv == &si_idleMovement);
}

void MotionMaster::Initialize()
{
    // stop current move
    if (!m_owner->IsStopped())
        m_owner->StopMoving();

    // set new default movement generator
    if (m_owner->GetTypeId() == TYPEID_UNIT /*&& !m_owner->hasUnitState(UNIT_STAT_CONTROLLED)*/)
    {
        MovementGenerator* movement = FactorySelector::selectMovementGenerator((Creature*)m_owner);
        if (movement)
            Mutate(movement, UNIT_ACTION_DOWAYPOINTS);
    }
}

MotionMaster::~MotionMaster()
{
}

void MotionMaster::MoveIdle()
{
    impl()->DropAllStates();
}

void MotionMaster::MoveRandom()
{
    if (m_owner->GetTypeId() == TYPEID_PLAYER)
    {
        sLog.outError("MotionMaster: %s attempt to move random.", m_owner->GetGuidStr().c_str());
    }
    else
    {
        DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "%s move random.", m_owner->GetGuidStr().c_str());
        Mutate(new RandomMovementGenerator<Creature>(*m_owner), UNIT_ACTION_DOWAYPOINTS);
    }
}

void MotionMaster::MoveTargetedHome()
{
   // if (m_owner->hasUnitState(UNIT_STAT_LOST_CONTROL))
       // return;

    // home movement is command that returns AI to his default state
    impl()->DropAllStates();

    if (m_owner->GetTypeId() == TYPEID_UNIT && !((Creature*)m_owner)->GetCharmerOrOwnerGuid())
    {
        // Manual exception for linked mobs
        if (m_owner->IsLinkingEventTrigger() && m_owner->GetMap()->GetCreatureLinkingHolder()->TryFollowMaster((Creature*)m_owner))
            DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s refollowed linked master", m_owner->GetGuidStr().c_str());
        else
        {
            DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s targeted home", m_owner->GetGuidStr().c_str());
            Mutate(new HomeMovementGenerator<Creature>(), UNIT_ACTION_EFFECT);
        }
    }
    else if (m_owner->GetTypeId() == TYPEID_UNIT && ((Creature*)m_owner)->GetCharmerOrOwnerGuid())
    {
        if (Unit *target = ((Creature*)m_owner)->GetCharmerOrOwner())
        {
            DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s follow to %s", m_owner->GetGuidStr().c_str(), target->GetGuidStr().c_str());
            Mutate(new FollowMovementGenerator<Creature>(*target,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE), UNIT_ACTION_DOWAYPOINTS);
        }
        else
        {
            DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "%s attempt but fail to follow owner", m_owner->GetGuidStr().c_str());
        }
    }
    else
        sLog.outError("MotionMaster: %s attempt targeted home", m_owner->GetGuidStr().c_str());
}

void MotionMaster::MoveConfused()
{
    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s move confused", m_owner->GetGuidStr().c_str());

    if (m_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new ConfusedMovementGenerator<Player>(), UNIT_ACTION_CONFUSED);
    else
        Mutate(new ConfusedMovementGenerator<Creature>(), UNIT_ACTION_CONFUSED);
}

void MotionMaster::MoveChase(Unit* target, float dist, float angle)
{
    // ignore movement request if target not exist
    if (!target)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s chase to %s", m_owner->GetGuidStr().c_str(), target->GetGuidStr().c_str());

    if (m_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new ChaseMovementGenerator<Player>(*target,dist,angle), UNIT_ACTION_CHASE);
    else
        Mutate(new ChaseMovementGenerator<Creature>(*target,dist,angle), UNIT_ACTION_CHASE);
}

void MotionMaster::MoveFollow(Unit* target, float dist, float angle)
{
    // ignore movement request if target not exist
    if (!target)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s follow to %s", m_owner->GetGuidStr().c_str(), target->GetGuidStr().c_str());

    if (m_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new FollowMovementGenerator<Player>(*target,dist,angle), UNIT_ACTION_CHASE);
    else
        Mutate(new FollowMovementGenerator<Creature>(*target,dist,angle), UNIT_ACTION_CHASE);
}

void MotionMaster::MovePoint(uint32 id, float x, float y, float z, bool generatePath)
{
    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s targeted point (Id: %u X: %f Y: %f Z: %f)", m_owner->GetGuidStr().c_str(), id, x, y, z );

    if (m_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new PointMovementGenerator<Player>(id,x,y,z, generatePath), UNIT_ACTION_ASSISTANCE);
    else
        Mutate(new PointMovementGenerator<Creature>(id,x,y,z, generatePath), UNIT_ACTION_ASSISTANCE);
}

void MotionMaster::MoveSeekAssistance(float x, float y, float z)
{
    if (m_owner->GetTypeId() == TYPEID_PLAYER)
    {
        sLog.outError("MotionMaster: %s attempt to seek assistance", m_owner->GetGuidStr().c_str());
    }
    else
    {
        DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s seek assistance (X: %f Y: %f Z: %f)",
            m_owner->GetGuidStr().c_str(), x, y, z );
        Mutate(new AssistanceMovementGenerator(x,y,z), UNIT_ACTION_ASSISTANCE);
    }
}

void MotionMaster::MoveSeekAssistanceDistract(uint32 time)
{
    if (m_owner->GetTypeId() == TYPEID_PLAYER)
    {
        sLog.outError("MotionMaster: %s attempt to call distract after assistance", m_owner->GetGuidStr().c_str());
    }
    else
    {
        DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s is distracted after assistance call (Time: %u)",
            m_owner->GetGuidStr().c_str(), time );
        Mutate(new AssistanceDistractMovementGenerator(time), UNIT_ACTION_ASSISTANCE);
    }
}

void MotionMaster::MoveFleeing(Unit* enemy, uint32 time)
{
    if (!enemy)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s flee from %s", m_owner->GetGuidStr().c_str(), enemy->GetGuidStr().c_str());

    if (m_owner->GetTypeId() == TYPEID_PLAYER)
        Mutate(new FleeingMovementGenerator<Player>(enemy->GetObjectGuid()), UNIT_ACTION_FEARED);
    else
    {
        if (time)
            Mutate(new TimedFleeingMovementGenerator(enemy->GetObjectGuid(), time), UNIT_ACTION_FEARED);
        else
            Mutate(new FleeingMovementGenerator<Creature>(enemy->GetObjectGuid()), UNIT_ACTION_FEARED);
    }
}

void MotionMaster::MoveWaypoint()
{
    if (m_owner->GetTypeId() == TYPEID_UNIT)
    {
        if (GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
        {
            sLog.outError("MotionMaster: Creature %s (Entry %u) attempt to MoveWaypoint() but creature is already using waypoint", m_owner->GetGuidStr().c_str(), m_owner->GetEntry());
            return;
        }

        Creature* creature = (Creature*)m_owner;

        DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: Creature %s (Entry %u) start MoveWaypoint()", m_owner->GetGuidStr().c_str(), m_owner->GetEntry());
        Mutate(new WaypointMovementGenerator<Creature>(*creature), UNIT_ACTION_DOWAYPOINTS);
    }
    else
    {
        sLog.outError("MotionMaster: Non-creature %s attempt to MoveWaypoint()", m_owner->GetGuidStr().c_str());
    }
}

void MotionMaster::MoveTaxiFlight(uint32 path, uint32 pathnode)
{
    if (m_owner->GetTypeId() == TYPEID_PLAYER)
    {
        if (path < sTaxiPathNodesByPath.size())
        {
            DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s taxi to (Path %u node %u)", m_owner->GetGuidStr().c_str(), path, pathnode);
            Mutate(new FlightPathMovementGenerator(sTaxiPathNodesByPath[path],pathnode), UNIT_ACTION_TAXI);
        }
        else
        {
            sLog.outError("MotionMaster: %s attempt taxi to (nonexistent Path %u node %u)",
                m_owner->GetGuidStr().c_str(), path, pathnode);
        }
    }
    else
    {
        sLog.outError("MotionMaster: %s attempt taxi to (Path %u node %u)",
            m_owner->GetGuidStr().c_str(), path, pathnode);
    }
}

void MotionMaster::MoveDistract(uint32 timer)
{
    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "MotionMaster: %s distracted (timer: %u)", m_owner->GetGuidStr().c_str(), timer);
    DistractMovementGenerator* mgen = new DistractMovementGenerator(timer);
    Mutate(mgen, UNIT_ACTION_EFFECT);
}

void MotionMaster::Mutate(MovementGenerator* mgen, UnitActionId stateId)
{
    impl()->PushAction(stateId, UnitActionPtr(mgen));
}

void MotionMaster::propagateSpeedChange()
{
//    impl()->UnitSpeedChanged();
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
{
    return const_cast<MotionMaster*>(this)->top()->GetMovementGeneratorType();
}

bool MotionMaster::GetDestination(float &x, float &y, float &z)
{
    if (m_owner->movespline->Finalized())
        return false;

    const G3D::Vector3& dest = m_owner->movespline->FinalDestination();
    x = dest.x;
    y = dest.y;
    z = dest.z;
    return true;
}

void MotionMaster::UpdateFinalDistanceToTarget(float fDistance)
{
    if (top()->GetMovementGeneratorType() == FOLLOW_MOTION_TYPE)
        top()->UpdateFinalDistance(fDistance);
}

MotionMaster::MotionMaster(Unit *unit) : m_owner(unit)
{
}

/** Does nothing */
void MotionMaster::Clear(bool reset /*= true*/, bool all /*= false*/)
{
    if (all)
        impl()->InitDefaults();
    else if (reset)
        impl()->DropAllStates();
}

MovementGenerator* MotionMaster::top()
{
    UnitActionPtr mgen = impl()->CurrentAction();
    if (!mgen)
    {
        sLog.outError("MotionMaster::top() %s has empty states list!", m_owner->GetGuidStr().c_str());
        return &si_idleMovement;
    }
    return ((MovementGenerator*)&*mgen);
}

UnitStateMgr* MotionMaster::impl()
{
    MANGOS_ASSERT(m_owner);
    UnitStateMgr* mgr = &m_owner->GetUnitStateMgr();
    MANGOS_ASSERT(mgr);
    return mgr;
}

bool MotionMaster::empty()
{
    MovementGenerator* mgen = top();
    if (isStatic(mgen))
        return true;

    return (mgen->GetMovementGeneratorType() == IDLE_MOTION_TYPE);
}

void MotionMaster::MoveJump(float x, float y, float z, float horizontalSpeed, float max_height, uint32 id)
{
    Movement::MoveSplineInit init(*m_owner);
    init.MoveTo(x,y,z);
    init.SetParabolic(max_height, 0);
    init.SetVelocity(horizontalSpeed);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), UNIT_ACTION_EFFECT);
}

void MotionMaster::MoveToDestination(float x, float y, float z, float o, Unit* target, float horizontalSpeed, float max_height, uint32 id)
{
    Movement::MoveSplineInit init(*m_owner);
    init.MoveTo(x,y,z, bool(target), bool(target));
    if (max_height > M_NULL_F)
        init.SetParabolic(max_height, 0);
    init.SetVelocity(horizontalSpeed);
    if (target)
        init.SetFacing(target);
    else
        init.SetFacing(o);
    init.Launch();
    Mutate(new EffectMovementGenerator(id), UNIT_ACTION_EFFECT);
}

void MotionMaster::MoveSkyDiving(float x, float y, float z, float o, float horizontalSpeed, float max_height, bool eject)
{
    Movement::MoveSplineInit init(*m_owner);
    init.MoveTo(x,y,z,false);
    init.SetParabolic(max_height, 0);
    init.SetVelocity(horizontalSpeed);
    init.SetFacing(o);
    if (!eject)
        init.SetTransportExit();
    init.Launch();
    Mutate(new EffectMovementGenerator(0), UNIT_ACTION_EFFECT);
}

void MotionMaster::MoveFall()
{
    // use larger distance for vmap height search than in most other cases
    float tz = m_owner->GetTerrain()->GetHeight(m_owner->GetPositionX(), m_owner->GetPositionY(), m_owner->GetPositionZ(), true, MAX_FALL_DISTANCE);
    if (tz <= INVALID_HEIGHT)
    {
        sLog.outError("MotionMaster::MoveFall: unable retrive a proper height at map %u (x: %f, y: %f, z: %f).",
            m_owner->GetMap()->GetId(), m_owner->GetPositionX(), m_owner->GetPositionX(), m_owner->GetPositionZ());
        return;
    }

    // Abort too if the ground is very near
    if (fabs(m_owner->GetPositionZ() - tz) < 0.1f)
        return;

    Movement::MoveSplineInit init(*m_owner);
    init.MoveTo(m_owner->GetPositionX(),m_owner->GetPositionY(),tz);
    init.SetFall();
    init.Launch();
    Mutate(new EffectMovementGenerator(0), UNIT_ACTION_EFFECT);
}
