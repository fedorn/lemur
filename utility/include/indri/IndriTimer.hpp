/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// IndriTimer.hpp
//
// 13 August 2004 -- tds
//

#ifndef INDRI_INDRITIMER_HPP

#include <iostream>
#include "lemur-platform.h"
/*! Utility class for printing timing statistics to a stream
 */
class IndriTimer {
private:
  /// when did we start.
  UINT64 _start;

public:
  IndriTimer();
  /// @return the current time as an unsigned 64 bit integer
  static UINT64 currentTime();
  /// start the timer
  void start();
  /// @return elapsed time since started as an unsigned 64 bit integer
  UINT64 elapsedTime() const;
  /// Print elapsed seconds to an output stream.
  /// @param out the stream to print to.
  void printElapsedSeconds( std::ostream& out ) const;
  /// Print elapsed microseconds to an output stream.
  /// @param out the stream to print to.
  void printElapsedMicroseconds( std::ostream& out ) const;
};

#endif // INDRI_INDRITIMER_HPP

