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

#include <emSim/Events.h>
#include <emSim/helpers.h>

namespace lfp
{
Events::Events(const size_t nEvents, const size_t nTimeSteps)
    : _nEvents(nEvents)
    , _nTimeSteps(nTimeSteps)
    , _flatPositions(alignedMalloc<float>(_nEvents * 3))
    , _radii(alignedMalloc<float>(_nEvents))
    , _powers(alignedMalloc<float>(_nTimeSteps * _nEvents))
{
}

void Events::addEvent(const glm::vec3& pos, const float radius)
{
    _flatPositions[_eventIndex * 3] = pos.x;
    _flatPositions[_eventIndex * 3 + 1] = pos.y;
    _flatPositions[_eventIndex * 3 + 2] = pos.z;
    _radii[_eventIndex] = radius;
    ++_eventIndex;
}

void Events::addEventPower(const float power)
{
    _powers[_powerIndex] = power;
    ++_powerIndex;
}

const float* Events::getFlatPositions() const
{
    return _flatPositions.get();
}

const float* Events::getRadii() const
{
    return _radii.get();
}

const float* Events::getPowers() const
{
    return _powers.get();
}

size_t Events::getEventsCount() const
{
    return _nEvents;
}

size_t Events::getTimeStepsCount() const
{
    return _nTimeSteps;
}
}
