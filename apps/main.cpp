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

#include <cmath>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <emSim/EventsLoader.h>
#include <emSim/SamplePoints.h>
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

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("");

    uint32_t neuronsPerBatch = 1000u;
    std::string inputFile;
    std::string outputFile;
    std::string target;
    std::string report;
    std::vector<glm::vec3> samplePointsPos;
    glm::vec2 timeRange = glm::vec2(-1.0f, -1.0f);

    // clang-format off
    desc.add_options()
        ("help,h", "Print this help message.\n")
        ("input,i", po::value<std::string>(&inputFile)->required( ), "Path to Blueconfig file.")
        ("output,o", po::value<std::string>(&outputFile)->required( ), "Path for the output file.")
        ("target", po::value<std::string>(&target), "The circuit's target.")
        ("report", po::value<std::string>(&report)->required(), "The name of the report.")
        ("start-time", po::value<float>(&timeRange.x), "The start time")
        ("end-time", po::value<float>(&timeRange.y), "The end time")
        ("sample-point", po::value<std::vector<glm::vec3>>(&samplePointsPos)->composing(),
         "The x y z positions of a sample point. Must be written in the form: "
         "--sample-point x,y,z")
        ("neurons-per-batch", po::value<uint32_t>(&neuronsPerBatch)->default_value(1000u),
         "To avoid out of memory problems EMSim compute the neuron's contibution to the field"
         "by batches of neurons. This option defines how many neurons will be loaded per batch.");
    // clang-format on

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }
        po::notify(vm);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }

    lfp::EventsLoader eventLoader(inputFile, target, report, neuronsPerBatch,
                                  timeRange);

    if (!samplePointsPos.empty())
    {
        lfp::SamplePoints samplePoints(eventLoader.getFramesCount(),
                                       samplePointsPos);
        for (uint32_t i = 0; i < eventLoader.getBatchesCount(); ++i)
        {
            samplePoints.refine(eventLoader.loadNextBatch());
        }
        samplePoints.writeToFile(eventLoader.getTimeRange(),
                                 eventLoader.getDt(), outputFile, inputFile,
                                 report, target);
    }

    return 0;
}
