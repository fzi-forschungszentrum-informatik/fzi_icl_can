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
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-16
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_STREAM_OPERATORS_H_INCLUDED
#define ICL_HARDWARE_CAN_STREAM_OPERATORS_H_INCLUDED

#include <iostream>
#include <icl_hardware_can/tCanMessage.h>

namespace icl_hardware {
namespace can {

ICL_HARDWARE_CAN_IMPORT_EXPORT
std::ostream& operator << (std::ostream& os, const tCanMessage& msg);

}
}

#endif
