#ifndef _INDEX_HPP
#define _INDEX_HPP

/*! \mainpage Lemur Toolkit Documentation


  
  \section install Installation 

 
  - After checking out the toolkit

      -# go to directory "lemur" 
      -# type in "configure", which will generate MakeDefns.

  - To make the lemur toolkit

      -# go to directory lemur
      -# type in "make all" or "make"

  - To clean the lemur toolkit (remove everything but the source)

      -# go to directory lemur
      -# type in "make clean"

  \section change  Change the toolkit


  - To modify an existing file or add a file to an existing directory:

      -# Make the changes
      -# Go to directory lemur
      -# Type in "make all"

  - To add a new (library) module to the toolkit:

      -# Add the module subdirectory to lemur 
      -# Put all include files in a subdirectory named "include" under the new  module directory
      -# Put all implementation files in a subdirectory named "src" under the new module directory
      -# Add the module directory name to the Makefile in directory lemur or lemurdev.
      -# Copy a Makefile from an existing module directory (e.g, index/src/Makefile) to <new-module-dir>/src, and change the first two lines to define this module and its dependent modules.

  - To add a new application directory to the toolkit:

      -# Add the application subdirectory to lemur
      -# Put all include files in a subdirectory named "include" under the new module directory
      -# Put all implementation files in a subdirectory named "src" under the new module directory
      -# Add the application directory name to the Makefile in directory  lemur or lemurdev.
      -# Copy a Makefile from an existing application directory (e.g, app/src/Makefile) to <new-application-dir>/src, and change the line that defines the dependent modules.


      \section runapp  To run lemur applications

  - Create a parameter file with value definitions for all the  input variables of an application. Terminate each line with a semicolon. 	For example, 
  <PRE>
    lemur = /usr0/mydata/index.lemur;
  </PRE>
   Most applications will display a list of required input variables, 
   if you run it with the "--help" option. 

  - Do "setenv PARAM YOUR-PARAM-FILENAME" or 
      "export PARAM=YOUR-PARAM-FILENAME", 
      depending on the shell you are running.


  -  Run the application program without any argument.


  \section sampledata To try the sample data

  - Go to the data directory

  - First, try to build the index by running lemur/app/src/BuildIndex with buildparam as the parameter file.
  - Second, built the support file (needed by the language model retrieval evaluation program) by running lemur/app/src/GenerateSmoothSupport with suppparam as the parameter file.
  - Now you can try any of the retrieval evaluation program with the appropriate parameter files. Each evaluation program will write the result to a file specified in the parameter file. 
      -# tfidfparam (result in res.tfidf) or tfidffbparam (result in res.tfidffb) are for app/src/TFIDFEval
      -# okapiparam (result in res.okapi) or okapifbparam (result in res.okapifb) are for app/src/OkapiEval
      -# lmparam1 (result in res.lm1)  or lmparam2 (res.lm2) are for app/src/SimpleUnigEval
  - Finally, you can compute the TREC style performance figures for any of the results by running "./eval result-file-name". For example, "./eval res.tfidf" would generate figures for res.tfidf. 


 */


///  Abstract Class for indexed document collection.

/*! 

  This is an abstract class that provides a uniform interface
for access to an indexed document collection. The following
is an example of using it.


<PRE>


Index &myIndex;

myIndex.open("index-file");


int t1;
... 

// now fetch doc info list for term t1
DocInfoList *docList = myIndex.docInfoList(t1);

docList->startIteration();

DocInfo *entry;
while (docList->hasMore()) {
  entry = docList->nextEntry();
  
  cout << "entry doc id: "<< entry->docID() <<endl;
  cout << "entry term count: "<< entry->termCount() << endl;
}

</PRE>
*/

// C. Zhai 02/08/2001

#include "TermInfoList.hpp"
#include "DocInfoList.hpp"

class Index {
public:

  virtual ~Index() {}; 

  /// @name Open index 
  //@{

  /// Open previously created Index, return true if opened successfully
  virtual bool open(const char * indexName)=0;
  //@}

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID
  virtual int term (const char * word)=0;

  /// Convert a termID to its spelling
  virtual const char * term (int termID)=0;

  /// Convert a spelling to docID
  virtual int document (const char * docIDStr)=0;

  /// Convert a docID to its spelling
  virtual const char * document (int docID)=0;

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  virtual int docCount () =0;

  /// Total count of unique terms in collection
  virtual int termCountUnique ()=0;

  /// Total counts of a term in collection
  virtual int termCount (int termID) const =0;

  /// Total counts of all terms in collection
  virtual int termCount () const =0;

  /// Average document length 
  virtual float docLengthAvg()=0;

  /// Total counts of doc with a given term
  virtual int docCount(int termID)=0;

  /// Total counts of terms in a document  
  virtual int docLength (int docID) const =0;

  //@}

  /// @name Index entry access
  //@{
  /// doc entries in a term index, @see DocList
  virtual DocInfoList *docInfoList(int termID)=0;

  /// word entries in a document index, @see TermList
  virtual TermInfoList *termInfoList(int docID)=0;

  //@}

};


#endif
