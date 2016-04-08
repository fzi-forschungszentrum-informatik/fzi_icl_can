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
* \author  Thomas Schamm <schamm@fzi.de>
* \date    06.09.2011
*
*/
//----------------------------------------------------------------------



#ifndef _icl_hardware_can_CANMESSAGEHELPER_H
#define _icl_hardware_can_CANMESSAGEHELPER_H

#include "icl_core/BaseTypes.h"
#include "icl_hardware_can/tCanMessage.h"
#include "icl_hardware_can/tCanMatrixParser.h"

namespace icl_hardware {
namespace can {


//implemented for data with max. length of 16
//no sure what structure data with more than 16 bit has
void constructCanMessageData(unsigned char *_data, int value, const unsigned int start_bit, const unsigned int signal_length, bool little_endian)
{
  assert(start_bit > 0);
  assert(signal_length > 0);
  assert((start_bit-1)+signal_length <= 64);

  int64_t data = static_cast<int64_t>(value);
  int offset = 0;

  if (little_endian)
  {
    unsigned int number_of_bytes = static_cast<unsigned int>(((signal_length-1)/8))+1;
    int64_t tmp_data = 0;

    //change of byte position
    for (unsigned int i= 0; i < number_of_bytes; ++i)
    {
      tmp_data <<= 8;
      tmp_data |= (data & 0xFF);
      data >>= 8;
    }
    data = tmp_data;

    if (signal_length < 8)
    {
      offset =2*(start_bit%8 == 0? 1:start_bit%8)-8;
    }
  }

  //bring the data to its right position
  data <<= 64 - (start_bit-offset) - signal_length;

  //save data to provided adress
  for (int i=7; i >= 0; --i)
  {
    _data[i] = static_cast<unsigned char>(data & 0xFF);
    data >>= 8;
  }
}

double parseCanMessage(const tCanMessage &message, const unsigned int start_bit, const unsigned int signal_length, const bool little_endian, const bool signedness, const double factor, const double offset)
{
  /*
   * The "raw value" of a signal is the value as it is transmitted
   * over the network. From cantools library
   */
  uint32_t rawValue   = 0;

  /*
   * compute some signal properties
   *
   * signal bit order:
   *
   *     7  6  5  4  3  2  1  0 offset
   *    bit
   * 0   7  6  5  4  3  2  1  0
   * 1  15 14 13 12 11 10  9  8
   * 2  23 22 21 20 19 18 17 16
   * 3  31 30 29 28 27 26 25 24
   * 4  39 38 37 36 35 34 33 32
   * 5  47 46 45 44 43 42 41 40
   * 6  55 54 53 52 51 50 49 48
   * 7  63 62 61 60 59 58 57 56
   * |
   * start_byte
   *
   * big endian place value exponent
   *                15 14 13 12   <- start_byte
   *    11 10  9  8  7  6  5  4
   *     3  2  1  0               <- end_byte
   *
   * little endian place value exponent
   *     3  2  1  0               <- start_byte
   *    11 10  9  8  7  6  5  4
   *                15 14 13 12   <- end_byte
   */
  uint8_t  bit_len      = signal_length;
  uint8_t  start_offset = (start_bit-1) & 7;
  uint8_t  start_byte   = (start_bit-1) / 8;
  uint8_t  data;
  int8_t  work_byte;
  uint8_t  shift;

  /* align signal into ulong32 */
  /* 0 = Big Endian, 1 = Little Endian */
  if(little_endian == false) { /* big endian */
    uint8_t  end_byte     = start_byte + (7 + bit_len - start_offset - 1)/8;
    uint8_t  end_offset   = (start_offset - bit_len + 1) & 7;

    /* loop over all source bytes from start_byte to end_byte */
    for(work_byte = start_byte; work_byte <= end_byte; work_byte++) {
/* fetch source byte */
      data = message.data[work_byte];

/* process source byte */
      if(work_byte == start_byte && start_offset != 7) {
  /* less that 8 bits in start byte? mask out unused bits */
        data &= (uint8_t)~0 >> (7 - start_offset);
        shift = start_offset + 1;
      } else {
        shift = 8; /* use all eight bits */
      }
      if(work_byte == end_byte && end_offset != 0) {
  /* less that 8 bits in end byte? shift out unused bits */
        data >>= end_offset;
        shift -= end_offset;
      }

/* store processed byte */
      rawValue <<= shift; /* make room for shift bits */
      rawValue |= data;   /* insert new bits at low position */
    }
  } else {
    /* little endian - similar algorithm with reverse bit significance  */
    uint8_t  end_byte     = start_byte + (bit_len + start_offset - 1)/8;
    uint8_t  end_offset   = (start_offset + bit_len - 1) & 7;

    for(work_byte = end_byte; work_byte >= start_byte; work_byte--) {
      data = message.data[work_byte];
      if(work_byte == end_byte && end_offset != 7) {
        data &= (uint8_t)~0 >> (7 - end_offset);
        shift = end_offset + 1;
      } else {
        shift = 8;
      }
      if(work_byte == start_byte && start_offset != 0) {
        data >>= start_offset;
        shift -= start_offset;
      }
      rawValue <<= shift;
      rawValue |= data;
    }
  }

  double physicalValue;

  /* perform sign extension */
  if(signedness && (bit_len < 32)) {
    int32_t m = 1<< (bit_len-1);
    rawValue = ((int32_t)rawValue ^ m) - m;
  }

  /*
     * Factor, Offset and Physical Unit
     *
     * The "physical value" of a signal is the value of the physical
     * quantity (e.g. speed, rpm, temperature, etc.) that represents
     * the signal.
     * The signal's conversion formula (Factor, Offset) is used to
     * transform the raw value to a physical value or in the reverse
     * direction.
     * [Physical value] = ( [Raw value] * [Factor] ) + [Offset]
     */
  if(signedness) {
    physicalValue = (double)(int32_t)rawValue
        * factor + offset;
  } else {
    physicalValue = (double)        rawValue
        * factor + offset;
  }
  return physicalValue;
}

template <typename T>
static bool parseCanMessage(const tCanMessage &message,
                            const unsigned int start_bit,
                            const unsigned int signal_length,
                            const T factor,
                            const T offset,
                            const T lower_border,
                            const T upper_border,
                            const bool little_endian,
                            const bool signedness,
                            T &data)
{
  data = static_cast<T>(parseCanMessage(message, start_bit, signal_length, little_endian, signedness, factor, offset));
  if ( (data < lower_border) || (data > upper_border) )
  {
    return false;
  }
  return true;
}

template <typename T>
static bool parseCanMessage(const tCanMessage &message,
                            const CanMatrixElement &matrix,
                            T &data)
{
  data = static_cast<T>(parseCanMessage(message, matrix.start_bit, matrix.signal_length, matrix.little_endian, matrix.signedness, matrix.conversion, matrix.offset));
  if ( (data < matrix.lower_border) || (data > matrix.upper_border) )
  {
    return false;
  }
  return true;
}

}
}

#endif // _icl_hardware_can_CANMESSAGEHELPER_H
