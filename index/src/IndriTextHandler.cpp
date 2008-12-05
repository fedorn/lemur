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

lemur::parse::IndriTextHandler::IndriTextHandler(const string &name, int memory, const lemur::api::Parser* p): parser(p),  curdocno(NULL) {

  // shadows attribute from Object.
  env.setMemory(memory);
  // lemur parsers do normalization, so turn it off.
  env.setNormalization(false);
  // have to set stopwords somewhere too (for queries)... bleah.
  // Have to force the IndexEnvironment to build a reverse lookup table for 
  // the docno field to support the Index::document(external id) api call
  std::vector<std::string> metadata;
  metadata.push_back(DOCIDKEY);
  // add forward and backward lookup tables for docno.
  env.setMetadataIndexedFields( metadata, metadata );
  if (indri::collection::Repository::exists(name)) {
    env.open(name);
    cout << "Existing repository "<< name <<" found, documents will be added to existing index" << endl;
  } else {
    env.create(name);
    cout << "Creating repository " << name << endl;
  }
  docid.key = DOCIDKEY;
}

lemur::parse::IndriTextHandler::~IndriTextHandler() {
  if (document.metadata.size() > 0) // got a new doc with no end doc.
    handleEndDoc();
  env.close();
  free(curdocno);
}

char* lemur::parse::IndriTextHandler::handleDoc(char * docno) {
  docbegin = parser->getDocBytePos();
  // stuff document id
  if (docno) {
    if (document.metadata.size() > 0) // got a new doc with no end doc.
      handleEndDoc();
    
    free(curdocno);
    curdocno = strdup(docno);
    docid.value = curdocno;
    // include the trailing '\0'
    docid.valueLength = strlen(docno) + 1;
    document.metadata.push_back(docid);
  } 
  return docno;
}

char * lemur::parse::IndriTextHandler::handleWord(char * word, const char* original, PropertyList* lst){
  if (word) {
    char* wordcopy = strdup(word);
    document.terms.push_back(wordcopy);
    indri::parse::TermExtent extent;
    extent.end = parser->fileTell() - docbegin;
    extent.begin = extent.end - strlen(original);
    document.positions.push_back(extent);
  }
  return word;
}

char* lemur::parse::IndriTextHandler::handleBeginTag(char* tag, const char* orig, PropertyList* props){
  const Property* prop = NULL;
  prop = props->getProperty("B_ELEM");
  if (prop) {
    // set the tag
    
  }
  return tag;
}

char* lemur::parse::IndriTextHandler::handleEndTag(char* tag, const char* orig, PropertyList* props){
  const Property* prop = NULL;
  prop = props->getProperty("E_ELEM");
  if (prop) {
    // set the tag
    // search for the begin tag
    // tags are nested so search from the end of the list
  }
  return tag;
}

void lemur::parse::IndriTextHandler::handleEndDoc() {
  if (document.metadata.size() == 0) return; // no docid, not a real doc.
  // make sure we have enough buffer room  
  int textsize = parser->fileTell()-docbegin;
  char *docsource = new char[textsize+1];
  // grab original text
  ifstream read(parser->getParseFile().c_str(), ios::binary);
  if (!read.is_open()) {
    // throw error
  }
  read.seekg(docbegin, ios::beg);
  read.read(docsource, textsize);
  read.close();
  // null terminate it
  docsource[textsize] = '\0';
  document.text = docsource;
  document.textLength = textsize+1;
  document.content = document.text;
  document.contentLength = textsize;

  // fix any bad tags

  // add to index
  env.addParsedDocument(&document);

  // clear old doc info
  delete[](docsource);
  for (int i=0; i<document.terms.size(); i++) 
    free(document.terms[i]);
  document.terms.clear();
  document.positions.clear();
  document.tags.clear();
  document.metadata.clear();
}
