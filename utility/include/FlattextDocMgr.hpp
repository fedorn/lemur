/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#ifndef _FLATTEXTDOCMGR_HPP
#define _FLATTEXTDOCMGR_HPP

#include "common_headers.hpp"
#include "TextHandlerManager.hpp"
#include "Exception.hpp"
#include "DocumentManager.hpp"

#define FT_SUFFIX ".flat"
#define FT_LOOKUP ".lookup"
#define FT_FID    ".fid"

class FlattextDocMgr : public DocumentManager, public TextHandler {
public:

  struct lookup_e {
    int fid;
    int offset;
    int bytes;
  };

  struct abc {
    bool operator() (char* s1, char* s2) const {
      return strcmp(s1, s2) < 0;
    }
  };

  /// constructor (for build) 
  ///             name = what to name this manager
  ///             mode = type of parser to use 
  ///             source = file with list of files this will manage
  FlattextDocMgr(string name, string mode, string source);  

  /// constructor (for open)
  ///             name = toc file for this manager (same as getMyID) 
  FlattextDocMgr(const char* name);

  virtual ~FlattextDocMgr();

  /// open previously created manager
  virtual bool open(const char*manname);

  /// get the ID string to use for opening this later
  virtual const char* getMyID();
  
  /// return the document associated with this ID
  virtual char* getDoc(const char* docID);

  virtual void buildMgr();

  char* handleDoc(char * docno);

  /// returns self cast to TextHandler (FIX INHERITANCE HERE!)
  virtual TextHandler* getTextHandler() { return this; }

private:
  /// open the file with all the sources and save internally
  bool readinSources(const char* fn);

  /// save info we'll need to open this Manager
  void writeTOC();

  bool loadTOC(const char* fn);
  bool loadFTLookup(const char* fn);
  bool loadFTFiles(const char* fn, int num);

  int numdocs;              // how many docs we have
  string parseMode;           // what type of parser we have
  long prevpos;              // pos of previous doc beginning
  string IDname;            // my name
  vector<string> sources;   // list of all source files
  int fileid, lastid;       // fileid of current/last file being processed
  ofstream writefpos;       // stream for writing out file positions
  map<char*, lookup_e*, abc> table; 
  lookup_e* entries;        // array of lookup entries
};

#endif
