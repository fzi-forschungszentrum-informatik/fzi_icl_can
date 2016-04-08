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
 * \date    2014-05-23
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_FILTER_HARDWARE_CAN_FILTER_MESSAGE_MAP_H_INCLUDED
#define ICL_HARDWARE_CAN_FILTER_HARDWARE_CAN_FILTER_MESSAGE_MAP_H_INCLUDED

#include <icl_hardware_can/filter/ImportExport.h>

#include <icl_sourcesink/DataSinkRegistration.h>
#include <icl_hardware_can/sink/HardwareCanSink.h>
#include <icl_sourcesink/DataFilter.h>
#include <icl_hardware_can/tCanMessage.h>

namespace icl_hardware {
namespace can {

/*! Aggregates tCanMessage data into a CanMessageMap.
 *  \note This Filter reuses a single CanMessageMap internally.
 *        Therefore, if you intend to apply in-place filters to the
 *        output, you should use a DataCopyFilter to create a copy,
 *        rather than manipulating the message map directly.
 */
class ICL_HARDWARE_CAN_FILTER_IMPORT_EXPORT HardwareCanFilterMessageMap : public icl_sourcesink::DataFilter<tCanMessage, CanMessageMap>
{
public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanFilterMessageMap> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanFilterMessageMap> ConstPtr;

  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+map",
        URISchemeInfo(
          "Converts tCanMessage to a CanMessageMap",
          "can+map",
          "No parameters needed.")));
    return schemes;
  }

  //! Constructor
  HardwareCanFilterMessageMap(const std::string& uri = "HardwareCanFilterMessageMap",
                              const std::string& name = "UnnamedHardwareCanFilterMessageMap");

  //! Destructor
  virtual ~HardwareCanFilterMessageMap();

  /*! In contrast to the default implementation, this does not reset
   *  the internal data element, but instead updates the existing one.
   *  \see DataFilter<T>::set()
   */
  virtual void set(const CanMessageStamped::Ptr& data);

  //! \see DataFilter<T>::filter()
  virtual bool filter(const CanMessageStamped& input_data,
                      icl_core::Stamped<CanMessageMap>& output_data);
};

SOURCESINK_DECLARE_GENERIC_SINK_FACTORY(HardwareCanFilterMessageMap)

}
}

#endif
