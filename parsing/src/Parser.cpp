/*==========================================================================
 *
 *  Copyright (c) 2003, Carnegie Mellon University. All Rights Reserved.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#include "Parser.hpp"
#include "FUtil.hpp"

Parser::Parser() {
  borrowedacros = NULL;
  myacros = NULL;
  cat = category;
  iden = identifier;
}

Parser::~Parser() {
  if (myacros)
    delete myacros;
}

void Parser::parse(const string &filename) {
  parsefile = filename;
  if (!fileExist(filename)) {
    LEMUR_THROW(LEMUR_IO_ERROR, "Cannot find file "+filename+" for parsing.");
  }
  parseFile(parsefile);
}

void Parser::setAcroList(const WordSet* acronyms) {
  clearAcros();
  borrowedacros = acronyms;
}

void Parser::setAcroList(string filename) {
  clearAcros();
  myacros = new WordSet(filename);
}

bool Parser::isAcronym(const char* word) {
  if (myacros) 
    return myacros->contains(word);
  if (borrowedacros)
    return borrowedacros->contains(word);
  return false;
}

void Parser::clearAcros() {
  if (myacros)
    delete myacros;
  borrowedacros = NULL;
}
