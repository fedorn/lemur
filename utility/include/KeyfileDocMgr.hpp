/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#ifndef _LEMUR_KEYFILE_DOCMGR_HPP
#define _LEMUR_KEYFILE_DOCMGR_HPP

#include "common_headers.hpp"
#include "DocumentManager.hpp"
#include "RVLCompress.hpp"
#include "TextHandlerManager.hpp"
#include "Match.hpp"
#include "Keyfile.hpp"

// array of byte offsets, indexed by token for each doc.
#define BT_POSITIONS ".btp"
// source file start, length
#define BT_LOOKUP ".btl"
// TOC
#define BT_TOC ".bdm"
// source files.
#define BT_FID ".bfi"


/*!
  Document manager using Keyfile for data storage. In addition to providing
  access to raw document text, also stores byte offsets (start and end byte)
  for each token within the document. Useful for passage windows or using
  query term match information for highlighting. Implements TextHandler
  interface for building the manager.
 */

class KeyfileDocMgr : public DocumentManager, public TextHandler {
public:
  /// default constructor 
  KeyfileDocMgr() {  myDoc = NULL;  numdocs = 0; }

  /// constructor (for open)
  ///             name = toc file for this manager (same as getMyID) 
  KeyfileDocMgr(const string &name);

  /// constructor (for build) 
  ///             name = what to name this manager
  ///             mode = type of parser to use 
  ///             source = file with list of files this will manage
  KeyfileDocMgr(string name, string mode, string source);  

  virtual ~KeyfileDocMgr();

  /// return the document associated with this ID
  char* getDoc(const string &docID) const;
  /// add entry for new doc
  virtual char* handleDoc(char * docno);
  /// finish entry for current doc
  virtual void handleEndDoc();
  /// Add start and end byte offsets for this term to the list of offsets
  virtual char *handleWord(char * word) {
    if (word != NULL) {
      int end = myparser->fileTell() - 1;
      int start = (end - strlen(word)) + 1;
      Match m;
      m.start = start - docEntry.offset;
      m.end = end - docEntry.offset;    
      offsets.push_back(m);
    }
    return word;
  }
  /// set myParser to p
  virtual void setParser(Parser *p) {
    myparser = p;
  }

  virtual Parser* getParser() const {
    return (TextHandlerManager::createParser(pm));
  }

  /// Build the document manager tables from the files previously provided
  /// in the constructor.
  virtual void buildMgr();
  /// return name of this document manager, with the file extension (.bdm).
  virtual const string &getMyID() const{
    return IDnameext;
  }

  /// get the array of Match entries for the tokens in the document
  /// named docID. The entries are indexed by token position (as is
  /// recorded in a TermInfoList object.
  vector<Match> getOffsets(const string &docID) const;

  /// Open and load the toc file manname.
  virtual bool open(const string &manname) {
    IDnameext = manname;
    IDname = manname.substr(0, manname.length() - 4);
    return loadTOC();
  }

protected:
  struct btl {
    int fid;
    long offset;
    long bytes;
  };

  Parser *myparser;
  virtual void writeTOC();
  virtual bool loadTOC();
  bool loadFTFiles(const string &fn, int num);
  // the return object
  mutable vector <Match> offsets;
  int numdocs;              // how many docs we have
  string pm;  // parse mode

  mutable Keyfile poslookup; // btree for lookup to positions list.
  mutable Keyfile doclookup; // btree for lookup to doc start.
  int dbcache;
  
  btl docEntry;
  char *myDoc;
  int doclen;
  string IDname;            // my name
  string IDnameext;			// my name w/ extension
  vector<string> sources;   // list of all source files
  int fileid;       // fileid of current/last file being processed
};

#endif // _LEMUR_KEYFILE_DOCMGR_HPP
