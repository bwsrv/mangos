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
#ifndef CREATUREAIIMPL_H
#define CREATUREAIIMPL_H

#include "CreatureAI.h"

template<class REAL_AI>
inline CreatureAI*
CreatureAIFactory<REAL_AI>::Create(void *data) const
{
    Creature* creature = reinterpret_cast<Creature *>(data);
    return (new REAL_AI(creature));
}

class EventMap : private std::map<uint32, uint32>
{
    public:
        EventMap() : _time(0), _phase(0) {}

        // Returns current timer value, does not represent real dates/times
        uint32 GetTimer() const { return _time; }

        // Removes all events and clears phase
        void Reset() { clear(); _time = 0; _phase = 0; }

        void Update(uint32 time) { _time += time; }

        uint32 GetPhaseMask() const { return (_phase >> 24) & 0xFF; }

        // Sets event phase, must be in range 1 - 8
        void SetPhase(uint32 phase)
        {
            if (phase && phase < 9)
                _phase = (1 << (phase + 24));
        }

        // Creates new event entry in map with given id, time, group if given (1 - 8) and phase if given (1 - 8)
        // 0 for group/phase means it belongs to no group or runs in all phases
        void ScheduleEvent(uint32 eventId, uint32 time, uint32 groupId = 0, uint32 phase = 0)
        {
            time += _time;
            if (groupId && groupId < 9)
                eventId |= (1 << (groupId + 16));
            if (phase && phase < 9)
                eventId |= (1 << (phase + 24));
            const_iterator itr = find(time);
            while (itr != end())
            {
                ++time;
                itr = find(time);
            }

            insert(std::make_pair(time, eventId));
        }

        // Removes event with specified id and creates new entry for it
        void RescheduleEvent(uint32 eventId, uint32 time, uint32 groupId = 0, uint32 phase = 0)
        {
            CancelEvent(eventId);
            ScheduleEvent(eventId, time, groupId, phase);
        }

        // Reschedules closest event
        void RepeatEvent(uint32 time)
        {
            if (empty())
                return;

            uint32 eventId = begin()->second;
            erase(begin());
            time += _time;
            const_iterator itr = find(time);
            while (itr != end())
            {
                ++time;
                itr = find(time);
            }

            insert(std::make_pair(time, eventId));
        }

        // Removes first event
        void PopEvent()
        {
            erase(begin());
        }

        // Gets next event id to execute and removes it from map
        uint32 ExecuteEvent()
        {
            while (!empty())
            {
                if (begin()->first > _time)
                    return 0;
                else if (_phase && (begin()->second & 0xFF000000) && !(begin()->second & _phase))
                    erase(begin());
                else
                {
                    uint32 eventId = (begin()->second & 0x0000FFFF);
                    erase(begin());
                    return eventId;
                }
            }
            return 0;
        }

        // Gets next event id to execute
        uint32 GetEvent()
        {
            while (!empty())
            {
                if (begin()->first > _time)
                    return 0;
                else if (_phase && (begin()->second & 0xFF000000) && !(begin()->second & _phase))
                    erase(begin());
                else
                    return (begin()->second & 0x0000FFFF);
            }

            return 0;
        }

        // Delay all events
        void DelayEvents(uint32 delay)
        {
            if (delay < _time)
                _time -= delay;
            else
                _time = 0;
        }

        // Delay all events having the specified Group
        void DelayEvents(uint32 delay, uint32 groupId)
        {
            uint32 nextTime = _time + delay;
            uint32 groupMask = (1 << (groupId + 16));
            for (iterator itr = begin(); itr != end() && itr->first < nextTime;)
            {
                if (itr->second & groupMask)
                {
                    ScheduleEvent(itr->second, itr->first - _time + delay);
                    erase(itr);
                    itr = begin();
                }
                else
                    ++itr;
            }
        }

        // Cancel events with specified id
        void CancelEvent(uint32 eventId)
        {
            for (iterator itr = begin(); itr != end();)
            {
                if (eventId == (itr->second & 0x0000FFFF))
                {
                    erase(itr);
                    itr = begin();
                }
                else
                    ++itr;
            }
        }

        // Cancel events belonging to specified group
        void CancelEventGroup(uint32 groupId)
        {
            uint32 groupMask = (1 << (groupId + 16));

            for (iterator itr = begin(); itr != end();)
            {
                if (itr->second & groupMask)
                {
                    erase(itr);
                    itr = begin();
                }
                else
                    ++itr;
            }
        }

        // Returns time of next event to execute
        // To get how much time remains substract _time
        uint32 GetNextEventTime(uint32 eventId) const
        {
            for (const_iterator itr = begin(); itr != end(); ++itr)
                if (eventId == (itr->second & 0x0000FFFF))
                    return itr->first;

            return 0;
        }

    private:
        uint32 _time;
        uint32 _phase;
};

#endif
