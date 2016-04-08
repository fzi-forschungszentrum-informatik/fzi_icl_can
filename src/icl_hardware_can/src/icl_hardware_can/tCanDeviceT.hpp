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
#ifndef _icl_hardware_can_tCanDeviceT_hpp_
#define _icl_hardware_can_tCanDeviceT_hpp_

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#ifndef _SYSTEM_WIN32_
# include <unistd.h>
#endif

#include <icl_core/os_lxrt.h>

#include "icl_hardware_can/tCanDeviceT.h"

#undef ICL_CORE_LOCAL_LOGGING
//#define ICL_CORE_LOCAL_LOGGING
#include "icl_hardware_can/Logging.h"

namespace icl_hardware {
namespace can {

template <typename TCanDescriptor>
tCanDeviceT<TCanDescriptor>::tCanDeviceT(const char *device_name, int flags,
    unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate,
    unsigned send_fifo_size, unsigned receive_fifo_size)
    : m_can_user(-1)
{
  m_can_device = InvalidCanDescriptor();
  bool success = false;
#ifdef _SYSTEM_LXRT_
  if (m_canlxrt_available && icl_core::os::IsThisLxrtTask() && CanDriverLxrtSupport())
  {
    char *check_pointer;
    const char *convert_string = device_name + strlen(device_name) - 1;
    m_can_device = (TCanDescriptor)strtol(convert_string, &check_pointer, 10);
    if (check_pointer == convert_string)
    {
      LOGGING_ERROR_CO(CAN, tCanDevice, CanDriverName(), "Error converting given device name '" << device_name << "' to LXRT-device id" << endl);
    }
    else
    {
      LOGGING_INFO_CO(CAN, tCanDevice, CanDriverName(), "Using LXRT extension for accessing can device " << (int)m_can_device << endl);
      m_can_user = CreateCanFifoUser(m_can_device, acceptance_code, acceptance_mask, baud_rate, send_fifo_size, receive_fifo_size);
      if (m_can_user >= 0)
      {
        success = true;
        LOGGING_INFO_CO(CAN, tCanDevice, CanDriverName(), "Can User: " << m_can_user << endl);
      }
      else
      {
       LOGGING_ERROR_CO(CAN, tCanDevice, CanDriverName(), "Error creating can fifo of LXRT-CAN-device " << (int)m_can_device << endl);
      }
    }
  }
  else
#endif
  {
    // Achtung, falls wir nicht lxrt-task sind oder kein lxrt can
    // interface zur Verf¸gung steht, wird der folgende code ausgef¸hrt:
    m_can_device = CanDeviceOpen(device_name, flags, acceptance_code, acceptance_mask, baud_rate, send_fifo_size, receive_fifo_size);
    if (CanDescriptorValid(m_can_device))
    {
      success = true;
      LOGGING_INFO_CO(CAN, tCanDevice, CanDriverName(), "Opened device " << device_name << " -> " << (uint64_t)m_can_device << endl);
    }
    else
    {
      LOGGING_ERROR_CO(CAN, tCanDevice, CanDriverName(), "Error open CAN-device '" << device_name << "' (errno=" << strerror(errno) << ")" << endl);
    }
  }

  if (!success)
    m_can_device = InvalidCanDescriptor();
}


template <typename TCanDescriptor>
tCanDeviceT<TCanDescriptor>::~tCanDeviceT()
{
  if (IsInitialized())
  {

#ifdef _SYSTEM_LXRT_
    if (m_can_user >= 0)
    {
      DestroyCanFifoUser(m_can_device, m_can_user);
      return;
    }
#endif

    CanDeviceClose(m_can_device);
  }
}

template <typename TCanDescriptor>
int tCanDeviceT<TCanDescriptor>::Send(const tCanMessage &msg)
{
#ifdef _SYSTEM_LXRT_
  if (m_can_user >= 0)
  {
    if (!icl_core::os::IsThisLxrtTask())
    {
      LOGGING_ERROR_CO(CAN, tCanDevice, CanDriverName(), "Send Using an LXRT CanDevice from a non LXRT task is not allowed" << endl);
    }
    else
    {
      return CanFifoSend(m_can_device, &msg);
    }
  }
#endif

  return CanDeviceSend(m_can_device, msg);
}

template <typename TCanDescriptor>
int tCanDeviceT<TCanDescriptor>::Receive(tCanMessage &msg)
{
#ifdef _SYSTEM_LXRT_
  if (m_can_user >= 0)
  {
    if (!icl_core::os::IsThisLxrtTask())
    {
     LOGGING_ERROR_CO(CAN, tCanDevice, CanDriverName(), "Receive Using an LXRT CanDevice from a non LXRT& task is not allowed" << endl);
    }
    else
    {
      return CanFifoReceive(m_can_device, m_can_user, &msg);
    }
  }
#endif

  return CanDeviceReceive(m_can_device, msg);
}

template <typename TCanDescriptor>
void tCanDeviceT<TCanDescriptor>::Reset()
{
#ifdef _SYSTEM_LXRT_
  if (m_can_user >= 0)
  {
    if (!icl_core::os::IsThisLxrtTask()) {
      LOGGING_ERROR_CO(CAN, tCanDevice, CanDriverName(), "Reset Using an LXRT CAN device from a non LXRT task is not allowed" << endl);
    }
    else
    {
      CanFifoReset(m_can_device);
    }
    return;
  }
#endif

  CanDeviceReset(m_can_device);
}

template <typename TCanDescriptor>
bool tCanDeviceT<TCanDescriptor>::IsInitialized()
{
  return CanDescriptorValid(m_can_device);
}

}
}

#endif
