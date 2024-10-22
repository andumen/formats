project(formats-tests VERSION 1.0.0 LANGUAGES CXX)

find_package(formats REQUIRED)

set(FORMATS_TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

FILE(GLOB_RECURSE tests_files ${FORMATS_TEST_SOURCE_DIR} "*.hpp" "*.h" "*.cpp" "*.cc" "*.c")

add_executable(${PROJECT_NAME} ${tests_files})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "formats" output_name "${PROJECT_NAME}")

if(FORMATS_FOUND)
	include_directories(${FORMATS_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${FORMATS_LIBRARY})
endif()
