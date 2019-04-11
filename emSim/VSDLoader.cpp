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

#include <iostream>

#include <servus/servus.h>

#include <emSim/VSDLoader.h>

namespace ems
{

VSDLoader::VSDLoader(const VSDParams& params)
    : _depth(params.depth)
    , _sigma(params.sigma)
    , _g0(params.g0)
    , _v0(params.v0)
    , _apThreshold(params.apThreshold)
    , _fraction(params.fraction)
    , _currentFrame(0u)
    , _bc(params.inputFile)
    , _attenuationCurve(params.curveFile, params.depth)
    , _timeRange(params.timeRange)
{
    _circuit.reset(new brain::Circuit(_bc));
    _gids = params.target.empty() ? _circuit->getRandomGIDs(params.fraction)
                                  : _circuit->getRandomGIDs(params.fraction, params.target);
    auto reportSource = _bc.getReportSource(params.reportVoltage);
    _reportVoltage.reset(new brion::CompartmentReport(reportSource, brion::MODE_READ, _gids));
    _reportArea.reset(new brion::CompartmentReport(servus::URI(params.reportArea), brion::MODE_READ, _gids));

    const uint32_t timeStepMultiplier = params.timeStep / _reportVoltage->getTimestep() + 0.5f;
    _dt = timeStepMultiplier * _reportVoltage->getTimestep();
    std::cout << "INFO: TimeStep rounded to a multiple of the report time step: " << _dt << std::endl;

    _validateTimeRange();

    _numberOfFrames = 1u + (_timeRange.y - _timeRange.x) / _dt;
    std::cout << "INFO: Total number of frames: " << _numberOfFrames << std::endl;

    if(_reportVoltage->getFrameSize() != _reportArea->getFrameSize())
         throw(std::runtime_error("ERROR: area and voltage report sizes don't match"));

    _reportArea->updateMapping(_gids);
    _areas = _reportArea->loadFrame(0.0f).get().data;

    _loadStaticEventGeometry(params.sensorDim, params.sensorRes, params.interpolateAttenuation); 

    if(params.exportSomaPixels)
        _writeSomaFile(params.outputFileName);
}

const std::shared_ptr<Volume> VSDLoader::loadNextFrame()
{
    _volume->clear(0.0f);
    brion::floatsPtr data = _reportVoltage->loadFrame(_currentFrame * _dt + _timeRange.x).get().data;
    for(uint32_t i = 0; i < _reportVoltage->getFrameSize(); ++i)
    {
        const int64_t index = _weightedLinks[i].voxelIndex; 
        if(index != -1)
        {
            const float voltage = std::min((*data)[i], _apThreshold);
            _volume->getData()[_weightedLinks[i].voxelIndex] += (voltage - _v0 + _g0) * _weightedLinks[i].weight;
        }
        else
            continue;
    }
    std::cout << "INFO: Frame: " << _currentFrame * _dt + _timeRange.x << " done" << std::endl; 
    ++_currentFrame;
    return _volume;
}

void VSDLoader::_loadStaticEventGeometry(const float sensorDim, const uint32_t sensorRes, const bool interpolate)
{
    _reportVoltage->updateMapping(_gids);
    std::cout << "INFO: loading " << _reportVoltage->getFrameSize() << " compartments..." << std::endl;

    uint32_t moprhosPerBatch = 10000u;
    uint32_t batchesCount = (_gids.size() - 1u) / moprhosPerBatch + 1u;
    std::vector<glm::vec3> positions;

    for (uint32_t currentBatch = 0u; currentBatch < batchesCount;
         ++currentBatch)
    {
        auto itFirst = _gids.begin();
        auto itLast = itFirst;

        std::advance(itFirst, currentBatch * moprhosPerBatch);

        const uint32_t lastIndex = (currentBatch + 1u) * moprhosPerBatch;
        if (lastIndex >= _gids.size())
            itLast = _gids.end();
        else
            std::advance(itLast, lastIndex);

        brain::GIDSet gidsPerBatch(itFirst, itLast);

        const auto morphologies =
            _circuit->loadMorphologies(gidsPerBatch,
                                       brain::Circuit::Coordinates::global);
        _reportVoltage->updateMapping(gidsPerBatch);
        const auto mapping = _computeInverseMapping();
        _computeStaticEventGeometry(mapping, morphologies, positions);
    }

    EventsAABB volumeAABB;
    const glm::vec3 center = (_circuitAABB.min + _circuitAABB.max) * 0.5f;
    volumeAABB.min = center - sensorDim / 2.0f;
    volumeAABB.max = center + sensorDim / 2.0f;
    volumeAABB.min.y = _circuitAABB.min.y;
    volumeAABB.max.y = _circuitAABB.max.y;
    const float voxelSize = (float)sensorDim / sensorRes;
    _volume.reset(new Volume(glm::vec3(voxelSize), glm::vec3(0.0f), volumeAABB));

    std::cout << "Volume size: " <<_volume->getSize().x << " " << _volume->getSize().y << " " << _volume->getSize().z << std::endl; 

    std::cout << "INFO: Circuit AABB: x:[" << _circuitAABB.min.x << " "
              << _circuitAABB.max.x << "] y:[" << _circuitAABB.min.y << " "
              << _circuitAABB.max.y << "] z:[" << _circuitAABB.min.z << " "
              << _circuitAABB.max.z << "]" << std::endl;

    std::cout << "INFO: Volume AABB: x:[" << volumeAABB.min.x << " "
              << volumeAABB.max.x << "] y:[" << volumeAABB.min.y << " "
              << volumeAABB.max.y << "] z:[" << volumeAABB.min.z << " "
              << volumeAABB.max.z << "]" << std::endl;

    _weightedLinks.resize(positions.size());
    _reportVoltage->updateMapping(_gids);

    for(uint32_t i = 0; i < positions.size(); ++i)
    {
        Link link = {0.0f, -1};
        const int64_t x = (positions[i].x - volumeAABB.min.x) / voxelSize;
        const int64_t y = (positions[i].y - volumeAABB.min.y) / voxelSize;
        const int64_t z = (positions[i].z - volumeAABB.min.z) / voxelSize;


        if((x >= 0 && x < _volume->getSize().x) && (y >= 0 && y < _volume->getSize().y) && (z >= 0 && z < _volume->getSize().z))
        {
            link.voxelIndex = z * _volume->getSize().y * _volume->getSize().x 
                              + y * _volume->getSize().x + x;
            link.weight = (*_areas)[i] * std::exp(-_sigma * (_depth - positions[i].y)) 
                          * _attenuationCurve.getAttenuation(positions[i].y, interpolate);
        }
        _weightedLinks[i] = link;        
    }
}

void VSDLoader::_computeStaticEventGeometry(const FlatInverseMapping& mapping,
                                            const brain::neuron::Morphologies& morphologies,
                                            std::vector<glm::vec3>& positions)
{
    size_t total = 0;
    for (const auto& j : mapping)
    {
        size_t offset;
        uint32_t cellIndex;
        uint32_t sectionId;
        uint16_t compartments;
        std::tie(offset, cellIndex, sectionId, compartments) = j;

	total += compartments;

        const auto& morphology = *morphologies[cellIndex];
        if (sectionId == 0)
        {
            const auto& soma = morphology.getSoma();
            const auto pos = soma.getCentroid();
            const float radius = soma.getMeanRadius();
            _circuitAABB.add(glm::vec3(pos.x(), pos.y(), pos.z()),
                             radius);            
            for (uint16_t k = 0; k != compartments; ++k)
                positions.push_back(glm::vec3(pos.x(), pos.y(), pos.z()));

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

	assert(compartments == samples.size());
	assert(compartments == points.size());
        for (const auto& point : points)
        {
            auto pos = point.get_sub_vector<3, 0>();
            float radius = compartmentLength * .2f;
            positions.push_back(glm::vec3(pos.x(), pos.y(), pos.z()));
            _circuitAABB.add(glm::vec3(pos.x(), pos.y(), pos.z()), radius);
        }
    }
}

void VSDLoader::_validateTimeRange()
{
    if (_timeRange.x < 0.0f || _timeRange.y < 0.0f)
    {
        _timeRange = glm::vec2(_reportVoltage->getStartTime(),
                               _reportVoltage->getEndTime() - _dt);
        std::cout << "WARNING: Time range used is the maximum available."
                  << std::endl;
    }
    else if (_timeRange.y > _reportVoltage->getEndTime() - _dt)
    {
        _timeRange.y = _reportVoltage->getEndTime() - _dt;
        std::cout << "WARNING: Time range is clamped to the maximum bound."
                  << std::endl;
    }

    if (_timeRange.x < _reportVoltage->getStartTime())
    {
        _timeRange.x = _reportVoltage->getStartTime();
        std::cout << "WARNING: Time range is clamped to the minimum bound."
                  << std::endl;
    }

    std::cout << "INFO: Time range is: [" << _timeRange.x << " " << _timeRange.y
              << "]"
              << " with DT: " << _dt << std::endl;

    if (_timeRange.x > _timeRange.y)
        throw(std::runtime_error("ERROR: invalid time range"));
}

FlatInverseMapping VSDLoader::_computeInverseMapping() const
{
    FlatInverseMapping mapping;
    mapping.reserve(_reportVoltage->getBufferSize());

    const auto& offsets = _reportVoltage->getOffsets();
    const auto& counts = _reportVoltage->getCompartmentCounts();
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

void VSDLoader::_writeSomaFile(const std::string& baseName) const
{
    const auto& somaPositions = _circuit->getPositions(_gids);
    const std::string fileName = baseName + "_soma_pixels.txt";
    std::ofstream file(fileName);
    if(!file.is_open())
    {
        throw(std::runtime_error("ERROR: cannot open soma pixel file")); 
    }
    else
    {
        file << "# Soma position and corresponding pixel index for "
                    "each cell, in the following format:\n"
             << "#     gid [ posX posY posZ ]: i j\n"
             << std::endl;

        size_t i = 0;
        const auto origin = _volume->getOrigin();
        const auto spacing = _volume->getVoxelSize();
        for(const auto& gid: _gids)
        {
           if(file.bad())
               break;

            const auto& pos = somaPositions[i++];
            file << gid << " " << pos << ": "
                 << std::floor((pos[0] - origin[0]) / spacing[0]) << " "
                 << std::floor((pos[1] - origin[1]) / spacing[1]) << " "
                 << std::floor((pos[2] - origin[2]) / spacing[2])
                 << std::endl;
        }
    }

    if(file.good())
        std::cout << "INFO: Soma positions written as " << fileName << std::endl;
    else
        throw(std::runtime_error("ERROR: cannot write soma pixel file")); 
}

size_t VSDLoader::getFramesCount() const
{
    return _numberOfFrames;
}

float VSDLoader::getDt() const
{
    return _dt;
}

glm::vec2 VSDLoader::getTimeRange() const
{
    return _timeRange;
}

const std::string& VSDLoader::getDataUnit() const
{
    return _reportVoltage->getDataUnit();
}

}
