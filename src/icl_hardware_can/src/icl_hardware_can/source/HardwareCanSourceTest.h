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
 * \date    2014-05-12
 *
 */
//----------------------------------------------------------------------
#ifndef ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_TEST_H_INCLUDED
#define ICL_HARDWARE_CAN_SOURCE_HARDWARE_CAN_SOURCE_TEST_H_INCLUDED

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/random.hpp>

#include <icl_hardware_can/source/ImportExport.h>
#include <icl_hardware_can/tCanMessage.h>
#include <icl_hardware_can/source/HardwareCanSource.h>

namespace icl_hardware {
namespace can {

/*! This source creates its own tCanMessage elements for test
 *  purposes.  Using a URI parameter, a pattern can be set which
 *  defines how the messages should be constructed.
 */
class ICL_HARDWARE_CAN_SOURCE_IMPORT_EXPORT HardwareCanSourceTest : public HardwareCanSource
{
public:
  //! Shared pointer shorthand.
  typedef boost::shared_ptr<HardwareCanSourceTest> Ptr;
  //! Const shared pointer shorthand.
  typedef boost::shared_ptr<const HardwareCanSourceTest> ConstPtr;

  //! Different test patterns.
  enum Pattern
  {
    /*! The IDs will be increased sequentially from 1 to 2047
     *  (2^11-1), then start again.  This is the default pattern.
     */
    P_LINEAR = 1,
    //! A fixed ID is sent at all times.
    P_FIXED,
    //! The IDs will be created randomly (seed is fixed).
    P_CHANGING_RANDOM,
    //! The IDs are created by a function, IDs can repeat.
    P_CHANGING_WITH_REPEAT
  };

  //! Always returns \c true for the test source.
  virtual bool good() const
  {
    return true;
  }

  //! URI schemes supported by this source.
  static icl_sourcesink::URISchemeMap supportedURISchemes()
  {
    using namespace icl_sourcesink;
    URISchemeMap schemes;
    schemes.insert(
      std::make_pair(
        "can+test",
        URISchemeInfo(
          "Test source for CAN message data.",
          "can+test:[?rate=<rate>][&pattern=<pattern>][&id_value=<id_value>]",
          "<rate>      Data generation rate in Hz.\n"
          "<pattern>   One of 'Linear' (default), 'Fixed', 'ChangingRandom', 'ChangingWithRepeat'.\n"
          "<id_value>  In Fixed mode, the ID to generate (default: 1).\n")));
    return schemes;
  }

  //! Returns the current data element.
  virtual CanMessageStamped::Ptr get() const
  {
    return m_buffer;
  }

  //! This is a non-seekable source.
  virtual bool isSeekable() const
  {
    return false;
  }

protected:
  //! Constructor.
  HardwareCanSourceTest(const std::string& uri = "",
                        const std::string& name = "HardwareCanSourceTest");

  //! Provide access for the manager.
  friend class icl_sourcesink::SourceSinkManager;

  //! Provide access for the factory.
  friend class icl_sourcesink::DataSourceFactory<HardwareCanSourceTest>;

  //! Advance to the next data element.
  bool advance();

  /*! Creates the next CAN message ID, according to the selected
   *  pattern generation method.
   */
  unsigned int createCanMessageId();

private:
  //! Random number generator.
  boost::random::mt19937 m_rng;

  //! Uniform distribution over possible CAN IDs.
  boost::random::uniform_int_distribution<unsigned int> m_can_id_distribution;

  //! Uniform distribution over possible payload bytes.
  boost::random::uniform_int_distribution<uint8_t> m_data_distribution;

  //! Buffers the latest generated message.
  CanMessageStamped::Ptr m_buffer;

  //! Data generation rate in Hz.
  float m_rate;

  //! The message generation pattern.
  Pattern m_pattern;

  //! The next CAN message ID to be used.
  uint16_t m_next_id;
};

SOURCESINK_DECLARE_GENERIC_SOURCE_FACTORY(HardwareCanSourceTest)

}
}

#endif
