####################################################
# G4VoxelData Unit Tests 
#
# File:      CMakeLists.txt
#
# Author:    Christopher M Poole,
# Email:     mail@christopherpoole.net
#
# Date:      8th August, 2013
####################################################

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4VOXELDATA_TESTS)

# GEANT4 core
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/../include)

# User code
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/../include)

add_executable(tests tests.cc ${sources} ${headers})
target_link_libraries(tests gtest gtest_main)

