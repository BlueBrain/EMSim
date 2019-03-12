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

#include <emSim/VSDLoader.h>
#include <emSim/Volume.h>

std::vector<float> projectVSD(const std::shared_ptr<ems::Volume> volume)
{
    std::vector<float> image(volume->getSize().x * volume->getSize().z, 0.0f);

    for(uint32_t i = 0; i < volume->getSize().z; ++i)
    {
        for(uint32_t j = 0; j < volume->getSize().x; ++j)
        {
            for(uint32_t k = 0; k < volume->getSize().y; ++k)
            {
                image[i * volume->getSize().x + j] += volume->getData()[i * volume->getSize().x * volume->getSize().y + k * volume->getSize().x + j];
            }
        }
    }
    return image;
}

void writeImageToFile(const std::vector<float>& image, const std::string& outputFile, const float time)
{
    std::ofstream output;
    output.open(outputFile + "_image_floats_" +
                    ems::createTimeStepSuffix(time) + ".raw",
                std::ios::out | std::ios::binary);
    output.write((char*)image.data(), sizeof(float) * image.size());
    output.close();
}

bool parseArgs(ems::VSDParams& params, int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("");

    // clang-format off
    desc.add_options()
        ("help,h", "Print this help message.\n")
        ("input,i", po::value<std::string>(&params.inputFile)->required( ), "Path to Blueconfig file.")
        ("output,o", po::value<std::string>(&params.outputFileName)->required( ), "Name of the output volume "
         "(if 'export-volume' specified) and 2D image files")
        ("target", po::value<std::string>(&params.target), "The circuit's target.")
        ("report-voltage", po::value<std::string>(&params.reportVoltage)->required(), "The name of the voltage report in the Blueconfig.")
        ("report-area", po::value<std::string>(&params.reportArea)->required(), "The path of the area report file.")
        ("sensor-res", po::value<size_t>(&params.sensorRes)->default_value(params.sensorRes), "Number of pixels per side "
         "of the square sensor.")
        ("sensor-dim", po::value<float>(&params.sensorDim)->default_value(params.sensorDim), "Length of side of the square "
         "sensor in micrometers.")
        ("curve", po::value<std::string>(&params.curveFile), "Path to the dye curve file (default: no attenuation)")
        ("start-time", po::value<float>(&params.timeRange.x), "The start time of the simulation")
        ("end-time", po::value<float>(&params.timeRange.y), "The end time of the simulation")
        ("fraction", po::value<float>(&params.fraction), "Specify the fraction [0.0 1.0] of gids to be used "
         "during the computation. Default is 1.0.")
        ("export-volume", "Will export a floating point volume for each time steps.")
        ("depth", po::value<float>(&params.depth)->default_value(params.depth), "Depth of the attenuation curve area of "
         "influence. It also defines the Y-coordinate at which it starts being applied down until y=0 (default: 2081.756 "
         "micrometers).")
        ("interpolate-attenuation", "Will export a floating point volume for each time steps.")
        ("sigma", po::value<float>(&params.sigma)->default_value(params.sigma), "Absorption + scattering coefficient "
         "(units per micrometer) in the Beer-Lambert law. Must be a positive value (default: 0.0045).")
        ("v0", po::value<float>(&params.v0)->default_value(params.v0), "Resting potential (default: -65 mV).")
        ("g0", po::value<float>(&params.g0)->default_value(params.g0), "Multiplier for surface area in background "
         "fluorescence term.")
        ("ap-threshold", po::value<float>(&params.apThreshold)->default_value(params.apThreshold), "Action potential threshold "
         "in millivolts.")
        ("soma-pixels", "Produce a text file containing the GIDs loaded and their corresponding 3D positions and indices "
         "in the resulting 2D image.");
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

    if (vm.count("interpolate-attenuation"))
        params.interpolateAttenuation = true;

    if (vm.count("soma-pixels"))
        params.somaPixel = true;

    if (vm.count("soma-pixels"))
        params.exportSomaPixels = true;

    return true;
}


void process(const ems::VSDParams& params)
{
    ems::VSDLoader vsdLoader(params);

    for(uint32_t i = 0; i < vsdLoader.getFramesCount(); ++i)
    {
        std::cout << "Computing frame: " << i << std::endl;
        const std::shared_ptr<ems::Volume> volume = vsdLoader.loadNextFrame();
        const float currentTime = vsdLoader.getTimeRange().x + i * vsdLoader.getDt();
        if(params.exportVolume)
        {
            volume->writeToFileMhd(currentTime, vsdLoader.getDataUnit(), 
                                   params.outputFileName);
        }
        std::vector<float> image = projectVSD(volume);
        writeImageToFile(image, params.outputFileName, currentTime);
    }
}

int main(int argc, char* argv[])
{
    ems::VSDParams params;
    if(parseArgs(params, argc, argv))
    {
        process(params);
        return 0;
    }

    return 1;
}
