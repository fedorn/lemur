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


#ifndef _INVFPINDEXMERGE_HPP
#define _INVFPINDEXMERGE_HPP

#include "common_headers.hpp"
#include "InvIndexMerge.hpp"
#include "InvFPDocList.hpp"
#include "InvFPTypes.hpp"

namespace lemur
{
  namespace index 
  {
    
    struct InvFPIndexReader {
      InvFPDocList* list;
      ifstream* reader;
    };

    // this class could actually be static
    class InvFPIndexMerge : public InvIndexMerge {
    public:
      /// constructor.  
      /// vector : a list of files to merge
      /// char* : name prefix of created files
      /// long  : the maximum size the index can be
      InvFPIndexMerge(char* buffer, long size, long maxfilesize=2100000000);
      InvFPIndexMerge(long buffersize=64000000, long maxfilesize=2100000000);
      virtual ~InvFPIndexMerge();

      /// merge these files and put the results into the intmed list
      virtual int mergeFiles(vector<string>* files, vector<string>* intmed, int level);

      /// do the final merge and write the lookup table
      virtual int finalMerge(vector<string>* files);


    private:
      void writeInvFIDs();
      /// figure out which readers point to the lowest termids
      void least(vector<InvFPIndexReader*>* r, vector<lemur::api::TERMID_T>* ret);
    };
  }
}

#endif
