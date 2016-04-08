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
 * \date    2010-12-21
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-16
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SINKS_H_INCLUDED
#define ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SINKS_H_INCLUDED

#include <icl_hardware_can/StreamOperators.h>
#include <icl_hardware_can/sink/HardwareCanSink.h>
#include <icl_hardware_can/sink/HardwareCanSinkCanfile.h>
// #include <icl_hardware_can/sink/HardwareCanSinkInterpretMessage.h>

#if not defined _SYSTEM_WIN32_
#include <icl_hardware_can/sink/HardwareCanSinkPeak.h>
#endif

// #include <icl_hardware_can/sink/HardwareCanSinkFactory.h>

#endif

