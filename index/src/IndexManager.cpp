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

#include "IndexManager.hpp"

#include "BasicIndex.hpp"
#include "InvFPIndex.hpp"
#include "InvIndex.hpp"
#include "KeyfileIncIndex.hpp"

Index *IndexManager::openIndex(const string &tocFile)
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
    ind = new InvFPIndex();
  } else if ((!strcmp(extension, "BSC")) ||
	     (!strcmp(extension, "bsc"))) {
    
    ind = new BasicIndex();
  } else if ((!strcmp(extension, "INV")) ||
	     (!strcmp(extension, "inv"))) {
    ind = new InvIndex();
  } else if (!strcmp(extension, "key")) {
    ind = new KeyfileIncIndex();
  } else {
    throw Exception("IndexManager", "unknown index file extension");
  }
  if (!(ind->open(indexTOCFile))) {
    throw Exception("IndexManager", "open index failed");
  }
  return (ind);
}
