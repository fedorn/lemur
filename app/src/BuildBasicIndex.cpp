
/// A BasicIndex builder
/*! \page BuildBasicIndex BuildBasicIndex Application

This application builds a BasicIndex. 

To use it, follow the general steps of running a lemur application
and set the following variables in the parameter file:

(1) inputFile: the path to the source file

(2) outputPrefix: a prefix name for your index (e.g.,  "index")

(3) maxDocuments: maximum number of documents to index (default: 1000000)

(4) maxMemory: maximum amount of memory to use for indexing (default:0x8000000, or 128MB)


The following is an example of use:
<PRE> 

 % cat buildparam
   
 inputFile    = /usr0/mydata/source;
 outputPrefix    = /usr0/mydata/index;
 maxDocuments = 200000;
 maxMemory    = 0x10000000;

 % setenv PARAM buildparam
 % BuildBasicIndex

 
 </PRE>



*/

#include "BasicDocStream.hpp"
#include "GammaCompress.hpp"
#include "BasicIndex.hpp"
#include "Param.hpp"
#include "String.hpp"

String inputFile;   
String outputPrefix;
int maxDocuments;   
int maxMemory;      

void GetAppParam() 
{
 inputFile    = ParamGetString("inputFile");
 outputPrefix = ParamGetString("outputPrefix");
 maxDocuments   = ParamGetInt("maxDocuments", 1000000);
 maxMemory      = ParamGetInt("maxMemory", 0x8000000);
}

int AppMain(int argc, char * argv[]) {
  
  BasicIndex lemur(new GammaCompress());
  BasicDocStream dstream(inputFile);
  lemur.build(&dstream, inputFile, outputPrefix, maxDocuments, maxMemory);

  return 0;
}

