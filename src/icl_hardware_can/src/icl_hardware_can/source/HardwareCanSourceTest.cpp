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
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-12
 *
 */
//----------------------------------------------------------------------
#include "HardwareCanSourceTest.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <icl_core_config/Config.h>
#include <icl_hardware_can/Logging.h>
#include <icl_sourcesink/SimpleURI.h>

namespace icl_hardware {
namespace can {

HardwareCanSourceTest::HardwareCanSourceTest(const std::string& uri, const std::string& name)
  : HardwareCanSource(uri, name),
    m_rng(),
    m_can_id_distribution(1, 2047),
    m_data_distribution(0, 255),
    m_buffer(),
    m_rate(100),
    m_pattern(P_LINEAR),
    m_next_id(1)
{
  icl_sourcesink::SimpleURI parsed_uri(uri);
  boost::optional<float> rate = parsed_uri.getQuery<float>("rate");
  if (rate)
  {
    m_rate = *rate;
  }

  boost::optional<std::string> pattern = parsed_uri.getQuery<std::string>("pattern");
  if (pattern)
  {
    if (*pattern == "Fixed")
    {
      m_pattern = P_FIXED;
    }
    else if (*pattern == "ChangingRandom")
    {
      m_pattern = P_CHANGING_RANDOM;
    }
    else if (*pattern == "ChangingWithRepeat")
    {
      m_pattern = P_CHANGING_WITH_REPEAT;
    }
    else if (*pattern == "Linear")
    {
      m_pattern = P_LINEAR;
    }
    else
    {
      LOGGING_WARNING_C(CAN, HardwareCanSourceTest,
                        "Unknown pattern '" << *pattern << "', falling back to Linear." << endl);
      m_pattern = P_LINEAR;
    }
  }

  if (m_pattern == P_FIXED)
  {
    boost::optional<uint16_t> id_value = parsed_uri.getQuery<uint16_t>("id_value");
    if (id_value)
    {
      if (*id_value > 0 && *id_value < 2048)
      {
        m_next_id = *id_value;
      }
      else
      {
        LOGGING_WARNING_C(CAN, HardwareCanSourceTest,
                          "ID value " << *id_value << ", out of range, starting at " << m_next_id << "." << endl);
      }
    }
  }

  advance();
}

bool HardwareCanSourceTest::advance()
{
  if (m_buffer)
  {
    // Simulate a frame rate.
    icl_core::os::usleep(long(1e6 / m_rate));
  }

  m_buffer.reset(
    new CanMessageStamped(
      tCanMessage(createCanMessageId()),
      icl_core::DataHeader(
        "",
        boost::posix_time::microsec_clock::local_time(),
        m_buffer ? m_buffer->header().sequence_number+1 : 0)));
  m_buffer->header().dsin = m_buffer->header().sequence_number;

  // Generate message payload.
  (*m_buffer)->rtr = 0;
  (*m_buffer)->dlc = 8;
  for (std::size_t i = 0; i < 8; ++i)
  {
    (*m_buffer)->data[i] = m_data_distribution(m_rng);
  }

  return true;
}

unsigned int HardwareCanSourceTest::createCanMessageId()
{
  unsigned int id;
  switch (m_pattern)
  {
    case P_FIXED:
    {
      id = m_next_id;
      break;
    }
    case P_CHANGING_RANDOM:
    {
      id = m_can_id_distribution(m_rng);
      break;
    }
    case P_CHANGING_WITH_REPEAT:
    {
      id = m_next_id;
      unsigned int factor(33), offset(54);
      m_next_id = (m_next_id % (2048 / factor)) + offset;
      break;
    }
    case P_LINEAR:
    default:
    {
      id = m_next_id++;
      if (m_next_id >= 2048)
      {
        m_next_id = 1;
      }
      break;
    }
  }

  return id;
}

}
}
