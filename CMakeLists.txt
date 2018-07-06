# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of EMSim <https://bbpcode.epfl.ch/browse/code/viz/EMSim/>

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

common_find_package(Boost REQUIRED COMPONENTS unit_test_framework program_options)
common_find_package(Brion REQUIRED)
common_find_package_post()

set(ISPC_BINARY ispc)

find_program(ISPC ispc)
if(NOT ISPC)
  set(ISPC_BINARY ${CMAKE_BINARY_DIR}/ispc-v1.9.2-linux/ispc)
  if(NOT EXISTS ${ISPC_BINARY})
    message("ispc not found. Trying to download... " ${ISPC_BINARY})
    file(DOWNLOAD
         https://sourceforge.net/projects/ispcmirror/files/v1.9.2/ispc-v1.9.2-linux.tar.gz
         ${CMAKE_BINARY_DIR}/ispc-v1.9.2-linux.tar.gz
         SHOW_PROGRESS)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ispc-v1.9.2-linux.tar.gz
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  endif()
endif()

add_subdirectory(glm)
add_subdirectory(apps)
add_subdirectory(emSim)
add_subdirectory(tests)

set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(EMSIM_PACKAGE_DEB_DEPENDS libboost-program-options-dev libglm-dev)
include(DoxygenRule)
