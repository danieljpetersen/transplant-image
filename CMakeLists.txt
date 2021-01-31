cmake_minimum_required(VERSION 3.6)
project(transplant_image)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FILE(GLOB_RECURSE SRCFILES source/*h source/*.cpp source/*.c source/*.cc source/*.hh)
add_executable(transplant_image ${SRCFILES})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(SFML 2.4.0 REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(transplant_image ${SFML_LIBRARIES})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")
set_target_properties(transplant_image PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Release")
target_compile_features(transplant_image PRIVATE cxx_range_for)