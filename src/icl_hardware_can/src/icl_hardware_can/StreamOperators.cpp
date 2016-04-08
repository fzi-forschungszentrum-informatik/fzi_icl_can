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
#include "StreamOperators.h"

#include <iomanip>

namespace icl_hardware {
namespace can {

std::ostream& operator << (std::ostream& os, const tCanMessage& msg)
{
  return os << "ID: " << std::dec << msg.id << " - DLC: " << unsigned(msg.dlc) << " - Data:"
            << std::hex << std::setfill('0')
            << " " << std::setw(2) << unsigned(msg.data[0])
            << " " << std::setw(2) << unsigned(msg.data[1])
            << " " << std::setw(2) << unsigned(msg.data[2])
            << " " << std::setw(2) << unsigned(msg.data[3])
            << " " << std::setw(2) << unsigned(msg.data[4])
            << " " << std::setw(2) << unsigned(msg.data[5])
            << " " << std::setw(2) << unsigned(msg.data[6])
            << " " << std::setw(2) << unsigned(msg.data[7])
            << std::dec;
}

}
}
