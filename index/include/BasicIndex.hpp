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


#ifndef _BASICINDEX_HPP
#define _BASICINDEX_HPP

/// Basic Indexer  (with arbitrary compressor)

/*!
  
   BasicIndex is a basic implementation of Index. It creates and manages
two indices (term->doc and doc->term) as well as a term lexicon and document id lexicon. The application can pass in any compressor when calling the build function. @See Index for an example of use. 

*/

//--------------------------------------------------------
// originally written by J. Lafferty, 11/00
//-----------------------------------------------------------------


#include <cstdio>
#include <ctime>
#include "common_headers.hpp"
#include "Compress.hpp"
#include "DocStream.hpp"
#include "FastList.hpp"
#include "IndexCount.hpp"
#include "MemList.hpp"
#include "String.hpp"
#include "Terms.hpp"
#include "Index.hpp"
#include "DocStream.hpp"

class BasicIndex : public Index {
public:
  /// constructor (used when opening an index)
  BasicIndex();

  /// constructor (used when building an index)
  BasicIndex (Compress * pc);

  virtual ~BasicIndex();
  
  /// Open previously created Index, return true if opened successfully
  virtual bool open(const string &indexName);

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID
  virtual TERMID_T term (const string &word) const { return terms[word];}

  /// Convert a termID to its spelling
  virtual const string term (TERMID_T termID) const { return terms[termID];} 

  /// Convert a spelling to docID
  virtual TERMID_T document (const string &docIDStr) const {return docids[docIDStr];}

  /// Convert a docID to its spelling
  virtual const string document (TERMID_T docID) const {return docids[docID];}

  /// return the term lexicon ID
  virtual const string termLexiconID() const { return wordVocabulary;} 
  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  virtual COUNT_T docCount () const  { return docids.size()-1;}

  /// Total count of unique terms in collection
  virtual COUNT_T termCountUnique () const { return terms.size()-1;}

  /// Total counts of a term in collection
  virtual COUNT_T termCount (TERMID_T termID) const { return countOfTerm[termID] ;}

  /// Total counts of all terms in collection
  //  virtual COUNT_T termCount () const {return (int)avgDocumentLength*(docids.size()-1) ;}
  virtual COUNT_T termCount () const {return numWords ;}
  // XXX Better to have an exact count!

  /// Average document length 
  virtual float docLengthAvg() const { return avgDocumentLength;}

  /// Total counts of doc with a given term
  virtual COUNT_T docCount(TERMID_T termID) const;

  /// Total counts of terms in a document  
  virtual COUNT_T docLength (DOCID_T docID) const { return countOfDoc[docID];}  ;

  //@}

  /// @name Index entry access
  //@{
  /// doc entries in a term index, caller should release the memory @see DocList
  virtual DocInfoList *docInfoList(TERMID_T termID) const ;

  /// word entries in a document index, caller should release the memory @see TermList
  virtual TermInfoList *termInfoList(DOCID_T docID) const ;

  //@}


  // Create basic inverted indices 
  void build(DocStream *collectionStream,
	     const string &file,
	     const string &outputPrefix, 
	     int totalDocs=0x1000000, int maxMemory=0x4000000,
	     int minimumCount=1, int maxVocSize=2000000);
private:
  void buildVocabulary(int maxVocSize, int minimumCount);
  void writeWordIndex(int indexNum, FastList<IndexCount> * dlw);
  int indexCollection();
  int headDocIndex();
  int headWordIndex();
  void createKeys();
  void mergeIndexFiles();
  void createKey(const string &inName, const string &outName, 
		 Terms & voc, int * byteOffset);
  int mergePair(const string &fn1, const string &fn2, const string &fn3);
  void writeIndexFile();

  ifstream textStream;

  String      prefix;
  String      textFile;
  String      wordVocabulary;
  String      documentVocabulary;
  String      wordIndexFile, documentIndexFile;
  String      wordKeyFile, documentKeyFile;
  Terms       terms;
  Terms       docids;
  int         numDocuments;
  int         numWords;
  int         numBytes;
  int         maxDocumentLength;
  float       avgDocumentLength;
  int         totalDocuments;

  // The following are "utility" variables
private:
  int         memorySegment;
  int         maxSegmentsPerIndex;
  time_t      timeToIndex;
  int         maximumMemory;
  // int *       byteOffsetOfDoc;
  MemList *   pMemList;  
  Compress *  pCompressor;
  bool deleteCompressor;
  DocStream * pDocStream;

private:
  // fields for managing indices
  mutable ifstream  wordIndexStream, documentIndexStream;
  int * woffset, * doffset; //  *toffset;
  int * tmpdarr, * tmpwarr;
  int * countOfTerm;
  int * countOfDoc;

};

#endif
