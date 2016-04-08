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
 * \date    2011-01-26
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-01-15
 *
 * \todo At the moment there is no real thread synchronization.
 *       Resource managment is up to the OS.  Main idea: main threads
 *       wait while there is no message in the array, when messages
 *       are received and none are incoming, the producer thread is
 *       put to wait while the main thread consumes these.
 */
//----------------------------------------------------------------------
#include "HardwareCanSourcePeak.h"

#include <deque>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include "icl_hardware_can/Logging.h"
#include "icl_hardware_can/tException.h"

#include <icl_core_thread/Thread.h>
#include <icl_core_thread/Mutex.h>
#include <icl_core_thread/ScopedMutexLock.h>
#include <icl_core_thread/Sem.h>
#include <icl_core_config/Config.h>
#include <icl_sourcesink/SimpleURI.h>

namespace icl_hardware {
namespace can {

/*! Internal worker thread which listens to a given CAN device and
 *  buffers the received messages.
 */
class HardwareCanSourcePeak::WorkerThread : public icl_core::thread::Thread
{
public:
  WorkerThread(tCanDevice *can_device, std::size_t max_buffer_size = 256)
    : icl_core::thread::Thread("CanWorkerThread", -10),
      m_can_device(can_device),
      m_recv_buffer(),
      m_max_buffer_size(max_buffer_size),
      m_buffer_mutex(),
      m_buffer_semaphore(0),
      m_use_can_mask(false),
      m_can_mask(),
      m_single_can_id(0),
      m_dsin(0),
      m_sequence_number(0)
  { }

  //! Worker thread main method which receives the CAN messages.
  virtual void run();

  //! Returns \c true if the buffer is nonempty.
  bool hasData() const;

  /*! Gets the next CAN message, blocking in case no data is in the
   *  buffer.
   */
  CanMessageStamped::Ptr get();

  //! Access the CAN device.
  boost::scoped_ptr<tCanDevice>& canDevice() { return m_can_device; }

  //! Sets the CAN mask used to filter out relevant CAN messages.
  void setCanMask(const CanMatrix& can_mask)
  {
    m_use_can_mask = true;
    m_can_mask = can_mask;
  }

