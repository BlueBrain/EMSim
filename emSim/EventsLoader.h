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

#ifndef _EventsLoader_h_
#define _EventsLoader_h_

#include <emSim/Events.h>

#include <brain/brain.h>
#include <brain/neuron/types.h>
#include <brion/brion.h>

namespace ems
{
/** Tuple of buffer offset, cell index, section ID, compartment counts */
typedef std::tuple<size_t, uint32_t, uint32_t, uint16_t> MappingElement;
typedef std::vector<MappingElement> FlatInverseMapping;

/**
 * This class is responsible for events loading. The event's geometric
 * data is loaded once. The event's powers need to be reloaded for each new
 * time step.
 */
class EventsLoader
{
public:
    /**
     * Reads all needed files and loads the events' geometric data.
     *
     * @param filePath the path to the BlueConfig file
     * @param target the circuit target to be loaded
     * @param report the circuit report to be loaded
     * @param timeRange a 2d vector with the start and end times to be loaded
     * @param fraction Specify a percentage of gids to be loaded
     */
    EventsLoader(const std::string& filePath, const std::string& target,
                 const std::string& report, const glm::vec2& timeRange,
                 const float fraction);

    /**
     * Update the events power values for the next frame.
     * @return the events with updated power values
     */
    const Events& loadNextFrame();

    /**
     * Return the currently loaded events without updating anything.
     * @return the events currently loaded.
     */
    const Events& getLoadedFrame() const;

    /**
     * @return the axis aligned bounding box of the circuit in um.
     */
    const EventsAABB& getCircuitAABB() const;

    /**
     * @return the number of frames/timesteps.
     */
    size_t getFramesCount() const;

    /**
     * @return a 2d vector with start and end times.
     */
    glm::vec2 getTimeRange() const;

    /**
     * @return the dt used for time steps.
     */
    float getDt() const;

    /**
     * @return a string containing the data unit. ex: "mA".
     */
    const std::string& getDataUnit() const;

private:
    void _loadStaticEventGeometry();
    void _computeStaticEventGeometry(const FlatInverseMapping& mapping,
                                     const brain::neuron::Morphologies& morphologies);
    void _validateTimeRange();
    void _validateCurrentReport(const brain::GIDSet& gidSet) const;
    FlatInverseMapping _computeInverseMapping() const;

    const brion::BlueConfig _bc;
    std::unique_ptr<brion::CompartmentReport> _report;
    std::unique_ptr<brain::Circuit> _circuit;

    brain::GIDSet _gids;
    uint32_t _numberOfFrames = 0u;
    glm::vec2 _timeRange = glm::vec2(0.0f, 0.0f);
    EventsAABB _circuitAABB;
    std::unique_ptr<Events> _events;
    uint32_t _currentFrame = 0u;
};
}
#endif // _EventsLoader_h_
