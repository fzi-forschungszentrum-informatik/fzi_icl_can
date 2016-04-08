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
#include "icl_hardware_can/tCanMessage.h"

#include <string.h>

namespace icl_hardware {
namespace can {

tCanMessage::tCanMessage(unsigned param_id, unsigned char param_dlc,
                         unsigned char param_rtr, const unsigned char* param_data)
{
  // initialization of fill bytes
  memset(this, 0, sizeof(tCanMessage));

  id = param_id;
  dlc = param_dlc;
  rtr = param_rtr;
  memset(data, 0, sizeof(data));
  if (param_data)
    memcpy(data, param_data, sizeof(data));
#ifdef _IC_BUILDER_CAN_ITEC_
  eff = 0;
  tv.tv_sec=0;
  tv.tv_usec=0;
#endif
}

tCanMessage::tCanMessage(const tCanMessage &other)
{
  // initialization of fill bytes
  memset(this, 0, sizeof(tCanMessage));

  id=other.id;
  dlc=other.dlc;
  rtr=other.rtr;
  memcpy(data, other.data, sizeof(data));
#ifdef _IC_BUILDER_CAN_ITEC_
  eff = other.eff;
  tv = other.tv;
#endif
}

tCanMessage& tCanMessage::operator = (const tCanMessage &other)
{
  id=other.id;
  dlc=other.dlc;
  rtr=other.rtr;
  memcpy(data, other.data, sizeof(data));
#ifdef _IC_BUILDER_CAN_ITEC_
  eff = other.eff;
  tv = other.tv;
#endif
  return *this;
}

}
}
