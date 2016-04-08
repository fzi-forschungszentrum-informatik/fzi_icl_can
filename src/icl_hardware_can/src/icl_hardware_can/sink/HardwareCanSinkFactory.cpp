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
 * tHardwareCanSinkFactory.cpp
 * <section>
 *
 * Created by Hugo Ritzkowski on 21.12.2010.
 * Copyright 2010
 * Company Forschungszentrum Informatik (FZI), Abteilung IDS.
 * All rights reserved.
 *
 */
// ----------------------------------------------------------
/*!
 *  \file       tHardwareCanSinkFactory.cpp
 *  \author     <a href="mailto:ritzkow@fzi.de"> Hugo Ritzkowski </a>
 *  \date       23.12.2010
 *
 *  \brief      A factory producing tHardwareSink objects.
 *
 */
// ----------------------------------------------------------

#include "tHardwareCanSinkFactory.h"
#include "tHardwareCanSinkCanfile.h"
#include "tHardwareCanSinkConsole.h"
#include "tHardwareCanSinkInterpretMessage.h"
#ifdef _SYSTEM_POSIX_
#include "tHardwareCanSinkPeak.h"
#endif

#include "icl_hardware_can/Logging.h"
#include "icl_core/SchemeParser.h"
#include <icl_core_config/Config.h>

namespace icl_hardware {
namespace can {

tHardwareCanSinkFactory::tHardwareCanSinkFactory()
{

}

tHardwareCanSinkFactory::~tHardwareCanSinkFactory()
{

}

tHardwareCanSink * tHardwareCanSinkFactory::Construct(std::string specifier)
{
    icl_core::SchemeParser parser;

    if (specifier == "")
    {
      LOGGING_DEBUG(icl_hardware::can::CAN, "No specifier was set, read settings from configfile" << "\n");
      specifier = interpretConfigFile();
    }

    if (parser.parseScheme(specifier))
    {
      // convert to lowercase
      std::string lspecifier(parser.getSchemeResult().specifier);
      for (unsigned int i = 0; i < lspecifier.size(); ++i)
      {
        lspecifier[i] = tolower(lspecifier[i]);
      }

      LOGGING_DEBUG(icl_hardware::can::CAN, "tHardwareCanSinkFactory parser result: \n");
      LOGGING_DEBUG(icl_hardware::can::CAN, " scheme_name: "<< parser.getSchemeResult().scheme_name << "\n");
      LOGGING_DEBUG(icl_hardware::can::CAN, " specifier: "<< parser.getSchemeResult().specifier << "\n");
      LOGGING_DEBUG(icl_hardware::can::CAN, " anchor: "<< parser.getSchemeResult().anchor << "\n");
      LOGGING_DEBUG(icl_hardware::can::CAN, " #queries: "<< parser.getSchemeResult().queries.size() << icl_core::logging::endl);

      if (parser.getSchemeResult().scheme_type == icl_core::FileScheme)
      {
        tHardwareCanSinkCanfile * hardware_data_sink = new tHardwareCanSinkCanfile();
        icl_core::QueryList query_list = parser.getSchemeResult().queries;
        unsigned int baud_rate = 0;
        for (unsigned int i=0; i<query_list.size(); ++i)
        {
          if (query_list[i].name == "baud")
          {
            baud_rate = atoi(query_list[i].value.c_str());
          }
        }
        if (baud_rate != 0)
        {
          hardware_data_sink->Open(parser.getSchemeResult().specifier, baud_rate);
        }
        else
        {
          hardware_data_sink->Open(parser.getSchemeResult().specifier);
        }
        return hardware_data_sink;
      }
      else if (parser.getSchemeResult().scheme_type == icl_core::OtherScheme)
      {
        if (parser.getSchemeResult().scheme_name == "console://")
        {
          if (icl_core::config::getDefault<bool>("/icl_hardware_can/use_interpret", false))
          {
            icl_core::String can_matrix = icl_core::config::getDefault<icl_core::String>("/icl_hardware_can/can_mask", "");
            if (can_matrix != "")
            {
              tHardwareCanSinkInterpretMessage * hardware_data_sink = new tHardwareCanSinkInterpretMessage();
              hardware_data_sink->Open(parser.getSchemeResult().specifier, can_matrix);
              return hardware_data_sink;
            }
            else
            {
              LOGGING_ERROR(icl_hardware::can::CAN, "Interpretation sink must be used together with CAN mask!" << icl_core::logging::endl);
            }
          }
          else
          {
            tHardwareCanSinkConsole * hardware_data_sink = new tHardwareCanSinkConsole();
            hardware_data_sink->Open(parser.getSchemeResult().specifier);
            return hardware_data_sink;
          }
        }
        if (parser.getSchemeResult().scheme_name == "can://")
        {
#ifdef _SYSTEM_POSIX_
          tHardwareCanSinkPeak * hardware_data_sink = new tHardwareCanSinkPeak();
          icl_core::QueryList query_list = parser.getSchemeResult().queries;
          unsigned int baud_rate = 0;
          for (unsigned int i=0; i<query_list.size(); ++i)
          {
            if (query_list[i].name == "baud")
            {
              baud_rate = atoi(query_list[i].value.c_str());
            }
          }
          if (baud_rate != 0)
          {
            hardware_data_sink->Open(parser.getSchemeResult().specifier, baud_rate);
          }
          else
          {
            hardware_data_sink->Open(parser.getSchemeResult().specifier);
          }

          return hardware_data_sink;
#else
          LOGGING_WARNING(icl_hardware::can::CAN, "can:// devices only available on posix platforms.");
          return NULL;
#endif
        }
      }

      LOGGING_ERROR(icl_hardware::can::CAN, "Could not distinguish HardwareSink from scheme \"" << specifier << "\"!" << icl_core::logging::endl);
    }
    else
    {
      LOGGING_ERROR(icl_hardware::can::CAN, "Failed to parse scheme \"" << specifier << "\"!" << icl_core::logging::endl);
    }

    return NULL;
}

std::string tHardwareCanSinkFactory::interpretConfigFile()
{
  std::string specifier;

  icl_core::String sink_specifier = icl_core::config::getDefault<icl_core::String>("/icl_hardware_can/sink_factory/sink", "");
  icl_core::String device_name = icl_core::config::getDefault<icl_core::String>("/icl_hardware_can/sink_factory/device", "");
  icl_core::String baud_rate = icl_core::config::getDefault<icl_core::String>("/icl_hardware_can/sink_factory/baudrate", "");

  specifier = sink_specifier + device_name;

  if (sink_specifier == "can://")
  {
    if (baud_rate != "")
    {
      specifier.append("?baud=" + baud_rate);
    }
  }
  else if (sink_specifier == "file://")
  {
    if (baud_rate != "")
    {
      specifier.append("baud=" + baud_rate);
    }
  }

  return specifier;
}

}
}
