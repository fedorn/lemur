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

  if (type.compare( "web") == 0) {
    parser = new WebParser();
  } else if (type.compare("reuters") == 0) {
    parser = new ReutersParser();
  } else if (type.compare("trec") == 0) {
    parser = new TrecParser();
  } else if (type.compare("chinese") == 0) {
    parser = new ChineseParser();
  } else if (type.compare("chinesechar") == 0) {
    parser = new ChineseCharParser();
  } else if (type.compare("arabic") == 0) {
    parser = new ArabicParser();
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

  if (type.compare("krovetz") == 0) {
    stemmer = new KStemmer();
    // if KstemmerDir is declared then resets STEM_DIR environment variable
    // this is how the krovetz stemmer was originally programmed. we should
    // consider changing it to not get the directory from the environment if
    // we're allowed to modify that code.    
    if (datadir.compare("") == 0) {
      datadir = ParamGetString("KstemmerDir");
    }
    if (!datadir.empty()) {
      datadir.insert(0,"STEM_DIR=");
      if (putenv((char*)datadir.c_str()))
	cerr << "putenv can not set STEM_DIR" << endl;  
    } else {
      cerr << "did not set STEM_DIR, must be in environment" << endl;
    }
  } else if (type.compare("arabic") == 0){
    if ((datadir.empty()) || (func.empty())) {
      ArabicStemmerParameter::get();
      // param get has defaults so it'll always get back values
      stemmer = new ArabicStemmer(ArabicStemmerParameter::stemDir, 
				  ArabicStemmerParameter::stemFunc); 
    } else {
      stemmer = new ArabicStemmer((char*)datadir.c_str(),(char*)func.c_str());    
    }
  } else if (type.compare("porter") == 0) {
    stemmer = new PorterStemmer();
  }
  
  return stemmer;
}

Stopper* TextHandlerManager::createStopper(string filename) {
  Stopper* stopper = NULL;
  if (filename.empty()) 
    filename = ParamGetString("stopwords");
  
  if (!filename.empty()) 
    stopper = new Stopper((char*)filename.c_str());
  
  return stopper;
}
