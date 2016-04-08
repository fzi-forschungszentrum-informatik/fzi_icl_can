# this is for emacs file handling -*- mode: cmake; indent-tabs-mode: nil -*-

# -- BEGIN LICENSE BLOCK ----------------------------------------------
# -- END LICENSE BLOCK ------------------------------------------------

#----------------------------------------------------------------------
# \file
#
# \author  Jan Oberlaender <oberlaender@fzi.de>
# \date    2014-05-18
#
#----------------------------------------------------------------------
ICMAKER_SET("icl_hardware_can_filter" IDE_FOLDER ${ICL_HARDWARE_IDE_FOLDER})

ICMAKER_ADD_SOURCES(
  HardwareCanFilterInterpretMessage.cpp
  HardwareCanFilterMessageMap.cpp
  )

ICMAKER_ADD_HEADERS(
  ImportExport.h
  HardwareCanFilters.h
  HardwareCanFilterInterpretMessage.h
  HardwareCanFilterMessageMap.h
  )

ICMAKER_GLOBAL_CPPDEFINES(-D_IC_BUILDER_ICL_HARDWARE_CAN_FILTER_)
ICMAKER_LOCAL_CPPDEFINES(-DICL_HARDWARE_CAN_FILTER_EXPORT_SYMBOLS)
ICMAKER_INCLUDE_DIRECTORIES(${ICL_HARDWARE_INCLUDE_DIRS})

ICMAKER_INTERNAL_DEPENDENCIES(EXPORT
  icl_core
  icl_core_config
  icl_core_logging
  icl_hardware_can
  icl_sourcesink
)

ICMAKER_EXTERNAL_DEPENDENCIES(
  Boost_DATE_TIME
)

ICMAKER_BUILD_LIBRARY()
ICMAKER_INSTALL_HEADERS("icl_hardware_can/filter")
