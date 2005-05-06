/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "IndexManager.hpp"

#include "InvFPIndex.hpp"
#include "InvIndex.hpp"
#include "KeyfileIncIndex.hpp"
#include "LemurIndriIndex.hpp"

lemur::api::Index *lemur::api::IndexManager::openIndex(const string &tocFile)
{
  const char *indexTOCFile = tocFile.c_str(); // rewrite this to use the string.
   
  int len = strlen(indexTOCFile);
  if (!(len>4 && indexTOCFile[len-4]=='.')) {
    ; // it must have format .xxx 
    throw Exception ("IndexManager","Index file must have format .xxx");
  }

  Index *ind;
  const char *extension = &(indexTOCFile[len-3]);
  if ( (!strcmp(extension, "IFP")) || 
       (!strcmp(extension, "ifp"))) {
    // InvFP
    ind = new lemur::index::InvFPIndex();
  } else if ((!strcmp(extension, "INV")) ||
             (!strcmp(extension, "inv"))) {
    ind = new lemur::index::InvIndex();
  } else if ((!strcmp(extension, "key")) ||
             (!strcmp(extension, "KEY"))) {
    ind = new lemur::index::KeyfileIncIndex();
  } else if ((!strcmp(extension, "ind")) ||
             (!strcmp(extension, "IND"))) {
    ind = new lemur::index::LemurIndriIndex();
  } else {
    throw Exception("IndexManager", "unknown index file extension");
  }
  if (!(ind->open(indexTOCFile))) {
    throw Exception("IndexManager", "open index failed");
  }
  return (ind);
}
