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

//TOC
#define ELEM_TOC ".elem"
#define ELEM_TOC_LEN 5

// elems
#define ELEM_ELS ".els"


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

class ElemDocMgr : public KeyfileDocMgr {
public:

  /// constructor (for open)
  ///             name = toc file for this manager (same as getMyID) 
  ElemDocMgr(const char *name);

  /// constructor (for build) 
  ///             name = what to name this manager
  ///             mode = type of parser to use 
  ///             source = file with list of files this will manage
  ElemDocMgr(string name, string mode, string source);  

  virtual ~ElemDocMgr();

  /// Handle a begin tag token
  virtual char* handleBeginTag(char* tag, char* orig, PropertyList* props);

  /// Handle a end tag token
  virtual char* handleEndTag(char* tag, char* orig, PropertyList* props);

  /// Get an element of a document with "element" name
  /// caller deletes memory with delete[]
  /// returns NULL if no such element exists for document
  virtual char* getElement(const char* docid, const char* element);

  /// Open and load the toc file manname.
  virtual bool open(const char* manname) {
    string tmp(manname);
    IDname = tmp.substr(0, tmp.length() - 5);
    return loadTOC();
  }

protected:
  virtual void writeTOC();
  virtual bool loadTOC();

  map<char*,btl,abc> elemtable; // map for temp creating elem strings

  Keyfile elements;  // btree for storing elements
};

#endif // _ELEMDOCMGR_HPP
