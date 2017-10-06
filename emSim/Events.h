/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *
 * This file is part of EMSim
 * <https://bbpcode.epfl.ch/code/#/admin/projects/viz/EMSim>
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

namespace lfp
{
class Events
{
public:
    Events(size_t nEvent, size_t nTimeSteps);

    Events(Events&& other) = default;
    Events& operator=(Events&& other) = default;

    Events(const Events& event) = delete;
    Events& operator=(const Events& event) = delete;

    void addEvent(const glm::vec3& pos, const float radius);
    void addEventPower(const float power);

    const float* getFlatPositions() const;
    const float* getRadii() const;
    const float* getPowers() const;
    size_t getEventsCount() const;
    size_t getTimeStepsCount() const;

private:
    size_t _nEvents = 0u;
    size_t _nTimeSteps = 0u;

    AlignedFloatPtr _flatPositions;
    AlignedFloatPtr _radii;
    AlignedFloatPtr _powers;

    size_t _eventIndex = 0u;
    size_t _powerIndex = 0u;
};
}

#endif // _EMSim_Events_h_
