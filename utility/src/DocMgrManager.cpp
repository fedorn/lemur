/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#include "DocMgrManager.hpp"
#include "Param.hpp"
#include "FlattextDocMgr.hpp"
#include "KeyfileDocMgr.hpp"

DocumentManager* DocMgrManager::createDocMgr(string type, string name, 
					     string parsetype, string sources) {
  DocumentManager* dm = NULL;
  if (name.empty()) {
    // didn't pass in name to for new manager 
    // try to get it from the stack
    name = ParamGetString("manager");
  }
  if (parsetype.empty()) {
    parsetype = ParamGetString("docFormat");
  }
  if (sources.empty()) {
    sources = ParamGetString("dataFiles");
  }
  if (type.empty()) {
    type = ParamGetString("managerType");
  }
  if (type == "flat") {
    if ((!name.empty()) && (!parsetype.empty()) && (!sources.empty())) {
      dm = new FlattextDocMgr(name, parsetype, sources);
    }
  } else if (type == "bdm") {
    if ((!name.empty()) && (!parsetype.empty()) && (!sources.empty())) {
      dm = new KeyfileDocMgr(name, parsetype, sources);
    }
  } // Add else if new types here...
  return dm;
}

DocumentManager* DocMgrManager::openDocMgr(string name) {
  DocumentManager* dm = NULL;
  int len = name.length();

  // look for name ending in .flat
  if (len - name.rfind(".flat") == 5)
    dm = new FlattextDocMgr(name.c_str());
  else if (len - name.rfind(".bdm") == 4)
    dm = new KeyfileDocMgr(name.c_str());
  // Add else if new types here...
  return dm;
}
