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
 * tCanMatrixParser.cpp
 * <section>
 *
 * Created by Hugo Ritzkowski on 2011-04-14.
 * Copyright 2011
 * Company Forschungszentrum Informatik (FZI), Abteilung IDS.
 * All rights reserved.
 *
 */
// ----------------------------------------------------------
/*!
 *  \file       tCanMatrixParser.cpp
 *  \author     <a href="mailto:ritzkow@fzi.de"> Hugo Ritzkowski </a>
 *  \date       14.04.2011
 *
 *  \brief
 *
 */
// ----------------------------------------------------------

#include "tCanMatrixParser.h"

#include "icl_hardware_can/Logging.h"
#include "icl_core_config/Config.h"

#include <boost/filesystem.hpp>
#include <map>
#include <tinyxml/tinystr.h>
#include <tinyxml/tinyxml.h>

#ifdef _IC_BUILDER_CANTOOLS_
extern "C" {
#include <dbcModel.h>
#include <dbcReader.h>
}
#endif

namespace icl_hardware {
namespace can {

tCanMatrixParser::tCanMatrixParser(const std::string identifier)
  : m_data_file_name(NULL),
    is_initialized(false)
{

  //get the filename of the xml-file
  if (identifier != "")
  {
    setDataFileName(boost::filesystem::path(identifier));
  }
  else
  {
    throw std::runtime_error("No CAN matrix provided!");
    return;
  }

  LOGGING_DEBUG(icl_hardware::can::CAN, "Opening CanMatrixParser with interpreter data from file: " << getDataFileName().string() << icl_core::logging::endl);

  int ret = 0;
#if !defined(BOOST_FILESYSTEM_VERSION) || BOOST_FILESYSTEM_VERSION == 2
  std::string extension = getDataFileName().extension();
#else
  std::string extension = getDataFileName().extension().string();
#endif

  if (extension == ".xml" || extension == ".canxml")
  {
    // open and check xml-file
    TiXmlHandle doc_handle(NULL);
    TiXmlDocument default_header_file(getDataFileName().string());

    if (default_header_file.LoadFile())
    {
      doc_handle = TiXmlHandle(&default_header_file);
      if (!doc_handle.FirstChildElement().ToElement())
      {
        // No valid root tag
        throw std::runtime_error("Found no root tag in " + getDataFileName().string());
        return;
      }
    }
    else
    {
      throw std::runtime_error("File " + getDataFileName().string() + " is not well-formed XML");
      return;
    }
    //get content of xml-file in std::map
    ret = mapContentOfFile(doc_handle);
  }
  else if (extension == ".dbc")
  {
#ifdef _IC_BUILDER_CANTOOLS_
    dbc_t* dbc;

    if (boost::filesystem::exists(getDataFileName()))
    {
      dbc = dbc_read_file(getDataFileName().string().c_str());
      if (!dbc->filename)
      {
        throw std::runtime_error("File " + getDataFileName().string() + " is not a valid DBC file");
        return;
      }
      ret = mapContentOfFile(dbc);
      dbc_free(dbc);
    }
    else
    {
      throw std::runtime_error("File " + getDataFileName().string() + " is not a valid file");
      return;
    }
#else
    throw std::runtime_error("Missing dbc support, please compile with cantools library.");
    return;
#endif
  }

  if(ret != 0)
  {
    throw std::runtime_error("CanMatrix could not be read.");
    return;
  }

  is_initialized = true;
}

tCanMatrixParser::~tCanMatrixParser()
{
  if (m_data_file_name != NULL)
  {
    delete m_data_file_name;
    m_data_file_name = NULL;
  }
}

const CanMatrix& tCanMatrixParser::getCanMatrix() const
{
  return m_canmatrix;
}

int tCanMatrixParser::mapContentOfFile(TiXmlHandle &doc_handle)
{
  //    <icl_hardware_can>
  //      <can_matrix>
  //        <message>
  //          <canid>32</canid>
  //          <conversion>0.001</conversion>
  //          <description>The velocity of the left wheel</description>
  //          <unit>1/s</unit>
  //          <data_start_bit> 2 </data_start_bit>  (index 0..63)
  //          <signal_length> 2 </signal_length>
  //          <offset> 10. </offset>
  //          <lower_bound> 0. </lower_bound>
  //          <upper_bound> 1. </upper_bound>
  //          <little_endian> 1 </little_endian>
  //          <signedness> 0 </signedness>
  //        </message>
  //        ...
  //      </can_matrix>
  //    </icl_hardware_can>

  TiXmlElement * element = doc_handle.FirstChildElement().ToElement();
  if (std::string(element->Value()) != "icl_hardware_can")
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "Found no <icl_hardware_can> root tag in " << getDataFileName().string() << icl_core::logging::endl);
    return -1;
  }

