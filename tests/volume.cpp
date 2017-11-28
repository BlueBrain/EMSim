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
#include <emSim/Volume.h>

#define BOOST_TEST_MODULE volume
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(computeVolume)
{
    lfp::EventsAABB aabb;
    aabb.add(glm::vec3(-1000.0f, -1500.0f, -1000.0f), 10.0f);
    aabb.add(glm::vec3(1000.0f, 1500.0f, 1000.0f), 10.0f);
    glm::vec3 extent( 256.0f, 128.0f, 256.0f );

    glm::vec3 resolution( 17.78125f, 24.59375f, 17.78125f );

    lfp::Volume volume( resolution, extent, aabb);

    BOOST_CHECK_EQUAL(volume.getSize().x, 128u);
    BOOST_CHECK_EQUAL(volume.getSize().y, 128u);
    BOOST_CHECK_EQUAL(volume.getSize().z, 128u);
    BOOST_CHECK_CLOSE(volume.getOrigin().x, -1138.0f, 0.01);
    BOOST_CHECK_CLOSE(volume.getOrigin().y, -1574.0f, 0.01);
    BOOST_CHECK_CLOSE(volume.getOrigin().z, -1138.0f, 0.01);
    BOOST_CHECK_CLOSE(volume.getResolution().x, 17.78125f, 0.01);
    BOOST_CHECK_CLOSE(volume.getResolution().y, 24.59375f, 0.01);
    BOOST_CHECK_CLOSE(volume.getResolution().z, 17.78125f, 0.01);

    lfp::Events events(2u);
    events.addEvent(glm::vec3(-500.0f, 0.0f, 0.0f), 5.0f);
    events.addEvent(glm::vec3(500.0f, 0.0f, 0.0f), 5.0f);
    events.getPowers()[0] = 10.0f;
    events.getPowers()[1] = 10.0f;

    volume.compute( events );

    BOOST_CHECK_CLOSE(volume.getData()[100], 0.002389415f, 0.01);
    BOOST_CHECK_CLOSE(volume.getData()[15000], 0.002556937, 0.01);
    BOOST_CHECK_CLOSE(volume.getData()[240000], 0.003130926, 0.01);
    BOOST_CHECK_CLOSE(volume.getData()[1000000], 0.00344999, 0.01);
}
