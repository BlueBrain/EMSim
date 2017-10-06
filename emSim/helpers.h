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

#ifndef _EMSim_helpers_h_
#define _EMSim_helpers_h_

#include <memory>

#include <glm/glm.hpp>

namespace lfp
{
const uint32_t alignment = 64u;

template <typename T>
struct AlignedMemoryDeleter
{
    void operator()(T* mem) { _mm_free(mem); }
};

using AlignedFloatPtr = std::unique_ptr<float[], AlignedMemoryDeleter<float>>;

template <typename T>
T* alignedMalloc(uint32_t numberOfElements)
{
    return (T*)_mm_malloc(numberOfElements * sizeof(T), alignment);
}
}

#endif // _EMSim_helpers_h_
