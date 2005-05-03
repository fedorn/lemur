/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

#ifndef _ELEMDOCMGR_HPP
#define _ELEMDOCMGR_HPP

#include "KeyfileDocMgr.hpp"
namespace lemur 
{
  namespace parse 
  {
    
    //TOC
#define ELEM_TOC ".elem"
#define ELEM_TOC_LEN 5

    // elems
#define ELEM_ELS ".els"

    // we should really do something about this, it appears all over the toolkit
#ifndef _ABC_OP
#define _ABC_OP
    struct abc
    {
      bool operator()(char* s1, char* s2) const{
        return strcmp(s1, s2) < 0;
      }
    };
#endif

    /*!
      Document manager using Keyfile for data storage. In addition to providing
      access to raw document text, also stores byte offsets (start and end byte)
      for each token within the document. Useful for passage windows or using
      query term match information for highlighting. Implements TextHandler
      interface for building the manager.
      Extended from KeyfileDocMgr to include another DB for storing elements of
      the document, ie titles. Elements can handle nested tags, but can have only
      one of each type of element for each document. If more than one of the same
      type of element, the last one is the one actually saved. If more than one of
      the same type element tags are nested, the inner tags are ignored.
    */

    class ElemDocMgr : public KeyfileDocMgr {
    public:

      /// constructor (for open)
      /// @param name  toc file for this manager (same as getMyID) 
      /// @param readOnly whether or not to open the manager in read only mode.
      ElemDocMgr(const string &name, bool readOnly = true);

      /// constructor (for build) 
      ///             name = what to name this manager
      ///             mode = type of parser to use 
      ///             source = file with list of files this will manage
      ElemDocMgr(string name, string mode, string source);  

      virtual ~ElemDocMgr();

      /// Handle a begin tag token
      virtual char* handleBeginTag(char* tag, const char* orig, PropertyList* props);

      /// Handle a end tag token
      virtual char* handleEndTag(char* tag, const char* orig, PropertyList* props);

      /// Get an element of a document with "element" name
      /// caller deletes memory with delete[]
      /// returns NULL if no such element exists for document
      virtual char* getElement(const char* docid, const char* element) const;

      /// Open and load the toc file manname.
      virtual bool open(const string &manname) {
        IDnameext = manname;
        IDname = manname.substr(0, manname.length() - 5);
        return loadTOC();
      }

    protected:
      virtual void writeTOC();
      virtual bool loadTOC();

      map<char*,btl,abc> elemtable; // map for temp creating elem strings

      lemur::file::Keyfile elements;  // btree for storing elements
    };
  }
}

#endif // _ELEMDOCMGR_HPP
