# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of EMSim <https://bbpcode.epfl.ch/code/#/admin/projects/viz/EMSim>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(EMSim VERSION 1.0.0)
set(EMSim_VERSION_ABI 1)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)

set(EMSIM_LICENSE LGPL)
set(EMSIM_DESCRIPTION "Electro-magnetic simulation software")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
set(EMSIM_DEB_DEPENDS libboost-program-options-dev libboost-test-dev libglm-dev)

include(Common)
common_find_package(glm REQUIRED SYSTEM)
common_find_package(Boost REQUIRED COMPONENTS unit_test_framework program_options)
common_find_package(Brion REQUIRED)
common_find_package_post()

add_subdirectory(apps)
add_subdirectory(emSim)
add_subdirectory(tests)

set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(EMSIM_PACKAGE_DEB_DEPENDS libboost-program-options-dev libglm-dev)
include(CommonCPack)
include(DoxygenRule)
