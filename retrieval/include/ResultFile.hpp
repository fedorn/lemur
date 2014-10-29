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


#ifndef _RESULTFILE_HPP
#define _RESULTFILE_HPP

#include "common_headers.hpp"
#include "Index.hpp"
#include "IndexedReal.hpp"
#include "ISet.hpp"
#include "String.hpp"
namespace lemur 
{
  namespace utility
  {
    /// Hash table entry for storing results
    struct ResultEntry {
    public:
      String key;
      unsigned int Hash() const { return key.Hash();}
      unsigned int hash() const { return Hash();} 
      bool operator==(const ResultEntry entry) { return (entry.key ==  this->key);}
      lemur::api::IndexedRealVector *res;
    };
  }
}
namespace lemur 
{
  namespace api
  {
    /// representation of result file 

    class ResultFile {
    public:
      ResultFile(bool TRECFormat = true) : trecFmt (TRECFormat), resTable(NULL) {
        resTable = new lemur::utility::ISet<lemur::utility::ResultEntry>(50);
      }
  
      ~ResultFile() { delete resTable;}

      /// Open and associate an input stream for reading, e.g., with getResult function
      void openForRead(istream &is, Index &index);
  
      /// Load all the results into an internal hash table, so as to allow random access to any of the results.
      void load(istream &is, Index &index);

      /// Read the results for a given query from the associated input stream into memory (stored in res), sequential reading, so appropriate order must be maintained and attempting to get the results for a query that has no results will fail.
      void getResult(const string& expectedQID, IndexedRealVector &res);

      /// Find the results for the given query id, the output variable res gets a pointer to the results, returns true iff found.
      bool findResult(const string& queryID, IndexedRealVector *&res);
  
      /// Associate an output stream for writing results
      void openForWrite( ostream &os, Index &index) {
    
        outStr = &os;
        ind = &index;
      }

      /// writing the results (stored in <tt> results</tt>) into the associated output stream, up to a maximum count.

      void writeResults(const string& queryID, IndexedRealVector *results, int maxCountOfResult);
  
    private:

      bool readLine();

      bool trecFmt;

      //  char curQID[300];
      string curQID;
      char curDID[300];
      double curSC;
      Index *ind;
      istream *inStr;
      ostream *outStr;
      bool eof;

      lemur::utility::ISet<lemur::utility::ResultEntry> * resTable;

    };
  }
}
#endif /* _RESULTFILE_HPP */
