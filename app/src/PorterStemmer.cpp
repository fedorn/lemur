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



/// A Porter Stemmer
/*! \page PorterStemmer PorterStemmer Application

This application (PorterStemmer.cpp) transforms a DocStream by stemming
all the words using a porter stemmer. The output format is the format
used in BasicDocStream. 

To use it, follow the general steps of running a lemur application
and set the following variables in the parameter file:

(1) inputFile: the path to the source file (e.g., /user/xxx/data/source)

(2) outputFile: the path to the name for the output file (e.g.,  /user/xxx/data/source.stem)


*/

#include "BasicDocStream.hpp"
#include "Stemmer.hpp"
#include "Param.hpp"
#include "String.hpp"

namespace LocalParameter{
  String inputFile;   
  String outputFile;
  void get() {
    inputFile    = ParamGetString("inputFile");
    outputFile = ParamGetString("outputFile");
  }    
};

void GetAppParam() 
{
  LocalParameter::get();
}

int AppMain(int argc, char * argv[]) {
  
  BasicDocStream dstream(LocalParameter::inputFile);
  ofstream ofs(LocalParameter::outputFile);

  dstream.startDocIteration();
  while (dstream.hasMore()) {
    Document *doc = dstream.nextDoc();
    ofs << "<DOC " << doc->getID() << ">\n";
    doc->startTermIteration();
    while (doc->hasMore()) {
      TokenTerm *term = doc->nextTerm();
      char buffer[5000];
      strcpy(buffer,term->spelling());
      Stemmer::stemWord(buffer);
      ofs << buffer << endl;
    }
    ofs << "</DOC>\n";
  }
  ofs.close();
  return 0;
}

