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
 * tCanMessageMetaDumper.h
 * <section>
 *
 * Created by Hugo Ritzkowski on 24.01.2012.
 * Copyright 2012
 * Company Forschungszentrum Informatik (FZI), Abteilung IDS.
 * All rights reserved.
 *
 */
// ----------------------------------------------------------
/*!
 *  \file       tCanMessageMetaDumper.h
 *  \author     <a href="mailto:ritzkow@fzi.de"> Hugo Ritzkowski</a>
 *  \date       24.01.2012
 *
 *  \brief      This modul gets a file with can-messages and constructs the
 *              adequate xml-file to it. Its data is written to the console
 *
 *
 */
// ----------------------------------------------------------

#ifndef tCanMessageDumper_h
#define tCanMessageDumper_h

#include "ImportExport.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace icl_hardware {

namespace can {

class ICL_HARDWARE_CAN_IMPORT_EXPORT tCanMessageMetaDumper
{
public:

    tCanMessageMetaDumper();

    ~tCanMessageMetaDumper();

    /*! gets the path to a can-file */
    void open(const std::string identifier = "", const int baudrate = 0);

    void close();

    /*! build the construct of a xml-can-file and outputs it to the console */
    bool constructXmlFile();

protected:

    int prepareFile();
    void setDataFileName(const boost::filesystem::path &file_name);
    const boost::filesystem::path & getDataFileName() const;

private:

    unsigned int m_baudrate;
    std::ifstream m_data_file;
    boost::filesystem::path *m_data_file_name;

    static const short m_major_fileversion = 1;
    static const short m_minor_fileversion = 2;

};

}

}

#endif // tCanMessageDumper_h
