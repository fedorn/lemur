/*==========================================================================
 *
 *  Copyright (c) 2003, Carnegie Mellon University. All Rights Reserved.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/
/*==========================================================================
 *
 *  Copyright (c) 2003, University of Massachusetts. All Rights Reserved.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#include "TextHandlerManager.hpp"
#include "WebParser.hpp"
#include "TrecParser.hpp"
#include "InQueryOpParser.hpp"
#include "PorterStemmer.hpp"
#include "KStemmer.hpp"
#include "Param.hpp"

// define each TextHandler's identifier
// this is the way to do static const string in C++
const string lemur::api::TextHandler::category = "TextHandler";
const string lemur::api::TextHandler::identifier = "TextHandler";

// these in Parser category
const string lemur::api::Parser::category = "parser";
const string lemur::api::Parser::identifier = "parser";
const string lemur::parse::TrecParser::identifier = "trec";
const string lemur::parse::WebParser::identifier = "web";
const string lemur::parse::InQueryOpParser::identifier = "inqueryop";


// these in Stemmer category
const string lemur::api::Stemmer::category = "stemmer";
const string lemur::api::Stemmer::identifier = "stemmer";
const string lemur::parse::PorterStemmer::identifier = "porter";
const string lemur::parse::KStemmer::identifier = "krovetz";

const string lemur::api::Stopper::category = "stopper";
const string lemur::api::Stopper::identifier = "stopper";

lemur::api::Parser* lemur::api::TextHandlerManager::createParser(string type, string acros) {  
  // Create the appropriate parser.
  Parser * parser = NULL;
  if (type.empty()) {
    // didn't pass in type, try to get it from the paramstack
    type = ParamGetString("docFormat");
  }

  // if it's still empty, return nothing
  if (type.empty())
    return NULL;

  // make it all lowercase
  for (int i=0;i<type.length();i++)
    type[i] = tolower(type[i]);
  
  if (type == lemur::parse::WebParser::identifier) {
    parser = new lemur::parse::WebParser();
  } else if (type == lemur::parse::TrecParser::identifier) {
    parser = new lemur::parse::TrecParser();
  }
  
  if (parser) {
    // tell the parser about the acronyms list
    if (acros.empty()) {
      acros = ParamGetString("acronyms");
    }
    if (!acros.empty()) {
      parser->setAcroList(acros);
    }
  }

  return parser;
}

lemur::api::Stemmer* lemur::api::TextHandlerManager::createStemmer(string type, string datadir, string func) {
  Stemmer *stemmer = NULL;
  if (type.empty()) {
    // didn't pass in type, try to get it from the paramstack
    type = ParamGetString("stemmer");
  }  

  // if it's still empty, return nothing
  if (type.empty())
    return NULL;

  // make it all lowercase
  for (int i=0;i<type.length();i++)
    type[i] = tolower(type[i]);

  try {
    if (type == lemur::parse::KStemmer::identifier) {
      stemmer = new lemur::parse::KStemmer();
      //      cerr << "created Kstemmer" << endl;
    } else if (type == lemur::parse::PorterStemmer::identifier) {
      stemmer = new lemur::parse::PorterStemmer();
      //cerr << "created porter stemmer" << endl;
    }
  } catch (Exception &ex) {
    LEMUR_RETHROW(ex, "Could not create Stemmer");
  }
  
  return stemmer;
}

lemur::api::Stopper* lemur::api::TextHandlerManager::createStopper(string filename) {
  Stopper* stopper = NULL;
  if (filename.empty()) 
    filename = ParamGetString("stopwords");

  if (!filename.empty()) {
    try {
      stopper = new Stopper(filename);
      //cerr << "created stopper" << endl;
    } catch (Exception &ex) {
      LEMUR_RETHROW(ex, "Could not create Stopper using file name");
    }
  }
  
  return stopper;
}
