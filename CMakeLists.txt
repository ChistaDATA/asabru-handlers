cmake_minimum_required(VERSION 3.15)
project(asabru_handlers)
# Enables generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Add compiler warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall)
endif ()

# Set DEBUG preprocessor flag
add_compile_definitions(DEBUG)

# add dependencies
include(cmake/CPM.cmake)

# Find zlib
find_package(ZLIB)

# Find Python
find_package(Python COMPONENTS Interpreter Development)
if (Python_FOUND)
    message(STATUS "Python found: ${Python_EXECUTABLE}")
else()
    message(FATAL_ERROR "Python not found.")
endif()
include_directories(${Python_INCLUDE_DIRS})

# Find OpenSSL library
find_package(OpenSSL REQUIRED)

# Find SqlLite3
find_package (SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

CPMAddPackage(
        NAME gzip_hpp
        GITHUB_REPOSITORY mapbox/gzip-hpp
        GIT_TAG 7546b35
        DOWNLOAD_ONLY YES
)

if (ASABRU_COMMONS_BUILD STREQUAL "GIT_TAG")
    CPMAddPackage(
            NAME asabru_commons
            GITHUB_REPOSITORY ChistaDATA/asabru-commons
            GIT_TAG ${ASABRU_COMMONS_GIT_TAG}
    )
elseif (ASABRU_COMMONS_BUILD STREQUAL "LOCAL_DIR")
    # For development purpose
    CPMAddPackage(
            NAME asabru_commons
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../asabru-commons/src
    )
else()
    CPMAddPackage(
            NAME asabru_commons
            GITHUB_REPOSITORY ChistaDATA/asabru-commons
            VERSION 0.0.2
    )
endif()

if (ASABRU_ENGINE_BUILD STREQUAL "GIT_TAG")
    CPMAddPackage(
            NAME asabru_engine
            GITHUB_REPOSITORY ChistaDATA/asabru-engine
            GIT_TAG ${ASABRU_ENGINE_GIT_TAG}
    )
elseif (ASABRU_ENGINE_BUILD STREQUAL "LOCAL_DIR")
    # For development purpose
    CPMAddPackage(
            NAME asabru_engine
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../asabru-engine/src
    )
else()
    CPMAddPackage(
            NAME asabru_engine
            GITHUB_REPOSITORY ChistaDATA/asabru-engine
            VERSION 0.0.2
    )
endif()

if (ASABRU_PARSERS_BUILD STREQUAL "GIT_TAG")
    CPMAddPackage(
            NAME asabru_parsers
            GITHUB_REPOSITORY ChistaDATA/asabru-parsers
            GIT_TAG ${ASABRU_PARSERS_GIT_TAG}
    )
elseif (ASABRU_PARSERS_BUILD STREQUAL "LOCAL_DIR")
    # For development purpose
    CPMAddPackage(
            NAME asabru_parsers
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../asabru-parsers/src
    )
else()
    CPMAddPackage(
            NAME asabru_parsers
            GITHUB_REPOSITORY ChistaDATA/asabru-parsers
            VERSION 0.0.1
    )
endif()

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
            PUBLIC ${Python_LIBRARIES}
            PUBLIC ${SQLite3_LIBRARIES}
            PUBLIC ${ZLIB_LIBRARIES}
            uv
            asabru_commons
            asabru_engine
            asabru_parsers
    )

    target_include_directories(${LIB_NAME}
            PUBLIC ${asabru_engine_SOURCE_DIR}/src
            PUBLIC ${asabru_parsers_SOURCE_DIR}/src
            PUBLIC ${gzip_hpp_SOURCE_DIR}/include
    )
endforeach (PLUGIN_SOURCE ${PLUGIN_SOURCES})
