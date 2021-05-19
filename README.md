# EMSIM

EMSim is a library that compute different electro-magnetic effects 
like LPF and VSD

## Features

EMSim provides the following functionality:
* Compute LFP
* Compute VSD

## Building from Source

EMSim requires a C++11 compiler and uses CMake to create a
platform-specific build environment. The following platforms and build
environments are tested:

* Linux: Ubuntu 16.04, RHEL 6.8 (Makefile, Ninja)

Building from source is as simple as:

    git clone --recursive https://github.com/BlueBrain/EMSim
    cd EMSim
    git submodule update --init
    mkdir build
    cd build
    cmake -DCLONE_SUBPROJECTS=ON -GNinja ..
    ninja

The final executables will be located in the build/bin folder.

## Usage

Compute the LFP values on 2 probes:

    emsim -i blueconfigFile -o outputFileName --target cuirtcuitTarget --report currentReport --sample-point 12,34,32 --sample-point 43,56,43

Compute the VSD with 1000um sensor size with a 512 resolution:

    emsimVSD -i blueconfigFile -o outputFileName --target cuirtcuitTarget --report-voltage voltageReport --report-area areaReport --sensor-dim 1000 --sensor-res 512

## Acknowledgement & Funding

This project was supported by funding to the Blue Brain Project, a research 
center of the École polytechnique fédérale de Lausanne (EPFL), from the Swiss 
government's ETH Board of the Swiss Federal Institutes of Technology.

Copyright (c) 2017-2021 Blue Brain Project/EPFL
