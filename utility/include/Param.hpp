/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

 ** 02/2005 -- dmf Rework to use indri Parameters.
 */

#ifndef _ParamH_
#define _ParamH_

#include "lemur-platform.h"
#include "String.hpp"

// replace with #defines?
namespace lemur 
{
  namespace api
  {
    //  Get routines:
    ///
    lemur::utility::String ParamGetString(const lemur::utility::String &s, 
                                          const lemur::utility::String &def);
    lemur::utility::String ParamGetString(const lemur::utility::String &s, 
                                          lemur::utility::String &value, 
                                          const lemur::utility::String &def);
    lemur::utility::String ParamGetString(const lemur::utility::String &s);
    int    ParamGet(const lemur::utility::String &s, 
                    lemur::utility::String &value);
    int    ParamGet(const lemur::utility::String &s, 
                    lemur::utility::String &value, 
                    const lemur::utility::String &def);
    int    ParamGet(const lemur::utility::String &s, int &value);
    int    ParamGet(const lemur::utility::String &s, int &value, 
                    const int &def);
    int    ParamGet(const lemur::utility::String &s, double &value);
    int    ParamGet(const lemur::utility::String &s, double &value, 
                    const double &def);
    int    ParamGet(const lemur::utility::String &s, float &value);
    int    ParamGet(const lemur::utility::String &s, float &value, 
                    const float &def);
    int    ParamGetInt(const lemur::utility::String &s, int def);
    int    ParamGetBit(const lemur::utility::String &s, int def);
    double ParamGetDouble(const lemur::utility::String &s, double def);
    float  ParamGetFloat(const lemur::utility::String &s, float def);
    INT64  ParamGetLongLong(const lemur::utility::String &s, INT64 def);

    //  Miscellaneous:
    ///
    int    ParamPushFile (const lemur::utility::String &s);
    lemur::utility::String ParamPopFile (void);
    void   ParamDisplay();

    /// \brief Add a parameter value pair. 
    /// If no parameter file has been pushed,
    /// a new empty Parameters object is put on the stack.
    void ParamSet(const lemur::utility::String &key, 
                  const lemur::utility::String &value);
  }
}

#endif
