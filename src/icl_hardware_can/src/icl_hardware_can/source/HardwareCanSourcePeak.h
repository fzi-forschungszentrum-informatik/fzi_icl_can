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
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_PEAK_H_INCLUDED
#define ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_PEAK_H_INCLUDED

#include <icl_hardware_can/source/ImportExport.h>
#include <icl_hardware_can/source/HardwareCanSource.h>

#include <icl_hardware_can/tCanDevice.h>
#include <icl_hardware_can/tCanMatrixParser.h>

#include <vector>

namespace icl_hardware {
namespace can {

/*! This Source reads from a given CAN bus and returns its messages as
 *  time-stamped tCanMessage objects.
 */
class ICL_HARDWARE_CAN_SOURCE_IMPORT_EXPORT HardwareCanSourcePeak : public HardwareCanSource
{
private:
  /*! Internal worker thread which listens to a given CAN device and
   *  buffers the received messages.
   */
  class WorkerThread;

public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanSourcePeak> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanSourcePeak> ConstPtr;

  //! URI schemes supported by this source.
  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+live",
        URISchemeInfo(
          "Reads CAN message data via a Peak CAN device.",
          "can+live://<path>[?baudrate=<baudrate>][&can_mask=<can_mask>|can_id=<can_id>][&buffer_size=<buffer_size>]",
          "<path>         Path to CAN device (default: /dev/pcan0)\n"
          "<baudrate>     CAN baudrate [kbps] (default: 500).\n"
          "<can_mask>     Path to CAN mask file\n"
          "               (default: from config option /icl_hardware_can/can_mask)\n"
          "<can_id>       Specific CAN ID to extract while ignoring all others\n"
          "<buffer_size>  Internal buffer size (default: 256 messages)")));
    return schemes;
  }

  virtual CanMessageStamped::Ptr get() const
  {
    return m_buffer;
  }

  virtual bool isSeekable() const { return false; }

  ~HardwareCanSourcePeak();

protected:
  //! Constructor.
  HardwareCanSourcePeak(const std::string& uri = "",
                        const std::string& name = "HardwareCanSourcePeak");

  //! Provide access for the manager.
  friend class icl_sourcesink::SourceSinkManager;

  //! Provide access for the factory.
  friend class icl_sourcesink::DataSourceFactory<HardwareCanSourcePeak>;

  //! Advance to the next data element.
  bool advance();

private:
  //! Buffers the latest data element.
  CanMessageStamped::Ptr m_buffer;

  /*! Time (in microseconds) we sleep before checking for the next CAN
   *  message.  Always set to half the CAN symbol time.  I.e., at 1
   *  Kbps this is set to 500 microseconds.  At 1 Mbps this is zero.
   */
  uint32_t m_sleep_time;

  /*! The worker thread that monitors the CAN device and buffers newly
   *  received data.
   */
  boost::scoped_ptr<WorkerThread> m_worker_thread;
};

SOURCESINK_DECLARE_GENERIC_SOURCE_FACTORY(HardwareCanSourcePeak)

}
}

#endif
