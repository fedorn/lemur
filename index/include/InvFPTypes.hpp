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


/* type definitions for objects we will use */
#ifndef _INVFPTYPES_H
#define _INVFPTYPES_H

#define IND_VERSION "2.0.2"

#include "common_headers.hpp"

typedef int   FILEID_T;
typedef int   TERMID_T;
typedef TERMID_T LOC_T;  // for simplifying implementation of memory management, it's helpful for termid and location to be of the same type.
typedef TERMID_T DOCID_T;
typedef float SCORE_T;
typedef char* TERM_T;
typedef char* EXDOCID_T;

// suffixes for filenames
#define INVINDEX  ".invf"
#define INVFPINDEX ".invfp"
#define INVLOOKUP  ".invlookup"
#define DTINDEX  ".dt"
#define DTLOOKUP  ".dtlookup"
#define TERMIDMAP  ".tid"
#define DOCIDMAP  ".did"
#define MAINTOC  ".inv"
#define INVFPTOC ".ifp"
#define DOCMGRMAP ".dm"

// what to call out of vocabulary ids
#define INVALID_STR "[OOV]"

// name for parameters
#define VERSION_PAR "VERSION"
#define NUMDOCS_PAR "NUM_DOCS"
#define NUMTERMS_PAR "NUM_TERMS"
#define NUMUTERMS_PAR "NUM_UNIQUE_TERMS"
#define AVEDOCLEN_PAR "AVE_DOCLEN"
#define INVINDEX_PAR  "INV_INDEX"
#define INVLOOKUP_PAR  "INV_LOOKUP"
#define DTINDEX_PAR  "DT_INDEX"
#define DTLOOKUP_PAR  "DT_LOOKUP"
#define TERMIDMAP_PAR  "TERMIDS"
#define DOCIDMAP_PAR  "DOCIDS"
#define NUMDT_PAR  "NUM_DTFILES"
#define NUMINV_PAR  "NUM_INVFILES"
#define DOCMGR_PAR  "DOCMGR_IDS"

struct LocatedTerm { // pair of term and its location
  TERMID_T term;
  LOC_T loc;
};

struct LLTerm { // pair of term and list of locations
  TERMID_T term;
  vector<LOC_T> loc;
};

struct dt_entry {   // an entry in the lookup table for docterm lists index
  FILEID_T fileid;  // which file the word is in
  long offset;        // what the offset into the file is
  int length;         // the length of the inverted list
  int docmgr;         // the docmgr id of manager for this doc
};

struct inv_entry {   // an entry in the lookup table for docterm lists index
  FILEID_T fileid;  // which file the word is in
  long offset;        // what the offset into the file is
  int ctf;            // collection term freq
  int df;             // doc freq
};

struct ltstr
{
  bool operator()(char* s1, char* s2) const{
    return strcmp(s1, s2) < 0;
  }
};

#endif
