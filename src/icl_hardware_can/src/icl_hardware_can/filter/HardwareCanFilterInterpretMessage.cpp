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
#include "HardwareCanFilterInterpretMessage.h"

#include <icl_core_config/Config.h>
#include <icl_sourcesink/SimpleURI.h>
#include "icl_hardware_can/CanMessageHelper.h"
#include "icl_hardware_can/Logging.h"

namespace icl_hardware {
namespace can {

HardwareCanFilterInterpretMessage::HardwareCanFilterInterpretMessage(const std::string &uri, const std::string &name)
  : icl_sourcesink::DataFilter<tCanMessage, InterpretedCanMessages>(uri, name),
    m_can_mask()
{
  icl_sourcesink::SimpleURI parsed_uri(uri);

  std::string can_mask = "";
  boost::optional<std::string> uri_can_mask = parsed_uri.getQuery<std::string>("can_mask");
  if (uri_can_mask)
  {
    can_mask = *uri_can_mask;
  }
  else
  {
    can_mask = icl_core::config::getDefault<std::string>("/icl_hardware_can/can_mask", "");
  }

  // Set CAN mask
  if (can_mask != "")
  {
    tCanMatrixParser parser(can_mask);
    if (parser.isActive())
    {
      m_can_mask = parser.getCanMatrix();
      LOGGING_DEBUG(CAN, "Using CAN Mask: " << can_mask << endl);
    }
    else
    {
      LOGGING_WARNING(CAN, "Could not use provided CAN mask " << can_mask << endl);
    }
  }
}

HardwareCanFilterInterpretMessage::~HardwareCanFilterInterpretMessage()
{ }

bool HardwareCanFilterInterpretMessage::filter(const icl_core::Stamped<tCanMessage>& input_msg,
                                               icl_core::Stamped<InterpretedCanMessages>& output_msg)
{
  if (input_msg->id == 0 && input_msg->dlc == 0 && input_msg->rtr == 0)
  {
    LOGGING_WARNING(CAN, "Received empty message." << endl);
    return false;
  }

  output_msg.header() = input_msg.header();
  output_msg->clear();

  CanMatrix::const_iterator mask_iter = m_can_mask.find(input_msg->id);
  if (mask_iter != m_can_mask.end())
  {
    for (std::size_t i = 0; i < mask_iter->second.size(); ++i)
    {
      double msg_value;
      parseCanMessage<double>(*input_msg,
                              mask_iter->second.at(i).start_bit,
                              mask_iter->second.at(i).signal_length,
                              mask_iter->second.at(i).conversion,
                              mask_iter->second.at(i).offset,
                              mask_iter->second.at(i).lower_border,
                              mask_iter->second.at(i).upper_border,
                              mask_iter->second.at(i).little_endian,
                              mask_iter->second.at(i).signedness,
                              msg_value);

      output_msg->push_back(InterpretedCanMessage(msg_value,
                                                  mask_iter->second.at(i).description,
                                                  mask_iter->second.at(i).unit));
    }
  }

  return true;
}

}
}
