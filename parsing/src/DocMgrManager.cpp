/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#include "DocMgrManager.hpp"


DocumentManager* DocMgrManager::createDocMgr(string type, string name, 
					     string parsetype, string sources) {
  DocumentManager* dm = NULL;
  if (name.empty()) {
    // didn't pass in name to for new manager 
    // try to get it from the stack
    name = ParamGetString("manager");
  }
  // not checking type from stack since there's no param name yet
  if (type == "flat") {
    if (parsetype.empty()) {
      parsetype = ParamGetString("docFormat");
    }
    if (sources.empty()) {
      sources = ParamGetString("dataFiles");
    }
    if ((!name.empty()) && (!parsetype.empty()) && (!sources.empty())) {
      return new FlattextDocMgr(name, parsetype, sources);
    }
  }
  return dm;
}

DocumentManager* DocMgrManager::openDocMgr(string name) {
  DocumentManager* dm = NULL;
  int len = name.length();

  // look for name ending in .flat
  if (len - name.rfind(".flat") == 5)
    return new FlattextDocMgr(name.c_str());

  return dm;
}
