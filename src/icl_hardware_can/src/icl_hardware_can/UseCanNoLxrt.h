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
 * \brief   Contains CAN driver interface functions
 *
 *
 */
//----------------------------------------------------------------------
#ifndef _icl_hardware_can_UseCanNoLxrt_h_
#define _icl_hardware_can_UseCanNoLxrt_h_

#include "icl_hardware_can/tCanMessage.h"

namespace icl_hardware {
namespace can {

//! no support for LXRT available
inline bool CanDriverLxrtSupport()
{
  return false;
}

//! Creates a can FIFO in kernel space
inline int CreateCanFifoUser(int device_id,
                             unsigned acceptance_code, unsigned acceptance_mask, unsigned int baud_rate,
                             unsigned send_fifo_size, unsigned receive_fifo_size)
{
  errno = ENOSYS;
  return -1;
}

//! Destroys a can FIFO in kernel space
inline int DestroyCanFifoUser(int device_id, int user_id)
{
  errno = ENOSYS;
  return -1;
}

//! Send a message via CAN-FIFO
inline int CanFifoSend(int device_id, const tCanMessage *msg)
{
  errno = ENOSYS;
  return -1;
}

//! Receive a message via CAN-FIFO
inline int CanFifoReceive(int device_id, int user, tCanMessage *msg)
{
  errno = ENOSYS;
  return -1;
}


//! Reset a CAN-device
inline int CanFifoReset(int device_id)
{
  errno = ENOSYS;
  return -1;
}

}
}

#endif
