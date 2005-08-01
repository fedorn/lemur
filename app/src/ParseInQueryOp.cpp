/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/*! \page ParseInQueryOp 

This application parses a file containing structured queries into
BasicDocStream format.

The parameters are:
<p>
<ol>
<li> <tt>stopwords</tt>: name of file containing the stopword list.
<li> <tt>acronyms</tt>: name of file containing the acronym list.
<li> <tt>docFormat</tt>: 
<ul>
<li> "trec" for standard TREC formatted documents 
<li> "web" for web TREC formatted documents
<li> "chinese" for segmented Chinese text (TREC format, GB encoding)
<li> "chinesechar" for unsegmented Chinese text (TREC format, GB encoding)
<li> "arabic" for Arabic text (TREC format, Windows CP1256 encoding)
</ul>
<li> <tt>stemmer</tt>: 
<ul>
<li> "porter" Porter stemmer.
<li> "krovetz" Krovetz stemmer.
<li> "arabic" arabic stemmer, requires additional parameters
<ol>
<li> <tt>arabicStemFunc</tt>: Which stemming algorithm to apply, one of:
<ul>
<li> arabic_stop          : arabic_stop
<li>  arabic_norm2         : table normalization
<li>  arabic_norm2_stop    : table normalization with stopping
<li>  arabic_light10       : light9 plus ll prefix
<li>  arabic_light10_stop  : light10 and remove stop words
</ul> 
</ol>
</ul>
<li> <tt>outputFile</tt>: name of the output file.
</ol>
<p> The structured query operators are:
<p>
<pre>
   Sum Operator:   #sum (T1 ... Tn )

     The terms or nodes contained in the sum operator are treated as
     having equal influence on the final result.  The belief values
     provided by the arguments of the sum are averaged to produce the
     belief value of the #sum node.

   Weighted Sum Operator:  #wsum (W1 T1 ... Wn Tn)

     The terms or nodes contained in the wsum operator contribute
     unequally to the final result according to the weight associated
     with each (Wx).  Note that this is a change from the InQuery
     operator, as there is no initial weight, Ws, for scaling the belief
     value of the sum.

   Ordered Distance Operator:  #N (T1 ... Tn)  or #odN (T1 ... Tn)

     The terms within an ODN operator must be found within N words of
     each other in the text in order to contribute to the document's
     belief value.  The "#N" version is an abbreviation of #ODN, thus
     #3(health care) is equivalent to #od3(health care).

   Un-ordered Window Operator:  #uwN(T1 ... Tn)

     The terms contained in a UWN operator must be found in any order
     within a window of N words in order for this operator to contribute
     to the belief value of the document.

   Phrase Operator:  #phrase(T1 ... Tn)

     The operator is treated as an ordered distance operator of 3
     (#od3).

   Passage Operator:  #passageN(T1 ... Tn)

     The passage operator looks for the terms or nodes within the
     operator to be found in a passage window of N words.  The document
     is rated based upon the score of it's best passage.

   Synonym Operator:  #syn(T1 ... Tn)

     The terms of the operator are treated as instances of the same
     term.

   And Operator:  #and(T1 ... Tn)

     The more terms contained in the AND operator which are found in a
     document, the higher the belief value of that document.

   Boolean And Operator:  #band(T1 ... Tn)

     All of the terms within a BAND operator must be found in a document
     in order for this operator to contribute to the belief value of
     that document.

   Boolean And Not Operator:  #bandnot (T N)

     Search for document matching the first argument but not the second.
     
   Or Operator:  #or(T1 ... Tn)

     One of terms within the OR operator must be found in a document for
     that document to get credit for this operator.


   Maximum Operator:  #max(T1 ... Tn)

     The maximum belief value of all the terms or nodes contained in the
     MAX operator is taken to be the belief value of this operator.

   Filter Require Operator: #filreq(arg1 arg2)

     Use the documents returned (belief list) of the first argument if
     and only if the second argument would return documents.  The value
     of the second argument does not effect the belief values of the
     first argument; only whether they will be returned or not.

   Filter Reject Operator: #filrej(arg1 arg2)

     Use the documents returned by the first argument if and only if
     there were no documents returned by the second argument.  The value
     of the second argument does not effect the belief values of the
     first argument; only whether they will be returned or not.

   Negation Operator:  #not(T1)

     The term or node contained in this operator is negated so that
     documents which do not contain it are rewarded.  
</pre>
<p>
The input query file is of the form:<br>
<pre>
#qN = queryNode ;
</pre>
where <b>N</b> is the query id and <b>queryNode</b> is one of the
aforementioned query operators. The query may span multiple lines and
must be terminated with the semicolon. The body of the query must not
contain a semicolon, as that will prematurely terminate the query.
<p>An example query:<br>
<pre>
#q18=#wsum(1 #sum(Languages and compilers for #1(parallel processors))
 2 #sum(highly horizontal microcoded machines)
 1 code 1 compaction
);
</pre>
*/
/*
  author: fff
 */

