# this is for emacs file handling -*- mode: cmake; indent-tabs-mode: nil -*-
ICMAKER_SET("icl_hardware_can_source" IDE_FOLDER ${ICL_HARDWARE_IDE_FOLDER})

ICMAKER_ADD_SOURCES(
  CanFileResolutionHelper.cpp
  HardwareCanSourceCanfile.cpp
  HardwareCanSourceTest.cpp
  )


ICMAKER_ADD_HEADERS(
  ImportExport.h
  CanFileResolutionHelper.h
  HardwareCanSources.h
  HardwareCanSource.h
  HardwareCanSourceCanfile.h
  HardwareCanSourceTest.h
  )

IF(UNIX OR APPLE)
  ICMAKER_ADD_SOURCES(
    HardwareCanSourcePeak.cpp
    )

  ICMAKER_ADD_HEADERS(
    HardwareCanSourcePeak.h
    )
ENDIF()

ICMAKER_GLOBAL_CPPDEFINES(-D_IC_BUILDER_ICL_HARDWARE_CAN_SOURCE_)
ICMAKER_LOCAL_CPPDEFINES(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DICL_HARDWARE_CAN_SOURCE_EXPORT_SYMBOLS)
ICMAKER_INCLUDE_DIRECTORIES(${ICL_HARDWARE_INCLUDE_DIRS})


ICMAKER_INTERNAL_DEPENDENCIES(EXPORT
  icl_core
  icl_core_config
  icl_core_logging
  icl_core_thread
  icl_math
  icl_sourcesink
  icl_hardware_can
  tinyxml
  )

ICMAKER_INTERNAL_DEPENDENCIES(OPTIONAL
  chassis
  icl_saalem
  )

ICMAKER_EXTERNAL_DEPENDENCIES(
  Boost_FILESYSTEM
  Boost_DATE_TIME
  )

ICMAKER_BUILD_LIBRARY()
ICMAKER_INSTALL_HEADERS("icl_hardware_can/source")
