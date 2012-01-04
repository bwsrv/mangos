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

#include "Common.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Unit.h"
#include "CreatureAI.h"
#include "Util.h"
#include "WorldPacket.h"
#include "movement/MoveSpline.h"

VehicleInfo::VehicleInfo(VehicleEntry const* entry) :
    m_vehicleEntry(entry)
{
}

VehicleKit::VehicleKit(Unit* base) : m_pBase(base), m_uiNumFreeSeats(0)
{
    for (uint32 i = 0; i < MAX_VEHICLE_SEAT; ++i)
    {
        uint32 seatId = GetBase()->GetVehicleInfo()->GetEntry()->m_seatID[i];

        if (!seatId)
            continue;


        if (VehicleSeatEntry const *seatInfo = sVehicleSeatStore.LookupEntry(seatId))
        {
            m_Seats.insert(std::make_pair(i, VehicleSeat(seatInfo)));

            if (seatInfo->IsUsable())
                ++m_uiNumFreeSeats;
        }
    }

    if (base)
    {
        if (GetBase()->GetVehicleInfo()->GetEntry()->m_flags & VEHICLE_FLAG_NO_STRAFE)
            GetBase()->m_movementInfo.AddMovementFlag2(MOVEFLAG2_NO_STRAFE);

        if (GetBase()->GetVehicleInfo()->GetEntry()->m_flags & VEHICLE_FLAG_NO_JUMPING)
            GetBase()->m_movementInfo.AddMovementFlag2(MOVEFLAG2_NO_JUMPING);

        if (GetBase()->GetVehicleInfo()->GetEntry()->m_flags & VEHICLE_FLAG_FULLSPEEDTURNING)
            GetBase()->m_movementInfo.AddMovementFlag2(MOVEFLAG2_FULLSPEEDTURNING);

        if (GetBase()->GetVehicleInfo()->GetEntry()->m_flags & VEHICLE_FLAG_ALLOW_PITCHING)
            GetBase()->m_movementInfo.AddMovementFlag2(MOVEFLAG2_ALLOW_PITCHING);

        if (GetBase()->GetVehicleInfo()->GetEntry()->m_flags & VEHICLE_FLAG_ALLOW_PITCHING)
            GetBase()->m_movementInfo.AddMovementFlag2(MOVEFLAG2_FULLSPEEDPITCHING);

    }
    SetDestination();
}

VehicleKit::~VehicleKit()
{
}

void VehicleKit::RemoveAllPassengers()
{
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *passenger = itr->second.passenger)
        {
            passenger->ExitVehicle();

            // remove creatures of player mounts
            if (passenger->GetTypeId() == TYPEID_UNIT)
                passenger->AddObjectToRemoveList();
        }
    }
}

bool VehicleKit::HasEmptySeat(int8 seatId) const
{
    if (seatId < 0)
        return (GetNextEmptySeat(0,true) != -1);

    SeatMap::const_iterator seat = m_Seats.find(seatId);
    // need add check on accessories-only seats...

    if (seat == m_Seats.end())
        return false;

    return !seat->second.passenger;
}

Unit *VehicleKit::GetPassenger(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);

    if (seat == m_Seats.end())
        return NULL;

    return seat->second.passenger;
}

