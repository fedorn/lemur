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


/// A BasicIndex builder
/*! \page BuildBasicIndex BuildBasicIndex Application

This application builds a BasicIndex. 

To use it, follow the general steps of running a lemur application
and set the following variables in the parameter file:

(1) inputFile: the path to the source file (e.g., /user/xxx/data/source)

(2) outputPrefix: the path prefix name for your index (e.g.,  /user/xxx/data/index)

(3) maxDocuments: maximum number of documents to index (default: 1000000)

(4) maxMemory: maximum amount of memory to use for indexing (default:0x8000000, or 128MB)

A "table-of-content" (TOC) file with a name of the format outputPrefix.bsc will be written
in the directory where the index is stored. 

The following is an example of use:
<PRE> 

 % cat buildparam
   
 inputFile    = /usr0/mydata/source;
 outputPrefix    = /usr0/mydata/index;
 maxDocuments = 200000;
 maxMemory    = 0x10000000;

 % BuildBasicIndex buildparam
 
 The TOC file is /usr0/mydata/index.bsc.
 
 </PRE>



*/

#include "BasicDocStream.hpp"
#include "GammaCompress.hpp"
#include "BasicIndex.hpp"
#include "Param.hpp"
#include "String.hpp"

namespace LocalParameter{
  String inputFile;   
  String outputPrefix;
  int maxDocuments;   
  int maxMemory;      
  void get() {
    inputFile    = ParamGetString("inputFile");
    outputPrefix = ParamGetString("outputPrefix");
    maxDocuments   = ParamGetInt("maxDocuments", 1000000);
    maxMemory      = ParamGetInt("maxMemory", 0x8000000);
    
  }    
};

void GetAppParam() 
{
  LocalParameter::get();
}

int AppMain(int argc, char * argv[]) {
  
  BasicIndex lemur(new GammaCompress());
  BasicDocStream dstream(LocalParameter::inputFile);
  lemur.build(&dstream, LocalParameter::inputFile, LocalParameter::outputPrefix, LocalParameter::maxDocuments, LocalParameter::maxMemory);

  return 0;
}

