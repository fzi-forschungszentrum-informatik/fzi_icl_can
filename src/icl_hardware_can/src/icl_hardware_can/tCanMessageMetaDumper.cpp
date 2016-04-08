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

// ----------------------------------------------------------
/*
 * tCanMessageMetaDumper.cpp
 * <section>
 *
 * Created by Hugo Ritzkowski on 24.01.2012.
 * Copyright 2012
 * Company Forschungszentrum Informatik (FZI), Abteilung IDS.
 * All rights reserved.
 *
 */
// ----------------------------------------------------------
/*!
 *  \file       tCanMessageMetaDumper.cpp
 *  \author     <a href="mailto:ritzkow@fzi.de"> Hugo Ritzkowski</a>
 *  \date       24.01.2012
 *
 *  \brief
 *
 *
 */
// ----------------------------------------------------------

#include "tCanMessageMetaDumper.h"

#include "icl_hardware_can/Logging.h"
#include "icl_core_config/Config.h"
#include <stdio.h>

namespace icl_hardware {

namespace can {

tCanMessageMetaDumper::tCanMessageMetaDumper()
  : m_baudrate(0),
    m_data_file_name(0)
{

}

tCanMessageMetaDumper::~tCanMessageMetaDumper()
{

}

void tCanMessageMetaDumper::open(const std::string identifier, const int baudrate)
{
  if (identifier != "")
  {
    setDataFileName(boost::filesystem::path(identifier));
  }
  else
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "No data file provided!" << icl_core::logging::endl);
    return;
  }

  if (baudrate != 0)
  {
    m_baudrate = baudrate;
  }
  else
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "No baudrate provided!" << icl_core::logging::endl);
    return;
  }

  int ret=prepareFile();

  if (ret!=0)
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "Failed to open file for recording!" << icl_core::logging::endl);
    return;
  }
}

void tCanMessageMetaDumper::close()
{
  std::cout << "</record>" << std::endl;
  if (m_data_file.is_open())
  {
    m_data_file.close();
  }
}

/*!
 *  This function reads through a file and outputs a block of meta-data for specific number of lines
 *  this number is declared by the given baudrate
 *  It returns false if no lines where read otherwise true
 */

bool tCanMessageMetaDumper::constructXmlFile()
{
  std::string line;
  unsigned int line_counter = 0;
  unsigned int next_commit = 1; //!< do the first commit at line 1, line 0 is the header

  //print header
  std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
  std::cout << "<record version=\"" << m_major_fileversion << "." << m_minor_fileversion << "\" baudrate=\"" << m_baudrate << "\" buffer=\"" << getDataFileName().string() << "\" >" << std::endl;

  //read through file, save every m_baudrate line the frame number, timestamp and fileposition
  double last_timestamp = -1.;
  while(m_data_file.good())
  {
    getline(m_data_file, line);

    if (line.length() == 0 || line.substr(0,1) == "#")
    {
      //empty line or comment
      continue;
    }
    ++line_counter;

    // assure that time sequence of can file in order
    double timestamp = atof((line.substr(0, line.find(' ', 0))).data());
    if (last_timestamp > timestamp)
    {
      if (!icl_core::config::getDefault<bool>("/icl_hardware_can/force_meta_dump", false))
      {
        LOGGING_ERROR(icl_hardware::can::CAN, "Time sequence of can file out of order, line number: " << line_counter << "" << icl_core::logging::endl);
        return false;
      }
      else
      {
        std::cout << "<!-- Warning: Time sequence of can file out of order -->" << std::endl;
      }
    }

    if (next_commit == line_counter)
    {
      unsigned int time_sec = static_cast<int>(timestamp);

      std::cout << "<frame number=\"" << line_counter;
      std::cout << "\" timesec=\"" << time_sec;
      std::cout << "\" timeusec=\"" << static_cast<int>((timestamp - time_sec)*10000000);
      std::cout << "\" filepos=\"" << m_data_file.tellg() << "\">";
      std::cout << " </frame>" << std::endl;
      next_commit += m_baudrate;
    }
    last_timestamp = timestamp;
  }

  if (line_counter < 1)
  {
    return false;
  }
  else
  {
    return true;
  }
}

int tCanMessageMetaDumper::prepareFile()
{
  m_data_file.open(getDataFileName().string().c_str(), std::ios::in);

  if (!m_data_file.is_open())
  {
    return -1;
  }

  return 0;
}

void tCanMessageMetaDumper::setDataFileName(const boost::filesystem::path &file_name)
{
  if (m_data_file_name)
  {
    delete m_data_file_name;
  }
  m_data_file_name = new boost::filesystem::path(file_name);
}

const boost::filesystem::path & tCanMessageMetaDumper::getDataFileName() const
{
  return *m_data_file_name;
}

}

}
