/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _TIMERH_
#define _TIMERH_

#include <ctime>
#include <iostream.h>
#include "String.hpp"

class Timer {
public:
  Timer(const String &string, int start=1) : message(string)
    { if (start) Start(); }
  void Start()
    {
      time(&t);
      cerr << "Beginning: " << message << endl << flush;
    }
  ~Timer() { if (t > 0) Stop(); }
  void Stop()
    {
      time_t total;
      time(&total);
      total -= t;
      cerr << message << " total time: ";
      if (total/3600) { cerr << total/3600 << " hours ";   total %= 3600; }
      if (total/60)   { cerr << total/60   << " minutes "; total %= 60; }
      cerr << total      << " seconds " << endl << flush;
      t = 0;
    }
  void Elapsed()
    {
      time_t total;
      time(&total);
      total -= t;
      cerr << message << " elapsed time: ";
      if (total/3600) { cerr << total/3600 << " hours ";   total %= 3600; }
      if (total/60)   { cerr << total/60   << " minutes "; total %= 60; }
      cerr << total      << " seconds " << endl << flush;
    }
  time_t getElapsed() const { time_t cur; time(&cur); return cur - t; }
private:
  time_t t;
  String message;
};

    

#endif
