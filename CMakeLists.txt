cmake_minimum_required(VERSION 3.15)
project(asabru_handlers)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Add compiler warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall)
endif ()

# Find Python
find_package(Python COMPONENTS Interpreter Development)
if (Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
else()
    message(FATAL_ERROR "Python not found.")
endif()
include_directories(${Python_INCLUDE_DIRS})

include_directories(../asabru-engine/src/socket)
include_directories(../asabru-engine/src/interface)
include_directories(../libuv/include)
include_directories(../nlohmann_json/single_include/nlohmann)
include_directories(../asabru-commons/src/config)
include_directories(../asabru-commons/src)
include_directories(../asabru-commons/src/util)
include_directories(../asabru-parsers/src)

# Set source directory
set(SOURCE_DIRECTORY "src")
# Collect all source files
file(GLOB_RECURSE PLUGIN_SOURCES "${SOURCE_DIRECTORY}/*.cpp" "${SOURCE_DIRECTORY}/**/*.cpp")
file(COPY src/python-handlers DESTINATION .)
# Add build directory
set(BUILD_DIRECTORY "build")


foreach (PLUGIN_SOURCE ${PLUGIN_SOURCES})
    get_filename_component(FILE_NAME ${PLUGIN_SOURCE} NAME)
    string(REPLACE ".cpp" "" LIB_NAME ${FILE_NAME})
    set(LIB_NAMES ${LIB_NAMES} ${LIB_NAME})
    add_library(${LIB_NAME} SHARED ${PLUGIN_SOURCE})
    set_property(TARGET ${LIB_NAME}
            PROPERTY POSITION_INDEPENDENT_CODE ON
            PROPERTY CMAKE_BUILD_TYPE Debug
            )
    target_link_libraries(
            ${LIB_NAME}
            ${Python_LIBRARIES}
            ${CMAKE_SOURCE_DIR}/../libuv/build/libuv.a
            ${CMAKE_SOURCE_DIR}/../asabru-commons/build/lib/libasabru_commons.a
            ${CMAKE_SOURCE_DIR}/../asabru-engine/build/lib/libasabru_engine.a
            ${CMAKE_SOURCE_DIR}/../asabru-parsers/build/lib/libasabru_parsers.a
    )
endforeach (PLUGIN_SOURCE ${PLUGIN_SOURCES})