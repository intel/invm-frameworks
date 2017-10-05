#
# Copyright (c) 2017, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of Intel Corporation nor the names of its contributors
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8.4)
project(i18n)

# Directories ...
get_filename_component(ROOT ./../ ABSOLUTE)

set(OUTPUT_DIR ${ROOT}/output/build/${OS_TYPE}/${BUILD_TYPE} CACHE PATH "Build Path")
set(HEADER_DIR ${OUTPUT_DIR}/include/libinvm-i18n)

set(CMAKE_BINARY_DIR ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --------------------------------------------------------------------------------------------------
# invm-i18n
# --------------------------------------------------------------------------------------------------
file(GLOB I18NHEADERS
	src/libIntel_i18n.h
	src/system.h
	)
file(COPY ${I18NHEADERS}
        DESTINATION ${HEADER_DIR})

set(SRC src/gettext.c
        src/gettext_dummy.c
        src/safe_str.c
        src/textdomain.c
        )
if(WIN_BUILD)
    set(SRC ${SRC} src/win_system.c)
else()
    set(SRC ${SRC} src/posix_system.c)
endif()

add_library(invm-i18n ${LIB_TYPE} ${SRC})

# this will create the symlinks on Linux
set_target_properties(invm-i18n
        PROPERTIES
            VERSION 1.0.0
            SOVERSION 1
        )
