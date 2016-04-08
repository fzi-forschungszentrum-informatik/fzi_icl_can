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
 * \date    2014-05-19
 *
 */
//----------------------------------------------------------------------
#include "CanFileResolutionHelper.h"

#include <icl_core/BaseTypes.h>
#include <icl_math/MathFunctions.h>

namespace icl_hardware {
namespace can {

CanFileResolutionHelper::CanFileResolutionHelper(const std::string& filename)
  : m_filename(filename),
    m_file(m_filename.c_str())
{
  m_file.imbue(std::locale("C"));

  // Find the first valid line.
  std::string line;
  std::ifstream::pos_type current_pos = m_file.tellg();
  while (m_file && (line.empty() || line[0] == '#'))
  {
    current_pos = m_file.tellg();
    std::getline(m_file, line);
  }
  if (tryParse(line, current_pos))
  {
    m_file.seekg(current_pos);
    m_current_position.dsin = icl_sourcesink::DSIN(current_pos);
  }
  else
  {
    // File is broken somehow, or end of file was reached.  Make sure
    // we go to the end in any case, so that subsequent good() calls
    // return false as well.
    m_file.seekg(0, std::ios_base::end);
  }
}

boost::posix_time::ptime CanFileResolutionHelper::timestampToPtime(double stamp)
{
  unsigned long stamp_usec(icl_math::round((stamp - icl_math::floor(stamp)) * 1e6));
  return boost::posix_time::ptime(boost::gregorian::date(1970,1,1),
                                  boost::posix_time::seconds(static_cast<unsigned long>(stamp))
                                  + boost::posix_time::microsec(stamp_usec));
}

bool CanFileResolutionHelper::advance()
{
  // We're on a valid line to begin with.  Skip beyond that, then find
  // the next valid line.
  std::string line;
  std::getline(m_file, line);
  line = "";
  std::ifstream::pos_type current_pos = m_file.tellg();
  while (m_file && (line.empty() || line[0] == '#'))
  {
    current_pos = m_file.tellg();
    std::getline(m_file, line);
  }
  if (tryParse(line, current_pos))
  {
    m_file.seekg(current_pos);
    return true;
  }
  else
  {
    // File is broken somehow, or end of file was reached.  Make sure
    // we go to the end in any case, so that subsequent good() calls
    // return false as well.
    m_file.seekg(0, std::ios_base::end);
    return false;
  }
}

bool CanFileResolutionHelper::tryParse(const std::string& line, uint64_t current_pos)
{
  // FZI-CAN FILE V1.0
  m_current_position.index = icl_sourcesink::Index(icl_sourcesink::Position::INVALID_INDEX);
  m_current_position.timestamp = boost::posix_time::not_a_date_time;
  m_current_position.dsin = icl_sourcesink::DSIN(icl_sourcesink::Position::INVALID_DSIN);
  std::istringstream ss(line);
  ss.imbue(std::locale("C"));
  double gmtime;
  unsigned id, dlc, msg0, msg1, msg2, msg3, msg4, msg5, msg6, msg7;
  try
  {
    if (ss >> std::dec >> gmtime >> id >> dlc
        >> std::hex >> msg0 >> msg1 >> msg2 >> msg3 >> msg4 >> msg5 >> msg6 >> msg7)
    {
      m_current_position.index = icl_sourcesink::Index(icl_sourcesink::Position::INVALID_INDEX);
      m_current_position.timestamp = timestampToPtime(gmtime);
      m_current_position.dsin = icl_sourcesink::DSIN(current_pos);
      m_current_message.reset(
        new CanMessageStamped(
          tCanMessage(id, dlc),
          icl_core::DataHeader("", m_current_position.timestamp)));
      m_current_message->header().dsin = m_current_position.dsin;
      (*m_current_message)->data[0] = msg0;
      (*m_current_message)->data[1] = msg1;
      (*m_current_message)->data[2] = msg2;
      (*m_current_message)->data[3] = msg3;
      (*m_current_message)->data[4] = msg4;
      (*m_current_message)->data[5] = msg5;
      (*m_current_message)->data[6] = msg6;
      (*m_current_message)->data[7] = msg7;
      return true;
    }
  }
  catch (...)
  {
    return false;
  }
  return bool(ss);
}

bool CanFileResolutionHelper::resolveHere()
{
  std::ifstream::pos_type current_pos = m_file.tellg();
  std::string line;
  std::getline(m_file, line);
  if (tryParse(line, current_pos))
  {
    m_file.seekg(current_pos);
    m_current_position.dsin = icl_sourcesink::DSIN(current_pos);
    return true;
  }
  else
  {
    // File is broken somehow, or end of file was reached.  Make sure
    // we go to the end in any case, so that subsequent good() calls
    // return false as well.
    m_file.seekg(0, std::ios_base::end);
    return false;
  }
}

}
}
