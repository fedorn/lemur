/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
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

