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
/*! \page BasicSummApp Basic Summarization Application

This application builds a sentence selection based
summary.

<P>
 This application generates a summary for a document with sentence selection.
<P>
To use it, follow the general steps of running a lemur application and
set the following variables in the parameter file:
<p>
<ol>

<li><tt>indexFile</tt>: the path to the index TOC. The index must be an
InvFPIndex (.ifp).

<li><tt>docID</tt>: the external id of the document to summarize.
<li><tt>summLength</tt>: summary length, default is 5 sentences.
</ol>
<p>
The summary is sent to standard out.

The following is an example of use:
<PRE> 

 % cat summparam
&lt;parameters&gt;
   &lt;indexFile&gt;idx.ifp&lt;/indexFile&gt;
   &lt;docID&gt;WTX001-B01-1&lt;/docID&gt;
&lt;/parameters&gt;
 % BasicSummApp summparam
 
 </PRE>

*/

#include "Param.hpp"
#include "BasicSumm.hpp"
#include "InvFPIndex.hpp"
#include <iostream>

using namespace lemur::api;
using namespace lemur::summarization;

namespace LocalParameter {
  std::string index;   
  std::string docID;
  int summLength;   
  void get() {
    index  = ParamGetString("index");
    docID = ParamGetString("docID");
    summLength   = ParamGetInt("summLength", 5);
    
  }    
};

void GetAppParam() {
  LocalParameter::get();
}

int AppMain(int argc, char* argv[]) {
  lemur::index::InvFPIndex idx(LocalParameter::index);
  // Create a basic summarizer
  lemur::summarization::BasicSumm* s = new lemur::summarization::BasicSumm(&idx);
  // Generate a summary
  s->summDocument(LocalParameter::docID, LocalParameter::summLength, NULL);
  // Print to stdout
  s->outputSumm();
  return 0;
}
