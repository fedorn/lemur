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
//  Get routines:
/////////////////////////////////////
String ParamGetString(const String &s, const String &def);
String ParamGetString(const String &s, String &value, const String &def);
String ParamGetString(const String &s);
int    ParamGet(const String &s, String &value);
int    ParamGet(const String &s, String &value, const String &def);
int    ParamGet(const String &s, int &value);
int    ParamGet(const String &s, int &value, const int &def);
int    ParamGet(const String &s, double &value);
int    ParamGet(const String &s, double &value, const double &def);
int    ParamGet(const String &s, float &value);
int    ParamGet(const String &s, float &value, const float &def);
int    ParamGetInt(const String &s, int def);
int    ParamGetBit(const String &s, int def);
double ParamGetDouble(const String &s, double def);
float  ParamGetFloat(const String &s, float def);
INT64  ParamGetLongLong(const String &s, INT64 def);

//  Miscellaneous:
/////////////////////////////////////
int    ParamPushFile (const String &s);
String ParamPopFile (void);
void   ParamDisplay();

/// add a parameter value pair. If no parameter file has been pushed,
/// a new empty Parameters object is put on the stack.
void ParamSet(const String &key, const String &value);

#endif
