/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "WriterInQueryHandler.hpp"
#include <cctype>


WriterInQueryHandler::WriterInQueryHandler(char * filename) {
  // create the output file
  outfile = new ofstream(filename);
  // set state in first doc
  first = true;
}

WriterInQueryHandler::~WriterInQueryHandler() {
  endDoc();
  // close the output file
  outfile->close();
  delete outfile;
}


char * 
WriterInQueryHandler::handleDoc(char * docno) {
  // output the end doc tag if not the first doc
  if (first) {
    first = false;
  } else {
    endDoc();
  }
  // begin the doc
  *outfile << "<DOC " << docno << ">" << endl;
  return docno;
}

char * 
WriterInQueryHandler::handleWord(char * word) {
  // output the word
  if (word != NULL) {
    *outfile << word << endl;
  }
  return word;
}

char * 
WriterInQueryHandler::handleSymbol(char * sym) {
  // output the word
  if (sym != NULL) {
    *outfile << sym<< endl;
  }
  return sym;
}

void 
WriterInQueryHandler::endDoc() {
  *outfile << "</DOC>" << endl;
}
