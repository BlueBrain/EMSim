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

#include <cstring>

#include <emSim/Events.h>
#include <emSim/helpers.h>

namespace ems
{
Events::Events(const size_t nEvents)
    : _nEvents(nEvents)
    , _flatPositions(alignedMalloc<float>(_nEvents * 3u))
    , _radii(alignedMalloc<float>(_nEvents))
    , _powers(alignedMalloc<float>(_nEvents))
{
    std::memset(_flatPositions.get(), 0.0f, _nEvents * 3u * sizeof(float));
    std::memset(_radii.get(), 0.0f, _nEvents * sizeof(float));
    std::memset(_powers.get(), 0.0f, _nEvents * sizeof(float));
}

void Events::addEvent(const glm::vec3& pos, const float radius)
{
    if (_eventIndex >= _nEvents)
        throw(std::runtime_error(
            "error: Cannot add event. Maximum number of events reached."));

    _flatPositions[_eventIndex * 3] = pos.x;
    _flatPositions[_eventIndex * 3 + 1] = pos.y;
    _flatPositions[_eventIndex * 3 + 2] = pos.z;
    _radii[_eventIndex] = radius;
    ++_eventIndex;
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

float* Events::getPowers()
{
    return _powers.get();
}

size_t Events::getEventsCount() const
{
    return _nEvents;
}
}
