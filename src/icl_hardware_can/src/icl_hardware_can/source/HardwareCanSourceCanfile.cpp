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
 * \date    2011-01-26
 * \author  Jan Oberlaender <oberlaender@fzi.de>
 * \date    2014-05-19
 *
 */
//----------------------------------------------------------------------
#include "HardwareCanSourceCanfile.h"

#include <icl_core_config/Config.h>
#include <icl_sourcesink/SimpleURI.h>
#include <icl_sourcesink/Position.h>

#include "icl_hardware_can/Logging.h"

namespace icl_hardware {
namespace can {

HardwareCanSourceCanfile::HardwareCanSourceCanfile(const std::string& uri,
                                                   const std::string& name)
  : HardwareCanSource(uri, name,
                      icl_sourcesink::PositionResolverBase::Ptr(
                        new icl_sourcesink::KeyframePositionResolver(
                          icl_sourcesink::FileResolutionHelper::Ptr(
                            new CanFileResolutionHelper(icl_sourcesink::SimpleURI(uri).path())),
                          1000))),
    m_buffer(),
    m_resolver_x(
      boost::dynamic_pointer_cast<icl_sourcesink::KeyframePositionResolver>(
        m_resolver)),
    m_resolution_helper(
      dynamic_cast<CanFileResolutionHelper *>(
        m_resolver_x->resolutionHelper()->clone())),
    m_sequence_number(0)
{
  prepareFile();
}

void HardwareCanSourceCanfile::prepareFile()
{
  if (!m_resolution_helper)
  {
    LOGGING_ERROR_C(CAN, HardwareCanSourceCanfile,
                    "Failed to open URI '" << uri() << "'." << endl);
    m_is_good = false;
    return;
  }

  // Check for a resolver index cache.
  LOGGING_DEBUG_C(CAN, HardwareCanSourceCanfile,
                  "Checking for resolver index cache." << endl);
  std::string resolver_index_filename =
    icl_sourcesink::SimpleURI(uri()).path()
    + icl_sourcesink::PositionResolverBase::INDEX_FILE_SUFFIX;
  LOGGING_DEBUG_C(CAN, HardwareCanSourceCanfile,
                  "Checking for resolver index cache at '" << resolver_index_filename << "'..." << endl);
  if (m_resolver->load(resolver_index_filename))
  {
    LOGGING_INFO_C(CAN, HardwareCanSourceCanfile,
                   "Successfully loaded resolver index cache from '" << resolver_index_filename << "'." << endl);
  }
  else
  {
    // Build the resolver index.
    LOGGING_DEBUG_C(CAN, HardwareCanSourceCanfile,
                    "No resolver index cache found, building index." << endl);
    uint64_t filesize = m_resolution_helper->size();
    std::size_t count = 0;
    for (; m_resolution_helper->good(); m_resolution_helper->advance())
    {
      icl_sourcesink::Position pos = m_resolution_helper->current();
      uint64_t filepos = m_resolution_helper->tellg();
      m_resolver_x->addEntry(pos.timestamp, pos.dsin, filepos);
      if (++count % 1000 == 0)
      {
        std::cerr << "Building resolver index: "
                  << int(double(filepos)*100.0/double(filesize)) << "%  \r"
                  << std::flush;
      }
    }
    std::cerr << "Resolver index completed.         " << std::endl;

    // Store resolver index cache.
    LOGGING_DEBUG_C(CAN, HardwareCanSourceCanfile,
                    "Storing resolver index cache at '" << resolver_index_filename << "'." << endl);
    if (!m_resolver->save(resolver_index_filename))
    {
      LOGGING_WARNING_C(CAN, HardwareCanSourceCanfile,
                        "Failed to store resolver index cache at '" << resolver_index_filename << "'." << endl);
    }
  }

  // Reset file to first element position and make fragmentable.
  m_resolver->makeFragmentable();
  m_is_good = seek(icl_sourcesink::Index(0));

  // Done.
  LOGGING_DEBUG_C(CAN, HardwareCanSourceCanfile,
                  "Resolver index complete, total number of elements: " << m_resolver->registerSize() << endl);
}

bool HardwareCanSourceCanfile::seekImpl(const icl_sourcesink::InternalIndex internal_index)
{
  // InternalIndex == RegisterIndex.
  icl_sourcesink::Position pos(m_resolver_x->mapIndex<icl_sourcesink::Index>(internal_index));
  boost::optional<uint64_t> seek_pos = m_resolver_x->resolveExtra(pos, icl_sourcesink::RT_EXACT);
  if (seek_pos)
  {
    if (!m_resolution_helper->seekg(*seek_pos))
    {
      LOGGING_ERROR_C(CAN, HardwareCanSourceCanfile,
                      "Failed to seek to position " << seek_pos
                      << " even though the resolver had a valid entry for that position!" << endl);
      m_buffer.reset();
      m_is_good = false;
      return false;
    }
    m_buffer = m_resolution_helper->currentMessage();
    m_buffer->header().sequence_number = m_sequence_number++;
    return true;
  }
  else
  {
    LOGGING_WARNING_C(CAN, HardwareCanSourceCanfile,
                      "Failed to resolve internal index " << internal_index.value
                      << ", probably end of file" << endl);
    m_buffer.reset();
    return false;
  }
}

bool HardwareCanSourceCanfile::advance()
{
  if (m_resolver->advance())
  {
    if (!m_resolution_helper->advance())
    {
      LOGGING_ERROR_C(CAN, HardwareCanSourceCanfile,
                      "Resolution helper failed to advance. This should not happen!" << endl);
      return false;
    }
    else
    {
      m_buffer = m_resolution_helper->currentMessage();
      m_buffer->header().sequence_number = m_sequence_number++;
      return true;
    }
  }
  else
  {
    LOGGING_WARNING_C(CAN, HardwareCanSourceCanfile,
                      "Failed to advance, probably end of file" << endl);
    m_buffer.reset();
    m_is_good = false;
    return false;
  }
}

}
}
