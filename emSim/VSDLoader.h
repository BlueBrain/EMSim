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

#ifndef _VSDLoader_h_
#define _VSDLoader_h_

#include <emSim/AttenuationCurve.h>
#include <emSim/Volume.h>

#include <brain/brain.h>
#include <brion/brion.h>

namespace ems
{

/** Tuple of buffer offset, cell index, section ID, compartment counts */
typedef std::tuple<size_t, uint32_t, uint32_t, uint16_t> MappingElement;
typedef std::vector<MappingElement> FlatInverseMapping;

struct VSDParams
{
    std::string inputFile;
    std::string outputFileName;
    std::string target;
    std::string reportVoltage;
    std::string reportArea;
    std::string curveFile;
    float sensorDim = 1000.0f;
    size_t sensorRes = 512u;
    float depth = 2081.756f;
    float sigma = 0.0045f;
    float g0 = 0.0f;
    float v0 = -65.0f;
    float apThreshold = std::numeric_limits<float>::max();
    float fraction = 1.0f;
    //float dt;
    glm::vec2 timeRange = glm::vec2(-1.0f, -1.0f);
    bool interpolateAttenuation = false;
    bool somaPixel = false;
    bool exportVolume = false;
    bool exportPointSprite = false;
    bool exportSomaPixels = false;
};

class VSDLoader
{
public:
    VSDLoader(const VSDParams& params);

    /**
     * Update the volume with voltage values from the next frame.
     * @return the updated volume
     */
    const std::shared_ptr<Volume> loadNextFrame();

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
    FlatInverseMapping _computeInverseMapping() const;
    void _validateTimeRange();
    void _loadStaticEventGeometry(const float sensorDim, const uint32_t sensorRes, const bool interpolate);
    void _computeStaticEventGeometry(const FlatInverseMapping& mapping,
                                     const brain::neuron::Morphologies& morphologies,
                                     std::vector<glm::vec3>& positions);
    void _writeSomaFile(const std::string& baseName) const;

    float _depth = 2081.756f;
    float _sigma = 0.0045f;
    float _g0 = 0.0f;
    float _v0 = -65.0f;
    float _apThreshold = 300.0f;
    float _fraction = 1.0f;
    float _dt = 0.1f;

    uint32_t _numberOfFrames = 0u;
    uint32_t _currentFrame = 0u;

    const brion::BlueConfig _bc;
    std::unique_ptr<brion::CompartmentReport> _reportVoltage;
    std::unique_ptr<brion::CompartmentReport> _reportArea;
    std::unique_ptr<brain::Circuit> _circuit;
    std::shared_ptr<Volume> _volume;
    AttenuationCurve _attenuationCurve;

    brain::GIDSet _gids;
    glm::vec2 _timeRange = glm::vec2(0.0f, 0.0f);
    EventsAABB _circuitAABB;

    struct Link
    {
        float weight = 0;
        int64_t voxelIndex = -1; 
    };

    brion::floatsPtr _areas;
    std::vector<Link> _weightedLinks;
};

}
#endif // _VSDLoader_h_
