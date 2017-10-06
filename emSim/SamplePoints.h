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

#ifndef _SamplePoints_h_
#define _SamplePoints_h_

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <emSim/Events.h>

namespace lfp
{
class SamplePoints
{
public:
    SamplePoints(uint32_t nTimeSteps, const std::vector<glm::vec3>& positions);

    SamplePoints(SamplePoints&& other) = default;
    SamplePoints& operator=(SamplePoints&& other) = default;

    SamplePoints(const SamplePoints& event) = delete;
    SamplePoints& operator=(const SamplePoints& event) = delete;

    void refine(const Events& events);
    void writeToFile(const glm::vec2& timeRange, const float dt,
                     const std::string& outputFile,
                     const std::string& blueconfig, const std::string& report,
                     const std::string& target);

    uint32_t _nSamplePoints;
    uint32_t _nTimeSteps;
    AlignedFloatPtr _flatPositions;
    AlignedFloatPtr _values;
    float _dt;
};
}
#endif // _SamplePoints_h_