  TiXmlElement* message_data = doc_handle.FirstChildElement().FirstChildElement().FirstChildElement().ToElement();
  if (std::string(message_data->Value()) == "message")
  {
    unsigned int id = 0;

    struct CanMatrixElement can_matrix_data = {"", 0., "", 0 , 0 , 0., 0., 0., false, false};

    if (checkXmlFile(message_data) == 0)
    {
      do
      {
        id = atoi(message_data->FirstChild("canid")->ToElement()->GetText());
        can_matrix_data.conversion = atof(message_data->FirstChild("conversion")->ToElement()->GetText());
        can_matrix_data.description = message_data->FirstChild("description")->ToElement()->GetText();
        can_matrix_data.unit = message_data->FirstChild("unit")->ToElement()->GetText();
        can_matrix_data.start_bit = atoi(message_data->FirstChild("data_start_bit")->ToElement()->GetText());
        can_matrix_data.signal_length = atoi(message_data->FirstChild("signal_length")->ToElement()->GetText());
        can_matrix_data.offset = atof(message_data->FirstChild("offset")->ToElement()->GetText());
        can_matrix_data.lower_border = atof(message_data->FirstChild("lower_border")->ToElement()->GetText());
        can_matrix_data.upper_border = atof(message_data->FirstChild("upper_border")->ToElement()->GetText());
        can_matrix_data.little_endian = bool(atoi(message_data->FirstChild("little_endian")->ToElement()->GetText()));
        can_matrix_data.signedness = bool(atoi(message_data->FirstChild("signedness")->ToElement()->GetText()));

        CanMatrix::iterator iter = m_canmatrix.find(id);

        if (iter != m_canmatrix.end())
        {
          //! ID is known and vector has to be extented
          iter->second.insert(iter->second.end(), can_matrix_data);
        }
        else
        {
          //! ID is unknown, make a new pair
          m_canmatrix.insert(std::pair<unsigned int, std::vector<CanMatrixElement> >(id, std::vector<CanMatrixElement>(1, can_matrix_data)));
        }

      }
      while ((message_data = message_data->NextSiblingElement()));
      message_data = NULL;
    }
    else
    {
      LOGGING_ERROR(icl_hardware::can::CAN, "Structure of xml-file is not compatible."  << icl_core::logging::endl);
      LOGGING_ERROR(icl_hardware::can::CAN, "Missing child element!" << icl_core::logging::endl);
      return -1;
    }
  }
  else
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "Structure of xml-file is not compatible " << getDataFileName().string() << icl_core::logging::endl);
    return -1;
  }
  element = NULL;

  return 0;
}

const int tCanMatrixParser::checkXmlFile(TiXmlElement *element) const
{

  if (element->FirstChild("canid") && element->FirstChild("conversion") && element->FirstChild("description")
      && element->FirstChild("unit") && element->FirstChild("data_start_bit") && element->FirstChild("signal_length")
      && element->FirstChild("offset") && element->FirstChild("lower_border") && element->FirstChild("upper_border")
      && element->FirstChild("little_endian") && element->FirstChild("signedness") )
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

int tCanMatrixParser::mapContentOfFile(void *dbc_)
{
#ifndef _IC_BUILDER_CANTOOLS_
  return -1;
#else
  dbc_t *dbc = reinterpret_cast<dbc_t*>(dbc_);

  if (!dbc)
  {
    return -1;
  }

  message_list_t *ml;
  signal_list_t *sl;

  for(ml = dbc->message_list; ml != NULL; ml = ml->next) {
    unsigned int id = ml->message->id;

    for(sl = ml->message->signal_list; sl != NULL; sl = sl->next) {
      struct CanMatrixElement can_matrix_data = {"", 0., "", 0 , 0 , 0., 0., 0., false, false};

      can_matrix_data.conversion = sl->signal->scale;
      can_matrix_data.description = sl->signal->name;
      can_matrix_data.unit = sl->signal->unit?sl->signal->unit:"";
      can_matrix_data.start_bit = sl->signal->bit_start + 1;
      can_matrix_data.signal_length = sl->signal->bit_len;
      can_matrix_data.offset = sl->signal->offset;
      can_matrix_data.lower_border = sl->signal->min;
      can_matrix_data.upper_border = sl->signal->max;
      can_matrix_data.little_endian = sl->signal->endianess;
      can_matrix_data.signedness = sl->signal->signedness;

      CanMatrix::iterator iter = m_canmatrix.find(id);

      if (iter != m_canmatrix.end())
      {
        //! ID is known and vector has to be extented
        iter->second.insert(iter->second.end(), can_matrix_data);
      }
      else
      {
        //! ID is unknown, make a new pair
        m_canmatrix.insert(std::pair<unsigned int, std::vector<CanMatrixElement> >(id, std::vector<CanMatrixElement>(1, can_matrix_data)));
      }
    }
  }

#endif
  return 0;
}

void tCanMatrixParser::setDataFileName(const boost::filesystem::path &file_name)
{
  if (m_data_file_name)
  {
    delete m_data_file_name;
  }
  m_data_file_name = new boost::filesystem::path(file_name);
}

const boost::filesystem::path & tCanMatrixParser::getDataFileName() const
{
  return *m_data_file_name;
}

const bool tCanMatrixParser::isActive() const
{
  return is_initialized;
}

std::ostream &operator<<(std::ostream &stream, const CanMatrix& can_matrix)
{
//  icl_graphics::tCameraParameterIO::WriteDeviceTag(&camera_parameter_provider, stream);
  stream << can_matrix.size();
  return stream;
}

}
}
