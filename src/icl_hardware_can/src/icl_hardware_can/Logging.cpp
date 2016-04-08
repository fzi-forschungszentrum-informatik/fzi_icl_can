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
 * \author  Klaus Uhl <uhl@fzi.de>
 * \date   2008-10-27
 *
 */
//----------------------------------------------------------------------
#include "icl_hardware_can/Logging.h"

namespace icl_hardware {
namespace can {

REGISTER_LOG_STREAM_OPERATOR( boost::posix_time::ptime)
REGISTER_LOG_STREAM(CAN)

}
}
