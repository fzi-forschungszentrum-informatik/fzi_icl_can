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
 * \author  Tobias Luksch, Technische Universitaet Kaiserslautern
 * \author  Klaus Uhl <uhl@fzi.de>
 * \author Georg Heppner<heppner@fzi.de>
 * \date    2005-09-28
 * \date    2015-10-27
 *
 * \brief   Contains compability definitions for using the Linux CAN driver by PEAK inside MCA.
 *
 */
//----------------------------------------------------------------------
#ifndef _icl_hardware_can_UsePeakCan_h_
#define _icl_hardware_can_UsePeakCan_h_

#ifdef _SYSTEM_LINUX_
# include <fcntl.h>
# include <unistd.h>
# include <sys/ioctl.h>
# include <string.h>
# include <errno.h>
# include <libpcan.h>
#endif

#include "icl_hardware_can/tCanMessage.h"
#include "icl_hardware_can/UseCanNoLxrt.h"
#include "Logging.h"

namespace icl_hardware {
namespace can {

typedef HANDLE tCanDescriptor;
inline bool CanDescriptorValid(tCanDescriptor can_device)
{
  return can_device != 0;
}
inline tCanDescriptor InvalidCanDescriptor()
{
  return 0;
}
inline const char* CanDriverName()
{
  return "PEAK-CAN";
}

inline WORD BaudRateSpecifier( unsigned int baud_rate )
{
  switch( baud_rate )
  {
    case 1000:
     return CAN_BAUD_1M;
    case 500:
     return CAN_BAUD_500K;
    case 250:
      return CAN_BAUD_250K;
    case 100:
      return CAN_BAUD_100K;
    case 50:
      return CAN_BAUD_50K;
    case 20:
      return CAN_BAUD_20K;
    case 10:
      return CAN_BAUD_10K;
    case 5:
      return CAN_BAUD_5K;

    default:
      LOGGING_ERROR( CAN, "Peak Can baud rate " << baud_rate << " not valid. Setting the baud rate to 1MB " << endl);
      return CAN_BAUD_1M;
  }
}
//! Open a can device and set parameters
/*!
  Function for open a can device and setting the corresponding parameters via ioctl.
  \param device_name The linux device name of the can device (e.g. '/dev/pcan0')
  \param flags       Flags for the linux open() call (e.g. 'O_RDWR|O_NONBLOCK')
  \param acceptance_code The acceptance_code of the CAN-card (ignored for peak can)
  \param acceptance_mask The acceptance_mask of the CAN-card (ignored for peak can)
  \param baud_rate   Baud rate for can communication in kbits
  \param send_fifo_size Size of the can driver send fifo. (ignored for peak can)
  \param receive_fifo_size Size of the can driver receive fifo. (ignored for peak can)
  Returns the file descriptor of the can device on success, \c -1 on failure.

  Be aware: the CAN-card parameters (\a acceptance_code, \a acceptance_mask, \a baud_rate)
  are set globally for the CAN-card and so for all registered users so far!

  Be aware: If there are more than one FIFO-user,
  make sure all users call their read() function periodically.
  Otherwhise the FIFO is running full so that \c NO user is able to receive further data any more.

 */
inline tCanDescriptor CanDeviceOpen(const char * device_name, int flags,
                                    unsigned char acceptance_code, unsigned char acceptance_mask, unsigned int baud_rate,
                                    unsigned send_fifo_size, unsigned receive_fifo_size)
{
  printf("flags:%i\n",flags);
  HANDLE handle;
  handle = LINUX_CAN_Open((char*) device_name, flags);

  // Timing problems when CAN_Init is called too early
  sleep(1);

  WORD br = BaudRateSpecifier( baud_rate );

  if ( CAN_Init(handle, br  , CAN_INIT_TYPE_ST ))
  {
    printf("Invalid CanDescriptor!\n");
    return InvalidCanDescriptor();
  }
  else
  {
    printf("Can Init successful!\n");
    return handle;
  }
}

inline int CanDeviceClose(tCanDescriptor _can_device)
{
  return CAN_Close(_can_device);
}

inline int CanDeviceSend(tCanDescriptor _can_device, const tCanMessage &msg)
{
  if (!_can_device)
    return -ENODEV;

  // return write(_can_device,&msg,sizeof(tCanMessage));

  TPCANMsg pmsg;
  pmsg.ID = msg.id;
  pmsg.MSGTYPE = MSGTYPE_STANDARD;
  // pmsg.MSGTYPE = msg.rtr;
  pmsg.LEN = msg.dlc;
  memcpy(pmsg.DATA, msg.data, pmsg.LEN);

  //   INFOMSG( "writing can msg: id:%d (0x%02x), type:%d, len:%d, data: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",  pmsg.ID, pmsg.ID, pmsg.MSGTYPE, pmsg.LEN, pmsg.DATA[ 0 ], pmsg.DATA[ 1 ], pmsg.DATA[ 2 ], pmsg.DATA[ 3 ],  pmsg.DATA[ 4 ], pmsg.DATA[ 5 ], pmsg.DATA[ 6 ], pmsg.DATA[ 7 ] );

  return CAN_Write(_can_device, &pmsg);
}

inline int CanDeviceReceive(tCanDescriptor _can_device, tCanMessage &msg)
{
  if (!_can_device)
    return -ENODEV;

  // Raw Can Message
  TPCANRdMsg pmsg;



  // DM("reading can msg.\n");
  /// Read from the the device handle.
  /// RET can take any of the following:
  /// CAN_ERR_QRCVEMPTY in case no data was read
  /// -1 in case a select function of the system failed..
  /// -1 in case the system ioctl fails (basic way to read the interface).
  /// 0 in case of succes
  int ret = LINUX_CAN_Read_Timeout(_can_device, &pmsg, 0);
  // DM("done\n");
  //DM("recieved msg\n");

  // In Case of an IOCTL or SELECT Error we could not even open the hardware itself. We will immediately terminate and retun a generic IO Error
  // It should only be -1 as error code but in case it isnt we will print it out here.
  if (ret < 0)
  {
    LOGGING_ERROR( CAN, "CAN DEVICE HANDLE CRITICAL ERROR. COULD NOT READ PROPERLY FROM DEVICE. Error Code: " << ret << endl);
    return -EIO; // Generic IO error
  }

  // Buffer is empty is the only return value still given as ret. all other errors need to be handled differently
  if (ret == CAN_ERR_QRCVEMPTY)
  {
    // As "no data" is actually no error we will just stop the read here"
    return -ENODATA;
  }

  // since pcan 6.x the error handling is changed and facilitates the Data field for error messages
  if (pmsg.Msg.MSGTYPE == MSGTYPE_STATUS )
  {
    // All 8 Data bytes are always present in the message so it is save to just read any of them without checking the lenght.
    // Hoever, it might contain garbage.

    // This should not happen but just to be save
    if (pmsg.Msg.DATA[3] == CAN_ERR_QRCVEMPTY)
    {
      return -ENODATA;
    }

    LOGGING_ERROR( CAN, "CAN ERROR DETECTED:  ");
    // Error Values are stored in data[3]!
    switch (pmsg.Msg.DATA[3])
    {
      case CAN_ERR_OK:            // no error
          LOGGING_ERROR( CAN, "no error code given in an error message....check protocol" << endl);
        break;
      case CAN_ERR_XMTFULL:       // transmit buffer full
        LOGGING_ERROR( CAN, "transmit Buffer is full" << endl);
        return -ENOBUFS;
        break;
      case CAN_ERR_OVERRUN:       // overrun in receive buffer
        LOGGING_ERROR( CAN, "ovverrun in receive buffer" << endl);
        return -ENOBUFS;
        break;
      case CAN_ERR_BUSLIGHT:      // bus error, errorcounter limit reached
        LOGGING_ERROR( CAN, "BUSLIGHT, errorcounter limit reached, please check your cable setup" << endl);
        return -EIO;
        break;
      case CAN_ERR_BUSHEAVY:      // bus error, errorcounter limit reached
        LOGGING_ERROR( CAN, "BUSHEAVY, errorcounter limit reached, please check your cable setup" << endl);
        return -EIO;
        break;
      case CAN_ERR_BUSOFF:        // bus error, 'bus off' state entered
        LOGGING_ERROR( CAN, "BUSOFF, 'bus off' state entered" << endl);
        return -EIO;
        break;
      case CAN_ERR_QOVERRUN:      // receive queue overrun
        LOGGING_ERROR( CAN, "receive queue overrun" << endl);
        return -ENOBUFS;
        break;
      case CAN_ERR_QXMTFULL:      // transmit queue full
        LOGGING_ERROR( CAN, "transmit queue full" << endl);
        return -ENOBUFS;
        break;
      case CAN_ERR_REGTEST:       // test of controller registers failed
        LOGGING_ERROR( CAN, "test of controller registers failed" << endl);
        return -EPROTO;
        break;
      case CAN_ERR_NOVXD:         // Win95/98/ME only
        LOGGING_ERROR( CAN, "NO VXD (win95 issue...)" << endl);
        return -EPROTO;
        break;
      case CAN_ERR_RESOURCE :     // can't create resource
        LOGGING_ERROR( CAN, "can't create resource" << endl);
        return -EPROTO;
        break;
      case CAN_ERR_ILLPARAMTYPE:  // illegal parameter
        LOGGING_ERROR( CAN, "illegal parameter" << endl);
        return -EPROTO;
        break;
      case CAN_ERR_ILLPARAMVAL:   // value out of range
        LOGGING_ERROR( CAN, "value out of range" << endl);
        return -EPROTO;
        break;
      case CAN_ERRMASK_ILLHANDLE: // wrong handle, handle error
        LOGGING_ERROR( CAN, "wrong handle, handle error" << endl);
        return -EPROTO;
        break;
      default:
        LOGGING_ERROR( CAN, "Unknown error : "<< pmsg.Msg.DATA[3]  << endl);
        // TODO:
        // do something usefull herer
        return -ENODATA;
        break;
    }
  }

  // At this point we have Extended, RTR and Standard messages left.
  // Standard can just be parsed, extended too, but we need to check the RTR
  // As this is only 0x01 in case of an RTR type message


  /* DM( "reading can msg: id:%d (0x%02x), type:%d, len:%d,
                data: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                pmsg.Msg.ID, pmsg.Msg.ID, pmsg.Msg.MSGTYPE, pmsg.Msg.LEN,
                pmsg.Msg.DATA[ 0 ], pmsg.Msg.DATA[ 1 ], pmsg.Msg.DATA[ 2 ], pmsg.Msg.DATA[ 3 ],
                pmsg.Msg.DATA[ 4 ], pmsg.Msg.DATA[ 5 ], pmsg.Msg.DATA[ 6 ], pmsg.Msg.DATA[ 7 ] );*/

  msg.id = pmsg.Msg.ID;
  msg.dlc = pmsg.Msg.LEN;
  if (pmsg.Msg.MSGTYPE == MSGTYPE_EXTENDED || pmsg.Msg.MSGTYPE == MSGTYPE_STATUS)
  {
    msg.rtr = 0;
  }
  else
  {
    // MSG Type Standard = 0x00 ,RTR = 0x01
    msg.rtr = pmsg.Msg.MSGTYPE;
  }

  memcpy(msg.data, pmsg.Msg.DATA, pmsg.Msg.LEN);

  // As all the errors have been captured before we just need to return something >0 in case of a succesfull read. This should be the case with msg.len
  return pmsg.Msg.LEN;
}

inline int CanDeviceReset(tCanDescriptor _can_device)
{
  if (!_can_device)
    return -ENODEV;
  return 0;
}

}
}

#endif
