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
#include "HardwareCanSinkCanfile.h"

#include <iomanip>
#include <boost/date_time.hpp>

#include <icl_sourcesink/SimpleURI.h>
#include <icl_hardware_can/Logging.h>
#include <icl_hardware_can/tException.h>

namespace icl_hardware {
namespace can {

HardwareCanSinkCanfile::HardwareCanSinkCanfile(const std::string& uri, const std::string& name)
  : HardwareCanSink(uri, name),
    m_data_file(),
    m_msg_container(),
    m_msg_container_size(500),
    m_commit_timediff(boost::posix_time::seconds(1)),
    m_last_msg_time(),
    m_last_commit_time()
{
  if (!prepareFile())
  {
    LOGGING_WARNING(CAN, "Failed to open URI " << uri << " for recording!" << endl);
    return;
  }

  m_msg_container.reserve(m_msg_container_size);
}

HardwareCanSinkCanfile::~HardwareCanSinkCanfile()
{ }

bool HardwareCanSinkCanfile::prepareFile()
{
  icl_sourcesink::SimpleURI parsed_uri(uri());

  // Extract message container size.
  boost::optional<std::size_t> msg_container_size = parsed_uri.getQuery<std::size_t>("container_size");
  if (msg_container_size)
  {
    if (msg_container_size > 0)
    {
      m_msg_container_size = *msg_container_size;
    }
    else
    {
      LOGGING_ERROR(CAN, "container_size must be > 0. Using default value " << m_msg_container_size << "." << endl);
    }
  }

  // Ensure the filename ends in ".can".
  std::string can_filename = parsed_uri.path();
  if (can_filename.size() < 4 || can_filename.substr(can_filename.size()-4) != ".can")
  {
    can_filename += ".can";
  }

  m_data_file.open(can_filename.c_str());
  if (!m_data_file)
  {
    LOGGING_ERROR(CAN, "Failed to open data file '" << m_data_file << "'." << endl);
    return false;
  }

  // Write file header.
  m_data_file << "# FZI-CAN Stream version " << cMAJOR_FILEVERSION << "." << cMINOR_FILEVERSION << std::endl;

  return true;
}

void HardwareCanSinkCanfile::set(const CanMessageStamped::Ptr& msg)
{
  if (!msg || ((*msg)->id == 0 && (*msg)->dlc == 0 && (*msg)->rtr == 0))
  {
    throw(tException(eMESSAGE_ERROR, "Received empty message"));
  }

  if (!m_last_msg_time.is_special() && msg->header().timestamp < m_last_msg_time)
  {
    LOGGING_WARNING(CAN, "CAN message sequence out of order! (" << msg->header().timestamp << " < " << m_last_msg_time << ")" << endl);
  }
  m_msg_container.push_back(msg);

  if (m_msg_container.size() >= m_msg_container_size
      || m_last_commit_time + m_commit_timediff > msg->header().timestamp)
  {
    if (m_data_file.bad())
    {
      throw tException(eFILE_ERROR, "Data file stream went bad.");
    }

    // Write content to file.
    m_data_file << std::setfill('0');
    for (std::vector<CanMessageStamped::ConstPtr>::const_iterator iter = m_msg_container.begin();
         iter != m_msg_container.end(); ++iter)
    {
      // Build unix timestamp.
      boost::posix_time::time_duration time_since_epoch
        = (*iter)->header().timestamp - boost::posix_time::from_time_t(0);
      uint64_t seconds = time_since_epoch.ticks() / boost::posix_time::time_duration::ticks_per_second();
      long microseconds = time_since_epoch.fractional_seconds() / (boost::posix_time::time_duration::ticks_per_second() / 1000000);
      m_data_file << std::dec << seconds << "." << std::setw(6) << microseconds
                  << " " << (**iter)->id << " " << unsigned((**iter)->dlc)
                  << std::hex
                  << " " << std::setw(2) << unsigned((**iter)->data[0])
                  << " " << std::setw(2) << unsigned((**iter)->data[1])
                  << " " << std::setw(2) << unsigned((**iter)->data[2])
                  << " " << std::setw(2) << unsigned((**iter)->data[3])
                  << " " << std::setw(2) << unsigned((**iter)->data[4])
                  << " " << std::setw(2) << unsigned((**iter)->data[5])
                  << " " << std::setw(2) << unsigned((**iter)->data[6])
                  << " " << std::setw(2) << unsigned((**iter)->data[7])
                  << "\n";

      if (m_data_file.bad())
      {
        throw tException(eFILE_ERROR, "Stream went bad while writing to file.");
      }
    }

    m_last_commit_time = msg->header().timestamp;
    m_msg_container.clear();
  }
  m_last_msg_time = msg->header().timestamp;
}

}
}
