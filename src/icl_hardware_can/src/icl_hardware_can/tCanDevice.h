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
 * \author  Jan Oberländer <oberlaen@fzi.de>
 * \date    2011-12-01
 * \author  Klaus Uhl <uhl@fzi.de>
 * \date    2008-10-27
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_T_CAN_DEVICE_H_INCLUDED
#define ICL_HARDWARE_CAN_T_CAN_DEVICE_H_INCLUDED

#include <fcntl.h>
#include <stdlib.h>
#ifndef _SYSTEM_WIN32_
# include <unistd.h>
#endif

#include "icl_hardware_can/tCanMessage.h"

namespace icl_hardware {
namespace can {

class ICL_HARDWARE_CAN_IMPORT_EXPORT tCanDevice
{
public:
  static tCanDevice *Create(const char *device_name, int flags,
                            unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate,
                            unsigned send_fifo_size, unsigned receive_fifo_size);

  virtual ~tCanDevice();
  virtual int Send(const tCanMessage &msg) = 0;
  virtual int Receive(tCanMessage &msg) = 0;
  virtual void Reset() = 0;
  virtual bool IsInitialized() = 0;

  static void CheckLXRTInterface();

protected:
  static bool m_canlxrt_available;
};

}
}

#endif
