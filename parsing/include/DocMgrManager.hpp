/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
 */

#ifndef _DOCMGRMANAGER_HPP
#define _DOCMGRMANAGER_HPP
#include "DocumentManager.hpp"

namespace lemur 
{
  namespace api
  {

    /// \brief This is a factory class for creating and opening DocumentManagers.    
    /*!
     * New DocumentManagers should be added here.
     *
     */
    class DocMgrManager {
    public:
      /// create a new DocumentManager object 
      /// Recognized types are: flat -- FlattextDocMgr; or 
      /// bdm -- KeyfileDocMgr.
      /// elem -- ElemDocMgr.
      static DocumentManager* createDocMgr(string type, string name="", 
                                           string parsetype="", 
                                           string sources = "");

      /// open an existing DocumentManager using the specified name
      /// similar to indexes, the DM's name extension should indicate what
      /// DM it is.
      /// Recognized extensions are : .flat -- FlattextDocMgr; or 
      /// .bdm -- KeyfileDocMgr.
      /// elem -- ElemDocMgr.
      /// caller deletes
      /// @param name the name of the document manager table of contents file.
      /// @param readOnly  open the manager in read only mode.
      static DocumentManager* openDocMgr(const string name, bool readOnly = true);
    };
  }
}

#endif
