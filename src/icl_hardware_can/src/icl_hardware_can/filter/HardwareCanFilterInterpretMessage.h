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
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-18
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_FILTER_INTERPRET_MESSAGE_H_INCLUDED
#define ICL_HARDWARE_CAN_FILTER_INTERPRET_MESSAGE_H_INCLUDED

#include <icl_hardware_can/filter/ImportExport.h>

#include <icl_sourcesink/DataSinkRegistration.h>
#include <icl_hardware_can/sink/HardwareCanSink.h>
#include <icl_sourcesink/DataFilter.h>
#include <icl_hardware_can/tCanMessage.h>
#include <icl_hardware_can/InterpretedCanMessage.h>
#include <icl_hardware_can/tCanMatrixParser.h>

namespace icl_hardware {
namespace can {

/*! This is currently the only SourceSink handling InterpretedCanMessages.
 *  So we define the ConsoleSink here.
 *  Move it to a more central place if there are more than this SourceSink.
 */
SOURCESINK_DECLARE_CONSOLE_SINK(InterpretedCanMessageSinkConsole, InterpretedCanMessages, "interpretedcan+console")



//! Converts tCanMessage to InterpretedCanMessage.
class ICL_HARDWARE_CAN_FILTER_IMPORT_EXPORT HardwareCanFilterInterpretMessage : public icl_sourcesink::DataFilter<tCanMessage, InterpretedCanMessages>
{
public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanFilterInterpretMessage> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanFilterInterpretMessage> ConstPtr;

  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+interpreter",
        URISchemeInfo(
          "Converts tCanMessage to InterpretedCanMessage",
          "can+interpreter:[?can_mask=<can_mask>]",
          "<can_mask>  Path to CAN mask file\n"
          "            (default: from config option /icl_hardware_can/can_mask)")));
    return schemes;
  }

  //! Constructor
  HardwareCanFilterInterpretMessage(const std::string& uri = "HardwareCanFilterInterpretMessage",
                                    const std::string& name = "UnnamedHardwareCanFilterInterpretMessage");

  //! Destructor
  virtual ~HardwareCanFilterInterpretMessage();

  //! \see DataFilter<T>::filter()
  virtual bool filter(const icl_core::Stamped<tCanMessage>& input_data,
                      icl_core::Stamped<InterpretedCanMessages>& output_data);

private:
  //! Describes how the CAN traffic is to be interpreted.
  CanMatrix m_can_mask;
};

SOURCESINK_DECLARE_GENERIC_SINK_FACTORY(HardwareCanFilterInterpretMessage)

}
}

#endif
