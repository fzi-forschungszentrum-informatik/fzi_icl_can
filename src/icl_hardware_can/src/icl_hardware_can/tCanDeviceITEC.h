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
 * \brief   Contains tCanDevice
 *
 * \b tCanDevice
 *
 * Class for accessing the can device.
 *
 */
//----------------------------------------------------------------------
#ifndef _icl_hardware_can_tCanDevciceITEC_h_
#define _icl_hardware_can_tCanDevciceITEC_h_

#ifdef _IC_BUILDER_CAN_ITEC_
# include "icl_hardware_can/UseITECCan.h"
# include "icl_hardware_can/tCanDeviceT.h"
namespace icl_hardware {
namespace can {
typedef tCanDeviceT<tCanDescriptor> tCanDeviceITEC;
}
}
#else
# error "ITEC CAN Driver not available"
#endif


#endif
