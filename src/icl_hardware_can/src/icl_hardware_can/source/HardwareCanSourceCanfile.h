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
#ifndef ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_CAN_FILE_H_INCLUDED
#define ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_CAN_FILE_H_INCLUDED

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <icl_core/BaseTypes.h>
#include <icl_sourcesink/KeyframePositionResolver.h>

#include <icl_hardware_can/source/ImportExport.h>
#include <icl_hardware_can/source/HardwareCanSource.h>
#include <icl_hardware_can/source/CanFileResolutionHelper.h>

namespace icl_hardware {
namespace can {

/*! This Source reads a given file and offers the possibility to
 *  return tCanMessage.  Messages will be returned by calling
 *  GetNextMessage().  Call Seek() to jump to a specific part of the
 *  file.
 *
 *  The FZI CAN file format (version 1.0) is structured as follows:
 *  \begincode
 *  gmtime msg_id dlc data[0] data[1] data[2] data[3] data[4] data[5] data[6] data[7]
 *  \endcode
 *  An example line is given below:
 *  \begincode
 *  1314957629.419675 55 8 0 0 0 0 1b f5 21 7a
 *  \endcode
 *  The message block time and position is stored in an additional XML
 *  file.
 */
class ICL_HARDWARE_CAN_SOURCE_IMPORT_EXPORT HardwareCanSourceCanfile : public HardwareCanSource
{
public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanSourceCanfile> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanSourceCanfile> ConstPtr;

  //! URI schemes supported by this source.
  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+file",
        URISchemeInfo(
          "Reads tCanMessage data from FZI CAN files.",
          "can+file:<filepath>[#start-end]",
          "<filepath> File to read.")));
    return schemes;
  }

  virtual CanMessageStamped::Ptr get() const
  {
    return m_buffer;
  }

  //! This source works on CAN files and is therefore seekable.
  virtual bool isSeekable() const
  {
    return true;
  }

protected:
  HardwareCanSourceCanfile(const std::string& uri,
                           const std::string& name = "HardwareCanSourceCanfile");

  void prepareFile();

  //! Provide access for the manager.
  friend class icl_sourcesink::SourceSinkManager;

  //! Provide access for the factory.
  friend class icl_sourcesink::DataSourceFactory<HardwareCanSourceCanfile>;

  //! Implementation of DataSourceBase::seekImpl().
  virtual bool seekImpl(const icl_sourcesink::InternalIndex internal_index);

  //! Implementation of DataSourceBase::advance().
  virtual bool advance();

private:
  //! Buffers the current data element.
  CanMessageStamped::Ptr m_buffer;

  /*! Shared pointer to the actual resolver type, to avoid repeated
   *  dynamic casting.
   */
  icl_sourcesink::KeyframePositionResolver::Ptr m_resolver_x;

  //! Our own resolution helper.
  CanFileResolutionHelper::Ptr m_resolution_helper;

  //! Ever-increasing sequence number.
  std::size_t m_sequence_number;
};

SOURCESINK_DECLARE_GENERIC_SOURCE_FACTORY(HardwareCanSourceCanfile)

}
}

#endif
