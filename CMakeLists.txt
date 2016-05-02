cmake_minimum_required(VERSION 2.8.3)
project(fzi_icl_can)

find_package(icl_core REQUIRED COMPONENTS icl_core_config icl_core_logging icl_core)
find_package(tinyxml)

SET(ICMAKER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icmaker")
INCLUDE(${ICMAKER_DIRECTORY}/CMakeLists.txt)


ADD_SUBDIRECTORY (src/icl_hardware_can)

