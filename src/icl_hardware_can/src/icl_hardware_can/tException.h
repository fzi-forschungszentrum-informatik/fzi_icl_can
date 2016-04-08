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
 * tException
 * icl_hardware
 *
 * Created by Hugo Ritzkowski on 2012-02-07.
 * Copyright 2012
 * Company Forschungszentrum Informatik (FZI), Abteilung IDS.
 * All rights reserved.
 *
 */
// ----------------------------------------------------------
/*!
 *  \file       tException.h
 *  \author     <a href="mailto:ritzkow@fzi.de">HUgo Ritzkowski</a>
 *  \date       2012-02-07
 *
 *  \brief      Contains icl_hardware::tException
 *
 */
// ----------------------------------------------------------

#ifndef ICL_HARDWARE_TEXCEPTION_H
#define ICL_HARDWARE_TEXCEPTION_H

#include <string>
#include <exception>

namespace icl_hardware
{
namespace can
{
/** Known exception types */
enum Hardware_Exception
{
  eHARDWARE_ERROR,
  ePEAK_DEVICE_ERROR,
  eTHREAD_ERROR,
  eMESSAGE_ERROR,
  eFILE_ERROR
};

class tException : public std::exception
{
public:
  /** Constructor for exception type and custom description */
  tException(Hardware_Exception exception, const std::string &_description)
    : description(_description),
      error_code(exception)
  {};

  /** Constructor for exception type and standard description */
  tException(Hardware_Exception exception)
    : error_code(exception)
  {
    switch(exception)
    {
    case eHARDWARE_ERROR:
    {
      this->description = "Hardware error.";
      break;
    }
    case ePEAK_DEVICE_ERROR:
    {
      this->description = "Problem with Peak-Can-Device.";
      break;
    }
    case eTHREAD_ERROR:
    {
      this->description = "Thread unscheduled terminated.";
    }
    case eMESSAGE_ERROR:
    {
      this->description = "Empty message received.";
    }
    case eFILE_ERROR:
    {
      this->description = "Error while writing to file.";
    }
    }
  };

  /** Destructor */
  ~tException() throw()
  {};

  /** @see std::exception#what */
  virtual const char* what() const throw()
  {
    return description.c_str();
  }

  /** Get a description string for the exception */
  inline std::string GetDescription()
  {
    return description;
  };

  /** Returns the error code(exception type) */
  inline Hardware_Exception GetErrorCode()
  {
    return error_code;
  };

protected:
  /** Error description */
  std::string description;

  /** Error code describing the error */
  Hardware_Exception error_code;
};
} /* can */
} /* icl_hardware */
#endif // ICL_HARDWARE_TEXCEPTION_H
