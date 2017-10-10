cmake_minimum_required(VERSION 2.8.3)
project(fzi_icl_can)

find_package(icl_core REQUIRED COMPONENTS icl_core_config icl_core_logging icl_core)
find_package(tinyxml)
find_package(catkin REQUIRED COMPONENTS libpcan)

# include the icl_hardware_can workspace
SET(ICMAKER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icmaker")
INCLUDE(${ICMAKER_DIRECTORY}/CMakeLists.txt)
ADD_SUBDIRECTORY (src/icl_hardware_can)

# reset the project name as otherwise it would be IcMaker
project(fzi_icl_can)

# install script to install pcan-module
install(PROGRAMS install_pcan_module.sh DESTINATION lib/${PROJECT_NAME})

# install package.xml as we are a plain cmake package
install(FILES package.xml DESTINATION share/icl_hardware_can)
