 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
