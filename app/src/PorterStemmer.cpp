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

