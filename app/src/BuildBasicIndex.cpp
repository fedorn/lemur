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



/// A BasicIndex builder
/*! \page BuildBasicIndex BuildBasicIndex Application

This application (BuildBasicIndex.cpp) builds a BasicIndex. 

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

