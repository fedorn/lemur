 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* type definitions for objects we will use */
#ifndef _INVFPTYPES_H
#define _INVFPTYPES_H

typedef int   FILEID_T;
typedef int   TERMID_T;
typedef TERMID_T LOC_T;  // for simplifying implementation of memory management, it's helpful for termid and location to be of the same type.
typedef TERMID_T DOCID_T;
typedef float SCORE_T;
typedef char* TERM_T;
typedef char* EXDOCID_T;

// suffixes for filenames
#define INVINDEX  ".invfp"
#define INVLOOKUP  ".invlookup"
#define DTINDEX  ".dt"
#define DTLOOKUP  ".dtlookup"
#define TERMIDMAP  ".tid"
#define DOCIDMAP  ".did"
#define MAINTOC  ".ifp"

// what to call out of vocabulary ids
#define INVALID_STR "[OOV]"

// name for parameters
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

struct LocatedTerm { // pair of term and its location
  TERMID_T term;
  LOC_T loc;
};

struct dt_entry {   // an entry in the lookup table for docterm lists index
  FILEID_T fileid;  // which file the word is in
  long offset;        // what the offset into the file is
  int length;         // the length of the inverted list
};

struct inv_entry {   // an entry in the lookup table for docterm lists index
  FILEID_T fileid;  // which file the word is in
  long offset;        // what the offset into the file is
  int ctf;            // collection term freq
  int df;             // doc freq
};

#endif
