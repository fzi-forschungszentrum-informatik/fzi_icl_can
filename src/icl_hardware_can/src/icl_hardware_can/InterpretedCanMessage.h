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
 * \date    2014-05-18
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_INTERPRETED_CAN_MESSAGE_H_INCLUDED
#define ICL_HARDWARE_CAN_INTERPRETED_CAN_MESSAGE_H_INCLUDED

#include <string>
#include <vector>
#include <icl_hardware_can/ImportExport.h>

namespace icl_hardware {
namespace can {

/*! Result of interpreting a CAN message.  Stores a floating-point
 *  value along with a unit and a description.
 */
struct ICL_HARDWARE_CAN_IMPORT_EXPORT InterpretedCanMessage
{
  double value;
  std::string description;
  std::string unit;

  //! Constructor.
  InterpretedCanMessage(const double value = 0,
                        const std::string& description = "",
                        const std::string& unit = "");
};

typedef std::vector<InterpretedCanMessage> InterpretedCanMessages;

ICL_HARDWARE_CAN_IMPORT_EXPORT std::ostream& operator<<(std::ostream& os, InterpretedCanMessages const& interpreted_can_messages);

}
}

#endif
