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

#ifndef TCANDEVICEDUMMY_H
#define TCANDEVICEDUMMY_H

#include "icl_hardware_can/tCanDevice.h"
namespace icl_hardware {
namespace can {

class ICL_HARDWARE_CAN_IMPORT_EXPORT tCanDeviceDummy : public tCanDevice
{
public:
  tCanDeviceDummy (const char* device_name, int flags, unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate, unsigned int send_fifo_size, unsigned int receive_fifo_size);

  virtual ~tCanDeviceDummy();
  virtual int Send(const tCanMessage &msg);
  virtual int Receive(tCanMessage &msg);
  virtual void Reset();
  virtual bool IsInitialized();

  virtual tCanMessage getLastMessage() const;

  /*!
   * \brief Adds a response to a request that will be sent later. By default, the data won't bee seen
   * by the receive function, until a message is sent to the device. This way, one can set
   * a response, then send the request and afterwards the response can be read from the
   * device with the receive function.
   * This can be changed by setting the \a wait_for_send parameter to false
   *
   * \param msg Response message
   * \param wait_for_send If set to true (default), the response won't be received until a send call is made.
   */
  virtual void addResponse(const tCanMessage& msg, const bool wait_for_send = true);

protected:
  std::vector<tCanMessage> m_sent_messages;
  std::vector<tCanMessage> m_received_messages;

  bool m_receive_enabled;
};

}} // End of NS

#endif // TCANDEVICEDUMMY_H
