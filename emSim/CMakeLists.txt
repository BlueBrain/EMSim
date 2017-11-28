# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of EMSim <https://bbpcode.epfl.ch/browse/code/viz/EMSim/>
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

set(ISPC_FILES ComputeSamplePoints ComputeVolume)

option(USE_ALIGNED_MALLOC "Use _mm_alloc instead of malloc" OFF)
if(USE_ALIGNED_MALLOC)
  add_definitions( -DUSE_ALIGNED_MALLOC )
endif()

set(EMSIMCOMMON_HEADERS)
foreach(ISPC_FILE ${ISPC_FILES})
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ISPC_FILE}.o
                            ${CMAKE_CURRENT_BINARY_DIR}/${ISPC_FILE}.h
                     COMMAND ${ISPC_BINARY} --target=avx ispc/${ISPC_FILE}.ispc
                             --addressing=64
                             --arch=x86-64
                             --opt=fast-math --pic
                             -o ${CMAKE_CURRENT_BINARY_DIR}/${ISPC_FILE}.o
                             -h ${CMAKE_CURRENT_BINARY_DIR}/${ISPC_FILE}.h
                             -O3
                     DEPENDS ispc/${ISPC_FILE}.ispc
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND EMSIMCOMMON_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${ISPC_FILE}.h )
endforeach()

set(EMSIMCOMMON_PUBLIC_HEADERS Events.h
                               EventsLoader.h
                               helpers.h
                               SamplePoints.h
                               Volume.h)

set(EMSIMCOMMON_SOURCES Events.cpp
                        EventsLoader.cpp
                        SamplePoints.cpp
                        Volume.cpp
                        ispc/tasksys.cpp)

foreach(ISPC_FILE ${ISPC_FILES})
  list(APPEND EMSIMCOMMON_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${ISPC_FILE}.o )
endforeach()

set(EMSIMCOMMON_LINK_LIBRARIES PUBLIC Brion Brain)

common_library(EMSimCommon)
