/*==========================================================================
 *
 *  Copyright (c) 2003, Carnegie Mellon University. All Rights Reserved.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#include "Parser.hpp"

Parser::Parser() {
  mine = true;
  acros = NULL;
}

Parser::~Parser() {
  if ((mine) && (acros))
    delete acros;
}

void Parser::setAcroList(WordSet* acronyms) {
  acros = acronyms;
  mine = false;
}

void Parser::setAcroList(string filename) {
  acros = new WordSet((char*)filename.c_str());
  mine = true;
}

bool Parser::isAcronym(char* word) {
  if (acros) 
    return acros->contains(word);
  return false;
}
