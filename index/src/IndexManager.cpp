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
  } else if ((!strcmp(extension, "LMR")) ||
	     (!strcmp(extension, "lmr"))) {
    
    ind = new BasicIndex();
  }
  ind->open(indexTOCFile);
  return (ind);
}
