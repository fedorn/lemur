/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _FLATTEXTDOCMGR_HPP
#define _FLATTEXTDOCMGR_HPP

#include "common_headers.hpp"
#include "TrecParser.hpp"
#include "WebParser.hpp"
#include "ReutersParser.hpp"
#include "Exception.hpp"
#include "DocumentManager.hpp"

#define FT_SUFFIX ".flat"
#define FT_LOOKUP ".lookup"
#define FT_FID    ".fid"

class FlattextDocMgr : public DocumentManager, public TextHandler {
public:
  enum ParseModes {TREC=0, WEB=1};

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
  FlattextDocMgr(char* name, ParseModes mode, char* source);
  
  /// constructor (for open)
  ///             name = toc file for this manager (same as getMyID) 
  FlattextDocMgr(const char* name);

  ~FlattextDocMgr();

  /// open previously created manager
  bool open(const char*manname);

  /// get the ID string to use for opening this later
  const char* getMyID();

  /// return the document associated with this ID
  char* getDoc(const char* docID);

  void buildMgr();

  char* handleDoc(char * docno);

  /// based on the given ParseMode, create and return a parser
  static Parser* createParser(ParseModes mode);

private:
  /// open the file with all the sources and save internally
  bool readinSources(char* fn);

  /// save info we'll need to open this Manager
  void writeTOC();

  bool loadTOC(char* fn);
  bool loadFTLookup(const char* fn);
  bool loadFTFiles(const char* fn, int num);

  int numdocs;              // how many docs we have
  ParseModes pm;           // what type of parser we have
  long prevpos;              // pos of previous doc beginning
  string IDname;            // my name
  vector<string> sources;   // list of all source files
  int fileid, lastid;       // fileid of current/last file being processed
  ofstream writefpos;       // stream for writing out file positions
  map<char*, lookup_e, abc> table; 
};

#endif
