/*==========================================================================
 * Copyright (c) 2000-2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#include "IndriTextHandler.hpp"

IndriTextHandler::IndriTextHandler(const string &name, int memory, const Parser* p): parser(p) {
  bufsize = 0;
  char* docsource = NULL;
  env.setMemory(memory);
  if (Repository::exists(name)) {
    env.open(name);
    cout << "Existing repository "<< name <<" found, documents will be added to existing index" << endl;
  } else {
    env.create(name);
    cout << "Creating repository " << name << endl;
  }
  docid.key = DOCIDKEY;
}

IndriTextHandler::~IndriTextHandler() {
  if (bufsize > 0)
    delete[]docsource;
  env.close();
  free(curdocno);
}

char* IndriTextHandler::handleDoc(char * docno) {
  docbegin = parser->getDocBytePos();
  // stuff document id

  if (docno) {
    free(curdocno);
    curdocno = strdup(docno);
    docid.value = curdocno;
    docid.valueLength = strlen(docno);
    document.metadata.push_back(docid);
  } 
}

char * IndriTextHandler::handleWord(char * word, const char* original, PropertyList* list){
  if (word) {
    char* wordcopy = strdup(word);
    document.terms.push_back(wordcopy);
    TermExtent extent;
    extent.end = parser->fileTell() - docbegin;
    extent.begin = extent.end - strlen(original);
    document.positions.push_back(extent);
  }
  return word;
}

char* IndriTextHandler::handleBeginTag(char* tag, const char* orig, PropertyList* props){
  const Property* prop = NULL;
  prop = props->getProperty("B_ELEM");
  if (prop) {
    // set the tag
    
  }
  return tag;
}

char* IndriTextHandler::handleEndTag(char* tag, const char* orig, PropertyList* props){
  const Property* prop = NULL;
  prop = props->getProperty("E_ELEM");
  if (prop) {
    // set the tag
    // search for the begin tag
    // tags are nested so search from the end of the list
  }
  return tag;
}

void IndriTextHandler::handleEndDoc() {
  // make sure we have enough buffer room
  int textsize = parser->fileTell()-docbegin;
  if (bufsize < textsize+1) {
    if (bufsize > 0) delete[]docsource;
    docsource = new char[textsize+1];
  }
  // grab original text
  ifstream read(parser->getParseFile().c_str(), ios::binary);
  if (!read.is_open()) {
    // throw error
  }
  read.seekg(docbegin, ios::beg);
  read.read(docsource, textsize);
  read.close();
  // null terminate it
  docsource[textsize] = '\n';
  document.text = docsource;
  document.textLength = textsize;

  // fix any bad tags

  // add to index
  env.addParsedDocument(&document);

  // clear old doc info
  for (int i=0; i<document.terms.size(); i++) 
    free(document.terms[i]);
  document.terms.clear();
  document.positions.clear();
  document.tags.clear();
  document.metadata.clear();
}
