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
 * \date    2015-02-23
 *
 */
//----------------------------------------------------------------------
#include "InterpretedCanMessage.h"

#include <boost/foreach.hpp>

namespace icl_hardware {
namespace can {

InterpretedCanMessage::InterpretedCanMessage(const double value,
                                             const std::string& description,
                                             const std::string& unit)
  : value(value),
    description(description),
    unit(unit)
{ }

std::ostream& operator<<(std::ostream& os, InterpretedCanMessages const& interpreted_can_messages)
{
  os << "Vector of interpreted CAN messages with " << interpreted_can_messages.size() << " messages.";

  BOOST_FOREACH(InterpretedCanMessage msg, interpreted_can_messages)
  {
    os << " -" << msg.description << "-" << msg.value << "-" << msg.unit << "- ";
  }

  return os;
}

}
}
