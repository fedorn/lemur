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


#ifndef _ParamH_
#define _ParamH_

#include "String.hpp"

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

//  Put routines:
/////////////////////////////////////
void   ParamSetRestartFile(const String &s);
void   ParamPut(const String &s, const double &value);
void   ParamPut(const String &s, const float  &value);
void   ParamPut(const String &s, const int    &value);
void   ParamPut(const String &s, const String &value);

//  Checkpointing:
/////////////////////////////////////
int    ParamCheckpointed (void);
void   ParamCheckpoint (void);

//  Miscellaneous:
/////////////////////////////////////
void   ParamDump (ostream &os);
void   ParamPushPrefix (const String &s);
void   ParamPopPrefix (void);
int    ParamPushFile (const String &s);
String ParamPopFile (void);
/// Display parameter bindings
void  ParamDisplay();

#endif