int8 VehicleKit::GetNextEmptySeat(int8 seatId, bool next) const
{

    if (m_Seats.empty() || seatId >= MAX_VEHICLE_SEAT)
        return -1;

    if (next)
    {
        for (SeatMap::const_iterator seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
            if ((seatId < 0 || seat->first >= seatId) && !seat->second.passenger && seat->second.seatInfo->IsUsable())
                return seat->first;
    }
    else
    {
        for (SeatMap::const_reverse_iterator seat = m_Seats.rbegin(); seat != m_Seats.rend(); ++seat)
            if ((seatId < 0 || seat->first <= seatId) && !seat->second.passenger && seat->second.seatInfo->IsUsable())
                return seat->first;
    }

    return -1;
}

bool VehicleKit::AddPassenger(Unit *passenger, int8 seatId)
{
    SeatMap::iterator seat;

    if (seatId < 0) // no specific seat requirement
    {
        for (seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
            if (!seat->second.passenger && (seat->second.seatInfo->IsUsable() || (seat->second.seatInfo->m_flags & SEAT_FLAG_UNCONTROLLED)))
                break;

        if (seat == m_Seats.end()) // no available seat
            return false;
    }
    else
    {
        seat = m_Seats.find(seatId);

        if (seat == m_Seats.end())
            return false;

        if (seat->second.passenger)
            return false;
    }

    VehicleSeatEntry const* seatInfo = seat->second.seatInfo;
    seat->second.passenger = passenger;

    if (!(seatInfo->m_flags & SEAT_FLAG_FREE_ACTION))
        passenger->addUnitState(UNIT_STAT_ON_VEHICLE);

    m_pBase->SetPhaseMask(passenger->GetPhaseMask(), true);

    passenger->m_movementInfo.ClearTransportData();
    passenger->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);
    if (GetBase()->m_movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
            passenger->m_movementInfo.SetTransportData(GetBase()->GetObjectGuid(),
            seatInfo->m_attachmentOffsetX + GetBase()->m_movementInfo.GetTransportPos()->x,
            seatInfo->m_attachmentOffsetY + GetBase()->m_movementInfo.GetTransportPos()->y,
            seatInfo->m_attachmentOffsetZ + GetBase()->m_movementInfo.GetTransportPos()->z,
            seatInfo->m_passengerYaw + GetBase()->m_movementInfo.GetTransportPos()->o,
            WorldTimer::getMSTime(), seat->first, seatInfo);

            DEBUG_LOG("VehicleKit::AddPassenger passenger %s transport offset on %s setted to %f %f %f %f (parent - %s)",
            passenger->GetObjectGuid().GetString().c_str(),
            passenger->m_movementInfo.GetTransportGuid().GetString().c_str(),
            passenger->m_movementInfo.GetTransportPos()->x,
            passenger->m_movementInfo.GetTransportPos()->y,
            passenger->m_movementInfo.GetTransportPos()->z,
            passenger->m_movementInfo.GetTransportPos()->o,
            GetBase()->m_movementInfo.GetTransportGuid().GetString().c_str());
    }
    else if (passenger->GetTypeId() == TYPEID_UNIT && b_dstSet)
    {
        passenger->m_movementInfo.SetTransportData(m_pBase->GetObjectGuid(),
        seatInfo->m_attachmentOffsetX + m_dst_x, seatInfo->m_attachmentOffsetY + m_dst_y, seatInfo->m_attachmentOffsetZ + m_dst_z,
        seatInfo->m_passengerYaw + m_dst_o, WorldTimer::getMSTime(), seat->first, seatInfo);
    }
    else
    {
        passenger->m_movementInfo.SetTransportData(m_pBase->GetObjectGuid(),
        seatInfo->m_attachmentOffsetX, seatInfo->m_attachmentOffsetY, seatInfo->m_attachmentOffsetZ,
        seatInfo->m_passengerYaw, WorldTimer::getMSTime(), seat->first, seatInfo);
    }

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)passenger)->GetCamera().SetView(m_pBase);

        WorldPacket data(SMSG_FORCE_MOVE_ROOT, 8+4);
        data << passenger->GetPackGUID();
        data << uint32((passenger->m_movementInfo.GetVehicleSeatFlags() & SEAT_FLAG_CAN_CAST) ? 2 : 0);
        passenger->SendMessageToSet(&data, true);
    }

    switch (m_pBase->GetEntry())
    {
        case 28817:
            passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
    }

    if (seat->second.seatInfo->m_flags & SEAT_FLAG_UNATTACKABLE || seat->second.seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
    {
        switch (m_pBase->GetEntry())
        {
            case 33651:                                     // VX 001
            case 33432:                                     // Leviathan MX
            case 33118:                                     // Ignis (Ulduar)
            case 32934:                                     // Kologarn Right Arm (Ulduar)
            case 30234:                                     // Nexus Lord's Hover Disk (Eye of Eternity, Malygos Encounter)
            case 30248:                                     // Scion's of Eternity Hover Disk (Eye of Eternity, Malygos Encounter)
                break;
            default:
                passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
        }
        passenger->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);
    }

    if (seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
    {
        if (!(m_pBase->GetVehicleInfo()->GetEntry()->m_flags & (VEHICLE_FLAG_ACCESSORY)))
        {
            m_pBase->StopMoving();
            m_pBase->GetMotionMaster()->Clear();
            m_pBase->CombatStop(true);
        }
        m_pBase->DeleteThreatList();
        m_pBase->getHostileRefManager().deleteReferences();
        m_pBase->SetCharmerGuid(passenger->GetObjectGuid());
        m_pBase->addUnitState(UNIT_STAT_CONTROLLED);

        passenger->SetCharm(m_pBase);

        if (m_pBase->HasAuraType(SPELL_AURA_FLY) || m_pBase->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED))
        {
            WorldPacket data;
            data.Initialize(SMSG_MOVE_SET_CAN_FLY, 12);
            data << m_pBase->GetPackGUID();
            data << (uint32)(0);
            m_pBase->SendMessageToSet(&data,false);
        }

        if (passenger->GetTypeId() == TYPEID_PLAYER)
        {
            m_pBase->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

            if (CharmInfo* charmInfo = m_pBase->InitCharmInfo(m_pBase))
            {
                charmInfo->InitVehicleCreateSpells(seat->first);
                charmInfo->SetReactState(REACT_PASSIVE);
            }

            Player* player = (Player*)passenger;
            player->SetMover(m_pBase);
            player->SetClientControl(m_pBase, 1);
            player->VehicleSpellInitialize();
        }

        if(!(((Creature*)m_pBase)->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_KEEP_AI))
            ((Creature*)m_pBase)->AIM_Initialize();

        if (m_pBase->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        {
            WorldPacket data2(SMSG_FORCE_MOVE_ROOT, 8+4);
            data2 << m_pBase->GetPackGUID();
            data2 << (uint32)(2);
            m_pBase->SendMessageToSet(&data2,false);
        }
        else if (passenger->m_movementInfo.GetMovementFlags() & MOVEFLAG_WALK_MODE)
            ((Creature*)m_pBase)->SetWalk(true);
        else
            ((Creature*)m_pBase)->SetWalk(false);

    }

    passenger->SendMonsterMoveTransport(m_pBase, SPLINETYPE_FACINGANGLE, SPLINEFLAG_UNKNOWN5, 0, 0.0f);

    RelocatePassengers(m_pBase->GetPositionX(), m_pBase->GetPositionY(), m_pBase->GetPositionZ()+0.5f, m_pBase->GetOrientation());

    UpdateFreeSeatCount();

    if (m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)m_pBase)->AI())
            ((Creature*)m_pBase)->AI()->PassengerBoarded(passenger, seat->first, true);
    }

    if (seatInfo->m_flagsB & VEHICLE_SEAT_FLAG_B_EJECTABLE_FORCED)
    {
        uint32 delay = seatInfo->m_exitMaxDuration * IN_MILLISECONDS;
        m_pBase->AddEvent(new PassengerEjectEvent(seatId,*m_pBase), delay);
        DEBUG_LOG("Vehicle::AddPassenger eject event for %s added, delay %u",passenger->GetObjectGuid().GetString().c_str(), delay);
    }

    return true;
}

