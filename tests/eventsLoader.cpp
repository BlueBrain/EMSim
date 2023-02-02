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

#include <emSim/Events.h>
#include <emSim/EventsLoader.h>

#define BOOST_TEST_MODULE eventsLoader
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(compareEventsLoader)
{
    const std::string blueconfig = "./data/BlueConfig";
    const std::string target = "slice";
    const glm::vec2 timeRange = glm::vec2(0.0f, 1.0f);
    const float fraction = 1.0;

    const std::string reportSONATA = "AllCompartments_SONATA";
    ems::EventsLoader eventLoaderSONATA(blueconfig, target, reportSONATA, timeRange, fraction);

    const std::string reportBin = "AllCompartments_bin";
    ems::EventsLoader eventLoaderBin(blueconfig, target, reportBin, timeRange, fraction);

    BOOST_CHECK_EQUAL(eventLoaderSONATA.getFramesCount(), eventLoaderBin.getFramesCount());

    // Skip the first timestep
    eventLoaderSONATA.loadNextFrame();
    eventLoaderBin.loadNextFrame();

    const ems::Events& eventsSONATA = eventLoaderSONATA.loadNextFrame();
    const ems::Events& eventsBin = eventLoaderBin.loadNextFrame();

    size_t nEventsSONATA = eventsSONATA.getEventsCount();
    size_t nEventsBin = eventsBin.getEventsCount();
    BOOST_CHECK_EQUAL(nEventsSONATA, 13581);

    BOOST_CHECK_CLOSE(eventsSONATA.getFlatPositions()[100], 1696.66992, 0.01);
    BOOST_CHECK_CLOSE(eventsSONATA.getRadii()[2000], 3.83608055, 0.01);
    BOOST_CHECK_CLOSE(eventsSONATA.getPowers()[11000], 0.000204455617, 0.01);
    BOOST_CHECK_CLOSE(eventsBin.getFlatPositions()[25000], 1870.90991, 0.01);
    BOOST_CHECK_CLOSE(eventsBin.getRadii()[700], 1.37590933, 0.01);
    BOOST_CHECK_CLOSE(eventsBin.getPowers()[42], -0.000897545018, 0.01);

    BOOST_CHECK_EQUAL_COLLECTIONS(eventsSONATA.getFlatPositions(), eventsSONATA.getFlatPositions() + 3 * nEventsSONATA,
                                  eventsBin.getFlatPositions(), eventsBin.getFlatPositions() + 3 * nEventsBin);
    BOOST_CHECK_EQUAL_COLLECTIONS(eventsSONATA.getRadii(), eventsSONATA.getRadii() + nEventsSONATA,
                                  eventsBin.getRadii(), eventsBin.getRadii() + nEventsBin);
    BOOST_CHECK_EQUAL_COLLECTIONS(eventsSONATA.getPowers(), eventsSONATA.getPowers() + nEventsSONATA,
                                  eventsBin.getPowers(), eventsBin.getPowers() + nEventsBin);
}
