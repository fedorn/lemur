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


Index *IndexManager::openIndex(const char *indexTOCFile)
{
  int len = strlen(indexTOCFile);
  assert (len>4 && indexTOCFile[len-4]=='.'); // it must have format .xxx 

  Index *ind;
  const char *extension = &(indexTOCFile[len-3]);
  if ( (!strcmp(extension, "TOC")) || 
       (!strcmp(extension, "toc"))) {
    // InvFP
    ind = new InvFPIndex();
  } else if ((!strcmp(extension, "BSC")) ||
	     (!strcmp(extension, "bsc"))) {
    
    ind = new BasicIndex();
  } else {
    throw Exception("IndexManager", "unknown index file extension");
  }
  ind->open(indexTOCFile);
  return (ind);
}
