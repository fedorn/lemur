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

/// representation of result file 

class ResultFile {
public:
  ResultFile(bool TRECFormat = true) : trecFmt (TRECFormat) {
  }
  
  ~ResultFile() {}

  /// Open and associate an input stream for reading, e.g., with getResult function
  void openForRead(istream &is, Index &index) {
    inStr=&is;
    ind = &index;
    eof = !readLine();
  }
  
  /// Read the results for a given query from the associated input stream into memory (stored in res) 
  void getResult(char *expectedQID, IndexedRealVector &res)
  {
    res.clear();
    if (eof || strcmp(curQID, expectedQID)) {
      if (!eof) {
	cerr << "expected query ID: "<< expectedQID << " actual ID: "<<curQID <<endl;
      } else {
	cerr << "Unexpected end of the result file\n";
      }
      cerr.flush();
 
      throw Exception("ResultFile", "query id mismatch between the original query and the result file");
    }
    do {
      res.PushValue(ind->document(curDID), curSC);
      if (! readLine()) {
	// end of file
	eof = true;
	break;
      }
    } while (!strcmp(curQID, expectedQID));
  }
  
  /// Associate an output stream for writing results
  void openForWrite( ostream &os, Index &index) {
    
    outStr = &os;
    ind = &index;
  }

  /// writing the results (stored in <tt> results</tt>) into the associated output stream, up to a maximum count.
  void writeResults(char *queryID, IndexedRealVector *results, int maxCountOfResult)
  {
    IndexedRealVector::iterator j;
    int count=0;
    for (j= results->begin();j!=results->end();j++) {
      if (count >= maxCountOfResult) {
      break;
      } else {
	*outStr << queryID 
	       << (trecFmt ? " Q0 ":" ")  
	       << ind->document((*j).ind) << " " 
	       << (trecFmt? " 0 ":" ")  
	       <<  (*j).val 
	       << (trecFmt? " Exp ":" ")  
	       << endl;
	count++;
	
      }
    }  
    outStr->flush();
  }
  
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

bool ResultFile::readLine()
{
    char dummy1[100];
    char dummy2[100];
    char dummy3[100];

    if (trecFmt) {
      return (*inStr >> curQID >> dummy1 >> curDID >> dummy2 >> curSC >> dummy3);
    } else {
      return (*inStr >> curQID >> curDID >> curSC);
    }
}

#endif /* _RESULTFILE_HPP */










