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
// IndriTimer
//
// 13 August 2004 -- tds
//

#include "indri/IndriTimer.hpp"
#include "lemur-compat.hpp"
#include <iomanip>
#ifndef WIN32
#include <sys/time.h>
#endif

IndriTimer::IndriTimer()
  :
  _start(0)
{
}

UINT64 IndriTimer::currentTime() {
#ifdef WIN32
  UINT64 ticks = ::GetTickCount();
  UINT64 thousand = 1000;
  return ticks * thousand;
#else
  struct timeval tv;
  gettimeofday(&tv, 0);
  UINT64 seconds = tv.tv_sec;
  UINT64 million = 1000000;
  UINT64 microseconds = tv.tv_usec;

  return seconds * million + microseconds;
#endif
}

void IndriTimer::start() {
  _start = currentTime();
}

UINT64 IndriTimer::elapsedTime() const {
  return currentTime() - _start;
}

void IndriTimer::printElapsedMicroseconds( std::ostream& out ) const {
  UINT64 elapsed = elapsedTime();
  const UINT64 million = 1000000;

  int minutes = int(elapsed / (60 * million));
  int seconds = int(elapsed/million - 60*minutes);
  int microseconds = int(elapsed % million);
  
  out << minutes
      << ":"
      << std::setw(2) << std::setfill('0')
      << seconds
      << "."
      << std::setw(6) << std::setfill('0')
      << microseconds;
}

void IndriTimer::printElapsedSeconds( std::ostream& out ) const {
  UINT64 elapsed = elapsedTime();
  const UINT64 million = 1000000;

  int minutes = int(elapsed / (60 * million));
  int seconds = int(elapsed/million - 60*minutes);
  int microseconds = int(elapsed % million);
  
  out << minutes
      << ":"
      << std::setw(2) << std::setfill('0')
      << seconds;
}

