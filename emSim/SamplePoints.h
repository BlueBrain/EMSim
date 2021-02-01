/* Copyright (c) 2017-2020, EPFL/Blue Brain Project
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *
 * This file is part of EMSim <https://github.com/BlueBrain/EMSim>
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

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <emSim/Events.h>

namespace ems
{
/**
 * This class is responsable for computing the values of sample points.
 * It allocates and stores the sample points for every timesteps.
 * A sample point is represented by a 3d float vector for the position
 * and a float for it's value.
 */
class SamplePoints
{
public:
    /**
     * Allocate all the memory needed to store the sample points at each
     * timestep. Set all data sample points values to 0.0f.
     * @param nTimeSteps the number of time steps
     * @param positions a vector containing all the positions of all sample
     * points
     * @throw std::bad_alloc if memory allocation did not work
     */
    SamplePoints(uint32_t nTimeSteps, const std::vector<glm::vec3>& positions);

    SamplePoints(SamplePoints&& other) = default;
    SamplePoints& operator=(SamplePoints&& other) = default;

    SamplePoints(const SamplePoints& event) = delete;
    SamplePoints& operator=(const SamplePoints& event) = delete;

    /**
     * Compute the values of all sample points for the next frame.
     * @param events the events of a single frame.
     */
    void computeNextFrame(const Events& events);

    /**
     * Write sample points values for all time steps in a file.
     * @param timeRange the time range used to compute sample points
     * @param dt the time dt used for computation
     * @param dataUnit a string describing the data units (ex: "mA")
     * @param outputFile the file name where the results will be written
     * @param blueconfig the name of the blueconfig
     * @param report the name of the report
     * @param target the name of the target
     */
    void writeToFile(const glm::vec2& timeRange, const float dt,
                     const std::string& dataUnit, const std::string& outputFile,
                     const std::string& blueconfig, const std::string& report,
                     const std::string& target);

    /**
     * @return The pointer to the sample points values.
     */
    const float* getValues() const;

private:
    size_t _nSamplePoints = 0u;
    size_t _nTimeSteps = 0u;
    uint32_t _currentFrame = 0u;
    AlignedFloatPtr _flatPositions;
    AlignedFloatPtr _values;
};
}
#endif // _SamplePoints_h_
