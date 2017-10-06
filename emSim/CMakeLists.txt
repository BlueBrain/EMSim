# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of EMSim <https://bbpcode.epfl.ch/code/#/admin/projects/viz/EMSim>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

set(ISPC_TARGET_LIST sse4 avx avx2 avx512knl-i32x16 avx512skx-i32x16)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ComputeSamplePoints.o
                          ${CMAKE_CURRENT_BINARY_DIR}/ComputeSamplePoints.h
                   COMMAND ispc --target=avx ispc/ComputeSamplePoints.ispc
                           --opt=fast-math --pic
                           -o ${CMAKE_CURRENT_BINARY_DIR}/ComputeSamplePoints.o
                           -h ${CMAKE_CURRENT_BINARY_DIR}/ComputeSamplePoints.h
                           -O3
                   DEPENDS ispc/ComputeSamplePoints.ispc
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(EMSIMCOMMON_PUBLIC_HEADERS Events.h
                                  EventsLoader.h
                                  helpers.h
                                  SamplePoints.h)
set(EMSIMCOMMON_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/ComputeSamplePoints.h)
set(EMSIMCOMMON_SOURCES Events.cpp
                           EventsLoader.cpp
                           SamplePoints.cpp
                           ispc/tasksys.cpp
                           ${CMAKE_CURRENT_BINARY_DIR}/ComputeSamplePoints.o)
set(EMSIMCOMMON_LINK_LIBRARIES PUBLIC Brion Brain glm)

common_library(EMSimCommon)
