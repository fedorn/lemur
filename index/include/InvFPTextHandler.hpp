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
#include "TextHandler.hpp"
#include "InvFPPushIndex.hpp"



#ifndef _INVFPTEXTHANDLER_HPP
#define _INVFPTEXTHANDLER_HPP

///
///  InvFPTextHandler builds an InvFPIndex using InvFPPushIndex.
///  This class is a destination TextHandler.  
///

class InvFPTextHandler : public TextHandler {

public:
  /// Create a InvFPTextHandler with index name filename 
  /// (minus the .ifp extension) and specified buffer size.
  InvFPTextHandler(char * filename, int bufferSize);
  ~InvFPTextHandler();

  /// Pushes the doc into the InvFPPushIndex.
  char * handleDoc(char * docno);
  /// Pushes the word into the InvFPPushIndex.
  char * handleWord(char * word);


private:
  /// Ends a document in the collection
  void endDoc();


  /// The indexer.
  InvFPPushIndex * index;
  /// Document properties object to be reused over repeated calls.
  DocumentProps * dp;
  /// Term object to be reused over repeated calls.
  InvFPTerm * term;
  /// Keeps track of document length.
  int docLength;
  /// Indicates whether object is before first doc
  /// true = yes, false = foundDoc has been called once.
  bool first;
};

#endif

