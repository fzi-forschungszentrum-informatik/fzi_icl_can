# this is for emacs file handling -*- mode: cmake; indent-tabs-mode: nil -*-

# To be used by other modules
SET(ICL_HARDWARE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE INTERNAL "")
SET(ICL_HARDWARE_IDE_FOLDER "icl_hardware_can")


FIND_PACKAGE(Cantools)
FIND_PACKAGE(PeakCan)

ICMAKER_REGISTER_PACKAGE(icl_hardware_can)

ADD_SUBDIRECTORY(src/icl_hardware_can)

ICMAKER_CONFIGURE_PACKAGE()
