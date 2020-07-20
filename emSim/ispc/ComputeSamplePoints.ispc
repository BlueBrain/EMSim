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

// Ec =  1 / (4 * PI * conductivity),
// with conductivity = 1 / 1000000 * 3.54 (siemens per micrometer)
#define Ec 281704.249f
#define THREAD_MULTIPLIER 4

task void computeValues(const uniform float eventFlatPos[],
                        const uniform float eventRadii[],
                        const uniform float eventPowers[],
                        const uniform unsigned int32 nEvents,
                        const uniform unsigned int32 currentFrame,
                        const uniform float spFlatPositions[],
                        uniform float spValues[],
                        const uniform unsigned int32 nSamplePoints,
                        const uniform unsigned int32 nSamplePointsPerThread)
{
    for (unsigned int32 i = 0; i < nSamplePointsPerThread; ++i)
    {
        const unsigned int32 currentSamplePoint =
            taskIndex * nSamplePointsPerThread + i;
        if (currentSamplePoint >= nSamplePoints)
            continue;

        const unsigned int64 spIndex = currentSamplePoint * 3;
        float accum = 0;
        for (unsigned int32 j = 0; j < nEvents; ++j)
        {
            const unsigned int64 eventIndex = j * 3;
            const float deltaX =
                spFlatPositions[spIndex] - eventFlatPos[eventIndex];
            const float deltaY =
                spFlatPositions[spIndex + 1] - eventFlatPos[eventIndex + 1];
            const float deltaZ =
                spFlatPositions[spIndex + 2] - eventFlatPos[eventIndex + 2];

            const float squaredDist =
                deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

            const float eventRadius = eventRadii[j];
            const float distInv = squaredDist > eventRadius * eventRadius
                                      ? rsqrt(squaredDist)
                                      : rcp(eventRadius);
            accum += eventPowers[j] * distInv;
        }
        spValues[currentFrame * nSamplePoints + currentSamplePoint] = Ec * accum;
    }
}

export void ComputeSamplePoints_ispc(const uniform float eventFlatPos[],
                                     const uniform float eventRadii[],
                                     const uniform float eventPowers[],
                                     const uniform unsigned int32 nEvents,
                                     const uniform unsigned int32 currentFrame,
                                     const uniform float spFlatPositions[],
                                     uniform float spValues[],
                                     const uniform unsigned int32 nSamplePoints)
{
    uniform unsigned int32 nThreads = num_cores() * THREAD_MULTIPLIER;
    if (nSamplePoints < 2 * nThreads)
        nThreads = nSamplePoints;

    const uniform unsigned int32 nSamplePointsPerThread =
        (nSamplePoints - 1) / nThreads + 1;

    launch[nThreads] computeValues(eventFlatPos, eventRadii, eventPowers,
                                   nEvents, currentFrame, spFlatPositions,
                                   spValues, nSamplePoints,
                                   nSamplePointsPerThread);
}
