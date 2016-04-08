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
 * tHardwareCanSinkFactory.h
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
 *  \file       tHardwareCanSinkFactory.h
 *  \author     <a href="mailto:ritzkow@fzi.de"> Hugo Ritzkowski </a>
 *  \date       23.12.2010
 *
 *  \brief
 *
 */
// ----------------------------------------------------------

#ifndef _icl_hardware_tHardwareCanSinkFactory_h_
#define _icl_hardware_tHardwareCanSinkFactory_h_

#include "icl_hardware_can/sink/ImportExport.h"
#include "icl_hardware_can/sink/tHardwareCanSink.h"

#include <icl_core/tString.h>

namespace icl_hardware {
namespace can {

/*! A factory producing tHardwareSink objects. The factory tries to guess
 * the requested tHardwareSink object from the string passed as argument,
 * which should be a filename.
 */

class ICL_HARDWARE_CAN_SINK_IMPORT_EXPORT tHardwareCanSinkFactory
{
public:
    /** Constructor */
    tHardwareCanSinkFactory();

    /** Destructor */
    virtual ~tHardwareCanSinkFactory();

    /*! Produce a tHardwareSink appropriate for the passed parameters.
    * Specifier can look like the following, e.g.
    * file:///path/to/file - to record to a file
    */
    tHardwareCanSink * Construct(std::string specifier);

protected:

    std::string interpretConfigFile();
};

}
}

#endif // _icl_hardware_tHardwareCanSinkFactory_h_
