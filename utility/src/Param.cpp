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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

 ** 02/2005 -- dmf Rework to use indri Parameters.
*/

#include "common_headers.hpp"
#include "String.hpp"
#include "Param.hpp"
#include "Exception.hpp"
#include "indri/Parameters.hpp"
#include <set>
/// Container class for parameter stack. Maintains a singleton instance
/// to push/pop Parameters * onto.
class ParamStack {
public:
  //
  static ParamStack &instance() {
    if (!_paramStackSingleton) {
      _paramStackSingleton = new ParamStack();
    };
    return *_paramStackSingleton;
  };
  Parameters *top() {
    if (params.empty())
      return NULL;
    else
      return params.top();
  }

  void push(Parameters *p) {
    params.push(p);
  };

  void pop() {
    if (!params.empty()) {
      Parameters *p = NULL;
      p = params.top();
      params.pop();
      delete(p);
    }
  };

  void addKey(const String &s) {
    keys.insert(std::string(s));
  }

  void displayKeys(){
    std::set<std::string>::iterator iter;
    for (iter = keys.begin(); iter != keys.end(); iter++) {
      std::cerr << *iter << std::endl;
    }
  }
  
  ~ParamStack() {
    // clean up
    keys.clear();
    while (!params.empty()) {
      pop();
    }
  };
  
private:
  ParamStack() {
  };
  static ParamStack* _paramStackSingleton;
  std::stack<Parameters * >params;
  // maintain a list of used keys (for ParamDisplay).
  std::set<std::string> keys;
};

ParamStack* ParamStack::_paramStackSingleton = 0;

//  Get routines:
/////////////////////////////////////

int ParamGetBit(const String &s, int def)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    bool retval= (def != 0) ? true : false;
    if (p) retval = p->get((std::string &) s, retval);
    return retval;
  }

int ParamGetInt(const String &s, int def)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    int retval = def;
    if (p) retval = p->get((std::string &)s, def);
    return retval;
  }

INT64 ParamGetLongLong(const String &s, INT64 def)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    INT64 retval = def;
    if (p) retval = p->get((std::string &)s, def);
    return retval;
  }

int ParamGet(const String &s, int &value)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    bool retval = true;
    if (p && p->exists((std::string &)s)) {
      value = p->get((std::string &)s, -1); 
    } else {
      retval = false;
    }
    return retval;
  }


int ParamGet(const String &s, int &value, const int &def)
  {
    if (ParamGet(s, value)) 
      return true;
    else {
       value = def;
       return false;
    }
  }


double ParamGetDouble(const String &s, double def)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    double retval = def;
    if (p) retval = p->get((std::string &)s, def);
    return retval;
  }


int ParamGet(const String &s, double &value)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    bool retval = true;
    if (p && p->exists((std::string &)s)) {
      value = p->get((std::string &)s, -1); 
    } else {
      retval = false;
    }
    return retval;
  }


int ParamGet(const String &s, double &value, const double &def)
  {
    if (ParamGet(s, value)) 
      return true;
    else {
       value = def;
       return false;
    }
  }


float ParamGetFloat(const String &s, float def)
  {
    return (float) ParamGet(s, (double) def);
  }


int ParamGet(const String &s, float &value)
  {
    int ret;
    double g;
    ret = ParamGet(s, g);
    if (ret)
      value = (float)g;
    return ret;
  }


int ParamGet(const String &s, float &value, const float &def)
  {
    double g;
    if (ParamGet(s, g)) {
      value = (float)g;
      return true;
    } else {
       value = def;
       return false;
    }
  }


int ParamGet(const String &s, String &value) 
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    bool retval = true;
    if (p && p->exists((std::string &)s)) {
      value = p->get((std::string &)s, ""); 
    } else {
      retval = false;
    }
    return retval;
  }


int ParamGet(const String &s, String &value, const String &def) 
  {
    if (ParamGet(s, value))
      return true; 
    else {
      value = def;
      return false;
    }
  }


String ParamGetString(const String &s, const String &def)
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(s);
    String retval;
    if (p) {
      retval = p->get((std::string &)s, (std::string &)def);
      /* trim leading/trailing whitespace as was done by 
	 parameters.c (old style param file) processing.
       */
      retval.erase(retval.find_last_not_of(" ") + 1);
      retval.erase(0, retval.find_first_not_of(" "));
    }
    else retval = def;
    return retval;
  }

String ParamGetString(const String &s, String &value, const String &def)
  {
    value = ParamGetString(s, def);
    return value;
  }

String ParamGetString(const String &s)
  {
    String def = "";
    return ParamGetString(s, def);
  }


//  Miscellaneous:
int    ParamPushFile (const String &s) 
  {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = new Parameters();
    bool retval = true;
    try {
      p->loadFile(s);
      ps.push(p);
    } catch (Exception &e) {
      delete(p);
      retval = false;
    }
    return retval;
  }

String ParamPopFile (void) 
  {
    ParamStack &ps = ParamStack::instance();
    /*    ps.displayKeys();*/
    ps.pop();
    /*
    Parameters *p = ps.top();
    if (p) {
      string s = p->get("index");
      std::cerr << p << ":" << s << std::endl;
      ps.displayKeys();
    }
    */
    String retval = "";
    return retval;
  }

// need to support registration (getAppParam calls).
void ParamDisplay() {
    ParamStack &ps = ParamStack::instance();
    ps.displayKeys();
}

void ParamSet(const String &key, const String &value) {
    ParamStack &ps = ParamStack::instance();
    Parameters *p = ps.top();
    ps.addKey(key);
    if (!p) {
      // no Parameters on the stack. Make one.
      p = new Parameters();
      ps.push(p);
    }
    p->set((std::string &)key, (std::string &)value);
}
