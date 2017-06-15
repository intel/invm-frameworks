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
project(invm-frameworks)

# --------------------------------------------------------------------------------------------------
# Setup
# --------------------------------------------------------------------------------------------------

# OS 
if (UNIX)
    set(LNX_BUILD 1)
    set(OS_TYPE linux)
elseif (MINGW)
    set(OS_TYPE windows)
    set(WIN_BUILD 1)
endif ()

# Build Type
if (RELEASE)
    set(BUILD_TYPE release)
else()
    set(BUILD_TYPE debug)
endif()

# LIB Type
set(LIB_TYPE SHARED)
option(BUILD_STATIC "Build libraries as STATIC" SHARED)
if (BUILD_STATIC)
    set(LIB_TYPE STATIC)
endif()

# Build Version
if (NOT BUILDNUM)
    execute_process(COMMAND git describe --abbrev=0 OUTPUT_VARIABLE BUILDNUM)
    if ("${BUILDNUM}" MATCHES "^([a-zA-Z-]*-)(.*)$")
        #replace
        string(REGEX REPLACE "^([a-zA-Z-]*-)" "" BUILDNUM "${BUILDNUM}")
        string(REGEX REPLACE "\n$" "" BUILDNUM "${BUILDNUM}")
    else ()
        execute_process(COMMAND pwd OUTPUT_VARIABLE BUILDNUM)
        set(BUILDNUM 99.99.99.9999)
    endif()
endif()

# --------------------------------------------------------------------------------------------------
# Project wide defines and flags
# --------------------------------------------------------------------------------------------------
set(CMAKE_C_FLAGS "-std=c99 -Wall -Werror -Wfatal-errors -fstack-protector-all -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=500")
set(CMAKE_CXX_FLAGS "-MMD -D__VERSION_MAJOR__=${VERSION_MAJOR} -D__VERSION_MINOR__=${VERSION_MINOR} -D__VERSION_HOTFIX__=${VERSION_HOTFIX} -D__VERSION_BUILDNUM__=${VERSION_BUILDNUM} -D__VERSION_NUMBER__=${BUILDNUM}")

if (RELEASE)
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
else()
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O -ggdb")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O -ggdb")
endif()

add_subdirectory(invm-i18n)
add_subdirectory(invm-cim)
add_subdirectory(invm-cli)

# --------------------------------------------------------------------------------------------------
# invm-frameworks rpm
# --------------------------------------------------------------------------------------------------
get_filename_component(PROJECT_ROOT ./ ABSOLUTE)
set(RPMBUILD_DIR ${PROJECT_ROOT}/output/rpmbuild)

add_custom_target(rpm 
        COMMAND ${CMAKE_COMMAND} -E echo "Entering make rpm..."
	# Make directories 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR} 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/BUILD 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/BUILDROOT 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/SOURCES 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/RPMS
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/SRPMS
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/SPECS
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RPMBUILD_DIR}/BUILD/invm-frameworks-${BUILDNUM}
	# Copy spec file 
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_ROOT}/install/linux/rel-release/*.spec ${RPMBUILD_DIR}/SPECS/invm-frameworks.spec
	# Update the spec file with build version
        COMMAND sed -i "s/^%define build_version .*/%define build_version ${BUILDNUM}/g" ${RPMBUILD_DIR}/SPECS/invm-frameworks.spec
	# Archive the directory 
        COMMAND cd ${PROJECT_ROOT} && git archive --format=tar --prefix="invm-frameworks-${BUILDNUM}/" HEAD | gzip -c > ${RPMBUILD_DIR}/SOURCES/invm-frameworks-${BUILDNUM}.tar.gz
	# rpmbuild
        COMMAND export QA_RPATHS=0x0002 && cd ${PROJECT_ROOT} && rpmbuild -ba ${RPMBUILD_DIR}/SPECS/invm-frameworks.spec --define "_topdir ${RPMBUILD_DIR}" --define "cflag ${CMAKE_C_FLAGS}"
        COMMAND ${CMAKE_COMMAND} -E echo "Exiting make rpm..." 
        COMMAND make clean_cmake 
        )   

# --------------------------------------------------------------------------------------------------
# invm-frameworks clean_cmake
# --------------------------------------------------------------------------------------------------
add_custom_target(clean_cmake
	COMMAND rm ${PROJECT_ROOT}/CMakeCache.txt
	COMMAND rm -rf ${PROJECT_ROOT}/CMakeFiles
	COMMAND rm -rf ${PROJECT_ROOT}/Makefile
	COMMAND rm -rf ${PROJECT_ROOT}/cmake_install.cmake
	COMMAND rm -rf ${PROJECT_ROOT}/invm-cim/CMakeFiles
	COMMAND rm -rf ${PROJECT_ROOT}/invm-cim/Makefile
	COMMAND rm -rf ${PROJECT_ROOT}/invm-cim/cmake_install.cmake
	COMMAND rm -rf ${PROJECT_ROOT}/invm-cli/CMakeFiles
	COMMAND rm -rf ${PROJECT_ROOT}/invm-cli/Makefile
	COMMAND rm -rf ${PROJECT_ROOT}/invm-cli/cmake_install.cmake
	COMMAND rm -rf ${PROJECT_ROOT}/invm-i18n/CMakeFiles
	COMMAND rm -rf ${PROJECT_ROOT}/invm-i18n/Makefile
	COMMAND rm -rf ${PROJECT_ROOT}/invm-i18n/cmake_install.cmake
)
