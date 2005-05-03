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
#include "ElemDocMgr.hpp"
namespace lemur 
{
  namespace api
  {
    
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
      if ((!name.empty()) && (!parsetype.empty()) && (!sources.empty())) {
        if (type == "flat") {
          dm = new lemur::parse::FlattextDocMgr(name, parsetype, sources);
        } else if (type == "bdm") {
          dm = new lemur::parse::KeyfileDocMgr(name, parsetype, sources);
        } else if (type == "elem") {
          dm = new lemur::parse::ElemDocMgr(name, parsetype, sources);
        } // Add else if new types here...
      }  
      return dm;
    }

    DocumentManager* DocMgrManager::openDocMgr(const string name, bool readOnly) {
      DocumentManager* dm = NULL;
      int len = name.length();

      // look for name ending in .flat
      if (len - name.rfind(".flat") == 5)
        dm = new lemur::parse::FlattextDocMgr(name);
      else if (len - name.rfind(".bdm") == 4)
        dm = new lemur::parse::KeyfileDocMgr(name, readOnly);
      else if (len - name.rfind(".elem") == 5)
        dm = new lemur::parse::ElemDocMgr(name, readOnly);
      // Add else if new types here...
      return dm;
    }
  }
}