void VehicleKit::RemovePassenger(Unit *passenger, bool dismount)
{
    SeatMap::iterator seat;

    for (seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
        if (seat->second.passenger == passenger)
            break;

    if (seat == m_Seats.end())
        return;

    seat->second.passenger = NULL;
    passenger->clearUnitState(UNIT_STAT_ON_VEHICLE);

    float px, py, pz, po;
    m_pBase->GetClosePoint(px, py, pz, m_pBase->GetObjectBoundingRadius(), 2.0f, M_PI_F);
    po = m_pBase->GetOrientation();

    passenger->m_movementInfo.ClearTransportData();
    passenger->m_movementInfo.RemoveMovementFlag(MOVEFLAG_ONTRANSPORT);

    if (seat->second.seatInfo->m_flags & SEAT_FLAG_UNATTACKABLE || seat->second.seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
    {
        passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    if (seat->second.seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
    {

        passenger->SetCharm(NULL);
        passenger->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);

        m_pBase->SetCharmerGuid(ObjectGuid());
        m_pBase->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        m_pBase->clearUnitState(UNIT_STAT_CONTROLLED);

        if (passenger->GetTypeId() == TYPEID_PLAYER)
        {
            Player* player = (Player*)passenger;
            player->SetMover(NULL);
            player->SetClientControl(m_pBase, 0);
            player->RemovePetActionBar();
        }

        if(!(((Creature*)m_pBase)->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_KEEP_AI))
            ((Creature*)m_pBase)->AIM_Initialize();
    }

    switch (m_pBase->GetEntry())
    {
        case 28817:
            passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
    }

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)passenger)->GetCamera().ResetView();

        WorldPacket data(SMSG_FORCE_MOVE_UNROOT, 8+4);
        data << passenger->GetPackGUID();
        data << uint32(2);
        passenger->SendMessageToSet(&data, true);
    }
    UpdateFreeSeatCount();

    if (m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)m_pBase)->AI())
            ((Creature*)m_pBase)->AI()->PassengerBoarded(passenger, seat->first, false);
    }
    if (dismount)
    {
        Dismount(passenger, seat->second.seatInfo);
        // only for flyable vehicles
        if (m_pBase->m_movementInfo.HasMovementFlag(MOVEFLAG_FLYING))
            m_pBase->CastSpell(passenger, 45472, true);    // Parachute
    }
}

