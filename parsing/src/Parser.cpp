/*==========================================================================
 *
 *  Copyright (c) 2003, Carnegie Mellon University. All Rights Reserved.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#include "Parser.hpp"
#include "indri/Path.hpp"

lemur::api::Parser::Parser() {
  borrowedacros = NULL;
  myacros = NULL;
  cat = category;
  iden = identifier;
}

lemur::api::Parser::~Parser() {
  if (myacros)
    delete myacros;
}

void lemur::api::Parser::parse(const string &filename) {
  parsefile = filename;
  if (!indri::file::Path::exists(filename)) {
    LEMUR_THROW(LEMUR_IO_ERROR, "Cannot find file "+filename+" for parsing.");
  }
  parseFile(parsefile);
}

void lemur::api::Parser::setAcroList(const lemur::utility::WordSet* acronyms) {
  clearAcros();
  borrowedacros = acronyms;
}

void lemur::api::Parser::setAcroList(string filename) {
  clearAcros();
  myacros = new lemur::utility::WordSet(filename);
}

bool lemur::api::Parser::isAcronym(const char* word) {
  if (myacros) 
    return myacros->contains(word);
  if (borrowedacros)
    return borrowedacros->contains(word);
  return false;
}

void lemur::api::Parser::clearAcros() {
  if (myacros)
    delete myacros;
  borrowedacros = NULL;
}
