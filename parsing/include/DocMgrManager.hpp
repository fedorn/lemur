/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#ifndef _DOCMGRMANAGER_HPP
#define _DOCMGRMANAGER_HPP

/**
 * This is a convenience class with static methods for creating and opening
 * DocumentManagers. New DocumentManagers should be added here.
 *
 */

#include "FlattextDocMgr.hpp"

class DocMgrManager {
public:
  /// create a new DocumentManager object 
  static DocumentManager* createDocMgr(string type, string name="", 
				       string parsetype="", string sources = "");

  /// open an existing DocumentManager using the specified name
  /// similar to indexes, the DM's name extension should indicate what
  /// MD it is.
  static DocumentManager* openDocMgr(string name);
};

#endif
