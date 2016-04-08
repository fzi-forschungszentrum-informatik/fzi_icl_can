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
 * \date    2008-10-27
 *
 * \brief   Contains tCanDevice
 *
 * \b tCanDevice
 *
 * Class for accessing the can device.
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_T_CAN_DEVICE_T_H_INCLUDED
#define ICL_HARDWARE_CAN_T_CAN_DEVICE_T_H_INCLUDED

#include <stdlib.h>

#include "icl_hardware_can/tCanDevice.h"

namespace icl_hardware {
namespace can {

struct tCanMessage;

template <typename TCanDescriptor>
class tCanDeviceT : public tCanDevice
{
public:
  tCanDeviceT(const char *device_name, int flags,
              unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate,
              unsigned send_fifo_size, unsigned receive_fifo_size);

  virtual ~tCanDeviceT();
  virtual int Send(const tCanMessage &msg);
  virtual int Receive(tCanMessage &msg);
  virtual void Reset();

  virtual bool IsInitialized();

private:
  TCanDescriptor m_can_device;
  int m_can_user;
};

}
}

#include "icl_hardware_can/tCanDeviceT.hpp"

#endif
