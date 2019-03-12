/* Copyright (c) 2017, EPFL/Blue Brain Pr#oject
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

#include <emSim/EventsLoader.h>

namespace ems
{
EventsLoader::EventsLoader(const std::string& filePath,
                           const std::string& target, const std::string& report,
                           const glm::vec2& timeRange, const float fraction)
    : _bc(filePath)
    , _timeRange(timeRange)
{
    _circuit.reset(new brain::Circuit(_bc));

    _gids = target.empty() ? _circuit->getRandomGIDs(fraction)
                           : _circuit->getRandomGIDs(fraction, target);

    auto reportSource = _bc.getReportSource(report);
    _report.reset(new brion::CompartmentReport(reportSource, brion::MODE_READ));

    _validateTimeRange();

    _numberOfFrames =
        1u + std::floor((_timeRange.y - _timeRange.x) / _report->getTimestep() +
                        0.5f);
    _validateCurrentReport(_gids);
    _loadStaticEventGeometry();
    _report->updateMapping(_gids);
}

const Events& EventsLoader::loadNextFrame()
{
    const auto& values = _report->loadFrame(_currentFrame * _report->getTimestep() +
                                            _timeRange.x).get().data;
    memcpy(_events->getPowers(), values->data(),
           _report->getFrameSize() * sizeof(float));
    ++_currentFrame;
    return *_events;
}

const Events& EventsLoader::getLoadedFrame() const
{
    return *_events;
}

void EventsLoader::_loadStaticEventGeometry()
{
    _report->updateMapping(_gids);
    _events.reset(new Events(_report->getFrameSize()));
    std::cout << "INFO: Loading " << _events->getEventsCount() << " compartments... " 
              << std::endl;

    constexpr size_t morphosPerBatch = 1000u;

    auto itFirst = _gids.begin();
    while (itFirst != _gids.end())
    {
        auto itLast = itFirst;
        for (size_t i = 0; i != morphosPerBatch && itLast != _gids.end(); ++i)
            ++itLast;

        brain::GIDSet gidsPerBatch(itFirst, itLast);

        const auto morphologies =
            _circuit->loadMorphologies(gidsPerBatch,
                                       brain::Circuit::Coordinates::global);
        _report->updateMapping(gidsPerBatch);
        const auto mapping = _computeInverseMapping();

        _computeStaticEventGeometry(mapping, morphologies);

        itFirst = itLast;
    }
    std::cout << "INFO: Full AABB: x:[" << _circuitAABB.min.x << " "
              << _circuitAABB.max.x << "] y:[" << _circuitAABB.min.y << " "
              << _circuitAABB.max.y << "] z:[" << _circuitAABB.min.z << " "
              << _circuitAABB.max.z << "]" << std::endl;
}

void EventsLoader::_computeStaticEventGeometry(
    const FlatInverseMapping& mapping,
    const brain::neuron::Morphologies& morphologies)
{
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
                auto pos = soma.getCentroid();
                float radius = soma.getMeanRadius();
                _events->addEvent(glm::vec3(pos.x(), pos.y(), pos.z()), radius);
                _circuitAABB.add(glm::vec3(pos.x(), pos.y(), pos.z()), radius);
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
        const float compartmentLength = normLength * neuronSection.getLength();

        const auto& points = neuronSection.getSamples(samples);

        for (const auto& point : points)
        {
            auto pos = point.get_sub_vector<3, 0>();
            float radius = compartmentLength * .2f;
            _events->addEvent(glm::vec3(pos.x(), pos.y(), pos.z()), radius);
            _circuitAABB.add(glm::vec3(pos.x(), pos.y(), pos.z()), radius);
        }
    }
}

void EventsLoader::_validateCurrentReport(const brain::GIDSet& gidSet) const
{
    brain::GIDSet testSet = {*gidSet.begin()};
    _report->updateMapping(testSet);

    const uint32_t compartmentCount = _report->getFrameSize();
    std::vector<float> compartmentCurrents(compartmentCount);
    const auto& values = _report->loadFrame(
                            (_timeRange.x + _timeRange.y) / 2u).get().data;
    memcpy(compartmentCurrents.data(), values->data(),
           compartmentCount * sizeof(float));

    float currentsSum = 0.0f;
    float absCurrentsSum = 0.0f;
    for (uint32_t i = 0; i < compartmentCount; ++i)
    {
        currentsSum += compartmentCurrents[i];
        absCurrentsSum += std::abs(compartmentCurrents[i]);
    }
    std::cout << "Normalized current sum: " << currentsSum / absCurrentsSum
              << std::endl;
}

void EventsLoader::_validateTimeRange()
{
    if (_timeRange.x < 0.0f || _timeRange.y < 0.0f)
    {
        _timeRange = glm::vec2(_report->getStartTime(),
                               _report->getEndTime() - _report->getTimestep());
        std::cout << "WARNING: Time range used is the maximum available."
                  << std::endl;
    }
    else if (_timeRange.y > _report->getEndTime() - _report->getTimestep())
    {
        _timeRange.y = _report->getEndTime() - _report->getTimestep();
        std::cout << "WARNING: Time range is clamped to the maximum bound."
                  << std::endl;
    }

    if (_timeRange.x < _report->getStartTime())
    {
        _timeRange.x = _report->getStartTime();
        std::cout << "WARNING: Time range is clamped to the minimum bound."
                  << std::endl;
    }

    std::cout << "INFO: Time range is: [" << _timeRange.x << " " << _timeRange.y
              << "]"
              << " with DT: " << _report->getTimestep() << std::endl;

    if (_timeRange.x > _timeRange.y)
        throw(std::runtime_error("ERROR: invalid time range"));
}

const EventsAABB& EventsLoader::getCircuitAABB() const
{
    return _circuitAABB;
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

const std::string& EventsLoader::getDataUnit() const
{
    return _report->getDataUnit();
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
