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

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_SKIP_BUILD_RPATH TRUE)

if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.8.12)
	cmake_policy(SET CMP0040 OLD)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/Find")

# --------------------------------------------------------------------------------------------------
# Setup
# --------------------------------------------------------------------------------------------------
if(ESX_BUILD)
	set(BUILD_ESX 1)
	set(OS_TYPE ESX)
	include(CMake/esx.cmake)
elseif(UNIX)
	set(LNX_BUILD 1)
	set(OS_TYPE linux)

	include(GNUInstallDirs)
	find_package(math REQUIRED)
elseif(MINGW OR MSVC)
	set(OS_TYPE windows)
	set(WIN_BUILD 1)
endif()

# Build Type
if(RELEASE)
	set(BUILD_TYPE release)
else()
	set(BUILD_TYPE debug)
endif()

# LIB Type
set(LIB_TYPE SHARED)
option(BUILD_STATIC "Build libraries as STATIC" OFF)
if(BUILD_STATIC)
	set(LIB_TYPE STATIC)
endif()

get_filename_component(ROOT ./ ABSOLUTE)

set(OUTPUT_DIR ${ROOT}/output/build/${OS_TYPE}/${BUILD_TYPE})
set(INCLUDE_DIR ${OUTPUT_DIR}/include)

set(CMAKE_BINARY_DIR ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Build Version
if(NOT BUILDNUM)
	execute_process(COMMAND git describe --abbrev=0 OUTPUT_VARIABLE BUILDNUM)
	if("${BUILDNUM}" MATCHES "^([a-zA-Z-]*-)(.*)$")
		string(REGEX REPLACE "^([a-zA-Z-]*-)" "" BUILDNUM "${BUILDNUM}")
		string(REGEX REPLACE "\n$" "" BUILDNUM "${BUILDNUM}")
	else()
		execute_process(COMMAND pwd OUTPUT_VARIABLE BUILDNUM)
		set(BUILDNUM 99.99.99.9999)
	endif()
endif()

# --------------------------------------------------------------------------------------------------
# Project wide defines and flags
# --------------------------------------------------------------------------------------------------
add_definitions(
	-D_GNU_SOURCE
	-D__VERSION_MAJOR__=${VERSION_MAJOR}
	-D__VERSION_MINOR__=${VERSION_MINOR}
	-D__VERSION_HOTFIX__=${VERSION_HOTFIX}
	-D__VERSION_BUILDNUM__=${VERSION_BUILDNUM}
	-D__VERSION_NUMBER__=${BUILDNUM}
	)

if(BUILD_STATIC)
	add_definitions(
		-DBUILD_STATIC
		)
endif()

if(RELEASE)
	add_definitions(
		-DRELEASE
		)
	if(MSVC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fno-strict-aliasing")
	endif()
else()
	if(MSVC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1")
	endif()
endif()

if(LNX_BUILD)
	add_definitions(
		-D__LINUX__
		)

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wformat-security -D_FORTIFY_SOURCE=2 -fstack-protector-all -D_XOPEN_SOURCE=500 -Wall -Wfatal-errors -MMD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fstack-protector-all -D_XOPEN_SOURCE=500 -Wall -Wfatal-errors -MMD")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -z noexecstack -z relro -z now -pie -fPIC")
endif()

if(WIN_BUILD)
	add_definitions(
		-D_WIN32_WINNT=0x0601
		-D__WINDOWS__
	)

	if(MSVC)
		add_definitions(
		-D__MSVC__
		-DWIN32_LEAN_AND_MEAN
		)

		#FIXME
		#4251 is export stl from dll issue
		#warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
		set(MSVC_WARN_DISABLES "/wd4251 /wd4290")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MSVC_WARN_DISABLES}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_WARN_DISABLES}")
	endif()
endif()

