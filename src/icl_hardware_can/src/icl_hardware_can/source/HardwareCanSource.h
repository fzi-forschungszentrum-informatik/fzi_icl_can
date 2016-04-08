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
 * \date    2011-01-26
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-12
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_H_INCLUDED
#define ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_H_INCLUDED

#include <icl_sourcesink/SourceSinkManager.h>
#include <icl_sourcesink/DataSourceRegistration.h>

#include <icl_hardware_can/tCanMessage.h>
#include <icl_hardware_can/StreamOperators.h>

namespace icl_hardware {
namespace can {

//! Base type for all sources providing tCanMessage data.
typedef icl_sourcesink::DataSource<tCanMessage> HardwareCanSource;

}
}

#endif
