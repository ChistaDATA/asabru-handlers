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

# Set DEBUG preprocessor flag
add_compile_definitions(DEBUG)

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

include_directories(../libuv/include)
include_directories(../gzip-hpp/include)
include_directories(../nlohmann_json/single_include/nlohmann)
include_directories(../asabru-parsers/src)

# Set source directory
set(SOURCE_DIRECTORY "src")
# Collect all source files
file(GLOB_RECURSE PLUGIN_SOURCES "${SOURCE_DIRECTORY}/*.cpp" "${SOURCE_DIRECTORY}/**/*.cpp")
file(COPY src/python-handlers DESTINATION .)
# Add build directory
set(BUILD_DIRECTORY "build")

MACRO(HEADER_DIRECTORIES search_path return_list)
    FILE(GLOB_RECURSE new_list ${search_path})
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES("../asabru-commons/src/*.h" header_dir_list)
include_directories(${header_dir_list})
HEADER_DIRECTORIES("../asabru-engine/src/*.h" header_dir_list)
include_directories(${header_dir_list})

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
            ${CMAKE_SOURCE_DIR}/../libuv/build/libuv.a
            ${CMAKE_SOURCE_DIR}/../asabru-commons/build/lib/libasabru_commons.a
            ${CMAKE_SOURCE_DIR}/../asabru-engine/build/lib/libasabru_engine.a
            ${CMAKE_SOURCE_DIR}/../asabru-parsers/build/lib/libasabru_parsers.a
    )
endforeach (PLUGIN_SOURCE ${PLUGIN_SOURCES})