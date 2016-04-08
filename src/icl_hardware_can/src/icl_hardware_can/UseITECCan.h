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
#ifndef _icl_hardware_can_UseITECCan_h_
#define _icl_hardware_can_UseITECCan_h_

#ifdef _SYSTEM_LINUX_
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/ioctl.h>
# include <string.h>
# include <stdlib.h>
#endif

#ifdef _SYSTEM_LXRT_
# include "CAN-lxrt.h"
#else
# include "icl_hardware_can/UseCanNoLxrt.h"
#endif

#include "icl_hardware_can/tCanMessage.h"

namespace icl_hardware {
namespace can {

typedef int tCanDescriptor;

inline bool CanDescriptorValid(tCanDescriptor can_device)
{
  return can_device >= 0;
}

inline tCanDescriptor InvalidCanDescriptor()
{
  return -1;
}

inline const char* CanDriverName()
{
  return "MCA-CAN";
}

#ifdef _SYSTEM_LXRT_
//! supporting for LXRT available
inline bool CanDriverLxrtSupport()
{
  return true;
}
#endif

#define IOCTL_CAN_IOCRESET    0x01
#define IOCTL_CAN_SETBAUDRATE 0x02
#define IOCTL_CAN_IOCSETRMASK 0x03
#define IOCTL_CAN_SETSENDFIFOSIZE 0x04
#define IOCTL_CAN_SETRECEIVEFIFOSIZE 0x05
#define IOCTL_CAN_KNOCKNOCK 0x06

#ifdef _SYSTEM_LINUX_

//! Open a can device and set parameters
/*!
  Function for open a can device and setting the corresponding parameters via ioctl.
  \param device_name The linux device name of the can device (e.g. '/dev/can0')
  \param flags       Flags for the linux open() call (e.g. 'O_RDWR|O_NONBLOCK')
  \param acceptance_code The acceptance_code of the CAN-card
  \param acceptance_mask The acceptance_mask of the CAN-card
  \param baud_rate   Baud rate for can communication
  \param send_fifo_size Size of the can driver send fifo.
  \param receive_fifo_size Size of the can driver receive fifo.
  Returns the file descriptor of the can device on success, \c -1 on failure.

  Be aware: the CAN-card parameters (\a acceptance_code, \a acceptance_mask, \a baud_rate)
  are set globally for the CAN-card and so for all registered users so far!

  Be aware: If there are more than one FIFO-user,
  make sure all users call their read() function periodically.
  Otherwhise the FIFO is running full so that \c NO user is able to receive further data any more.

 */
inline tCanDescriptor CanDeviceOpen(const char *device_name, int flags,
                                    unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate,
                                    unsigned send_fifo_size, unsigned receive_fifo_size)
{
  int can_device = open(device_name, flags);
  if (can_device >= 0)
  {
    long value = receive_fifo_size;
	if (ioctl(can_device, IOCTL_CAN_KNOCKNOCK, value)<0)
	{
		close(can_device);
		return -1;
	}
    if (ioctl(can_device, IOCTL_CAN_SETRECEIVEFIFOSIZE, value) < 0)
    {
      close(can_device);
      return -1;
    }
    value = send_fifo_size;
    if (ioctl(can_device, IOCTL_CAN_SETSENDFIFOSIZE, value) < 0)
    {
      close(can_device);
      return -1;
    }
    value = acceptance_mask;
    value = ((value << 8)) | (acceptance_code);
    if (ioctl(can_device, IOCTL_CAN_IOCSETRMASK, value) < 0)
    {
      close(can_device);
      return -1;
    }
    value = baud_rate;
    if (ioctl(can_device, IOCTL_CAN_SETBAUDRATE, value) < 0)
    {
      close(can_device);
      return -1;
    }
  }
  return can_device;
}

//! Close an open can device
/*!
  Function to close an open can device.
  \param can_device file descriptor of the can device
  Returns the value from the linux-close() call
 */
inline int CanDeviceClose(tCanDescriptor can_device)
{
  return close(can_device);
}

inline int CanDeviceSend(tCanDescriptor can_device, const tCanMessage &msg)
{
  return write(can_device, &msg, sizeof(tCanMessage));
}

inline int CanDeviceReceive(tCanDescriptor can_device, tCanMessage &msg)
{
  return read(can_device, &msg, sizeof(tCanMessage));
}

//! Reset an open can device
/*!
  Function to reset an open can device.
  \param can_device file descriptor of the can device
  Returns the value from the linux-ioctl() call.
 */
inline int CanDeviceReset(tCanDescriptor can_device)
{
  long value = 0;
  return ioctl(can_device, IOCTL_CAN_IOCRESET, value);
}
#endif

}
}

#endif
