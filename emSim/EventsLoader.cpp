/* Copyright (c) 2017, EPFL/Blue Brain Pr#oject
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

#include <emSim/EventsLoader.h>

namespace lfp
{
EventsLoader::EventsLoader(const std::string& filePath,
                           const std::string& target, const std::string& report,
                           const uint32_t neuronsPerBatch,
                           const glm::vec2& timeRange)
    : _bc(filePath)
    , _neuronsPerBatch(neuronsPerBatch)
    , _currentBatch(0u)
    , _timeRange(timeRange)
{
    _circuit.reset(new brain::Circuit(_bc));

    _gids = target.empty() ? _circuit->getGIDs() : _circuit->getGIDs(target);
    _numberOfBatches = _gids.size() % neuronsPerBatch == 0
                           ? _gids.size() / neuronsPerBatch
                           : _gids.size() / neuronsPerBatch + 1;

    auto reportSource = _bc.getReportSource(report);
    _report.reset(new brion::CompartmentReport(reportSource, brion::MODE_READ));

    _validateTimeRange();

    _numberOfFrames =
        1u + std::floor((_timeRange.y - _timeRange.x) / _report->getTimestep() +
                        0.5f);

    std::cout << "Number of batches: " << _numberOfBatches
              << " Number of Frames: " << _numberOfFrames << std::endl;
}

void EventsLoader::_validateTimeRange()
{
    if (_timeRange.x < 0.0f || _timeRange.y < 0.0f)
    {
        _timeRange = glm::vec2(_report->getStartTime(),
                               _report->getEndTime() - _report->getTimestep());
        std::cout << "Time range used is the maximum available: ["
                  << _timeRange.x << " " << _timeRange.y << "]" << std::endl;
    }
    else if (_timeRange.y > _report->getEndTime() - _report->getTimestep())
    {
        _timeRange.y = _report->getEndTime() - _report->getTimestep();
        std::cout << "Time range is clamped to the maximum bound: ["
                  << _timeRange.x << " " << _timeRange.y << "]" << std::endl;
    }

    if (_timeRange.x < _report->getStartTime())
    {
        _timeRange.x = _report->getStartTime();
        std::cout << "Time range is clamped to the minimum bound: ["
                  << _timeRange.x << " " << _timeRange.y << "]" << std::endl;
    }

    if (_timeRange.x > _timeRange.y)
        throw(std::runtime_error("error: invalid time range"));
}

Events EventsLoader::loadNextBatch()
{
    std::cout << "Start loading batch: " << _currentBatch << std::endl;

    auto itFirst = _gids.begin();
    auto itLast = itFirst;

    const uint32_t firstIndex = _currentBatch * _neuronsPerBatch;
    if (firstIndex + 1u > _gids.size())
        return Events(0u, 0u);
    else
        std::advance(itFirst, _currentBatch * _neuronsPerBatch);

    const uint32_t lastIndex = (_currentBatch + 1u) * _neuronsPerBatch;
    if (lastIndex + 1u > _gids.size())
        itLast = _gids.end();
    else
        std::advance(itLast, lastIndex);

    brain::GIDSet gidsPerBatch(itFirst, itLast);

    std::cout << "Updating mapping... " << std::endl;
    _report->updateMapping(gidsPerBatch);
    const auto mapping = _computeInverseMapping();
    std::cout << "Loading morphologies... " << std::endl;
    const auto morphologies =
        _circuit->loadMorphologies(gidsPerBatch,
                                   brain::Circuit::Coordinates::global);
    Events events = _createEvents(mapping, morphologies);

    std::cout << "Number of event: " << events.getEventsCount()
              << " Number of timeSteps: " << events.getTimeStepsCount()
              << std::endl;

    ++_currentBatch;

    return events;
}

Events EventsLoader::_createEvents(
    const FlatInverseMapping& mapping,
    const brain::neuron::Morphologies& morphologies)
{
    std::cout << "Loading events... " << std::endl;

    const auto& frames =
        _report->loadFrames(_timeRange.x, _timeRange.y + _report->getTimestep())
            .get();
    size_t frameSize = _report->getFrameSize();

    std::cout << "Number of compartments: " << frameSize << std::endl;

    Events events(frameSize, _numberOfFrames);
    for (size_t i = 0; i < _numberOfFrames; ++i)
    {
        size_t frameOffset = i * frameSize;
        for (const auto& j : mapping)
        {
            size_t offset;
            uint32_t cellIndex;
            uint32_t sectionId;
            uint16_t compartments;
            std::tie(offset, cellIndex, sectionId, compartments) = j;

            const auto& morphology = *morphologies[cellIndex];

            if (sectionId == 0)
            {
                const auto& soma = morphology.getSoma();
                for (uint16_t k = 0; k != compartments; ++k)
                {
                    if (i == 0)
                    {
                        auto pos = soma.getCentroid();
                        float radius = soma.getMeanRadius();
                        events.addEvent(glm::vec3(pos.x(), pos.y(), pos.z()),
                                        radius);
                    }
                    events.addEventPower(
                        (*frames.data)[frameOffset + offset + k]);
                }
                continue;
            }

            brion::floats samples;
            samples.reserve(compartments);
            // normalized compartment length
            const float normLength = 1.f / float(compartments);
            for (float k = normLength * .5f; k < 1.0; k += normLength)
                samples.push_back(k);

            const auto& neuronSection = morphology.getSection(sectionId);

            // actual compartment length
            const float compartmentLength =
                normLength * neuronSection.getLength();

            const auto& points = neuronSection.getSamples(samples);

            uint16_t k = 0;
            for (const auto& point : points)
            {
                if (i == 0)
                {
                    auto pos = point.get_sub_vector<3, 0>();
                    float radius = compartmentLength * .2f;
                    events.addEvent(glm::vec3(pos.x(), pos.y(), pos.z()),
                                    radius);
                }
                events.addEventPower((*frames.data)[frameOffset + offset + k]);
                ++k;
            }
        }
    }
    return events;
}

uint32_t EventsLoader::getBatchesCount() const
{
    return _numberOfBatches;
}

size_t EventsLoader::getFramesCount() const
{
    return _numberOfFrames;
}

float EventsLoader::getDt() const
{
    return _report->getTimestep();
}

glm::vec2 EventsLoader::getTimeRange() const
{
    return _timeRange;
}

FlatInverseMapping EventsLoader::_computeInverseMapping() const
{
    FlatInverseMapping mapping;
    mapping.reserve(_report->getBufferSize());

    const auto& offsets = _report->getOffsets();
    const auto& counts = _report->getCompartmentCounts();
    for (size_t i = 0; i != offsets.size(); ++i)
    {
        for (size_t j = 0; j != offsets[i].size(); ++j)
        {
            const size_t count = counts[i][j];
            if (count != 0)
                mapping.push_back(std::make_tuple(offsets[i][j], i, j, count));
        }
    }
    std::sort(mapping.begin(), mapping.end());
    return mapping;
}
}
