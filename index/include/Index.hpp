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


#ifndef _INDEX_HPP
#define _INDEX_HPP

/*! \mainpage Lemur Toolkit Documentation


  
  \section install Installation 

 
  - After checking out the toolkit

      -# go to directory "lemur" 
      -# type in "configure", which will generate MakeDefns.

  - To make the lemur toolkit

      -# go to directory lemur
      -# type in "gmake all" or "gmake"

  - To clean the lemur toolkit (remove everything but the source)

      -# go to directory lemur
      -# type in "gmake clean"

  \section change  Change the toolkit


  - To modify an existing file or add a file to an existing directory:

      -# Make the changes
      -# Go to directory lemur
      -# Type in "gmake all"

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
    index = /usr0/mydata/index.bsc;
  </PRE>
   Most applications will display a list of required input variables, 
   if you run it with the "--help" option. 

  -  Run the application program without the parameter file as the only argument.


  \section sampledata To try the sample data

  - Go to the data directory, run "test.sh" which is a self-explained shell script that will build an index, run several retrieval algorithms with some sample parameter files, and then evaluate the retrieval performance.


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
// this returns a dynamic instance, so you'll need to delete it
DocInfoList *docList = myIndex.docInfoList(t1);

docList->startIteration();

DocInfo *entry;
while (docList->hasMore()) {
  entry = docList->nextEntry(); 
  // this returns a pointer to a *static* memory, do don't delete entry!
  
  cout << "entry doc id: "<< entry->docID() <<endl;
  cout << "entry term count: "<< entry->termCount() << endl;
}

delete docList;
</PRE>
*/

// C. Zhai 02/08/2001

#include "TermInfoList.hpp"
#include "DocInfoList.hpp"
#include "DocumentManager.hpp"

class Index {
public:

  virtual ~Index() {}; 

  /// @name Open index 
  //@{

  /// Open previously created Index, return true if opened successfully, <tt>indexName</tt> should be the full name of the table-of-content file for the index. E.g., "index.bsc" for an index built with the basic indexer. 
  virtual bool open(const char * indexName)=0;
  //@}

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID, returns 0 if out of vocabulary. Valid index starts at 1.
  virtual int term (const char * word)=0;

  /// Convert a valid termID to its spelling
  virtual const char * term (int termID)=0;

  /// Convert a spelling to docID, returns 0 if out of vocabulary. Valid index starts at 1.
  virtual int document (const char * docIDStr)=0;

  /// Convert a valid docID to its spelling
  virtual const char * document (int docID)=0;

  /// A String identifier for the document manager to get at the source
  /// of the document with this document id
  //  virtual const char* docManager(int docID) { return NULL;}
  virtual DocumentManager* docManager(int docID) {return NULL;}


  /// Return a string ID for the term lexicon (usually the file name of the lexicon)
  /*! This function should be pure virtual; the default implementation
     is just for convenience. Appropriate implementation to be done in the future. 
  */
  virtual const char *termLexiconID() { return NULL;} 

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  virtual int docCount () =0;

  /// Total count of unique terms in collection, i.e., the term vocabulary size
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
  /// returns a new instance of DocInfoList which represents the doc entries in a term index, you must delete the instance later. @see DocInfoList
  virtual DocInfoList *docInfoList(int termID)=0;

  /// returns a new instance of TermInfoList which represents the word entries in a document index, you must delete the instance later. @see TermInfoList
  virtual TermInfoList *termInfoList(int docID)=0;

  //@}

};


#endif



