cmake_minimum_required(VERSION 2.8.4)
project(intel_cli_framework)

# Directories ...
get_filename_component(ROOT ./.. ABSOLUTE)

set(OUTPUT_PATH output/build/${OS_TYPE}/${BUILD_TYPE})
set(OUTPUT_DIR ${ROOT}/${OUTPUT_PATH})
set(INCLUDE_DIR ${OUTPUT_DIR}/include)
set(HEADER_DIR ${INCLUDE_DIR}/libinvm-cli)

set(CMAKE_BINARY_DIR ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Dependencies
option(USE_INSTALLED_FRAMEWORK "Use installed version of framework libraries instead of project output" OFF)
set(I18N_INCLUDE_DIR ${ROOT}/${OUTPUT_PATH}/include)

if(USE_INSTALLED_FRAMEWORK)
    find_library(invm-i18n NAMES invm-i18n)
else()
    find_library(invm-i18n NO_DEFAULT_PATH NAMES invm-i18n PATHS ${ROOT}/${OUTPUT_PATH})
endif()

# --------------------------------------------------------------------------------------------------
# Project wide defines and flags
# --------------------------------------------------------------------------------------------------
add_definitions(-D__INTEL_I18N__) # use intel i18n library

# --------------------------------------------------------------------------------------------------
# invm-cli
# --------------------------------------------------------------------------------------------------
file(GLOB CLIFRAMEWORK_HEADERS
        "src/framework/*.h"
        )
file(COPY ${CLIFRAMEWORK_HEADERS}
        DESTINATION ${HEADER_DIR})

file(GLOB SRC src/framework/*.cpp)

add_library(invm-cli ${LIB_TYPE} ${SRC})

target_compile_definitions(invm-cli
        PRIVATE -D_FORTIFY_SOURCE=2
        PRIVATE -D_XOPEN_SOURCE=500
)

# this will create the symlinks on Linux
set_target_properties(invm-cli
       PROPERTIES
       VERSION 1.0.0
       SOVERSION 1
       )

if(WIN_BUILD)
    target_link_libraries(invm-cli
            ws2_32
            mswsock)
elseif(LNX_BUILD)
    target_link_libraries(invm-cli
            dl)
else()
    target_link_libraries(invm-cli
            dl m)
endif()

target_link_libraries(invm-cli invm-i18n)
target_include_directories(invm-cli PUBLIC ${I18N_INCLUDE_DIR})
target_compile_options(invm-cli PRIVATE -O)
