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
 * \date    2010-12-22
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-17
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SINK_HARDWARE_CAN_SINK_CANFILE_H_INCLUDED
#define ICL_HARDWARE_CAN_SINK_HARDWARE_CAN_SINK_CANFILE_H_INCLUDED

#include <icl_sourcesink/DataSinkRegistration.h>

#include <icl_hardware_can/sink/ImportExport.h>
#include <icl_hardware_can/sink/HardwareCanSink.h>

#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

namespace icl_hardware {
namespace can {

/*! This Sink writes CAN data in the FZI CAN file format (version 1),
 *  formatted as follows:
 *  \begincode
 *  timestamp msg_id dlc data[0] data[1] data[2] data[3] data[4] data[5] data[6] data[7]
 *  \endcode
 *  An example line is given below:
 *  \begincode
 *  1302866616.057185 55 8 0 0 0 0 1b f5 21 7a
 *  \endcode
 */
class ICL_HARDWARE_CAN_SINK_IMPORT_EXPORT HardwareCanSinkCanfile : public HardwareCanSink
{
public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanSinkCanfile> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanSinkCanfile> ConstPtr;

  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+file",
        URISchemeInfo(
          "Writes CAN message data to a file, using the FZI CAN file format.",
          "can+file:<path>[?container_size=<container_size>]",
          "<path>            Path to the CAN file. A .can suffix is added if not yet\n"
          "                  present.\n"
          "<container_size>  Number of messages to buffer before writing out to disk\n"
          "                  (default: 500).\n"
          )));
    return schemes;
  }

  //! Constructor
  HardwareCanSinkCanfile(const std::string& uri = "HardwareCanSinkCanfile",
                         const std::string& name = "UnnamedHardwareCanSinkCanfile");

  //! Destructor
  virtual ~HardwareCanSinkCanfile();

  //! \see DataSink<T>::set()
  virtual void set(const CanMessageStamped::Ptr& msg);

protected:
  //! Opens the output file and writes the file header.
  bool prepareFile();

private:
  //! CAN data file stream.
  std::ofstream m_data_file;

  /*! Message container which buffers data before writing it to disk,
   *  in order to reduce unnecessary disk accesses.
   */
  std::vector<CanMessageStamped::ConstPtr> m_msg_container;
  /*! Maximum size of the message container.  When this is reached,
   *  data is written out to disk.
   */
  std::size_t m_msg_container_size;
  /*! Maximum time difference between two commits.  When this is
   *  reached, data is written out to disk.
   */
  boost::posix_time::time_duration m_commit_timediff;

  //! Timestamp of the last processed message.
  boost::posix_time::ptime m_last_msg_time;
  //! Time at which the last message was committed to disk.
  boost::posix_time::ptime m_last_commit_time;

  //! Major version number of the file format.
  static const short cMAJOR_FILEVERSION = 1;
  //! Minor version number of the file format.
  static const short cMINOR_FILEVERSION = 2;
};

SOURCESINK_DECLARE_GENERIC_SINK_FACTORY(HardwareCanSinkCanfile)

}
}

#endif
