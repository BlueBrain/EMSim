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

#ifndef _Volume_h_
#define _Volume_h_

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <emSim/Events.h>
#include <emSim/helpers.h>

namespace ems
{
/**
 * This class is responsable for computing a 3d volume.
 * It allocates and stores the volume for a single timestep.
 */
class Volume
{
public:
    /**
     * Allocate all the memory needed to store the volume for a single
     * timestep. All voxels values are set to 0.0f.
     * @param voxelSize a 3d vector describing the (anisotropic) size of voxel
     * circuit units
     * @param extent a 3d vector containing the extent of the volume. The extent
     * will increase the size of the volume to provide more space arround the
     * events.
     * @param circuitAABB the bounding box of the events
     * @throw std::bad_alloc if memory allocation did not work
     */
    Volume(const glm::vec3& voxelSize, const glm::vec3& extent,
           const EventsAABB& circuitAABB);

    Volume(Volume&& other) = default;
    Volume& operator=(Volume&& other) = default;

    Volume(const Volume& event) = delete;
    Volume& operator=(const Volume& event) = delete;

    void clear(const float value = 0.0f);

    /**
     * Write sample points values for all time steps in a file.
     * @param time the current time
     * @param the timeStep used for the computation
     * @param dataUnit a string describing the data units (ex: "mA")
     * @param outputFile the file name where the volume will be written
     * @param blueconfig the name of the blueconfig
     * @param report the name of the report
     * @param target the name of the target
     */
    void writeToFile(const float time, const float timeStep, 
                     const std::string& dataUnit, const std::string& outputFile, 
                     const std::string& blueconfig, const std::string& report, 
                     const std::string& target);

   /**
     * Write sample points values for all time steps in a file.
     * @param time the current time
     * @param dataUnit a string describing the data units (ex: "mA")
     * @param outputFile the file name where the volume will be written
     */
    void writeToFileMhd(const float time, const std::string& dataUnit, 
                     const std::string& outputFile);

    /**
     * @return 3d vector containing the size of the volume in voxels.
     */
    const glm::uvec3& getSize() const;

    /**
     * @return 3d vector containing the origin of the volume in circuit units
     */
    const glm::vec3& getOrigin() const;

    /**
     * @return 3d vector containing the (anisotropic) size of a voxel in micrometers.
     */
    const glm::vec3& getVoxelSize() const;

    /**
     * @return The pointer to the voxels values.
     */
    float* getData();

    /**
     * @return The const pointer to the voxels values.
     */
    const float* getData() const;

private:
    uint64_t _getVoxelCount() const;

    glm::vec3 _voxelSize;
    glm::uvec3 _volumeSize;
    AlignedFloatPtr _data;
    glm::vec3 _origin;
};
}
#endif // _Volume_h_