#include "TextHandlerManager.hpp"
#include "WriterInQueryHandler.hpp"
#include "InQueryOpParser.hpp"
#include "InqArabicParser.hpp"
#include "indri/Path.hpp"
#include "Param.hpp"
#include "Exception.hpp"

using namespace lemur::api;

// Local parameters used by the application
namespace LocalParameter {

  // name of file containing stopwords
  string stopwords;
  // name of file containing acronyms
  string acronyms;
  // format of documents (trec or web)
  string docFormat;
  // stemmer to use
  string stemmer;
  // query output file
  string qryOutFile;

  void get() {
    docFormat = ParamGetString("docFormat");
    // convert docFormat to lowercase
    for (int i = 0; i < docFormat.length(); i++)
      docFormat[i] = tolower(docFormat[i]);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    stemmer = ParamGetString("stemmer");
    qryOutFile = ParamGetString("outputFile");
  }
};

// put more usage info in here
void usage(int argc, char ** argv) {
  cerr << "Usage:" << endl
       << argv[0] << " paramfile datfile1 datfile2 ... " << endl
       << endl
       << "ParseInQuery parses queries using a lemur Parser class." << endl
       << "Summary of parameters:" << endl << endl
       << "\tstopwords - name of file containing stopword list" << endl
       << "\t            Words in this file should be one per line." << endl
       << "\tacronyms - name of file containing acronym list (one word" << endl
       << "\t           per line).  Uppercase words recognized as acronyms" << endl
       << "\t           (eg USA U.S.A. USAs USA's U.S.A.) are left " << endl
       << "\t           uppercase as USA if USA is in the acronym list." << endl
       << "\tstemmer - \"porter\" to use Porter's stemmer. " << endl
       << "\t          \"krovetz\" to use Krovetz's stemmer (kstemmer). " << endl  
       << "\t          (def = no stemmer)" << endl       
       << "\t          \"arabic\" to use an Arabic stemmer. " << endl  
       << "\t          (def = no stemmer)" << endl
       << "\tarabicStemFunc - which Arabic stemmer algorithm. " << endl;
}

// get application parameters
void GetAppParam() {
  LocalParameter::get();
}


int AppMain(int argc, char * argv[]) {
  if (argc < 3) {
    usage(argc, argv);
    return -1;
  }

  // Create the appropriate parser.
  Parser * parser;
  if (LocalParameter::docFormat == "arabic")
    parser = new lemur::parse::InqArabicParser();
  else
    parser = new lemur::parse::InQueryOpParser();

  if (!LocalParameter::acronyms.empty()) {
    parser->setAcroList(LocalParameter::acronyms);
  }

  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper(LocalParameter::stopwords);

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer(LocalParameter::stemmer);

  lemur::parse::WriterInQueryHandler writer(LocalParameter::qryOutFile);

  // chain the parser/queryHandler/stopper/stemmer/writer

  TextHandler * th = parser;

  if (stopper != NULL) {
    th->setTextHandler(stopper);
    th = stopper;
  }

  if (stemmer != NULL) {
    th->setTextHandler(stemmer);
    th = stemmer;
  } 

  th->setTextHandler(&writer);

  // parse the data files
  for (int i = 2; i < argc; i++) {
    cerr << "Parsing " << argv[i] << endl;
    string filename(argv[i]);
    if (!indri::file::Path::exists(filename)) {
      throw Exception("ParseInQuery", "datfile specified does not exist");
    }
    parser->parse(filename);
  }

  // free memory
  delete(stopper);
  delete(stemmer);
  delete(parser);
  return 0;
}

