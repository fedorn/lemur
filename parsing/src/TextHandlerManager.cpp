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
#include "ChineseParser.hpp"
#include "ChineseCharParser.hpp"
#include "ReutersParser.hpp"
#include "ArabicParser.hpp"
#include "InqArabicParser.hpp"
#include "InQueryOpParser.hpp"
#include "IdentifinderParser.hpp"
#include "BrillPOSParser.hpp"
#include "PorterStemmer.hpp"
#include "KStemmer.hpp"
#include "ArabicStemmer.hpp"

// define each TextHandler's identifier
// this is the way to do static const string in C++
const string TextHandler::category = "TextHandler";
const string TextHandler::identifier = "TextHandler";

// these in Parser category
const string Parser::category = "Parser";
const string Parser::identifier = "parser";
const string TrecParser::identifier = "trec";
const string WebParser::identifier = "web";
const string ReutersParser::identifier = "reuters";
const string ChineseParser::identifier = "chinese";
const string ChineseCharParser::identifier = "chinesechar";
const string ArabicParser::identifier = "arabic";
const string InqArabicParser::identifier = "inqarabic";
const string BrillPOSParser::identifier = "brill";
const string IdentifinderParser::identifier = "identifinder";
const string InQueryOpParser::identifier = "inqueryop";


// these in Stemmer category
const string Stemmer::category = "Stemmer";
const string Stemmer::identifier = "stemmer";
const string PorterStemmer::identifier = "porter";
const string KStemmer::identifier = "krovetz";
const string ArabicStemmer::identifier = "arabic";

const string Stopper::category = "Stopper";
const string Stopper::identifier = "Stopper";

Parser* TextHandlerManager::createParser(string type, string acros) {  
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
  
  if (type == WebParser::identifier) {
    parser = new WebParser();
  } else if (type == ReutersParser::identifier) {
    parser = new ReutersParser();
  } else if (type == TrecParser::identifier) {
    parser = new TrecParser();
  } else if (type == ChineseParser::identifier) {
    parser = new ChineseParser();
  } else if (type ==ChineseCharParser::identifier) {
    parser = new ChineseCharParser();
  } else if (type == ArabicParser::identifier) {
    parser = new ArabicParser();
  } else if (type == InqArabicParser::identifier) {
    parser = new InqArabicParser();
  } else if (type == BrillPOSParser::identifier) {
    parser = new BrillPOSParser();
  } else if (type == IdentifinderParser::identifier) {
    parser = new IdentifinderParser();
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

Stemmer* TextHandlerManager::createStemmer(string type, string datadir, string func) {
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
    if (type == KStemmer::identifier) {
      if (datadir.compare("") == 0) {
	datadir = ParamGetString("KstemmerDir");
      }
      stemmer = new KStemmer(datadir);
    } else if (type == ArabicStemmer::identifier){
      if ((datadir.empty()) || (func.empty())) {
	ArabicStemmerParameter::get();
	// param get has defaults so it'll always get back values
	stemmer = new ArabicStemmer(ArabicStemmerParameter::stemDir, 
				    ArabicStemmerParameter::stemFunc); 
      } else {
	stemmer = new ArabicStemmer((char*)datadir.c_str(),(char*)func.c_str());    
      }
    } else if (type == PorterStemmer::identifier) {
      stemmer = new PorterStemmer();
    }
  } catch (Exception &ex) {
    LEMUR_RETHROW(ex, "Could not create Stemmer");
  }
  
  return stemmer;
}

Stopper* TextHandlerManager::createStopper(string filename) {
  Stopper* stopper = NULL;
  if (filename.empty()) 
    filename = ParamGetString("stopwords");
  
  if (!filename.empty()) {
    try {
      stopper = new Stopper(filename);
    } catch (Exception &ex) {
      LEMUR_RETHROW(ex, "Could not create Stopper using file name");
    }
  }
  
  return stopper;
}
