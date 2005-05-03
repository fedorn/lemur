/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/



/// A simple summarizer
/*! \page BasicSummApp Application



This application (MMRSummApp.cpp) builds an MMR based summary


<P>
 This application generates a summary for a document with MMR.
<P>
To use it, follow the general steps of running a lemur application and set the following variables in the parameter file:
<p>
<ol>
<li><tt>indexFile</tt>: the path to the index TOC.
<li><tt>docID</tt>: the document to summarize.
<li><tt>query</tt>: A stemmed query to use for q-based summary
<li><tt>summLength</tt>: summary length.<br>
</ol>
<p>
The summary is sent to standard out.

The following is an example of use:
<PRE> 

 % cat summparam
   
   indexFile = idx.invfp;
   docID = WTX001-B01-1;

 % MMRSummApp summparam
 
 </PRE>


*/

#include "Param.hpp"
#include "MMRSumm.hpp"
#include "InvFPIndex.hpp"
#include <iostream>

using namespace lemur::api;
using namespace lemur::summarization;

namespace LocalParameter{
  std::string index;   
  std::string docID;
  std::string query;
  int summLength;   
  void get() {
    index  = ParamGetString("index");
    docID = ParamGetString("docID");
    summLength   = ParamGetInt("summLength", 5);
    query = ParamGetString("query", "");
    
  }    
};

void GetAppParam() 
{
  LocalParameter::get();
}


int AppMain(int argc, char* argv[]) {

  lemur::index::InvFPIndex idx(LocalParameter::index);

  // Create an MMR summarizer
  lemur::summarization::MMRSumm* s = new lemur::summarization::MMRSumm(&idx);

  if ( !(LocalParameter::query.empty()) ) {
    // generic summary
    s->summDocument(LocalParameter::docID, LocalParameter::summLength, NULL);
  } else {
    // query relevent summary
    s->summDocument(LocalParameter::docID, 15, LocalParameter::query);
  }

  s->outputSumm();
  return 0;
}
