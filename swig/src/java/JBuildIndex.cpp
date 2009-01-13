/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.lemurproject.org
 *
 *==========================================================================
*/

#include "JBuildIndex.h"
#include "TextHandlerManager.hpp"
#include "IndriTextHandler.hpp"
#include "KeyfileTextHandler.hpp"
#include "KeyfileIncIndex.hpp"
#include "DocMgrManager.hpp"
#include "Param.hpp"
#include "indri/Path.hpp"

using namespace lemur::api;

namespace BIParam {
  int memory;

  // name (minus extension) of the database
  string index;
  // type of index to build
  string indexType;
  // name of file containing stopwords
  string stopwords;
  // name of file containing acronyms
  string acronyms;
  // format of documents (trec or web)
  string docFormat;
  // whether or not to stem
  string stemmer;
  // file with source files
  string dataFiles;
  string manager; // name of document manager
  string mgrType; // which doc mgr to use
  bool countStopWords;

  void get() {
    index = ParamGetString("index");
    indexType = ParamGetString("indexType");
    manager = ParamGetString("manager");
    mgrType = ParamGetString("managerType");
    memory = ParamGetInt("memory", 128000000);
    stopwords = ParamGetString("stopwords");
    acronyms = ParamGetString("acronyms");
    docFormat = ParamGetString("docFormat");
    dataFiles = ParamGetString("dataFiles");
    stemmer = ParamGetString("stemmer");
    countStopWords = (ParamGetString("countStopWords", "false") == "true");
  }
}

JNIEXPORT void JNICALL Java_lemurproject_lemur_ui_JBuildIndex_buildIndex
(JNIEnv * env, jobject upclass, jstring paramfile) 
{
  const char* file;
  jboolean iscopy;

  // set up class for throwing exceptions
  jclass exception;
  exception = env->FindClass("java/lang/Exception");

  // set up method for sending messages
  jmethodID writeMesg;
  writeMesg = env->GetMethodID(env->GetObjectClass(upclass), "displayMessage", "(Ljava/lang/String;)V");

  // convert string
  file = env->GetStringUTFChars(paramfile, &iscopy);

  ParamPushFile(file);
	BIParam::get();
  if (iscopy == JNI_TRUE)
    env->ReleaseStringUTFChars(paramfile, file);
    
  // Cannot create anything without Index name
	if (BIParam::index.empty()) {
		string s = "Could not find name of index to build. Did you try to modify the auto-generated param file?";
    env->ThrowNew(exception, s.c_str());
    return;
  }

	if (BIParam::indexType.empty()) {
		string s = "Could not find type of index you want to build. Did you try to modify the auto-generated param file?";
    env->ThrowNew(exception, s.c_str());
    return;
	}

  DocumentManager* docmgr = NULL;
  // Create DocumentManager with appropriate parse mode
  if (!BIParam::manager.empty()) {
    docmgr = DocMgrManager::createDocMgr(BIParam::mgrType, 
				       BIParam::manager, 
				       BIParam::docFormat, 
				       BIParam::dataFiles);
    if (!docmgr) {
      jstring str = env->NewStringUTF("\n WARNING: CONTINUING TO BUILD INDEX WITHOUT DOCUMENT MANAGER. COULD NOT CREATE DOCUMENT MANAGER.\n");
      env->CallVoidMethod(upclass, writeMesg, str);
    } 
  }

	// Create the appropriate parser and acronyms list if needed
	Parser * parser = NULL;
	parser = TextHandlerManager::createParser(BIParam::docFormat, BIParam::acronyms);
	// if failed to create parser, abort
	if (!parser) {
		string s = "Error trying to create parser. Build index failed.";
    env->ThrowNew(exception, s.c_str());
    return;	
	}
	  
	// Create the stopper if needed.
	Stopper * stopper = NULL;
	try {
		stopper = TextHandlerManager::createStopper(BIParam::stopwords);
	} catch (Exception &ex) {
    jstring str = env->NewStringUTF("\n WARNING: CONTINUING TO BUILD INDEX WITHOUT STOPWORDS FILE LOADED.\n");
    env->CallVoidMethod(upclass, writeMesg, str);
	}
	  
	// Create the stemmer if needed.
	Stemmer * stemmer = NULL;
	try {
		stemmer = TextHandlerManager::createStemmer(BIParam::stemmer);
	} catch (Exception &ex) {
    jstring str = env->NewStringUTF("\n WARNING: CONTINUING TO BUILD INDEX WITHOUT STEMMING\n");
    env->CallVoidMethod(upclass, writeMesg, str);
  }

	TextHandler* indexer;
	lemur::index::KeyfileIncIndex* index = NULL;

	if (BIParam::indexType == "indri") {
		indexer = new lemur::parse::IndriTextHandler(BIParam::index,
					BIParam::memory,
					parser);
	} else if (BIParam::indexType == "key") {
		index = new lemur::index::KeyfileIncIndex(BIParam::index,
					BIParam::memory);
		indexer = new lemur::parse::KeyfileTextHandler(index,
						BIParam::countStopWords);
                if (docmgr)
                  ((lemur::parse::KeyfileTextHandler *)indexer)->setDocManager(docmgr->getMyID());
	} 

	// chain the parser/stopper/stemmer/indexer
  TextHandler *th;
  if (docmgr) {
    th = dynamic_cast<TextHandler*>(docmgr);
    cerr << "TH set" << endl;
  }
  else
    th = parser;

	if (stopper != NULL) {
		th->setTextHandler(stopper);
		th = stopper;
	}

	if (stemmer != NULL) {
		th->setTextHandler(stemmer);
		th = stemmer;
	} 

	th->setTextHandler(indexer);

  if (docmgr) {
    string msg = "Parsing files";
    jstring str = env->NewStringUTF(msg.c_str());
    env->CallVoidMethod(upclass, writeMesg, str);
    docmgr->buildMgr();
  } else {
	  // parse the data files
	  if (!indri::file::Path::exists(BIParam::dataFiles)) {
		  string s = "Error finding list of files to index. Did you delete the auto-generated .data file?";
      env->ThrowNew(exception, s.c_str());
      return;	
	  }
	  ifstream source(BIParam::dataFiles.c_str());
	  if (!source.is_open()) {
 	    string s = "Could not open list of files to index. Did you edit the auto-generated .data file?";
      env->ThrowNew(exception, s.c_str());
      return;	
	  } else {
		  string filename;
		  while (getline(source, filename)) {
        string msg = "Parsing file: " + filename;
        jstring str = env->NewStringUTF(msg.c_str());
        env->CallVoidMethod(upclass, writeMesg, str);
		    try {
		      parser->parse(filename);
		    } catch (Exception &ex) {
		      msg = "Skipping file. Could not parse. " + ex.what();
          jstring str = env->NewStringUTF(msg.c_str());
          env->CallVoidMethod(upclass, writeMesg, str);
		    }
      } //while
    } //else
  }

  // free memory
	delete(indexer);
	delete(stopper);
	delete(stemmer);
	delete(parser);
	if (index)
		delete(index);
  delete(docmgr);
  ParamPopFile();
}
