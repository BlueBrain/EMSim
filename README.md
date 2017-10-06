# EMSIM

EMSim is a library that compute different electro-magnetic effects 
like LPF and VSD

## Features

Hello provides the following functionality:
* Compute LFP

## Building from Source

EMSim requires a C++11 compiler and uses CMake to create a
platform-specific build environment. The following platforms and build
environments are tested:

* Linux: Ubuntu 16.04, RHEL 6.8 (Makefile, Ninja)

Building from source is as simple as:

    git clone --recursive https://bbpcode.epfl.ch/code/a/viz/EMSim
    mkdir EMSim/build
    cd EMSim/build
    cmake -DCLONE_SUBPROJECTS=ON -GNinja ..
    ninja

