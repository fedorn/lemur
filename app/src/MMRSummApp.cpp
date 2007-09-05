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
/*! \page MMRSummApp

This application builds an MMR based summary.

<P>
 This application generates a summary for a document with MMR.

<P> To use it, follow the general steps of running a lemur application
and set the following variables in the parameter file:

<ol>
<li><tt>index</tt>: the path to the index TOC.
<li><tt>docID</tt>: the document to summarize.
<li><tt>query</tt>: A stemmed query to use for q-based summary
<li><tt>summLength</tt>: summary length. Default is 5 sentences.
</ol>
<p>
The summary is sent to standard out.

The following is an example of use:
<PRE> 

 % cat summparam
&lt;parameters&gt;
   &lt;index&gt;idx.key&lt;/index&gt;
   &lt;docID&gt;WTX001-B01-1&lt;/docID&gt;
&lt;/parameters&gt;

 % MMRSummApp summparam
 
 </PRE>
*/

#include "Param.hpp"
#include "MMRSumm.hpp"
#include "IndexManager.hpp"
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

  lemur::api::Index * idx = IndexManager::openIndex(LocalParameter::index);

  // Create an MMR summarizer
  lemur::summarization::MMRSumm* s = new lemur::summarization::MMRSumm(idx);
  
  if ( LocalParameter::query.empty() ) {
    // generic summary
    s->summDocument(LocalParameter::docID, LocalParameter::summLength, "");
  } else {
    // query relevent summary
    s->summDocument(LocalParameter::docID, 15, LocalParameter::query);
  }

  s->outputSumm();
  delete(idx);
  return 0;
}