void VehicleKit::Reset()
{
    RemoveAllPassengers();
    InstallAllAccessories(m_pBase->GetEntry());
    UpdateFreeSeatCount();
}

void VehicleKit::InstallAllAccessories(uint32 entry)
{
    VehicleAccessoryList const* mVehicleList = sObjectMgr.GetVehicleAccessoryList(entry);
    if (!mVehicleList)
        return;

    for (VehicleAccessoryList::const_iterator itr = mVehicleList->begin(); itr != mVehicleList->end(); ++itr)
        InstallAccessory(&*itr);
}

void VehicleKit::InstallAccessory(VehicleAccessory const* accessory)
{
    if (Unit *passenger = GetPassenger(accessory->uiSeat))
    {
        // already installed
        if (passenger->GetEntry() == accessory->uiAccessory)
            return;

        passenger->ExitVehicle();
    }

    if (Creature* summoned = m_pBase->SummonCreature(accessory->uiAccessory,
        m_pBase->GetPositionX() + accessory->m_offsetX, m_pBase->GetPositionY() + accessory->m_offsetY, m_pBase->GetPositionZ() + accessory->m_offsetZ, m_pBase->GetOrientation() + accessory->m_offsetX,
        TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
    {
        SetDestination(accessory->m_offsetX,accessory->m_offsetY,accessory->m_offsetZ,accessory->m_offsetO,0.0f,0.0f);
        summoned->SetCreatorGuid(ObjectGuid());
        summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        summoned->EnterVehicle(this, accessory->uiSeat);
        SetDestination();
    }
}

void VehicleKit::UpdateFreeSeatCount()
{
    m_uiNumFreeSeats = 0;

    for (SeatMap::const_iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (!itr->second.passenger && itr->second.seatInfo->IsUsable())
            ++m_uiNumFreeSeats;
    }

    uint32 flag = m_pBase->GetTypeId() == TYPEID_PLAYER ? UNIT_NPC_FLAG_PLAYER_VEHICLE : UNIT_NPC_FLAG_SPELLCLICK;

    if (m_uiNumFreeSeats)
        m_pBase->SetFlag(UNIT_NPC_FLAGS, flag);
    else
        m_pBase->RemoveFlag(UNIT_NPC_FLAGS, flag);
}

void VehicleKit::RelocatePassengers(float x, float y, float z, float ang)
{
    for (SeatMap::const_iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *passenger = itr->second.passenger)
        {
            float px = x + passenger->m_movementInfo.GetTransportPos()->x;
            float py = y + passenger->m_movementInfo.GetTransportPos()->y;
            float pz = z + passenger->m_movementInfo.GetTransportPos()->z;
            float po = ang + passenger->m_movementInfo.GetTransportPos()->o;
            passenger->UpdateAllowedPositionZ(px, py, pz);
            passenger->SetPosition(px, py, pz, po);
        }
    }
}

VehicleSeatEntry const* VehicleKit::GetSeatInfo(Unit* passenger)
{
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *_passenger = itr->second.passenger)
            if (_passenger = passenger)
                return itr->second.seatInfo;
    }
    return NULL;
}

int8 VehicleKit::GetSeatId(Unit* passenger)
{
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *_passenger = itr->second.passenger)
            if (_passenger = passenger)
                return itr->first;
    }
    return -1;
}

