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
#ifndef ICL_HARDWARE_CAN_SOURCE_CAN_FILE_RESOLUTION_HELPER_H_INCLUDED
#define ICL_HARDWARE_CAN_SOURCE_CAN_FILE_RESOLUTION_HELPER_H_INCLUDED

#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>

#include <icl_sourcesink/Position.h>
#include <icl_sourcesink/FileResolutionHelper.h>
#include <icl_hardware_can/source/HardwareCanSource.h>

namespace icl_hardware {
namespace can {

class CanFileResolutionHelper : public ::icl_sourcesink::FileResolutionHelper
{
public:
  //! Convenience shared pointer shorthand.
  typedef boost::shared_ptr<CanFileResolutionHelper> Ptr;
  //! Convenience shared pointer shorthand (const version).
  typedef boost::shared_ptr<const CanFileResolutionHelper> ConstPtr;

  CanFileResolutionHelper(const std::string& filename);

  virtual CanFileResolutionHelper *clone() const
  {
    CanFileResolutionHelper *my_clone = new CanFileResolutionHelper(m_filename);
    my_clone->m_file.clear();
    my_clone->seekg(tellg());
    return my_clone;
  }

  virtual bool good() const
  {
    return m_file.good();
  }

  virtual icl_sourcesink::Position current() const
  {
    return m_current_position;
  }

  virtual bool advance();

  virtual std::ifstream::pos_type size() const
  {
    std::ifstream::pos_type curr = m_file.tellg();
    m_file.seekg(0, std::ios::end);
    std::ifstream::pos_type result = m_file.tellg();
    m_file.seekg(curr);
    return result;
  }

  virtual std::ifstream::pos_type tellg() const
  {
    return m_file.tellg();
  }

  virtual bool seekg(std::ifstream::pos_type position)
  {
    m_file.clear();
    m_file.seekg(position);
    return resolveHere();
  }

  virtual bool seekg(std::ifstream::off_type offset, std::ios_base::seekdir direction)
  {
    m_file.clear();
    m_file.seekg(offset, direction);
    return resolveHere();
  }

  static boost::posix_time::ptime timestampToPtime(double stamp);

  CanMessageStamped::Ptr currentMessage() const { return m_current_message; }

protected:
  bool tryParse(const std::string& line, uint64_t current_pos);

  bool resolveHere();

private:
  std::string m_filename;
  mutable std::ifstream m_file;
  icl_sourcesink::Position m_current_position;
  CanMessageStamped::Ptr m_current_message;
};

}
}

#endif
