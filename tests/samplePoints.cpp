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

#include <cmath>

#include <emSim/Events.h>
#include <emSim/SamplePoints.h>

#define BOOST_TEST_MODULE samplePoints
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(refineSamplePoints)
{
    const size_t nTimeSteps = 91u;
    const size_t nEvents = 2u;
    lfp::Events events(nEvents);

    events.addEvent(glm::vec3(-0.5f, 0.0f, 0.0f), 0.25f);
    events.addEvent(glm::vec3(0.5f, 0.0f, 0.0f), 0.25f);

    std::vector<glm::vec3> positions;
    positions.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    positions.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    positions.push_back(glm::vec3(-0.5f, 0.0f, 0.0f));
    positions.push_back(glm::vec3(-0.6f, 0.0f, 0.0f));

    lfp::SamplePoints samplePoints(nTimeSteps, positions);

    const float power = 1.0f;
    for (uint32_t i = 0; i < nTimeSteps; ++i)
    {
        events.getPowers()[0] = -power * std::sin(M_PI * (float)i / 180.0f);
        events.getPowers()[1] = power * std::sin(M_PI * (float)i / 180.0f);

        samplePoints.computeNextFrame(events);
    }

    BOOST_CHECK_CLOSE(samplePoints.getValues()[2], 0.0f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[7], 0.0f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[202], 0.0f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[8], -14.7492f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[9], -15.1961f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[31], 039.2614f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[271], 303.871f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[270], -303.871f, 0.01);
    BOOST_CHECK_CLOSE(samplePoints.getValues()[136], 170.521f, 0.01);
}
