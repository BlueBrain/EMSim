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

#ifndef _EMSim_helpers_h_
#define _EMSim_helpers_h_

#include <iomanip>
#include <memory>
#include <sstream>

#include <glm/glm.hpp>

namespace ems
{
const uint32_t alignment = 32u;

template <typename T>
struct AlignedMemoryDeleter
{
#ifdef USE_ALIGNED_MALLOC
    void operator()(T* mem) { _mm_free(mem); }
#else
    void operator()(T* mem) { free(mem); }
#endif
};

using AlignedFloatPtr = std::unique_ptr<float[], AlignedMemoryDeleter<float>>;

template <typename T>
T* alignedMalloc(size_t numberOfElements)
{
#ifdef USE_ALIGNED_MALLOC
    T* ptr = (T*)_mm_malloc(numberOfElements * sizeof(T), alignment);
#else
    T* ptr = (T*)malloc(numberOfElements * sizeof(T));
#endif
    if (ptr == 0)
        throw(std::bad_alloc( ));

    return ptr;
}

inline std::string createTimeStepSuffix(const float time)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << time;
    std::string timeStepRounded = stream.str();

    for (uint32_t i = timeStepRounded.length() - 1; i > 0; --i)
    {
        if ((timeStepRounded[i] == '0') && (timeStepRounded[i-1] != '.'))
            timeStepRounded.erase(i, 1);
        else
            break;
    }
    return timeStepRounded;
}

struct EventsAABB
{
    void add(const glm::vec3& pos, const float radius)
    {
        if (pos.x - radius < min.x)
            min.x = pos.x - radius;
        if (pos.y - radius < min.y)
            min.y = pos.y - radius;
        if (pos.z - radius < min.z)
            min.z = pos.z - radius;

        if (pos.x + radius > max.x)
            max.x = pos.x + radius;
        if (pos.y + radius > max.y)
            max.y = pos.y + radius;
        if (pos.z + radius > max.z)
            max.z = pos.z + radius;
    }

    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max(),
                              -std::numeric_limits<float>::max(),
                              -std::numeric_limits<float>::max());
};

}

#endif // _EMSim_helpers_h_
