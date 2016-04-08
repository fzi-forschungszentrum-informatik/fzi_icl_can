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

#include "icl_hardware_can/tCanDevice.h"

#include <icl_core/os_lxrt.h>

#include "icl_hardware_can/Logging.h"

#ifdef _IC_BUILDER_CAN_MCA_
# include "icl_hardware_can/tCanDeviceMCA.h"
namespace icl_hardware {
namespace can {
typedef tCanDeviceMCA tCanDeviceImpl;
}
}
#elif defined _IC_BUILDER_CAN_PEAK_
# include "icl_hardware_can/tCanDevicePeak.h"
namespace icl_hardware {
namespace can {
typedef tCanDevicePeak tCanDeviceImpl;
}
}
#elif defined _IC_BUILDER_CAN_ITEC_
# include "icl_hardware_can/tCanDeviceITEC.h"
namespace icl_hardware {
namespace can {
typedef tCanDeviceITEC tCanDeviceImpl;
}
}
#else
# include "icl_hardware_can/UseDummyCan.h"
# include "icl_hardware_can/tCanDeviceT.h"
namespace icl_hardware {
namespace can {
typedef tCanDeviceT<tCanDescriptor> tCanDeviceImpl;
}
}
#endif

namespace icl_hardware {
namespace can {

bool tCanDevice::m_canlxrt_available = false;

tCanDevice::~tCanDevice()
{
}

void tCanDevice::CheckLXRTInterface()
{
#ifdef _SYSTEM_LXRT_
  if (!icl_core::os::IsLxrtAvailable())
  {
    LOGGING_WARNING_C(CAN, tCanDevice, "LXRT not available!" << endl);
    return;
  }
  int user = CreateCanFifoUser(0, 0xff, 0xff, 1000, 500, 500);
  if (user == -ENOSYS)
  {
    LOGGING_WARNING_C(CAN, tCanDevice, "No LXRT CAN functions available (Probe failed)!" << endl);
    return;
  }
  else
  {
    DestroyCanFifoUser(0, user);
  }

  LLOGGING_INFO_C(CAN, tCanDevice, "CAN: LXRT interface available\n");
  m_canlxrt_available = true;
#endif
}

tCanDevice *tCanDevice::Create(const char *device_name, int flags,
                               unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate,
                               unsigned send_fifo_size, unsigned receive_fifo_size)
{
  return new tCanDeviceImpl(device_name, flags, acceptance_code, acceptance_mask, baud_rate, send_fifo_size, receive_fifo_size);
}

}
}
