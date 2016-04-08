// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

// -- BEGIN LICENSE BLOCK ----------------------------------------------
// This file is part of FZIs ic_workspace.
//
// This program is free software licensed under the LGPL
// (GNU LESSER GENERAL PUBLIC LICENSE Version 3).
// You can find a copy of this license in LICENSE folder in the top
// directory of the source code.
//
// © Copyright 2016 FZI Forschungszentrum Informatik, Karlsruhe, Germany
//
// -- END LICENSE BLOCK ------------------------------------------------

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Hugo Ritzkowski <ritzkow@fzi.de>
 * \date    2010-12-22
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-16
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SINK_HARDWARE_CAN_SINK_H_INCLUDED
#define ICL_HARDWARE_CAN_SINK_HARDWARE_CAN_SINK_H_INCLUDED

#include <icl_sourcesink/SourceSinkManager.h>
#include <icl_sourcesink/DataSink.h>
#include <icl_sourcesink/ConsoleSink.h>

#include <icl_hardware_can/source/HardwareCanSource.h>

namespace icl_hardware {
namespace can {

//! Base type for all sinks providing tCanMessage data.
typedef icl_sourcesink::DataSink<tCanMessage> HardwareCanSink;

//! ConsoleSink for CAN message data.
SOURCESINK_DECLARE_CONSOLE_SINK(HardwareCanSinkConsole, tCanMessage, "can+console")

}
}

#endif
