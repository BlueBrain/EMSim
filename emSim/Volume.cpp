/* Copyright (c) 2017, EPFL/Blue Brain Pr#oject
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

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <emSim/ComputeVolume.h>
#include <emSim/Volume.h>
#include <emSim/helpers.h>

namespace lfp
{
Volume::Volume(const glm::vec3& voxelSize, const glm::vec3& extent,
               const EventsAABB& circuitAABB)
    : _voxelSize(voxelSize)
    , _volumeSize(glm::uvec3(
          (circuitAABB.max.x - circuitAABB.min.x + extent.x) / voxelSize.x +
              0.5f,
          (circuitAABB.max.y - circuitAABB.min.y + extent.y) / voxelSize.y +
              0.5f,
          (circuitAABB.max.z - circuitAABB.min.z + extent.z) / voxelSize.z +
              0.5f))
    , _data(alignedMalloc<float>(_getVoxelCount()))
    , _origin(glm::vec3(circuitAABB.min.x - extent.x / 2.0f,
                        circuitAABB.min.y - extent.y / 2.0f,
                        circuitAABB.min.z - extent.z / 2.0f))
{
    std::cout << "INFO: Volume size is [" << _volumeSize.x << " "
              << _volumeSize.y << " " << _volumeSize.z << "]" << std::endl;
    std::memset(_data.get(), 0.0f, _getVoxelCount() * sizeof(float));
}

void Volume::compute(const Events& events)
{
    ispc::ComputeVolume_ispc(events.getFlatPositions(), events.getRadii(),
                             events.getPowers(), events.getEventsCount(),
                             _data.get(), _volumeSize.x, _volumeSize.y,
                             _volumeSize.z, _voxelSize.x, _voxelSize.y,
                             _voxelSize.z, _origin.x, _origin.y, _origin.y);
}

void Volume::writeToFile(const float timeStep, const std::string& dataUnit,
                         const std::string& outputFile,
                         const std::string& blueconfig,
                         const std::string& report, const std::string& target)
{
    std::ofstream output;
    output.open(outputFile + "_volume_floats_" +
                    _createTimeStepSuffix(timeStep) + ".raw",
                std::ios::out | std::ios::binary);
    output.write((char*)_data.get(), sizeof(float) * _getVoxelCount());
    output.close();

    std::string voltUnit = dataUnit;
    std::replace(voltUnit.begin(), voltUnit.end(), 'A', 'V');

    std::ofstream info;
    info.open(outputFile + "_volume_info_" + _createTimeStepSuffix(timeStep) + ".txt");
    info << "# File generated by EMSim tool:\n"
         << "# - BlueConfig: " << blueconfig << "\n"
         << "# - Target: " << target << "\n"
         << "# - Report: " << report << "\n"
         << "# - Time step: " << timeStep << "\n"
         << "# - Units: " << voltUnit << "\n"
         << "# - SizeInVoxels: " << _volumeSize.x << " " << _volumeSize.y << " " << _volumeSize.z << "\n"
         << "# - SizeInMicrons: " << _volumeSize.x * _voxelSize.x << " " << _volumeSize.y * _voxelSize.y << " " << _volumeSize.z * _voxelSize.z << "\n"
         << "#" << std::endl;

    std::cout << "INFO: Volume for timestep " << _createTimeStepSuffix(timeStep) << " written to disk." << std::endl;
}

std::string Volume::_createTimeStepSuffix(const float timeStep) const
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(4) << timeStep;
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

const glm::uvec3& Volume::getSize() const
{
    return _volumeSize;
}

const glm::vec3& Volume::getOrigin() const
{
    return _origin;
}

const glm::vec3& Volume::getResolution() const
{
    return _voxelSize;
}

const float* Volume::getData() const
{
    return _data.get();
}

uint64_t Volume::_getVoxelCount() const
{
    return (uint64_t)_volumeSize.x * (uint64_t)_volumeSize.y *
           (uint64_t)_volumeSize.z;
}
}