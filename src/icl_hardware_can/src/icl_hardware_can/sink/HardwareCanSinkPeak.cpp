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
 * \author  Hugo Ritzkowski <ritzkow@fzi.de>
 * \date    2011-04-06
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-18
 *
 */
//----------------------------------------------------------------------
#include "HardwareCanSinkPeak.h"

#include <icl_core_config/Config.h>
#include <icl_hardware_can/Logging.h>
#include <icl_core_thread/tThread.h>
#include <icl_sourcesink/SimpleURI.h>

namespace icl_hardware {
namespace can {

HardwareCanSinkPeak::HardwareCanSinkPeak(const std::string& uri, const std::string& name)
  : HardwareCanSink(uri, name),
    m_can_device()
{
  icl_sourcesink::SimpleURI parsed_uri(uri);

  uint32_t can_baudrate = 500;
  boost::optional<uint32_t> uri_baudrate = parsed_uri.getQuery<uint32_t>("baudrate");
  if (uri_baudrate)
  {
    can_baudrate = *uri_baudrate;
  }

  LOGGING_DEBUG(CAN, "Device: " << parsed_uri.path() << endl);
  LOGGING_DEBUG(CAN, "Baudrate: " << can_baudrate << " kbps" << endl);

  LOGGING_DEBUG(CAN, "Opening CAN-Device... " << endl);
  tCanDevice::CheckLXRTInterface();
  m_can_device.reset(tCanDevice::Create(parsed_uri.path().c_str(),
                                        O_RDWR | O_NONBLOCK,
                                        0xff,
                                        0xff,
                                        can_baudrate,
                                        300,
                                        8000));

  // Check if CAN device was initialized successfully.
  if (m_can_device->IsInitialized())
  {
    LOGGING_DEBUG(CAN, "CAN device successfully initialized." << endl);
  }
  else
  {
    m_can_device.reset();
    LOGGING_ERROR(CAN, "Error initializing CAN device." << endl);
    return;
  }
}

HardwareCanSinkPeak::~HardwareCanSinkPeak()
{ }

void HardwareCanSinkPeak::set(const CanMessageStamped::Ptr& msg)
{
  if (!m_can_device)
  {
    LOGGING_ERROR(CAN, "CAN device is not available, ignoring received message." << endl);
    return;
  }

  if (!msg || ((*msg)->id == 0 && (*msg)->dlc == 0 && (*msg)->rtr == 0))
  {
    LOGGING_WARNING(CAN, "No regular message received." << endl);
    return;
  }

  m_can_device->Send(**msg);
}

}
}
