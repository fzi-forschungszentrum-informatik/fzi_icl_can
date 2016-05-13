cmake_minimum_required(VERSION 2.8.3)
project(fzi_icl_can)

find_package(icl_core REQUIRED COMPONENTS icl_core_config icl_core_logging icl_core)
find_package(tinyxml)

# The IC_MAKER requests PeakCan, we help it to find the ROS package version
# That's why we copied those two header files in there...
set(PeakCan_ROOT ${PROJECT_SOURCE_DIR}/common)

# Build targets for the pcan library
add_custom_target(
    build_libpcan ALL
    COMMAND cmake -E chdir ${PROJECT_SOURCE_DIR} make -f Makefile.tarball
)
add_custom_target(
    copy_pcan ALL
    COMMAND cmake -E copy ${PROJECT_SOURCE_DIR}/common/lib/libpcan.so lib
)
add_custom_target(
    copy_pcan0 ALL
    COMMAND cmake -E copy ${PROJECT_SOURCE_DIR}/common/lib/libpcan.so.0 lib
)
add_custom_target(
    copy_pcan06 ALL
    COMMAND cmake -E copy ${PROJECT_SOURCE_DIR}/common/lib/libpcan.so.0.6 lib
)
message(STATUS "Copying libs to ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_LIB_DESTINATION}")

# include the icl_hardware_can workspace
SET(ICMAKER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icmaker")
INCLUDE(${ICMAKER_DIRECTORY}/CMakeLists.txt)
ADD_SUBDIRECTORY (src/icl_hardware_can)

# reset the project name as otherwise it would be IcMaker
project(fzi_icl_can)

# model dependencies
add_dependencies(copy_pcan build_libpcan)
add_dependencies(copy_pcan0 build_libpcan)
add_dependencies(copy_pcan06 build_libpcan)
add_dependencies(icl_hardware_can copy_pcan06)

# install targets for libpcan
install(PROGRAMS common/lib/libpcan.so common/lib/libpcan.so.0 common/lib/libpcan.so.0.6
  DESTINATION lib
)
install(DIRECTORY common/include/libpcan/
  DESTINATION lib/${PROJECT_NAME}
)

# install script to install pcan-module
install(PROGRAMS install_pcan_module.sh DESTINATION lib/${PROJECT_NAME})

# install package.xml as we are a plain cmake package
install(FILES package.xml DESTINATION share/icl_hardware_can)