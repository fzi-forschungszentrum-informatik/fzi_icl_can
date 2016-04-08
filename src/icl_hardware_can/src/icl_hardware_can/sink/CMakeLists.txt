# this is for emacs file handling -*- mode: cmake; indent-tabs-mode: nil -*-
ICMAKER_SET("icl_hardware_can_sink" IDE_FOLDER ${ICL_HARDWARE_IDE_FOLDER})

ICMAKER_ADD_SOURCES(
  # HardwareCanSink.cpp
  HardwareCanSinkCanfile.cpp
  # HardwareCanSinkFactory.cpp
  )

ICMAKER_ADD_HEADERS(
  ImportExport.h
  HardwareCanSinks.h
  HardwareCanSink.h
  HardwareCanSinkCanfile.h
  # HardwareCanSinkFactory.h
  )

IF(UNIX OR APPLE)
  ICMAKER_ADD_SOURCES(
    HardwareCanSinkPeak.cpp
    )

  ICMAKER_ADD_HEADERS(
    HardwareCanSinkPeak.h
    )
ENDIF()

ICMAKER_GLOBAL_CPPDEFINES(-D_IC_BUILDER_ICL_HARDWARE_CAN_SINK_)
ICMAKER_LOCAL_CPPDEFINES(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -DICL_HARDWARE_CAN_SINK_EXPORT_SYMBOLS)
ICMAKER_INCLUDE_DIRECTORIES(${ICL_HARDWARE_INCLUDE_DIRS})

ICMAKER_INTERNAL_DEPENDENCIES(EXPORT
  icl_core
  icl_core_config
  icl_core_logging
  icl_hardware_can
  icl_sourcesink
)

ICMAKER_INTERNAL_DEPENDENCIES(
  icl_core_thread
)

ICMAKER_EXTERNAL_DEPENDENCIES(
  Boost_FILESYSTEM
  Boost_DATE_TIME
)

ICMAKER_BUILD_LIBRARY()
ICMAKER_INSTALL_HEADERS("icl_hardware_can/sink")
