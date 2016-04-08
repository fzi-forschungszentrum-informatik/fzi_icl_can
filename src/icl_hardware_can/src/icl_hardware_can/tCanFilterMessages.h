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
 * \date    2011-05-26
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_T_CAN_FILTER_MESSAGES_H_INCLUDED
#define ICL_HARDWARE_CAN_T_CAN_FILTER_MESSAGES_H_INCLUDED

#include "icl_hardware_can/tCanMessage.h"
#include "icl_hardware_can/ImportExport.h"

#include <icl_core/tString.h>

#include <fstream>
#include <boost/filesystem.hpp>
#include <map>

namespace icl_hardware {
namespace can {

/*! This class opens a CAN message file and constructs a map of known
 *  CAN message IDs and a multimap in which it tracks all those
 *  message bytes which change without extraneous cause.  This map is
 *  used in the function checkMessage() when called with a
 *  tCanMessage.
 */
class ICL_HARDWARE_CAN_IMPORT_EXPORT tCanFilterMessages
{
public:
  /** Constructor */
  tCanFilterMessages();

  /** Destructor */
  ~tCanFilterMessages();

  void Open(const std::string identifer);

  bool getStatus() const;

  //!< function returns true if message data changed because of human interaction
  bool checkMessage(const tCanMessage& msg) const;

protected:

  bool byteIsChangingConstantly(unsigned int i, unsigned int id) const;

  void mapContentOfFile();
  void StringToCanMsg(icl_hardware::can::tCanMessage &msg, std::string str);

  int PrepareFiles();

  void setDataFileName(const boost::filesystem::path &file_name);
  const boost::filesystem::path & getDataFileName() const;

private:
  struct DataWrapper
  {
    unsigned char data[8];
  };

  std::map<unsigned int, DataWrapper> m_table_id_to_data;
  std::multimap<unsigned int, unsigned int> m_table_id_to_changing_bytes;

  //! File handler for data file
  std::ifstream m_data_file;

  boost::filesystem::path *m_data_file_name;

  DataWrapper m_message_data;

  bool is_initialized;
};

}
}

#endif
