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
#include "tCanFilterMessages.h"

#include <map>
#include <boost/filesystem.hpp>

#include "icl_hardware_can/Logging.h"

namespace icl_hardware {
namespace can {

tCanFilterMessages::tCanFilterMessages()
  : m_data_file_name(NULL),
    is_initialized(false)
{
}

tCanFilterMessages::~tCanFilterMessages()
{
  if (m_data_file_name != NULL)
  {
    delete m_data_file_name;
    m_data_file_name = NULL;
  }
}

void tCanFilterMessages::Open(const std::string identifier)
{
  //get the filename of the can-message-file
  if (identifier != "")
  {
    setDataFileName(boost::filesystem::path(identifier));
  }
  else
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "No data file provided!" << icl_core::logging::endl);
    return;
  }

  LOGGING_DEBUG(icl_hardware::can::CAN, "Opening canmatrixParser with data from file: " << getDataFileName().string() << icl_core::logging::endl);

  // open and check can-file
  int ret=PrepareFiles();

  if (ret!=0)
  {
    LOGGING_ERROR(icl_hardware::can::CAN, "No such file as: " << identifier << icl_core::logging::endl);
    return;
  }

  mapContentOfFile();

  is_initialized = true;
}

bool tCanFilterMessages::checkMessage(const icl_hardware::can::tCanMessage& message) const
{
  std::map<unsigned int, DataWrapper>::const_iterator iter = m_table_id_to_data.find(message.id);

  if (iter != m_table_id_to_data.end())
  {
    for (unsigned int i=0; i < 8; i++)
    {
      //! check if data is different and actual byte is not known as changing byte
      if (message.data[i] != iter->second.data[i] && !byteIsChangingConstantly(i, message.id))
      {
        return true;
      }
    }
  }
  else
  {
    return true; //!< id hasn't been listed before
  }

  return false;
}

bool tCanFilterMessages::byteIsChangingConstantly(unsigned int i, unsigned int id) const
{
  std::multimap<unsigned int, unsigned int>::const_iterator iter_multi = m_table_id_to_changing_bytes.find(id);
  if (iter_multi != m_table_id_to_changing_bytes.end())
  {
    while (id == iter_multi->first)
    {
      if (iter_multi->second == i)
      {
        return true;
      }
      iter_multi++;
    }
  }
  return false;
}

void tCanFilterMessages::mapContentOfFile()
{
  std::string line;
  icl_hardware::can::tCanMessage message;

  while (m_data_file.good())
  {
    getline(m_data_file, line);
    if (line.length() == 0)
    {
      //lines with no value are ignored
      continue;
    }

    //copy of line has to be used, because in the actuell version, the string is cut in pieces
    StringToCanMsg(message, line);

    //if a comment or an empty line was read, message.empty()
    if (message.id != 0 || message.dlc != 0 || message.rtr != 0)
    {
      std::map<unsigned int, DataWrapper>::iterator iter = m_table_id_to_data.find(message.id);

      if (iter == m_table_id_to_data.end()) //!< id is not known up to this point, save its id and data in map
      {
        LOGGING_TRACE(icl_hardware::can::CAN, "id: " << message.id << " is newly registered" << icl_core::logging::endl);
        memcpy(m_message_data.data, message.data, sizeof(message.data));
        m_table_id_to_data.insert(std::pair<unsigned int, DataWrapper>(message.id, m_message_data));
      }
      else
      {
        //! id is known, analyse if there has been a change in bytes
        for (unsigned int i=0; i < 8; i++)
        {
          if (iter->second.data[i] != message.data[i]) //!< check if data is not the same as saved data
          {
            std::multimap<unsigned int, unsigned int>::iterator iter_multi = m_table_id_to_changing_bytes.find(iter->first);
            bool known_byte = false;

            if (iter_multi != m_table_id_to_changing_bytes.end())
            {
              //! check if actual byte is allready known as changing byte otherwise register it
              while(iter_multi->first == message.id && !known_byte)
              {
                if (iter_multi->second == i)
                {
                  known_byte = true;
                }
                iter_multi++;
              }
            }
            if (!known_byte)
            {
              LOGGING_TRACE(icl_hardware::can::CAN, "id: " << message.id << " - add byte: " << i << icl_core::logging::endl);
              m_table_id_to_changing_bytes.insert(std::pair <unsigned int, unsigned int> (message.id, i));
            }
          }
        }
      }
    }
  }
}

void tCanFilterMessages::StringToCanMsg(icl_hardware::can::tCanMessage &message, std::string str)
{
  unsigned int pos = 0;
  int word_length = 0;

  if (str.substr(0,1) == "#")
  {
    // skip lines beginning with a comment
    return;
  }

  //FZI-CAN FILE V1.0 has 11 values
  for (int i=0; i < 11; i++)
  {
    size_t position = str.find(' ', pos);
    if (position == std::string::npos)
    {
      word_length = str.size() - pos;
    }
    else
    {
      word_length = position - pos;
    }

    switch(i)
    {
      case 0:
      {
        // message.gmtime = atof((str.substr(pos, word_length)).data());
        break;
      }
      case 1:
      {
        message.id = atoi((str.substr(pos, word_length)).data());
        break;
      }
      case 2:
      {
        message.dlc = atoi((str.substr(pos, word_length)).data());
        break;
      }
      default:
      {

        message.data[i-3] = (unsigned char)(strtol((str.substr(pos, word_length)).data(), NULL, 16));
        break;
      }
    }

    pos += word_length+1;
  }
}

int tCanFilterMessages::PrepareFiles()
{
  //open files
  m_data_file.open(getDataFileName().string().c_str(), std::ios::in);

  //check all files are open
  if (!m_data_file.is_open())
  {
    return -1;
  }

  return 0;
}

void tCanFilterMessages::setDataFileName(const boost::filesystem::path &file_name)
{
  if (m_data_file_name)
  {
    delete m_data_file_name;
  }
  m_data_file_name = new boost::filesystem::path(file_name);
}

const boost::filesystem::path& tCanFilterMessages::getDataFileName() const
{
  return *m_data_file_name;
}

bool tCanFilterMessages::getStatus() const
{
  return is_initialized;
}

}
}
