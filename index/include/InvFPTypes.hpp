/* type definitions for objects we will use */
#ifndef _TYPES_H
#define _TYPES_H

typedef int   FILEID_T;
typedef int   TERMID_T;
typedef TERMID_T LOC_T;  // for simplifying implementation of memory management, it's helpful for termid and location to be of the same type.
typedef TERMID_T DOCID_T;
typedef float SCORE_T;
typedef char* TERM_T;

struct LocatedTerm { // pair of term and its location
  TERMID_T term;
  LOC_T loc;
};

#endif
