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

#ifndef _EventsLoader_h_
#define _EventsLoader_h_

#include <emSim/Events.h>

#include <brain/brain.h>
#include <brain/neuron/types.h>
#include <brion/brion.h>

namespace lfp
{
/** Tuple of buffer offset, cell index, section ID, compartment counts */
typedef std::tuple<size_t, uint32_t, uint32_t, uint16_t> MappingElement;
typedef std::vector<MappingElement> FlatInverseMapping;

class EventsLoader
{
public:
    EventsLoader(const std::string& filePath, const std::string& target,
                 const std::string& report, const uint32_t neuronsPerBatch,
                 const glm::vec2& timeRange);

    Events loadNextBatch();

    uint32_t getBatchesCount() const;
    size_t getFramesCount() const;
    glm::vec2 getTimeRange() const;
    float getDt() const;

private:
    Events _createEvents(const FlatInverseMapping& mapping,
                         const brain::neuron::Morphologies& morphologies);
    void _validateTimeRange();
    FlatInverseMapping _computeInverseMapping() const;

    const brion::BlueConfig _bc;
    std::unique_ptr<brion::CompartmentReport> _report;
    std::unique_ptr<brain::Circuit> _circuit;

    uint32_t _numberOfBatches = 0u;
    const uint32_t _neuronsPerBatch = 0u;
    uint32_t _currentBatch = 0u;
    brain::GIDSet _gids;
    uint32_t _numberOfFrames = 0u;
    glm::vec2 _timeRange = glm::vec2(0.0f, 0.0f);
};
}
#endif // _EventsLoader_h_
