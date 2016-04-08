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
 * \author  Alexander Bierbaum
 * \author  Klaus Uhl <uhl@fzi.de>
 * \date    2007-10-01
 *
 * \brief   Contains dummy CAN driver interface functions.
 *
 */
//----------------------------------------------------------------------
#ifndef _icl_hardware_can_UseDummyCan_h_
#define _icl_hardware_can_UseDummyCan_h_

#include "icl_hardware_can/Logging.h"
#include "icl_hardware_can/tCanMessage.h"

namespace icl_hardware {
namespace can {

typedef int tCanDescriptor;
static tCanDescriptor no_can_instances = 0;

inline bool CanDescriptorValid(tCanDescriptor can_device)
{
  return can_device != 0;
}

inline tCanDescriptor InvalidCanDescriptor()
{
  return 0;
}

inline const char* CanDriverName()
{
  return "DummyCAN";
}

inline tCanDescriptor CanDeviceOpen(const char* /*device_name*/, int /*flags*/,
                                    unsigned char /*acceptance_code*/, unsigned char /*acceptance_mask*/, unsigned int /*baud_rate*/,
                                    unsigned /*send_fifo_size*/, unsigned /*receive_fifo_size*/)
{
  tCanDescriptor can_device = no_can_instances++;
  LOGGING_INFO(CAN, "Opening DummyCAN (" << can_device << ")" << endl);
  return can_device;
}

inline int CanDeviceClose(tCanDescriptor /*_can_device*/)
{
  return 0;
}

inline int CanDeviceSend(tCanDescriptor /*_can_device*/, const tCanMessage& /*msg*/)
{
   return 0;
}

inline int CanDeviceReceive(tCanDescriptor /*_can_device*/, tCanMessage& /*msg*/)
{
  return 0;
}

inline int CanDeviceReset(tCanDescriptor /*_can_device*/)
{
  return 0;
}

}
}

#endif
