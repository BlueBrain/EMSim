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
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <emSim/EventsLoader.h>
#include <emSim/SamplePoints.h>
#include <emSim/Volume.h>
#include <emSim/helpers.h>

namespace std
{
std::istream& operator>>(std::istream& in, glm::vec3& position)
{
    std::string arg;
    in >> arg;

    std::vector<std::string> parts;
    boost::split(parts, arg, boost::is_any_of(","));

    position.x = boost::lexical_cast<float>(parts[0]);
    position.y = boost::lexical_cast<float>(parts[1]);
    position.z = boost::lexical_cast<float>(parts[2]);

    return in;
}
}

struct EmsimParams
{
    std::string inputFile;
    std::string outputFile;
    std::string target;
    std::string report;
    std::vector<glm::vec3> samplePointsPos;
    glm::vec2 timeRange = glm::vec2(-1.0f, -1.0f);
    glm::vec3 voxelSize = glm::vec3(4.0f, 4.0f, 4.0f);
    glm::vec3 extent = glm::vec3(0.0f, 0.0f, 0.0f);
    bool exportVolume = false;
    float fraction = 1.0f;
};

bool parseArgs(EmsimParams& params, int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("");

    // clang-format off
    desc.add_options()
        ("help,h", "Print this help message.\n")
        ("input,i", po::value<std::string>(&params.inputFile)->required( ), "Path to Blueconfig file.")
        ("output,o", po::value<std::string>(&params.outputFile)->required( ), "Path for the output file.")
        ("target", po::value<std::string>(&params.target), "The circuit's target.")
        ("report", po::value<std::string>(&params.report)->required(), "The name of the report.")
        ("start-time", po::value<float>(&params.timeRange.x), "The start time")
        ("end-time", po::value<float>(&params.timeRange.y), "The end time")
        ("fraction", po::value<float>(&params.fraction), "Specify the fraction [0.0 1.0] of gids to be used "
         "during the computation. Default is 1.0.")
        ("export-volume", "Will export a floating point volume for each time steps.\n")
        ("voxel-size", po::value<glm::vec3>(&params.voxelSize), "The size in each dimension "
         "of a voxel in circuit units. Default is 4.0,4.0,4.0. Must be written in the form: "
         "--voxel-size rx,ry,rz")
        ("volume-extent", po::value<glm::vec3>(&params.extent), "Specify an additional 3d extent for the "
         "volume in micrometers. Default is 0.0,0.0,0.0. Must be written in the form: "
         "--volume-extent ex,ey,ez")
        ("sample-point", po::value<std::vector<glm::vec3>>(&params.samplePointsPos)->composing(),
         "The x y z positions of a sample point. Must be written in the form: "
         "--sample-point x,y,z");
    // clang-format on

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }
        po::notify(vm);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return false;
    }

    if (vm.count("export-volume"))
        params.exportVolume = true;

    return true;
}

void process(const EmsimParams& params)
{
    lfp::EventsLoader eventLoader(params.inputFile, params.target, params.report,
                                  params.timeRange, params.fraction);

    std::unique_ptr<lfp::SamplePoints> samplePoints;
    std::unique_ptr<lfp::Volume> volume;

    if (!params.samplePointsPos.empty())
        samplePoints.reset(new lfp::SamplePoints(eventLoader.getFramesCount(),
                                                 params.samplePointsPos));

    if (params.exportVolume)
        volume.reset(
            new lfp::Volume(params.voxelSize, params.extent, eventLoader.getCircuitAABB()));

    for (uint32_t i = 0; i < eventLoader.getFramesCount(); ++i)
    {
        const lfp::Events& events = eventLoader.loadNextFrame();

        if(!params.samplePointsPos.empty())
            samplePoints->computeNextFrame(events);

        if(params.exportVolume)
        {
            volume->compute(events);
            volume->writeToFile(eventLoader.getTimeRange().x +
                                    i * eventLoader.getDt(),
                                eventLoader.getDataUnit(), params.outputFile,
                                params.inputFile, params.report, params.target);
        }
    }

    if (!params.samplePointsPos.empty())
    {
        samplePoints->writeToFile(eventLoader.getTimeRange(),
                                  eventLoader.getDt(),
                                  eventLoader.getDataUnit(), params.outputFile,
                                  params.inputFile, params.report, params.target);
    }
}

int main(int argc, char* argv[])
{
    EmsimParams params;
    if(parseArgs(params, argc, argv))
    {
        process(params);
        return 0;
    }

    return 1;
}
