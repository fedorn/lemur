/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

/* type definitions for index objects */
/* these probably should be been in a namespace */
#ifndef _INDEXTYPES_H
#define _INDEXTYPES_H

#include "common_headers.hpp"

typedef int   FILEID_T;
typedef int   TERMID_T;
typedef TERMID_T LOC_T;  // for simplifying implementation of memory management, it's helpful for termid and location to be of the same type.
typedef TERMID_T DOCID_T;
typedef float SCORE_T;
typedef string TERM_T;
typedef string EXDOCID_T;
typedef void* POS_T;  // Used by DocLists and TermLists

#endif
