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
 * \date    2014-05-17
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SINK_HARDWARE_CAN_SINK_PEAK_H_INCLUDED
#define ICL_HARDWARE_CAN_SINK_HARDWARE_CAN_SINK_PEAK_H_INCLUDED

#include <icl_sourcesink/DataSinkRegistration.h>

#include <icl_hardware_can/sink/ImportExport.h>
#include <icl_hardware_can/sink/HardwareCanSink.h>
#include <icl_hardware_can/tCanDevice.h>

namespace icl_hardware {
namespace can {

/*! This sink receives a tCanMessage and sends it to a predefined Peak
 *  CAN device.
 */
class ICL_HARDWARE_CAN_SINK_IMPORT_EXPORT HardwareCanSinkPeak : public HardwareCanSink
{
public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanSinkPeak> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanSinkPeak> ConstPtr;

  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+peak",
        URISchemeInfo(
          "Writes CAN message data to a Peak CAN device.",
          "can+peak:<path>[?baudrate=<baudrate>]",
          "<path>         Path to CAN device (default: /dev/pcan0)\n"
          "<baudrate>     CAN baudrate [kbps] (default: 500).")));
    return schemes;
  }

  //! Constructor
  HardwareCanSinkPeak(const std::string& uri = "HardwareCanSinkPeak",
                      const std::string& name = "UnnamedHardwareCanSinkPeak");

  //! Destructor
  virtual ~HardwareCanSinkPeak();

  //! \see DataSink<T>::set()
  virtual void set(const CanMessageStamped::Ptr& msg);

private:
  //! The CAN device.
  boost::scoped_ptr<tCanDevice> m_can_device;
};

SOURCESINK_DECLARE_GENERIC_SINK_FACTORY(HardwareCanSinkPeak)

}
}

#endif
