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
 * \date    2014-05-23
 *
 */
//----------------------------------------------------------------------
#include "HardwareCanFilterMessageMap.h"

namespace icl_hardware {
namespace can {

HardwareCanFilterMessageMap::HardwareCanFilterMessageMap(const std::string& uri, const std::string& name)
  : icl_sourcesink::DataFilter<tCanMessage, CanMessageMap>(uri, name)
{ }

HardwareCanFilterMessageMap::~HardwareCanFilterMessageMap()
{ }

void HardwareCanFilterMessageMap::set(const CanMessageStamped::Ptr& data)
{
  if (!data)
  {
    m_data.reset();
    return;
  }
  if (!m_data)
  {
    m_data.reset(new icl_core::Stamped<CanMessageMap>);
  }
  if (!filter(*data, *m_data))
  {
    m_data.reset();
  }
}

bool HardwareCanFilterMessageMap::filter(const icl_core::Stamped<tCanMessage>& input_data,
                                         icl_core::Stamped<CanMessageMap>& output_data)
{
  output_data.header() = input_data.header();
  (*output_data)[input_data->id] = *input_data;
  return true;
}

}
}
