# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of EMSim <https://github.com/BlueBrain/EMSim>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(EMSim VERSION 1.0.0)
set(EMSim_VERSION_ABI 1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(glm)  #XXX: as of brion 3.2.0, this has to come before find_package(Brion)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(Brion REQUIRED)

set(ISPC_BINARY ispc)
find_program(ISPC ispc)

enable_testing()

add_subdirectory(apps)
add_subdirectory(emSim)
add_subdirectory(tests)

export(PACKAGE emSim)

#include(DoxygenRule)
