# this is for emacs file handling -*- mode: cmake; indent-tabs-mode: nil -*-
ICMAKER_SET("icl_hardware_can" IDE_FOLDER ${ICL_HARDWARE_IDE_FOLDER})

ICMAKER_ADD_SOURCES(
  Logging.cpp
  tCanDevice.cpp
  tCanDeviceDummy.cpp
  tCanFilterMessages.cpp
  tCanMatrixParser.cpp
  tCanMessageMetaDumper.cpp
  tCanMessage.cpp
  InterpretedCanMessage.cpp
  StreamOperators.cpp
)

ICMAKER_ADD_HEADERS(
  ImportExport.h
  Logging.h
  tCanDevice.h
  tCanDeviceDummy.h
  tCanDeviceITEC.h
  tCanDeviceMCA.h
  tCanDevicePeak.h
  tCanDeviceT.h
  tCanDeviceT.hpp
  tCanFilterMessages.h
  tCanMatrixParser.h
  tCanMessageMetaDumper.h
  tCanMessage.h
  tException.h
  CanMessageHelper.h
  InterpretedCanMessage.h
  UseCanNoLxrt.h
  UseDummyCan.h
  UseITECCan.h
  UseMCACan.h
  UsePeakCan.h
  StreamOperators.h
)

ICMAKER_LOCAL_CPPDEFINES(-DICL_HARDWARE_CAN_EXPORT_SYMBOLS)
ICMAKER_GLOBAL_CPPDEFINES(-D_IC_BUILDER_ICL_HARDWARE_CAN_)
ICMAKER_INCLUDE_DIRECTORIES(${ICL_HARDWARE_INCLUDE_DIRS})

ICMAKER_EXTERNAL_DEPENDENCIES(OPTIONAL EXPORT
  PeakCan
)

ICMAKER_INTERNAL_DEPENDENCIES(
  icl_core
  icl_core_config
  icl_core_logging
  tinyxml
)

ICMAKER_EXTERNAL_DEPENDENCIES(OPTIONAL
  Cantools
)

ICMAKER_BUILD_LIBRARY()
ICMAKER_INSTALL_HEADERS(${icmaker_target})

ADD_SUBDIRECTORY(sink)
ADD_SUBDIRECTORY(source)
ADD_SUBDIRECTORY(filter)
