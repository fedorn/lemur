/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/



/// A simple summarizer
/*! \page BasicSummApp Application



This application (BasicSummApp.cpp) builds a sentence selection based summary


<P>
 This application generates a summary for a document with sentence selection.
<P>
To use it, follow the general steps of running a lemur application and set the following variables in the parameter file:
<p>
<ol>
<li><tt>indexFile</tt>: the path to the index TOC.
<li><tt>docID</tt>: the document to summarize.
<li><tt>summLength</tt>: summary length.<br>
</ol>
<p>
The summary is sent to standard out.

The following is an example of use:
<PRE> 

 % cat summparam
   
   indexFile = idx.invfp;
   docID = WTX001-B01-1;

 % BasicSummApp summparam
 
 </PRE>


*/

#include "Param.hpp"
#include "String.hpp"
#include "BasicSumm.hpp"
#include "InvFPIndex.hpp"
#include <iostream>


namespace LocalParameter{
  String index;   
  String docID;
  int summLength;   
  void get() {
    index  = ParamGetString("index");
    docID = ParamGetString("docID");
    summLength   = ParamGetInt("summLength", 5);
    
  }    
};

void GetAppParam() 
{
  LocalParameter::get();
}

int AppMain(int argc, char* argv[]) {

  InvFPIndex idx(LocalParameter::index);

  // Create a basic summarizer
  BasicSumm* s = new BasicSumm(&idx);
  
  // Generate a summary
  s->summDocument(LocalParameter::docID, LocalParameter::summLength, NULL);

  // Print to stdout
  s->outputSumm();

  return 0;

}
