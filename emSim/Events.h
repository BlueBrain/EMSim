/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *
 * This file is part of EMSim
 * <https://bbpcode.epfl.ch/browse/code/viz/EMSim/>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _EMSim_Events_h_
#define _EMSim_Events_h_

#include <cstddef>
#include <cstdint>

#include <glm/glm.hpp>

#include <emSim/helpers.h>

namespace ems
{
/**
 * This class store the events' geometric data (position and radius) and power
 * values for one time step. The geometric data consist of an event's 3d
 * position and radius and is constant for all time steps. The power values
 * changes every time steps and need to be reloaded.
 */
class Events
{
public:
    /**
     * Allocate all the memory needed to store the data. Set all data values
     * to 0.0f.
     * @param nEvent The number of events
     * @throw std::bad_alloc if memory allocation did not work.
     */
    Events(size_t nEvent);

    Events(Events&& other) = default;
    Events& operator=(Events&& other) = default;

    Events(const Events& event) = delete;
    Events& operator=(const Events& event) = delete;

    /**
     * Add events' geometric data. This function does not allocate any
     * memory. The memory is already allocated in the constructor.
     * @param pos The event's position
     * @param radius The event's radius
     * @throw std::runtime_error if the allocated space is exceeded
     */
    void addEvent(const glm::vec3& pos, const float radius);

    /**
     * Return a const pointer to the events' positions. The positions are stored
     * in x,y,z order.
     * @return The pointer to the events positions.
     */
    const float* getFlatPositions() const;

    /**
     * @return The const pointer to the events' radii.
     */
    const float* getRadii() const;

    /**
     * @return The const pointer to the events' powers.
     */
    const float* getPowers() const;

    /**
     * @return The pointer to the events' powers.
     */
    float* getPowers();

    /**
     * @return the number of stored events.
     */
    size_t getEventsCount() const;

private:
    size_t _nEvents = 0u;

    AlignedFloatPtr _flatPositions;
    AlignedFloatPtr _radii;
    AlignedFloatPtr _powers;

    size_t _eventIndex = 0u;
};
}

#endif // _EMSim_Events_h_
