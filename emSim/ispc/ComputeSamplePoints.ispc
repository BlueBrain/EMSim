/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *
 * This file is part of LFPNator <https://github.com/???????/??????>
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
    const uniform float eventPowers[], const uniform unsigned int32 nTimeSteps,
    const uniform unsigned int32 nEvents, const uniform float spFlatPositions[],
    uniform float spValues[], const uniform unsigned int32 nSamplePoints,
    const uniform unsigned int32 nIterations)
{
    for (unsigned int32 i = 0; i < nSamplePoints; ++i)
        for (unsigned int32 j = 0; j < nEvents; ++j)
        {
            for (unsigned int32 k = 0; k < nIterations; ++k)
            {
                const unsigned int32 eventIndex = j * 3;
                const unsigned int32 spIndex = i * 3;
                const unsigned int32 timeIndex = taskIndex * nIterations + k;

                const float deltaX =
                    spFlatPositions[spIndex] - eventFlatPos[eventIndex];
                const float deltaY =
                    spFlatPositions[spIndex + 1] - eventFlatPos[eventIndex + 1];
                const float deltaZ =
                    spFlatPositions[spIndex + 2] - eventFlatPos[eventIndex + 2];

                const float squaredDist =
                    deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

                const float eventRadius = eventRadii[j];
                float distInv = 0;
                if (squaredDist > eventRadius * eventRadius)
                    distInv = rsqrt(squaredDist);
                else
                    distInv = rcp(eventRadius);

                if (timeIndex < nTimeSteps)
                    spValues[timeIndex * nSamplePoints + i] +=
                        Ec * eventPowers[timeIndex * nEvents + j] * distInv;
            }
        }
}

export void ComputeSamplePoints_ispc(
    const uniform float eventFlatPos[], const uniform float eventRadii[],
    const uniform float eventPowers[], const uniform unsigned int32 nTimeSteps,
    const uniform unsigned int32 nEvents, const uniform float spFlatPositions[],
    uniform float spValues[], const uniform unsigned int32 nSamplePoints)
{
    const uniform unsigned int32 nThreads = num_cores() * THREAD_MULTIPLIER;

    const uniform unsigned int32 nIterations = nTimeSteps / nThreads + 1;

    launch[nThreads] computeValues(eventFlatPos, eventRadii, eventPowers,
                                   nTimeSteps, nEvents, spFlatPositions,
                                   spValues, nSamplePoints, nIterations);
}