  //! Sets a single CAN ID used to filter out relevant CAN messages.
  void setSingleCanID(uint16_t can_id)
  {
    m_single_can_id = can_id;
  }

private:
  //! The CAN device.
  boost::scoped_ptr<tCanDevice> m_can_device;
  /*! Buffers CAN messages in case they are not consumed quickly
   *  enough.
   */
  std::deque<CanMessageStamped::Ptr> m_recv_buffer;
  //! Buffer size limit.
  std::size_t m_max_buffer_size;
  //! Synchronizes receive buffer access.
  mutable icl_core::thread::Mutex m_buffer_mutex;
  //! Handles producer/consumer notification.
  icl_core::thread::Semaphore m_buffer_semaphore;
  //! If \c true, #m_can_mask is used to filter the CAN stream.
  bool m_use_can_mask;
  /*! Only CAN message IDs contained here are passed on to the user.
   *  \note If #m_single_can_id is non-zero, it has priority over
   *        this.
   */
  CanMatrix m_can_mask;
  /*! If non-zero, only this message ID is passed on to the user.
   *  This has priority over #m_can_mask.
   */
  uint16_t m_single_can_id;
  /*! Current item number.  This increments for \e every CAN message
   *  received, even messages that are filtered out.
   */
  std::size_t m_dsin;
  /*! Current sequence number.  This increments for every CAN message
   *  that is received and passed on to the user, i.e. \e not filtered
   *  out.
   */
  std::size_t m_sequence_number;
};

void HardwareCanSourcePeak::WorkerThread::run()
{
  while (execute())
  {
    if (!m_can_device->IsInitialized())
    {
      // TODO: Try to reconnect
      throw tException(ePEAK_DEVICE_ERROR, "Lost connection to peak device.");
    }
    CanMessageStamped::Ptr msg(new CanMessageStamped);
    while (m_can_device->Receive(**msg) > 0)
    {
      msg->header().timestamp = boost::posix_time::microsec_clock::local_time();
      msg->header().dsin = m_dsin++;
      // TODO: Check if message ID is in CAN map
      {
        msg->header().sequence_number = m_sequence_number++;
        msg->header().coordinate_system = "";
        icl_core::thread::ScopedMutexLock lock(m_buffer_mutex);
        m_recv_buffer.push_front(msg);
        msg.reset(new CanMessageStamped);
        if (m_recv_buffer.size() > m_max_buffer_size)
        {
          LOGGING_WARNING(CAN, "Buffer overflow, too many outstanding messages!" << endl);
          double message_rate = double(m_recv_buffer.size())
            / ((m_recv_buffer.front()->header().timestamp
                - m_recv_buffer.back()->header().timestamp).total_microseconds()
               * 1e-6);
          LOGGING_DEBUG(CAN, "Received " << message_rate << " msg/s." << endl);
          while (m_recv_buffer.size() > m_max_buffer_size)
          {
            m_recv_buffer.pop_back();
          }
        }
      }
      m_buffer_semaphore.post();
      // Minimal usleep at this point, to decrease the number of
      // receivable incoming messages
      // TODO: Check with Hugo why this is necessary
      usleep(300);
    }
    usleep(5000);
  }
}

CanMessageStamped::Ptr HardwareCanSourcePeak::WorkerThread::get()
{
  m_buffer_semaphore.wait();
  icl_core::thread::ScopedMutexLock lock(m_buffer_mutex);
  CanMessageStamped::Ptr msg = m_recv_buffer.back();
  m_recv_buffer.pop_back();
  return msg;
}

bool HardwareCanSourcePeak::WorkerThread::hasData() const
{
  icl_core::thread::ScopedMutexLock lock(m_buffer_mutex);
  return !m_recv_buffer.empty();
}

//----------------------------------------------------------------------
// Main class
//----------------------------------------------------------------------

HardwareCanSourcePeak::HardwareCanSourcePeak(const std::string& uri, const std::string& name)
  : HardwareCanSource(uri, name),
    m_buffer(),
    m_worker_thread()
{
  m_is_good = false;
  icl_sourcesink::SimpleURI parsed_uri(uri);

  uint32_t can_baudrate = 500;
  boost::optional<uint32_t> uri_baudrate = parsed_uri.getQuery<uint32_t>("baudrate");
  if (uri_baudrate)
  {
    can_baudrate = *uri_baudrate;
  }

  uint32_t can_buffer_size = 256;
  boost::optional<uint32_t> uri_buffer_size = parsed_uri.getQuery<uint32_t>("buffer_size");
  if (uri_buffer_size)
  {
    can_buffer_size = *uri_buffer_size;
  }

  uint16_t can_id = 0;
  boost::optional<uint16_t> uri_can_id = parsed_uri.getQuery<uint16_t>("can_id");
  if (uri_can_id)
  {
    if (*uri_can_id > 2047)
    {
      LOGGING_ERROR(CAN, "Illegal value for 'can_id' (must be <= 2047). Ignoring." << endl);
    }
    else
    {
      can_id = *uri_can_id;
    }
  }

  std::string can_mask = "";
  boost::optional<std::string> uri_can_mask = parsed_uri.getQuery<std::string>("can_mask");
  if (uri_can_mask)
  {
    can_mask = *uri_can_mask;
  }
  else
  {
    can_mask = icl_core::config::getDefault<std::string>("/icl_hardware_can/can_mask", "");
  }

  LOGGING_DEBUG(CAN, "Device: " << parsed_uri.path() << endl);
  LOGGING_DEBUG(CAN, "Baudrate: " << can_baudrate << " kbps" << endl);

  LOGGING_DEBUG(CAN, "Opening CAN device... " << endl);
  tCanDevice::CheckLXRTInterface();
  m_worker_thread.reset(new WorkerThread(
                          tCanDevice::Create(
                            parsed_uri.path().c_str(),
                            O_RDWR | O_NONBLOCK,
                            0xff,
                            0xff,
                            can_baudrate,
                            300,
                            8000),
                          can_buffer_size));

  // Check if CAN device was initialized successfully.
  if (m_worker_thread->canDevice()->IsInitialized())
  {
    LOGGING_DEBUG(CAN, "CAN device successfully initialized." << endl);
    m_worker_thread->start();
  }
  else
  {
    m_worker_thread->canDevice().reset();
    LOGGING_ERROR(CAN, "Error initializing CAN device." << endl);
    return;
  }

  // Set CAN mask
  if (can_id > 0)
  {
    LOGGING_DEBUG(CAN, "Using single CAN ID: " << can_id << endl);
    m_worker_thread->setSingleCanID(can_id);
  }
  else if (can_mask != "")
  {
    tCanMatrixParser parser(can_mask);
    if (parser.isActive())
    {
      m_worker_thread->setCanMask(parser.getCanMatrix());
      LOGGING_DEBUG(CAN, "Using CAN Mask: " << can_mask << endl);
    }
    else
    {
      LOGGING_WARNING(CAN, "Could not use provided CAN mask " << can_mask << endl);
    }
  }

  m_sleep_time = 500000/can_baudrate;

  // Wait for first CAN message.
  m_is_good = advance();
}

HardwareCanSourcePeak::~HardwareCanSourcePeak()
{
  if (m_worker_thread)
  {
    m_worker_thread->stop();
    m_worker_thread->join();
  }
}

bool HardwareCanSourcePeak::advance()
{
  if (!m_worker_thread)
  {
    LOGGING_ERROR(CAN, "Worker thread nonexistent for some reason." << endl);
    m_is_good = false;
  }
  else if (!m_worker_thread->canDevice())
  {
    LOGGING_ERROR(CAN, "CAN device object disappeared." << endl);
    m_is_good = false;
  }
  else if (!m_worker_thread->canDevice()->IsInitialized())
  {
    LOGGING_ERROR(CAN, "CAN device is closed." << endl);
    m_is_good = false;
  }
  else if (!m_worker_thread->running())
  {
    LOGGING_ERROR(CAN, "Worker thread was terminated unexpectedly." << endl);
    m_is_good = false;
  }
  m_buffer = m_worker_thread->get();
  return bool(m_buffer);
}

}
}
