 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "common_headers.hpp"
#include "String.hpp"
#include "Param.hpp"
extern "C" {
#include "parameters.h"
}

#define MAXLEN 65536

static char *pChar=(char *)NULL;

//  Get routines:
/////////////////////////////////////

int ParamGetBit(const String &s, int def)
  {
    return param_getb(s, def);
  }

int ParamGetInt(const String &s, int def)
  {
    return param_geti(s, def);
  }

INT64 ParamGetLongLong(const String &s, INT64 def)
  {
    return param_getll(s, def);
  }

int ParamGet(const String &s, int &value)
  {
    return param_symvarie(s, &value);
  }


int ParamGet(const String &s, int &value, const int &def)
  {
    if (param_symvarie(s, &value)) return 1;
    else {
       value = def;
       return 0;
    }
  }


double ParamGetDouble(const String &s, double def)
  {
    return param_getf(s, def);
  }


int ParamGet(const String &s, double &value)
  {
    return param_symvarfe(s, &value);
  }


int ParamGet(const String &s, double &value, const double &def)
  {
    if (param_symvarfe(s, &value)) return 1;
    else {
       value = def;
       return 0;
    }
  }


float ParamGetFloat(const String &s, float def)
  {
    return (float) param_getf(s, (double) def);
  }


int ParamGet(const String &s, float &value)
  {
    int i;
    double g;

    i = param_symvarfe(s, &g);
    value = g;
    return i;
  }


int ParamGet(const String &s, float &value, const float &def)
  {
    double g;

    if (param_symvarfe(s, &g)) {
        value = g;
        return 1;
    }
    else {
       value = def;
       return 0;
    }
  }


int ParamGet(const String &s, String &value) 
  {
    int i;

    if (!pChar) pChar = new char[MAXLEN+1];
    i = param_symvarce(s, pChar);
    value = pChar;
    return i;
  }


int ParamGet(const String &s, String &value, const String &def) 
  {
    int i;

    if (!pChar) pChar = new char[MAXLEN+1];
    i = param_symvarce(s, pChar);
    if (i) value = pChar;
    else value = def;
    return i;
  }


String ParamGetString(const String &s, const String &def)
  {
    int i;
    String value;

    if (!pChar) pChar = new char[MAXLEN+1];
    i = param_symvarce(s, pChar);
    if (i) {
       value = pChar;
       return value;
    }
    else return def;
  }

String ParamGetString(const String &s, String &value, const String &def)
  {
    int i;
   
    if (!pChar) pChar = new char[MAXLEN+1];
    i = param_symvarce(s, pChar);
    if (i) {
       value = pChar;
       return value;
    }
    else {
         value = def;
         return value;
    }
  }

String ParamGetString(const String &s)
  {
    int i;
    String value;

    if (!pChar) pChar = new char[MAXLEN+1];
    i = param_symvarce(s, pChar);
    if (i) {
       value = pChar;
       return value;
    }
    else {
      // cerr << "Param: value for ddname \"" << s 
      //    << "\" not supplied in DDINF" << endl;
      // assert(0);
      return ""; // null(String);
    }
  }

//  Miscellaneous:
//////////////////////////////////////
//void   ParamDump (ostream &os) {
//   param_dump;
//}

void   ParamPushPrefix (const String &s)
  {
     param_push_prefix(s);
  }

void   ParamPopPrefix (void)
  {
     param_pop_prefix();
  }

int    ParamPushFile (const String &s) 
  {
     return param_push_file (s);
  }

String ParamPopFile (void) 
  {
     char *retval;
     retval = param_pop_file();
     if (retval == NULL) retval = "";
     return String(retval);
  }

void ParamDisplay() {
  param_display();
}

void ParamSet(const String &key, const String &value) {
  param_set(key, value);
}
