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

#ifndef NULL
#define NULL 0
#endif

#ifndef _TEXTHANDLER_HPP
#define _TEXTHANDLER_HPP


///  This class serves as an interface for classes working with the parsers.
///  The setTextHandler function allows chaining of TextHandlers, so that 
///  information is passed from one TextHandler to the next.  This is useful
///  for chaining things like stopword lists and stemmers.
///
///  A source in the chain of TextHandlers does not need to do anything
///  in the foundDoc and foundWord functions.  An example of a source is 
///  a parser.  A destination in the chain of TextHandlers does not need
///  to forward calls or store a when the setTextHandler function is
///  called.  An example of a destination would be a class that pushes
///  the words and documents into an InvFPPushIndex (InvFPTextHandler)
///  or writes to file (WriterTextHandler).  Classes in the middle of a 
///  chain, like Stopper or Stemmer, need to provide full functionality
///  for all functions.  When their foundDoc or foundWord is called, they
///  will possibly manipulate the data, then forward the info via
///  calling the foundDoc/foundWord function of their TextHandler.


//  Might make more sense as TextSource and TextDestination with
//  functions in the middle of the chain inheriting from both.


class TextHandler {

public:
  TextHandler() {
    textHandler = NULL;
  }

  /// Found a document with document number
  virtual void foundDoc(char * docno) {
    docno = handleDoc(docno);
    if (textHandler != NULL) 
      textHandler->foundDoc(docno);
  }
  /// Found a word
  virtual void foundWord(char * word) {
    word = handleWord(word);
    if (textHandler != NULL)
      textHandler->foundWord(word);
  }
  /// Set the TextHandler that this TextHandler will pass information on to.
  virtual void setTextHandler(TextHandler * th) {
    textHandler = th;
  }

  /// Handle a doc
  virtual char * handleDoc(char * docno) { return docno; }
  /// Handle a word, possibly transforming it
  virtual char * handleWord(char * word) { return word; }

protected:
  /// The next textHandler in the chain
  TextHandler * textHandler;
};

#endif

