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
 * \author  Klaus Uhl <uhl@fzi.de>
 * \date    2008-10-27
 *
 * \brief   Contains CAN driver interface functions
 *
 *
 */
//----------------------------------------------------------------------
#ifndef _icl_hardware_can_tCanMessage_h_
#define _icl_hardware_can_tCanMessage_h_

#include <map>
#include <boost/shared_ptr.hpp>

#include <icl_core/DataHeader.h>

#include "icl_hardware_can/ImportExport.h"

namespace icl_hardware {
namespace can {

//! Implements a struct representing a can message
/*! This struct implements a can message type with components \a id, \a dlc, \a rtr and
 * \a data.
 */
struct ICL_HARDWARE_CAN_IMPORT_EXPORT tCanMessage
{
  unsigned id;
#ifdef _IC_BUILDER_CAN_ITEC_
  unsigned char eff;       /* Extended frame format bit */
#endif
  unsigned char dlc;
  unsigned char rtr;
  unsigned char data[8];
#ifdef _IC_BUILDER_CAN_ITEC_
  struct timeval tv; /* Timestamp */
#endif

  /*!
   * Creates a CAN message from scratch.
   */
  tCanMessage(unsigned param_id = 0,
              unsigned char param_dlc = 0,
              unsigned char param_rtr = 0,
              const unsigned char* param_data = 0);

  /*!
   * Creates a CAN message as a copy of another CAN message.
   */
  tCanMessage(const tCanMessage &other);

  /*!
   * Assigns another CAN message to this message.
   */
  tCanMessage& operator = (const tCanMessage &other);

};

/*! A time-stamped CAN message.  Replaces the obsolete
 *  tCanMessageTimed.
 */
typedef icl_core::Stamped<tCanMessage> CanMessageStamped;
//! Maps CAN IDs to the latest received message for each ID.
typedef std::map<unsigned, CanMessageStamped> CanMessageMap;
//! Convenience shared pointer shorthand for CanMessageMap.
typedef boost::shared_ptr<CanMessageMap> CanMessageMapPtr;
//! Convenience shared pointer shorthand for CanMessageMap (const version).
typedef boost::shared_ptr<const CanMessageMap> CanMessageMapConstPtr;

}
}

#endif