void VehicleKit::Dismount(Unit* passenger, VehicleSeatEntry const* seatInfo)
{
    if (!passenger)
        return;

    float ox, oy, oz, oo;
    m_pBase->GetPosition(ox, oy, oz);
    oo = m_pBase->GetOrientation();
    ox += seatInfo->m_attachmentOffsetX;
    oy += seatInfo->m_attachmentOffsetY;
    oz += seatInfo->m_attachmentOffsetX;
    oo += seatInfo->m_passengerYaw;

    passenger->SetPosition(ox, oy, oz + 0.5f, oo);

    Unit* base = m_pBase->GetVehicle() ? m_pBase->GetVehicle()->GetBase() : m_pBase;

    if (b_dstSet)
    {
        // parabolic traectory (catapults)
        float speed = ((m_dst_speed > 0.0f) ? m_dst_speed : (seatInfo ? seatInfo->m_exitSpeed : 28.0f));
        float verticalSpeed = speed * sin(m_dst_elevation);
        float horisontalSpeed = speed * cos(m_dst_elevation);
        float moveTimeHalf =  verticalSpeed / ((seatInfo && seatInfo->m_exitGravity > 0.0f) ? seatInfo->m_exitGravity : Movement::gravity);
        float max_height = - Movement::computeFallElevation(moveTimeHalf,false,-verticalSpeed);
        passenger->UpdateAllowedPositionZ(m_dst_x, m_dst_y, m_dst_z);
        passenger->MonsterMoveJump(m_dst_x, m_dst_y, m_dst_z,passenger->GetOrientation(), horisontalSpeed, max_height, false);

    }
    else if (seatInfo)
    {
        // half-parabolic traectory (unmount)

        float horisontalSpeed = seatInfo->m_exitSpeed;

        base->GetClosePoint(m_dst_x, m_dst_y, m_dst_z, base->GetObjectBoundingRadius(), frand(2.0f, 3.0f), frand(M_PI_F/2.0f,3.0f*M_PI_F/2.0f));
        passenger->UpdateAllowedPositionZ(m_dst_x, m_dst_y, m_dst_z);
        passenger->MonsterMoveJump(m_dst_x, m_dst_y, m_dst_z + 0.1f, passenger->GetOrientation(), horisontalSpeed, 0.0f, false);
    }
    else
    {
        // jump from vehicle without seatInfo (? error case)
        base->GetClosePoint(m_dst_x, m_dst_y, m_dst_z, base->GetObjectBoundingRadius(), 2.0f, M_PI_F);
        passenger->UpdateAllowedPositionZ(m_dst_x, m_dst_y, m_dst_z);
        passenger->MonsterMoveWithSpeed(m_dst_x, m_dst_y, m_dst_z + 0.5f, 28);
    }

    SetDestination();
}

void VehicleKit::SetDestination(float x, float y, float z, float o, float speed, float elevation)
{
    m_dst_x = x;
    m_dst_y = y;
    m_dst_z  = z;
    m_dst_o  = o;
    m_dst_speed  = speed;
    m_dst_elevation  = elevation;

    if (fabs(m_dst_x) > 0.001 ||
        fabs(m_dst_y) > 0.001 ||
        fabs(m_dst_z) > 0.001 ||
        fabs(m_dst_o) > 0.001 ||
        fabs(m_dst_speed) > 0.001 ||
        fabs(m_dst_elevation) > 0.001)
        b_dstSet = true;
};

bool PassengerEjectEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (!m_vehicle.GetVehicleInfo())
        return true;

    VehicleKit* pVehicle = m_vehicle.GetVehicleKit();

    if (!pVehicle)
        return true;

    Unit* passenger = pVehicle->GetPassenger(m_seatId);

    if (passenger && passenger->IsInWorld() && passenger->hasUnitState(UNIT_STAT_ON_VEHICLE))
    {
        uint32 controlSpell = 0;
        Unit::AuraList const& controlAuras = m_vehicle.GetAurasByType(SPELL_AURA_CONTROL_VEHICLE);
        for(Unit::AuraList::const_iterator i = controlAuras.begin(); i != controlAuras.end(); ++i)
        {
            if ((*i)->GetCasterGuid() == passenger->GetObjectGuid())
            {
                controlSpell = (*i)->GetId();
                break;
            }
        }

        if (controlSpell)
        {
            m_vehicle.RemoveAurasByCasterSpell(controlSpell, passenger->GetObjectGuid());
        }
        else
            passenger->ExitVehicle();
    }
    return true;
}
