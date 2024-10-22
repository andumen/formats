cmake_minimum_required(VERSION 3.15)

message(STATUS "BUILD PROJECT FORMATS...")

SET(FORMATS_INCLDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(FORMATS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(FORMATS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(${FORMATS_CMAKE_DIR}/auto_source_group.cmake)
include(${FORMATS_CMAKE_DIR}/compile_option.cmake)

SET(FORMATS_MESSAGE "Formats: ")

if(POLICY CMP0091)
    message(STATUS "${FORMATS_MESSAGE} SET CMP0091 NEW")
	cmake_policy(SET CMP0091 NEW)
endif()

project(formats VERSION 1.0.0 LANGUAGES CXX)
SET_COMPILE_OPTION()

FILE(GLOB_RECURSE build_files LIST_DIRECTORIES false ${FORMATS_SOURCE_DIR} "formats/*.hpp" "formats/*.h" "formats/*.cpp" "formats/*.cc" "formats/*.c")

SET_SOURCE_GROUP(${build_files})

add_library(${PROJECT_NAME} STATIC ${build_files})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "formats" output_name "${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")

target_include_directories(${PROJECT_NAME} PUBLIC ${FORMATS_INCLDE_DIR})

if("${CMAKE_GENERATOR}" STREQUAL "Xcode")
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
endif()
 
if(WITH_TESTS)
	list(APPEND CMAKE_MODULE_PATH "${FORMATS_CMAKE_DIR}")
	add_subdirectory(tests)
endif()