# --------------------------------------------------------------------------------------------------
# libinvm-cim
# --------------------------------------------------------------------------------------------------
file(GLOB CIMFRAMEWORK_HEADERS
	invm-cim/src/framework/*.h
	invm-cim/src/cimom/*.h
	invm-cim/src/common/logger/*.h
	invm-cim/src/common/string/*.h
	invm-cim/src/common/time/*.h
	invm-cim/src/common/*.h
	)
if(WIN_BUILD)
		file(GLOB OS_HDR_FILES "invm-cim/src/cimom/wmi/*.h")
else()
		file(GLOB OS_HDR_FILES "invm-cim/src/cimom/cmpi/*.h")
endif()
list (APPEND CIMFRAMEWORK_HEADERS ${OS_HDR_FILES})

file(COPY ${CIMFRAMEWORK_HEADERS}
	DESTINATION ${INCLUDE_DIR}/libinvm-cim
	)

FILE(GLOB CIMFRAMEWORK_SRC
	invm-cim/src/common/logger/*
	invm-cim/src/common/string/*
	invm-cim/src/common/time/*
	invm-cim/examples/demoprovider/os/*
	invm-cim/src/common/*
	invm-cim/src/framework/*
	invm-cim/src/cimom/*
	)

if(WIN_BUILD)
	file(GLOB OS_SRC_FILES invm-cim/src/cimom/wmi/*)
else()
	file(GLOB OS_SRC_FILES invm-cim/src/cimom/cmpi/*)
endif()

list(APPEND CIMFRAMEWORK_SRC ${OS_SRC_FILES})

add_library(invm-cim ${LIB_TYPE} ${CIMFRAMEWORK_SRC})

set_target_properties(invm-cim
	PROPERTIES
	VERSION 2.0.0
	SOVERSION 2
	)

target_include_directories(invm-cim PUBLIC
	invm-cim/src/cimom
	invm-cim/src/cimom/cmpi
	invm-cim/src/framework
	invm-cim/src/common
	invm-cim/src/common/logger
	invm-cim/src/common/string
	invm-cim/src/common/time
	${ROOT}/external/invm-cim/rapidxml-1.13
	${ROOT}/external/invm-cim/cmpi/include/cmpi
	${ROOT}/external/invm-cim/cmpi/include
	)

target_link_libraries(invm-cim
	${CMAKE_DL_LIBS}
	${MATH_LIBRARIES}
	)

if(WIN_BUILD)
	if(MSVC)
		target_link_libraries(invm-cim
			comsuppw
			Wbemuuid
			ws2_32
			)
	else()
		target_link_libraries(invm-cim
			ws2_32
			mswsock)
	endif()
endif()

if(MSVC)
	if(${LIB_TYPE} STREQUAL "SHARED")
		target_compile_definitions(invm-cim
			PUBLIC -D__INVM_CIM_DLL__
			PRIVATE -D__INVM_CIM_DLL_EXPORTS__
			)
	endif()
else()
	target_compile_definitions(invm-cim
		PRIVATE -D__WBEM_PREFIX__=Intel
		PRIVATE -DCMPI_PLATFORM_LINUX_GENERIC_GNU=1
		PRIVATE -DCMPI_VER_86=1
		)
endif()

# --------------------------------------------------------------------------------------------------
# invm-cli
# --------------------------------------------------------------------------------------------------
file(GLOB CLIFRAMEWORK_HEADERS
	invm-cli/src/framework/*.h
	)

file(COPY ${CLIFRAMEWORK_HEADERS}
	DESTINATION ${INCLUDE_DIR}/libinvm-cli
	)

file(GLOB CLIFRAMEWORK_SRC invm-cli/src/framework/*.cpp)

add_library(invm-cli ${LIB_TYPE} ${CLIFRAMEWORK_SRC})

set_target_properties(invm-cli
	PROPERTIES
	VERSION 2.0.0
	SOVERSION 2
	)

target_link_libraries(invm-cli
	${CMAKE_DL_LIBS}
	${MATH_LIBRARIES}
	invm-i18n
	)

if(WIN_BUILD)
	target_link_libraries(invm-cli
		ws2_32
		mswsock
		)
endif()

target_include_directories(invm-cli PUBLIC ${INCLUDE_DIR})

target_compile_definitions(invm-cli
	PUBLIC -D__INTEL_I18N__
	)

if(MSVC)
	if(${LIB_TYPE} STREQUAL "SHARED")
		target_compile_definitions(invm-cli
			PRIVATE -D__INVM_CLI_DLL__
			PRIVATE -D__INVM_CLI_DLL_EXPORTS__
			PRIVATE -D__INVM_CLI_TYPES__
			)
	endif()
endif()


# --------------------------------------------------------------------------------------------------
# invm-i18n
# --------------------------------------------------------------------------------------------------
file(GLOB I18NFRAMEWORK_HEADERS
	invm-i18n/src/libIntel_i18n.h
	)

file(COPY ${I18NFRAMEWORK_HEADERS}
	DESTINATION ${INCLUDE_DIR}/libinvm-i18n
	)

file(GLOB I18NFRAMEWORK_SRC
	invm-i18n/src/gettext.c
	invm-i18n/src/gettext_dummy.c
	invm-i18n/src/safe_str.c
	invm-i18n/src/textdomain.c
	)

if(ESX_BUILD OR LNX_BUILD)
	file(GLOB SYSTEM_SRC invm-i18n/src/posix_system.c)
elseif(WIN_BUILD)
	file(GLOB SYSTEM_SRC invm-i18n/src/win_system.c)
endif()
list(APPEND I18NFRAMEWORK_SRC ${SYSTEM_SRC})

add_library(invm-i18n ${LIB_TYPE} ${I18NFRAMEWORK_SRC})

set_target_properties(invm-i18n
	PROPERTIES
	VERSION 2.0.0
	SOVERSION 2
	)

if(MSVC)
	if(${LIB_TYPE} STREQUAL "SHARED")
		target_compile_definitions(invm-i18n
			PRIVATE -DI18N_EXPORTS
			)
	endif()
endif()

# --------------------------------------------------------------------------------------------------
# Pkgconfig
# --------------------------------------------------------------------------------------------------
configure_file(${ROOT}/install/linux/libinvm-i18n.pc.in ${OUTPUT_DIR}/libinvm-i18n.pc @ONLY)
configure_file(install/linux/libinvm-cli.pc.in ${OUTPUT_DIR}/libinvm-cli.pc @ONLY)
configure_file(install/linux/libinvm-cim.pc.in ${OUTPUT_DIR}/libinvm-cim.pc @ONLY)

# --------------------------------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------------------------------
if(LNX_BUILD)
	install(FILES ${OUTPUT_DIR}/libinvm-i18n.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
	install(TARGETS invm-i18n LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES ${I18NFRAMEWORK_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libinvm-i18n)

	install(FILES ${OUTPUT_DIR}/libinvm-cli.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
	install(TARGETS invm-cli DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES ${CLIFRAMEWORK_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libinvm-cli)

	install(FILES ${OUTPUT_DIR}/libinvm-cim.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
	install(TARGETS invm-cim DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES ${CIMFRAMEWORK_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libinvm-cim)
endif()

# --------------------------------------------------------------------------------------------------
# RPM build
# --------------------------------------------------------------------------------------------------
if(LNX_BUILD)
	set(RPMBUILD_DIR ${ROOT}/output/rpmbuild)

	if(EXISTS "/etc/redhat-release")
		set(LINUX_DIST rel)
	elseif(EXISTS "/etc/SuSE-release")
		set(LINUX_DIST sle)
	elseif(EXISTS "/etc/debian_version")
		set(LINUX_DIST debian)
	else()
		set(LINUX_DIST Unrecognized Linux distribution)
	endif()

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
		COMMAND ${CMAKE_COMMAND} -E copy ${ROOT}/install/linux/${LINUX_DIST}-release/*.spec ${RPMBUILD_DIR}/SPECS/invm-frameworks.spec

		# Update the spec file with build version
		COMMAND sed -i "s/^%define build_version .*/%define build_version ${BUILDNUM}/g" ${RPMBUILD_DIR}/SPECS/invm-frameworks.spec

		# Archive the directory
		COMMAND cd ${ROOT} && git archive --format=tar --prefix="invm-frameworks-${BUILDNUM}/" HEAD | gzip -c > ${RPMBUILD_DIR}/SOURCES/invm-frameworks-${BUILDNUM}.tar.gz

		# rpmbuild
		COMMAND export QA_RPATHS=0x0002 && cd ${ROOT} && rpmbuild -ba ${RPMBUILD_DIR}/SPECS/invm-frameworks.spec --define "_topdir ${RPMBUILD_DIR}" --define "cflag ${CMAKE_C_FLAGS}"
		COMMAND ${CMAKE_COMMAND} -E echo "Exiting make rpm..."
		)
endif()

# --------------------------------------------------------------------------------------------------
# Internal Includes
# --------------------------------------------------------------------------------------------------
if(EXISTS ${ROOT}/CMake/unit_test.cmake AND NOT EXTERNAL)
	#TODO: Unit tests only on linux
	if (NOT MSVC AND NOT ESX_BUILD)
		include(CMake/unit_test.cmake)
	endif()
endif()
