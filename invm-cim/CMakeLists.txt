cmake_minimum_required(VERSION 2.8.11)
project(intel_cim_framework)

get_filename_component(ROOT ./.. ABSOLUTE)

set(OUTPUT_PATH output/build/${OS_TYPE}/${BUILD_TYPE})
set(OUTPUT_DIR ${ROOT}/${OUTPUT_PATH})
set(INCLUDE_DIR ${OUTPUT_DIR}/include)
set(HEADER_DIR ${INCLUDE_DIR}/libinvm-cim)

set(CMAKE_BINARY_DIR ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# --------------------------------------------------------------------------------------------------
# libinvm-cim
# --------------------------------------------------------------------------------------------------
file(GLOB CIMFRAMEWORK_HEADERS
	"src/framework/*.h"
	"src/cimom/*.h"
	"src/common/logger/*.h"
	"src/common/string/*.h"
	"src/common/time/*.h"
	"src/common/*.h"
	)

if (WIN_BUILD)
	file(GLOB OS_HDR_FILES "src/cimom/wmi/*.h")
else ()
	file(GLOB OS_HDR_FILES "src/cimom/cmpi/*.h")
endif ()

list (APPEND CIMFRAMEWORK_HEADERS ${OS_HDR_FILES})

file(COPY ${CIMFRAMEWORK_HEADERS}
	DESTINATION ${HEADER_DIR})

FILE(GLOB CIMFRAMEWORK_SRC
	src/cimom/*
	src/common/logger/*
	src/common/string/*
	src/common/time/*
	examples/demoprovider/os/*
	src/common/*
	src/framework/*
	)

if (WIN_BUILD)
	file(GLOB OS_SRC_FILES src/cimom/wmi/*)
else ()
	file(GLOB OS_SRC_FILES src/cimom/cmpi/*)

endif ()

list (APPEND CIMFRAMEWORK_SRC ${OS_SRC_FILES})

add_library(invm-cim ${LIB_TYPE} ${CIMFRAMEWORK_SRC})

set_target_properties(invm-cim
       PROPERTIES
       VERSION 1.0.0
       SOVERSION 1
       )

target_compile_definitions(invm-cim
        PRIVATE -D_FORTIFY_SOURCE=2
        PRIVATE -D_XOPEN_SOURCE=500
        PRIVATE -D__WBEM_PREFIX__=Intel_
        PRIVATE -D__ADD_MANUFACTURING__=1
        PRIVATE -DPOCO_STATIC
        PRIVATE -D_GNU_SOURCE
        PRIVATE -D__PRODUCT_SHAREDSTATE_DIR__=""
        PRIVATE -DCMPI_PLATFORM_LINUX_GENERIC_GNU=1
        PRIVATE -DCMPI_VER_86=1
)

target_include_directories(invm-cim PUBLIC
        src/cimom
        src/cimom/cmpi
        src/framework
        src/common
        src/common/logger
        src/common/string
        src/common/time
        ${ROOT}/external/invm-cim/rapidxml-1.13
        ${ROOT}/external/invm-cim/cmpi/include
        ${ROOT}/external/invm-cim/cmpi/include/cmpi
        )
target_compile_options(invm-cim PRIVATE -O)
