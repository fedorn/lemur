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


#ifndef _RESULTFILE_HPP
#define _RESULTFILE_HPP

#include "common_headers.hpp"
#include "Index.hpp"
#include "IndexedReal.hpp"

/// representation of result file 

class ResultFile {
public:
  ResultFile(bool TRECFormat = true) : trecFmt (TRECFormat) {
  }
  
  ~ResultFile() {}

  /// Open and associate an input stream for reading, e.g., with getResult function
  void openForRead(istream &is, Index &index);
  
  /// Read the results for a given query from the associated input stream into memory (stored in res) 
  void getResult(char *expectedQID, IndexedRealVector &res);
  
  /// Associate an output stream for writing results
  void openForWrite( ostream &os, Index &index) {
    
    outStr = &os;
    ind = &index;
  }

  /// writing the results (stored in <tt> results</tt>) into the associated output stream, up to a maximum count.

  void writeResults(char *queryID, IndexedRealVector *results, int maxCountOfResult);
  
private:

  bool readLine();

  bool trecFmt;

  char curQID[300];
  char curDID[300];
  double curSC;
  Index *ind;
  istream *inStr;
  ostream *outStr;
  bool eof;

};


#endif /* _RESULTFILE_HPP */










