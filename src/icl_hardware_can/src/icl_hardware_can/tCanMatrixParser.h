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
 * tCanMatrixParser.h
 * <section>
 *
 * Created by Hugo Ritzkowski on 01.04.2011.
 * Copyright 2011
 * Company Forschungszentrum Informatik (FZI), Abteilung IDS.
 * All rights reserved.
 *
 */
// ----------------------------------------------------------
/*!
 *  \file       tCanMatrixParser.h
 *  \author     <a href="mailto:ritzkow@fzi.de"> Hugo Ritzkowski</a>
 *  \date       01.04.2011
 *
 *  \brief      This modul constructs a map of a can-mask-file
 *              the map can be request by getcanmatrix()
 *
 */
// ----------------------------------------------------------

#ifndef _icl_hardware_tCanMatrixParser_h_
#define _icl_hardware_tCanMatrixParser_h_

#include "icl_hardware_can/ImportExport.h"

#include <icl_core/tString.h>

#include <fstream>
#include <boost/filesystem.hpp>
#include <map>
#include <vector>

// Forward declaration
class TiXmlHandle;
class TiXmlElement;

namespace icl_hardware {
namespace can {

struct ICL_HARDWARE_CAN_IMPORT_EXPORT CanMatrixElement
{
  std::string description;
  double conversion;
  std::string unit;
  unsigned int start_bit;
  unsigned int signal_length;
  double offset;
  double lower_border;
  double upper_border;
  bool little_endian;
  bool signedness;
};

typedef std::map<unsigned int, std::vector<CanMatrixElement> > CanMatrix;

class ICL_HARDWARE_CAN_IMPORT_EXPORT tCanMatrixParser
{
public:
    /** Constructor */
    tCanMatrixParser(const std::string identifier = "");

    /** Destructor */
    virtual ~tCanMatrixParser();

    const CanMatrix &getCanMatrix() const;

    const bool isActive() const;

private:
    int mapContentOfFile(TiXmlHandle& doc_handle);
    int mapContentOfFile(void* dbc_);

    const int checkXmlFile(TiXmlElement *element) const;
    void setDataFileName(const boost::filesystem::path &file_name);
    const boost::filesystem::path & getDataFileName() const;

    CanMatrix m_canmatrix;
    boost::filesystem::path *m_data_file_name;
    bool is_initialized;
};

ICL_HARDWARE_CAN_IMPORT_EXPORT std::ostream& operator<<(std::ostream &stream, const CanMatrix& can_matrix);

}
}

#endif // _icl_hardware_tCanMatrixParser_h_
