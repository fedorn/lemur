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

#ifndef _RESULTFILE_HPP
#define _RESULTFILE_HPP

/// representation of result file 

class ResultFile {
public:
  ResultFile(bool TRECFormat = true) : trecFmt (TRECFormat) {
  }
  
  ~ResultFile() {}

  void openForRead(istream &is, Index &index) {
    inStr=&is;
    ind = &index;
    eof = !readLine();
  }
  
  void getResult(char *expectedQID, IndexedRealVector &res)
  {
    res.clear();
    if (eof || strcmp(curQID, expectedQID)) {
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
  
  void openForWrite( ostream &os, Index &index) {
    
    outStr = &os;
    ind = &index;
  }

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
