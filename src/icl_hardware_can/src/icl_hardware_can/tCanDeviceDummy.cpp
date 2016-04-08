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
* \author  Felix Mauch <mauch@fzi.de>
* \date    2015-10-05
*
*/
//----------------------------------------------------------------------


#include <icl_core/os_lxrt.h>

#include "icl_hardware_can/Logging.h"

#include "icl_hardware_can/tCanDeviceDummy.h"

namespace icl_hardware {
namespace can {

tCanDeviceDummy::tCanDeviceDummy(const char* device_name,
                                 int flags,
                                 unsigned char acceptance_code,
                                 unsigned char acceptance_mask,
                                 unsigned int baud_rate,
                                 unsigned int send_fifo_size,
                                 unsigned int receive_fifo_size)
  : m_receive_enabled(false)
{
 LOGGING_DEBUG(CAN, "Dummy CAN adapter created" << endl);
}


tCanDeviceDummy::~tCanDeviceDummy()
{

}

int tCanDeviceDummy::Send(const tCanMessage& msg)
{
  LOGGING_TRACE(CAN, "Dummy CAN mesage sent..." << endl);
  m_sent_messages.push_back(msg);
  m_receive_enabled = true;
  return 0;
}

int tCanDeviceDummy::Receive(tCanMessage& msg)
{
  if (m_received_messages.size() > 0 && m_receive_enabled)
  {
    LOGGING_TRACE(CAN, "Receive method called..." << endl);
    msg = m_received_messages.back();
    m_received_messages.pop_back();
    return 1;
  }
  return 0;
}

void tCanDeviceDummy::Reset()
{

}


bool tCanDeviceDummy::IsInitialized()
{
  return true;
}

tCanMessage tCanDeviceDummy::getLastMessage() const
{
  if (m_sent_messages.size() > 0)
  {
    return m_sent_messages.back();
  }
  else
  {
    LOGGING_ERROR (CAN, "No sent messages in cache!" << endl);
  }
  return tCanMessage();
}

void tCanDeviceDummy::addResponse(const tCanMessage& msg, const bool wait_for_send)
{
  LOGGING_TRACE(CAN, "Added response message..." << endl);
  m_receive_enabled = !wait_for_send;
  m_received_messages.push_back(msg);
}



}
}
