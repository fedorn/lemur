/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

/* type definitions for index objects */
#ifndef _INDEXTYPES_H
#define _INDEXTYPES_H

#include <string>
namespace lemur
{
  namespace api
  {
    
    typedef int   FILEID_T;
    // All four of TERMID_T, DOCID_T, LOC_T, and COUNT_T need to be
    // the same size for the Inv(FP) and Keyfile indexes to work
    // without rewrite. 10/05/2004 -- dmf
    typedef int TERMID_T;
    typedef TERMID_T LOC_T;
    typedef TERMID_T DOCID_T;
    typedef TERMID_T COUNT_T;
    typedef float SCORE_T;
    typedef std::string TERM_T;
    typedef std::string EXDOCID_T;
    typedef void* POS_T;  // Used by DocLists and TermLists
  }
}

#endif
