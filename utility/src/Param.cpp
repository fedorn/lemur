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
namespace lemur 
{
  namespace utility
  {
    /// Container class for parameter stack. Maintains a singleton instance
    /// to push/pop Parameters * onto.
    class ParamStack {
    public:
      /// Clean up
      static void clear() {
        delete (_paramStackSingleton);
        _paramStackSingleton = NULL;
      }
      
      /// Obtain the singleton instance, instantiating if necessary.
      static ParamStack &instance() {
        if (!_paramStackSingleton) {
          _paramStackSingleton = new ParamStack();
        };
        return *_paramStackSingleton;
      };
      /*!
        @return the top of the parameter stack or NULL if the stack is empty.
      */
      indri::api::Parameters *top() {
        if (params.empty())
          return NULL;
        else
          return params.top();
      }
      /*!
        @param p Parameters to push onto the stack.
      */
      void push(indri::api::Parameters *p) {
        params.push(p);
      };
      /// pop the parameter stack, deleting the popped element.
      void pop() {
        if (!params.empty()) {
          indri::api::Parameters *p = NULL;
          p = params.top();
          params.pop();
          delete(p);
        }
      };
      /*!
        Add a parameter name to the list of known parameters.
        @param s the name to add.
        @see displayKeys
       */
      void addKey(const lemur::utility::String &s) {
        keys.insert(std::string(s));
      }

      /*!
        Write the list of known parameter names to cerr.
        @see ParamDisplay
       */
      void displayKeys(){
        std::set<std::string>::iterator iter;
        for (iter = keys.begin(); iter != keys.end(); iter++) {
          std::cerr << *iter << std::endl;
        }
      }
      /// clean up.
      ~ParamStack() {
        // clean up
        keys.clear();
        while (!params.empty()) {
          pop();
        }
      };
  
    private:
      /// private constructor to ensure singleton pattern.
      ParamStack() {
      };
      /// the singleton instance.
      static ParamStack* _paramStackSingleton;
      /// the stack of Parameters.
      std::stack<indri::api::Parameters * >params;
      /// maintain a list of used keys (for ParamDisplay).
      std::set<std::string> keys;
    };
    ParamStack* ParamStack::_paramStackSingleton = 0;
  }
}


namespace lemur 
{
  namespace api
  {

    //  Get routines:

    int ParamGetBit(const lemur::utility::String &s, int def)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      bool retval= (def != 0) ? true : false;
      if (p) retval = p->get((std::string &) s, retval);
      return retval;
    }

    int ParamGetInt(const lemur::utility::String &s, int def)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      int retval = def;
      if (p) retval = p->get((std::string &)s, def);
      return retval;
    }

    INT64 ParamGetLongLong(const lemur::utility::String &s, INT64 def)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      INT64 retval = def;
      if (p) retval = p->get((std::string &)s, def);
      return retval;
    }

    int ParamGet(const lemur::utility::String &s, int &value)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      bool retval = true;
      if (p && p->exists((std::string &)s)) {
        value = p->get((std::string &)s, -1); 
      } else {
        retval = false;
      }
      return retval;
    }


    int ParamGet(const lemur::utility::String &s, int &value, const int &def)
    {
      if (ParamGet(s, value)) 
        return true;
      else {
        value = def;
        return false;
      }
    }


    double ParamGetDouble(const lemur::utility::String &s, double def)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      double retval = def;
      if (p) retval = p->get((std::string &)s, def);
      return retval;
    }


    int ParamGet(const lemur::utility::String &s, double &value)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      bool retval = true;
      if (p && p->exists((std::string &)s)) {
        value = p->get((std::string &)s, -1); 
      } else {
        retval = false;
      }
      return retval;
    }


    int ParamGet(const lemur::utility::String &s, double &value, const double &def)
    {
      if (ParamGet(s, value)) 
        return true;
      else {
        value = def;
        return false;
      }
    }


    float ParamGetFloat(const lemur::utility::String &s, float &def)
    {
      return (float) ParamGet(s, (double&) def);
    }


    int ParamGet(const lemur::utility::String &s, float &value)
    {
      int ret;
      double g;
      ret = ParamGet(s, g);
      if (ret)
        value = (float)g;
      return ret;
    }


    int ParamGet(const lemur::utility::String &s, float &value, const float &def)
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


    int ParamGet(const lemur::utility::String &s, lemur::utility::String &value) 
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      bool retval = true;
      if (p && p->exists((std::string &)s)) {
        value = p->get((std::string &)s, ""); 
      } else {
        retval = false;
      }
      return retval;
    }


    int ParamGet(const lemur::utility::String &s, lemur::utility::String &value, const lemur::utility::String &def) 
    {
      if (ParamGet(s, value))
        return true; 
      else {
        value = def;
        return false;
      }
    }


    lemur::utility::String ParamGetString(const lemur::utility::String &s, const lemur::utility::String &def)
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(s);
      lemur::utility::String retval;
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

    lemur::utility::String ParamGetString(const lemur::utility::String &s, lemur::utility::String &value, const lemur::utility::String &def)
    {
      value = ParamGetString(s, def);
      return value;
    }

    lemur::utility::String ParamGetString(const lemur::utility::String &s)
    {
      lemur::utility::String def = "";
      return ParamGetString(s, def);
    }


    //  Miscellaneous:
    int    ParamPushFile (const lemur::utility::String &s) 
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = new indri::api::Parameters();
      bool retval = true;
      try {
        p->loadFile(s);
        ps.push(p);
      } catch (Exception &e) {
        e.writeMessage();
        delete(p);
        retval = false;
      }
      return retval;
    }
    /// Reset Parameter stack to empty and deallocate all memory.
    void ParamClear() {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      ps.clear();
    }
    
    lemur::utility::String ParamPopFile (void) 
    {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      ps.pop();
      lemur::utility::String retval = "";
      return retval;
    }

    // need to support registration (getAppParam calls).
    void ParamDisplay() {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      ps.displayKeys();
    }

    void ParamSet(const lemur::utility::String &key, const lemur::utility::String &value) {
      lemur::utility::ParamStack &ps = lemur::utility::ParamStack::instance();
      indri::api::Parameters *p = ps.top();
      ps.addKey(key);
      if (!p) {
        // no Parameters on the stack. Make one.
        p = new indri::api::Parameters();
        ps.push(p);
      }
      p->set((std::string &)key, (std::string &)value);
    }
  }
}
