
/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#include <cstring>
#include "common_headers.hpp"

#ifndef NULL
#define NULL 0
#endif

#ifndef _TEXTHANDLER_HPP
#define _TEXTHANDLER_HPP

#include "PropertyList.hpp"


#define MAXWORDSIZE 1024

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
///  The original should be preserved and passed on as is. 
///  Properties can be associated with token using the PropertyList.


///  TextHandlers have their own internal buffer for modification of
///  the string.  The foundWord function copies the word into the buffer
///  then calls handleWord with the copy.  The handleWord function may
///  then modify the string and return the pointer to the string. 
///  This process is also done for foundDoc/handleDoc.

//  Might make more sense as TextSource and TextDestination with
//  functions in the middle of the chain inheriting from both.
#include <cstdio>

class TextHandler {

public:
  enum TokenType {BEGINDOC = 1, ENDDOC = 2, WORDTOK = 3, 
		  BEGINTAG = 4, ENDTAG = 5, SYMBOLTOK = 6};
  static const string category;
  static const string identifier;

  TextHandler() {
    textHandler = NULL;
    buffer[MAXWORDSIZE-1] = '\0';
    cat = category;
    iden = identifier;
  }
  virtual ~TextHandler() {}
  
  /// Set the TextHandler that this TextHandler will pass information on to.
  virtual void setTextHandler(TextHandler * th) {
    textHandler = th;
  }
  /// Set the TextHandler that this TextHandler will pass information on to.
  virtual TextHandler * getTextHandler() {
    return textHandler;
  }

  virtual void foundToken(TokenType type, 
			  char * token = NULL, 
			  const char * orig = NULL,
			  PropertyList * properties = NULL) {
    char * t = NULL;

    if (token != NULL) {
      strncpy(buffer, token, MAXWORDSIZE - 1);
      t = buffer;
    } 

    switch (type) {

    case BEGINDOC:
      t = handleBeginDoc(t, orig, properties);
      break;
    case ENDDOC:
      t = handleEndDoc(t, orig, properties);
      break;
    case WORDTOK:
      t = handleWord(t, orig, properties);
      break;
    case BEGINTAG:
      t = handleBeginTag(t, orig, properties);
      break;
    case ENDTAG:
      t = handleEndTag(t, orig, properties);
      break;            
    case SYMBOLTOK:
      t = handleSymbol(t, orig, properties);
      break;            
    }

    if (textHandler != NULL) {
      textHandler->foundToken(type, t, orig, properties);
    }
  }

  /// Handle a doc begin - default implementation calls
  /// handleDoc for backwords compat
  virtual char * handleBeginDoc(char * docno, const char * original,
				PropertyList * list) {
    return handleDoc(docno);
  }
  /// Handle a doc end - default implementation calls
  /// old handleEndDoc for backwords compat
  virtual char * handleEndDoc(char * token, const char * original,
			      PropertyList * list) {
    handleEndDoc();
    return token;
  }
  /// Handle a word - default implementation calls
  /// old handleWord for backwords compat
  virtual char * handleWord(char * word, const char * original,
			    PropertyList * list) {
    return handleWord(word);
  }
  /// Handle a begin tag
  virtual char * handleBeginTag(char * tag, const char * original,
				PropertyList * list) {
    return tag;
  }
  /// Handle an end tag 
  virtual char * handleEndTag(char * tag, const char * original,
			      PropertyList * list) {
    return tag;
  }

  /// Handle a symbol - default implementation calls
  /// old handleSymbol for backwords compat
  virtual char * handleSymbol(char * symbol, const char * original,
			      PropertyList * list) {
    return handleSymbol(symbol);
  }



  // For backwards compatability
  /// Found a document with document number
  virtual void foundDoc(char * docno) {
    foundToken(BEGINDOC, docno, docno);
  }
  virtual void foundDoc(char * docno, const char * original) {
    foundToken(BEGINDOC, docno, original);
  }
  /// Found a word
  virtual void foundWord(char * word) {
    foundToken(WORDTOK, word, word);
  }
  virtual void foundWord(char * word, const char * original) {
    foundToken(WORDTOK, word, original);
  }
  /// Found end of doc
  virtual void foundEndDoc() {
    foundToken(ENDDOC);
  }
  /// Found a word
  virtual void foundSymbol(char * sym) {
    foundToken(SYMBOLTOK, sym, sym);
  }  
  // Kept for backwords compatability
  /// Handle a doc
  virtual char * handleDoc(char * docno) { return docno; }
  /// Handle a word, possibly transforming it
  virtual char * handleWord(char * word) { return word; }
  /// Handle the end of the doc
  virtual void handleEndDoc() { }
  /// Handle a word, possibly transforming it
  virtual char * handleSymbol(char * sym) { return sym; }

  /// Return the category TextHandler this is
  virtual string getCategory() { return cat; }
  /// Return a unique identifier for this TextHandler object
  virtual string getIdentifier() { return iden; }
protected:
  /// The next textHandler in the chain
  TextHandler * textHandler;
  string cat;
  string iden;

  char buffer[MAXWORDSIZE];
};


#endif

