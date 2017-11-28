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

// Ec =  1 / (4 * PI * conductivity),
// with conductivity = 1 / 3.54 (siemens per meter)
#define Ec 0.281704249f
#define THREAD_MULTIPLIER 4

task void computeValues(
    const uniform float eventFlatPos[], const uniform float eventRadii[],
    const uniform float eventPowers[], const uniform unsigned int32 nEvents,
    uniform float volumeData[], const uniform unsigned int32 sizeX,
    const uniform unsigned int32 sizeY, const uniform unsigned int32 sizeZ,
    const uniform float resX, const uniform float resY,
    const uniform float resZ, const uniform float originX,
    const uniform float originY, const uniform float originZ,
    const uniform unsigned int32 zSliceSize)
{
    const unsigned int32 startZ = taskIndex * zSliceSize;
    const unsigned int32 endZ = startZ + zSliceSize;

    for (unsigned int32 z = startZ; z < endZ; ++z)
    {
        if (z >= sizeZ)
            continue;

        for (unsigned int32 y = 0; y < sizeY; ++y)
        {
            for (unsigned int32 x = 0; x < sizeX; ++x)
            {
                const float voxelPosX = originX + x * resX;
                const float voxelPosY = originY + y * resY;
                const float voxelPosZ = originZ + z * resZ;

                float voxelValue = 0.0f;
                for (unsigned int32 i = 0; i < nEvents; ++i)
                {
                    const unsigned int32 eventIndex = i * 3;

                    const float deltaX = voxelPosX - eventFlatPos[eventIndex];
                    const float deltaY =
                        voxelPosY - eventFlatPos[eventIndex + 1];
                    const float deltaZ =
                        voxelPosZ - eventFlatPos[eventIndex + 2];

                    const float squaredDist =
                        deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

                    const float eventRadius = eventRadii[i];
                    float distInv = 0;
                    if (squaredDist > eventRadius * eventRadius)
                        distInv = rsqrt(squaredDist);
                    else
                        distInv = rcp(eventRadius);

                    voxelValue += Ec * eventPowers[i] * distInv;
                }
                const unsigned int64 index =
                    (unsigned int64)z * (unsigned int64)sizeX *
                        (unsigned int64)sizeY +
                    (unsigned int64)y * (unsigned int64)sizeX +
                    (unsigned int64)x;
                volumeData[index] = voxelValue;
            }
        }
    }
}

export void ComputeVolume_ispc(
    const uniform float eventFlatPos[], const uniform float eventRadii[],
    const uniform float eventPowers[], const uniform unsigned int32 nEvents,
    uniform float volumeData[], const uniform unsigned int32 sizeX,
    const uniform unsigned int32 sizeY, const uniform unsigned int32 sizeZ,
    const uniform float resX, const uniform float resY,
    const uniform float resZ, const uniform float originX,
    const uniform float originY, const uniform float originZ)
{
    const uniform unsigned int32 nThreads = num_cores() * THREAD_MULTIPLIER;

    // Integer ceil. Works if sizeZ != 0
    const uniform unsigned int32 zSliceSize = (sizeZ - 1) / nThreads + 1;

    launch[nThreads] computeValues(eventFlatPos, eventRadii, eventPowers,
                                   nEvents, volumeData, sizeX, sizeY, sizeZ,
                                   resX, resY, resZ, originX, originY, originZ,
                                   zSliceSize);
}
