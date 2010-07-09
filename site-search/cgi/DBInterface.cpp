#include "DBInterface.h"
#include "indri/CompressedCollection.hpp"
#include "lemur-compat.hpp"

using namespace lemur::api;

/** construction / destruction **/
DBInterface::DBInterface() {
  output=NULL;
}

DBInterface::DBInterface(CGIOutput *outputInterface) {
  output=outputInterface;
}

DBInterface::~DBInterface() {
  delete _env;
}


/** private functions **/

indri::index::Index* DBInterface::_indexWithDocument( indri::collection::Repository::index_state& indexes, int documentID ) {
  for( int i=0; i<indexes->size(); i++ ) {
    int lowerBound = (*indexes)[i]->documentBase();
    int upperBound = (*indexes)[i]->documentBase() + (*indexes)[i]->documentCount();

    if( lowerBound <= documentID && upperBound > documentID ) {
      return (*indexes)[i];
    }
  }
  return 0;
}

indri::utility::greedy_vector<indri::index::FieldExtent> *DBInterface::getFieldTags(indri::index::Index *index, long docid, int fieldID) {
  indri::utility::greedy_vector<indri::index::FieldExtent> *retVal=new indri::utility::greedy_vector<indri::index::FieldExtent>;

  if (!index) return retVal;

  indri::index::TermList *tList=(indri::index::TermList*)index->termList(docid);
  if (!tList) return retVal;

  indri::utility::greedy_vector<indri::index::FieldExtent>::iterator fIter=(tList->fields()).begin();
  while (fIter!=(tList->fields()).end()) {
    if ((*fIter).id==fieldID) {
      retVal->push_back(*fIter);
    }
    fIter++;
  }
  delete tList;

  return retVal;
}


int DBInterface::getFieldCountOfTerm(indri::index::Index *index, long docid, long termid, indri::utility::greedy_vector<indri::index::FieldExtent> *anchorFields) {
  if ((!index) || (!anchorFields)) return 0;

  indri::index::TermList *tList=(indri::index::TermList*)index->termList(docid);
  if (!tList) return 0;

  int retCount=0;

  indri::utility::greedy_vector<indri::index::FieldExtent>::iterator fIter=anchorFields->begin();
  while (fIter!=anchorFields->end()) {
    for (long i=(*fIter).begin; i < (*fIter).end; i++) {
      if ((tList->terms())[i]==termid) {
        ++retCount;
        break;
      }
    } // end for (long i=(*fIter).begin; i < (*fIter).end; i++)
    fIter++;
  } // end while (fIter!=anchorFields->end())
  delete tList;

  return retCount;
}

int DBInterface::getTFFieldTagCount(indri::index::Index *index, long docid, long termid, indri::utility::greedy_vector<indri::index::FieldExtent> *fields) {
  if ((!index) || (!fields)) return 0;

  const indri::index::TermList *tList=index->termList(docid);
  if (!tList) return 0;

  int retCount=0;

  indri::utility::greedy_vector<indri::index::FieldExtent>::iterator fIter=fields->begin();
  while (fIter!=fields->end()) {
    for (long i=(*fIter).begin; i < (*fIter).end; i++) {
      if ((tList->terms())[i]==termid) {
        ++retCount;
      }
    } // end if ((tList->terms())[i]==termid)
    fIter++;
  } // end while (fIter!=anchorFields->end())
  delete tList;

  return retCount;
}

/*

Deprecated - use getTFFieldTagCount()

int DBInterface::getTFAnchorTagCount(indri::index::Index *index, long docid, long termid, indri::utility::greedy_vector<indri::index::FieldExtent> *anchorFields) {
  if ((!index) || (!anchorFields)) return 0;

  const indri::index::TermList *tList=index->termList(docid);
  if (!tList) return 0;

  int retCount=0;

  indri::utility::greedy_vector<indri::index::FieldExtent>::iterator fIter=anchorFields->begin();
  while (fIter!=anchorFields->end()) {
    for (long i=(*fIter).begin; i < (*fIter).end; i++) {
      if ((tList->terms())[i]==termid) {
        ++retCount;
      }
    } // end if ((tList->terms())[i]==termid)
    fIter++;
  } // end while (fIter!=anchorFields->end())
  delete tList;

  return retCount;
}
*/

std::string DBInterface::getStemmedTerm(std::string term) {
  // get the QueryEnvironment
  openIndex();
  std::string retWord;
  retWord = _env->stemTerm(term);
  return retWord;
}

void DBInterface::openIndex() {
  if (_env != NULL) return;
  try {
    _env = new indri::api::QueryEnvironment();
    vector<string> thisQueryHostVec=CGIConfiguration::getInstance().getQueryHostVec(pathToIndex);
    if (thisQueryHostVec.size()==0) {
      _env->addIndex(pathToIndex.c_str());
    } else {
      for (vector<string>::iterator vIter=thisQueryHostVec.begin(); vIter!=thisQueryHostVec.end(); vIter++) {
        _env->addServer(*vIter);
      }
    }
  } catch (...) {
    _env = NULL;
  }
}

/** public functions **/

void DBInterface::setDataRoot(string rootPath) {
  dataRoot=rootPath;
}

void DBInterface::setIndexPath(string indexPath) {
  pathToIndex=indexPath;
}

void DBInterface::setOutput(CGIOutput *outputInterface) {
  output=outputInterface;
}

string DBInterface::getDataRoot() {
  return dataRoot;
}

string DBInterface::getIndexPath() {
  return pathToIndex;
}

void DBInterface::displayIndexStatistics(int indexID) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", "Error opening index: " + pathToIndex);
    return;
  }

  UINT64 docCount=0;
  long double avgDocLen=0;
  UINT64 numUniqueTerms=0;
  UINT64 numTotalTerms=0;

  docCount = _env->documentCount();
  numTotalTerms = _env->termCount();
  numUniqueTerms = _env->termCountUnique();
  avgDocLen = (long double)numTotalTerms / (long double)docCount;
  stringstream statsString;
  statsString << "Corpus Size: " << docCount << " document";
  if (docCount!=1) {
    statsString << "s";
  }
  statsString << "\n";
  statsString << "Corpus Length (in words): " << numTotalTerms << "\n";
  statsString << "Unique Terms: " << numUniqueTerms << "\n";
  statsString << "Average Document Length: " << avgDocLen << " words\n";
  stringstream iTitle;
  iTitle << "Index Statistics for Index ID " << indexID;
  output->displayDataPage(statsString.str(), iTitle.str());
}

void DBInterface::listIndexFields() {
  openIndex();
  if (_env == NULL) {
    output->writeErrorMessage("Cannot open repository.","Can't open indri repository: " + pathToIndex);
    return;
  }
  stringstream fieldsString;
  std::vector<std::string> fields = _env->fieldList();
  std::vector<std::string>::iterator fIter=fields.begin();
  while (fIter!=fields.end()) {
    fieldsString << (*fIter) << "\n";
    fIter++;
  }
  output->displayDataPage(fieldsString.str(), "Index Fields Available:");
}

#if 0
std::string DBInterface::getSummaryString(const lemur::api::DocumentManager* dm, lemur::api::Index *db, lemur::parse::StringQuery* q, int resultID, string docext) {
  //
  // Build excerpt from MatchInfo matches --
  //

  std::string retSummary="";

  string doc="";
  char *tmpDoc=dm->getDoc(docext);
  if (tmpDoc) {
    doc.append(tmpDoc);
  }

  MatchInfo* mi = MatchInfo::getMatches(*db, *q, resultID);

  int matchcount = mi->count();
  //int docmatches[matchcount*2];
  int *docmatches = new int[matchcount*2]; //studio .NET doesn't like

  // mi->startIteration();
  for (int m=0; m < matchcount; m++) {
    TMatch tm = mi->at(m);
    //TMatch tm = mi->nextMatch();
    docmatches[m*2] = tm.start;
    docmatches[m*2+1] = tm.end;
  } // end for (int m=0; m < matchcount; m++)

  if (matchcount > 0) {
    // Display first three matches with passage30 for each, try to break on whitespace
    int start, end;  // bounds of passage
    int first, last; // occurrences of whitespace
    int finalpassage = matchcount;

    if (finalpassage > 3) {
      finalpassage = 3;
    }

    bool badTextOffset=false;

    for (int m=0; (!badTextOffset) && (m < finalpassage); m++) {
      start = docmatches[m*2] - 30;
      end = docmatches[m*2+1] + 30;

      if (start < 0) {
	start = 0;
      }

      if (end >= doc.length()) {
	end = doc.length() - 1;
      }

      first = start;
      last = end;

      for (int w = (docmatches[m*2] + start) / 2; w > start; w--) {
	if (w >= doc.length()) {
	  badTextOffset=true;
	} else {
	  if (doc.at(w) == ' ') first = w;
	}
      }

      for (int w = (docmatches[m*2+1] + end) / 2; w < end; w++) {
	if (w >= doc.length()) {
	  badTextOffset=true;
	} else {
	  if (doc.at(w) == ' ') last = w;
	}
      }

      if (!badTextOffset) {
	docmatches[m*2] = first;
	docmatches[m*2+1] = last;
      }
    } // end for (int m=0; m < finalpassage; m++)

    retSummary.append("...");
    if ((!badTextOffset) && (finalpassage > 1)) {
      //
      // Combine overlapping passages
      //
      for (int m=1; m < finalpassage; m++) {
	if (docmatches[m*2-1] >= docmatches[m*2]) {
	  docmatches[m*2] = docmatches[m*2-2];
	} else {
	  // no overlap so display previous passage
	  string passage = doc.substr(docmatches[m*2-2],
				      docmatches[m*2-1]+1 - docmatches[m*2-2]);
	  retSummary.append(passage + "...");
	}
      } // end for (int m=1; m < finalpassage; m++)
    } // end if (finalpassage > 1)
      // Display last passage (max third)
    if (!badTextOffset) {
      std::string passage = doc.substr(docmatches[finalpassage*2-2],
				       docmatches[finalpassage*2-1]+1 - docmatches[finalpassage*2-2]);
      retSummary.append(passage + "...");
    }
  } // end if (matchcount > 0)

  int pos = 0;
  while ((pos = retSummary.find('<',pos)) != string::npos) {
    retSummary.replace(pos, 1, "&lt;", 4);
  }

  pos = 0;
  while ((pos = retSummary.find('>',pos)) != string::npos) {
    retSummary.replace(pos, 1, "&gt;", 4);
  }

  // clean up
  delete[](docmatches);

  // Done with matches from MatchInfo -----------
  return retSummary;
}
#endif

/**
 * @param datasourceID the index of the database used for this search
 * @param results pointer to the vector of results
 * @param listLength the max. number of results to show on this page
 * @param rankStart the starting number of the first result of the page
 */
#if 0
void DBInterface::displaySearchResults(int datasourceID, 
                                       indri::api::QueryEnvironment *indriEnvironment, lemur::api::IndexedRealVector *results, 
                                       int listLength, int rankStart) { 

  
  // start the page...

  output->setMaxResultsPerPage(listLength);

  //
  //  If someone tries to go past the end of the list, don't.
  //
  if (rankStart >= results->size()) {
    rankStart = results->size() - 1;
  }

  int maxResultsToGet=results->size();
  if (DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE!=0) {
    maxResultsToGet=min(results->size(), DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE);
  }

  output->setResultStatistics(  datasourceID, rankStart,
                                min(rankStart+listLength, maxResultsToGet),
                                maxResultsToGet
                              );

  output->displayResultsPageBeginning();

  stringstream htmlListStart;
  htmlListStart << "<ol type=1 start=\"" << (rankStart + 1) << "\">\n";
  output->outputString(htmlListStart.str());


  for (int i=rankStart;(i<listLength+rankStart) && (i<maxResultsToGet);i++) {

    //
    // get DocMgr
    //
    const DocumentManager* dm = db->docManager((*results)[i].ind);

    //
    // fetch possible title
    //
    string docext = db->document((*results)[i].ind);

    //
    // Get the summary item (if any)
    //
    string buf = getSummaryString(dm, db, q, (*results)[i].ind, docext);

    // not using an Indri index - use the old fashioned methods...
    const lemur::parse::ElemDocMgr* elem = dynamic_cast<const lemur::parse::ElemDocMgr*>(dm);
    if (elem) {
      char* ptitle = elem->getElement(docext.c_str(),"TITLE");
      char* pnotes = elem->getElement(docext.c_str(),"DATE");
      string notes = "";

      if (pnotes) {
        notes = pnotes;
      }

      if (!ptitle) {
        ptitle = elem->getElement(docext.c_str(), "HEADLINE");
      }

      if (ptitle) {
        output->writeSearchResult(docext, docext, ptitle, buf, 
				  (*results)[i].val, datasourceID, 
				  (*results)[i].ind);
        delete[]ptitle;
      } else {
        output->writeSearchResult(docext, docext, docext, buf, 
				  (*results)[i].val, datasourceID, 
				  (*results)[i].ind);
      } // end if (ptitle)
    } else {
      output->writeSearchResult(docext, docext, docext, buf, 
				(*results)[i].val, datasourceID, 
				(*results)[i].ind);
    } // end [else] if (elem)
  } // for (int i=rankStart;(i<listLength+rankStart) && (i<results.size());i++)

  output->outputString("</ol>\n");
  output->displayResultsPageEnding();

}
#endif
void DBInterface::search(int datasourceID, string &query, long listLength,
			 long rankStart, QUERY_INTERFACE_TYPE queryType) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }

  if (queryType==QUERY_INTERFACE_INDRI) {
    int maxToRet=DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE;
    if (maxToRet==0) maxToRet=DEFAULT_MAX_INDRI_RESULTS;

    IndriSearchInterface *indSearch=new IndriSearchInterface(output, _env, dataRoot);
    indSearch->performSearch(query, maxToRet, datasourceID, listLength, rankStart);

    delete indSearch;
  } else {
    // unknown query type!
    output->writeErrorMessage("Unknown query type", "Unknown query type.");
    return;
  }
}

void DBInterface::getParsedDoc(long docID) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }
  //FIXME:
#if 0
  TermInfoList* termlist = db->termInfoListSeq(docID);
  if (!termlist) {
    output->writeErrorMessage("Cannot find parsed document.", 
			      "Cannot find parsed document termlist.");
    delete db;
    return;
  }

  TermInfoList::iterator it;
  std::string doc = "";

  termlist->startIteration();
  while (termlist->hasMore()) {
    TermInfo *thisEntry=termlist->nextEntry();
    if (thisEntry) {
      doc+=db->term(thisEntry->termID()) + " ";
    }
  }

  // just print out the whole string
  output->displayDataPage(doc, "Parsed Document", false);

  delete termlist;

  // close and release the index...
  delete db;
#endif
}

void DBInterface::getDocIID(long docID) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }

  std::vector<lemur::api::DOCID_T> docIDs;
  docIDs.push_back(docID);
  std::vector<indri::api::ParsedDocument*> documents = 
    _env->documents(docIDs);
  // Just take the first one (there should be only one)
  indri::api::ParsedDocument* document = documents[0];
  if (document) {
    output->displayDataPage(documents[0]->text, "Document by Internal ID");
    return;
  }
  stringstream docidStrStr;
  docidStrStr << docID;
  output->writeErrorMessage("Document not found.", "Document not found. Document: " + docidStrStr.str());
}

void DBInterface::getDocXID(string *docID) {
  if (!docID) {
    output->writeErrorMessage("DocID is NULL.", "DocID is NULL.");
    return;
  }

  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }
  std::vector<std::string> idList;
  idList.push_back(docID->c_str());
  std::vector<lemur::api::DOCID_T> docIDs = 
    _env->documentIDsFromMetadata("docno", idList);
  std::vector<indri::api::ParsedDocument*> documents = 
    _env->documents(docIDs);

  // Just take the first one (there should be only one)
  indri::api::ParsedDocument* document = documents[0];
  if (document) {
    output->displayDataPage(document->text, "Document by External ID");
    return;
  }
  output->writeErrorMessage("Error retrieving document.", 
			    "Cannot retrieve document.");
}

void DBInterface::getTermCorpusStats(string *term) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }
  stringstream statsString;
  if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
    statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
  }
  statsString << setw(9) << _env->termCount (*term)   << "  " << setw(9) << _env->documentCount(*term) << "\n";
  
  output->displayDataPage(statsString.str(), "Corpus Statistics");
}

void DBInterface::getTermInvList (string *term) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }
  stringstream statsString;
  //FIXME
#if 0
  if (!dlist) {
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    statsString << setw(9) << 0 << "  " << setw(9) << 0 << "\n";
  } else {
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    statsString << setw(9) << db->termCount (termid)   << "  " << setw(9) << dlist->docFreq() << "\n\n";

    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "docid"  << "  " << setw(9) << "doclen" << "  " << setw(9) << "tf"     << "\n";
    }

    dlist->startIteration();
    while (dlist->hasMore()) {
      DocInfo* info = dlist->nextEntry();
      statsString << setw(9) << info->docID()  << "  " << setw(9) << db->docLength(info->docID()) << "  " << setw(9) << info->termCount() << "\n";
    }

    delete dlist;
  }
  output->displayDataPage(statsString.str(), "Inverted list for " + (*term));

  delete db;
#endif
}

void DBInterface::getTermInvPosList (string *term) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", "Error opening index: " + pathToIndex);
    return;
  }
  // FIXME:
#if 0
  stringstream statsString;
  if (!dlist) {
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    statsString << setw(9) << 0 << "  " << setw(9) << 0 << "\n";
  } else {
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    statsString << setw(9) << db->termCount (termid)   << "  " << setw(9) << dlist->docFreq() << "\n\n";
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "docid"  << "  " << setw(9) << "doclen" << "  " << setw(9) << "tf"     << "  " << setw(9) << "positions"     << "\n";
    }

    dlist->startIteration();
    while (dlist->hasMore()) {
      DocInfo* info = dlist->nextEntry();
      int tf=info->termCount();
      const LOC_T *pos=info->positions();
      statsString << setw(9) << info->docID()  << "  " << setw(9) << db->docLength(info->docID()) << "  " << setw(9) << tf << "  " << setw(9);
      for (int i=0; i<tf; i++) {
        statsString << pos[i] << "  ";
      }
      statsString << "\n";
    }
    delete dlist;
  }
  output->displayDataPage(statsString.str(), "Inverted list for " + (*term));

  delete db;
#endif
}

void DBInterface::getWordStem(string *term) {
  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", "Error opening index: " + pathToIndex);
    return;
  }
  std::string stemmedWord=getStemmedTerm(*term);
  INT64 count = _env->stemCount(stemmedWord);
  // OOV have tf==0.
  if (count == 0) {
    output->displayDataPage("[OOV]\n", "Word stem for " + (*term));
  } else {
    output->displayDataPage(stemmedWord, "Word stem for " + (*term));
  }
}


void DBInterface::getTermInvListField(string *term) {
  // first - get the field position (if any)
  string::size_type dotPos=term->find(".", 0);

  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", "Error opening index: " + pathToIndex);
    return;
  }
  //FIXME:
#if 0
  // our output stream string
  stringstream statsString;

  string theTerm="";
  string theField="";

  // got it - separate the two...
  if (dotPos==string::npos) {
    // no field? use the mainbody field (only if it exists!)
    theTerm=term->c_str();

    const std::vector< indri::collection::Repository::Field > repFields=_repository.fields();
    std::vector< indri::collection::Repository::Field >::const_iterator fIter=repFields.begin();
    for (; (fIter!=repFields.end() && theField.length()==0); fIter++) {
      if ((*fIter).name=="mainbody") {
	theField="mainbody";
      }
    }

    if (theField.length()==0) {
      // not found? use the old method...
      getTermInvList(term);
      return;
    }
    
  } else {
    theTerm=term->substr(0, dotPos);
    theField=term->substr(dotPos+1);
  }

  // get the stemmed term (if any)
  string stemmedTerm=getStemmedTerm(theTerm, db);

  // get the internal term ID
  int termid = db->term(theTerm.c_str());

  // get inverted list
  lemur::index::InvDocList *dlist = 0L;
  if (termid) {
    dlist=(lemur::index::InvDocList*) db->docInfoList (termid);
  }

  if (!dlist) {
    // no inverted list? Just output the headers...
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    // term count and doc frequency of 0.
    statsString << setw(9) << 0 << "  " << setw(9) << 0 << "\n";
  } else {
    // got the list - print headers
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    statsString << setw(9) << db->termCount (termid)   << "  " << setw(9) << dlist->docFreq() << "\n\n";

    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "docid"  << "  "
		  << setw(9) << "# " << theField  << "  "
		  << setw(9) << theField << "_freq"  << "  "
		  << setw(9) << "tf (" << theField << ")"  << "\n";
    } // if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM)

    // iterate through the inv. list
    dlist->startIteration();
    while (dlist->hasMore()) {
			
      // get the next entry...
      DocInfo* info = dlist->nextEntry();
      if (info) {
	int fieldCount=0;    // total number of fields
	int fieldFreq=0;     // number of fields where term occurs
	int fieldTFCount=0;  // total term frequency within the fields

	// we unfortunately need to do this for every document - 
	// we don't know if the document is in a different index within
	// the repository...
	indri::collection::Repository::index_state indexes = _repository.indexes();

	// ensure we get the correct internal index
	indri::index::Index* index = _indexWithDocument(indexes,info->docID());
	if (index) {
	  // get the correct internal field ID
	  int fieldID=index->field(theField);

	  // get a vector of the field tags
	  indri::utility::greedy_vector<indri::index::FieldExtent> *aTagVec=getFieldTags(index, info->docID(), fieldID);
	  if (aTagVec) {
	    // get the count (size)
	    fieldCount=aTagVec->size();
	    // get the actual # of fields (total)
	    fieldFreq=getFieldCountOfTerm(index, info->docID(), termid,
					  aTagVec);
	    // get the tf per field...
	    fieldTFCount=getTFFieldTagCount(index, info->docID(), termid,
					    aTagVec);
	    delete aTagVec;
	  } // end if (aTagVec)
	} // end if (index)

	if (fieldFreq > 0) {
	  statsString << setw(9) << info->docID() << "  "
		      << setw(9) << fieldCount << "  "
		      << setw(9) << fieldFreq << "  "
		      << setw(9) << fieldTFCount << "\n";
	} // end if (fieldFreq > 0)
      } // end if (info)
    } // end while (dlist->hasMore())


    // we're done here - cleanup.
    delete dlist;
  } // end if (!dlist)

  output->displayDataPage(statsString.str(), "Inverted list for " + (*term));

  _repository.close();
  delete db;
#endif
} // end function: void getTermInvListField(string*)


void DBInterface::getTermInvPosListField(string *term) {
  // first - get the field (if any)
  string::size_type dotPos=term->find(".", 0);

  openIndex();
  if (!_env) {
    output->writeErrorMessage("Cannot open index.", 
			      "Error opening index: " + pathToIndex);
    return;
  }
  // FIXME:
#if 0
  // our output stream string
  stringstream statsString;

  string theTerm="";
  string theField="";

  // got it - separate the two...
  if (dotPos==string::npos) {
    // no field? use the mainbody field (only if it exists!)
    theTerm=term->c_str();

    const std::vector< indri::collection::Repository::Field > repFields=_repository.fields();
    std::vector< indri::collection::Repository::Field >::const_iterator fIter=repFields.begin();
    for (; (fIter!=repFields.end() && theField.length()==0); fIter++) {
      if ((*fIter).name=="mainbody") {
	theField="mainbody";
      }
    }

    if (theField.length()==0) {
      // not found? use the old method...
      getTermInvList(term);
      return;
    }
    
  } else {
    theTerm=term->substr(0, dotPos);
    theField=term->substr(dotPos+1);
  }

  // get the stemmed term (if any)
  string stemmedTerm=getStemmedTerm(theTerm, db);

  // get the internal term ID
  int termid = db->term(theTerm.c_str());

  // get inverted list
  lemur::index::InvDocList *dlist = 0L;
  if (termid) {
    dlist=(lemur::index::InvDocList*) db->docInfoList (termid);
  }

  if (!dlist) {
    // no inverted list? Just output the headers...
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    // term count and doc frequency of 0.
    statsString << setw(9) << 0 << "  " << setw(9) << 0 << "\n";
  } else {
    // got the list - print headers
    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "ctf" << "  " << setw(9) << "df" << "\n";
    }
    statsString << setw(9) << db->termCount (termid) << "  " << setw(9) 
		<< dlist->docFreq() << "\n\n";

    if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM) {
      statsString << setw(9) << "docid"  << "  "
		  << setw(9) << "# " << theField  << "  "
		  << setw(9) << theField << "_freq"  << "  "
		  << setw(9) << "tf (" << theField << ")"  << "  "
                  << setw(9) << theField << "_n_tf..."     << "  "
                  << setw(9) << theField << "_n_loc_x..."  << "\n";
    } // if (output->getOutputMode()!=CGI_OUTPUT_PROGRAM)

    // iterate through the inv. list
    dlist->startIteration();
    while (dlist->hasMore()) {
			
      // get the next entry...
      DocInfo* info = dlist->nextEntry();
      if (info) {
	int fieldCount=0;    // total number of fields
	int fieldFreq=0;     // number of fields where term occurs
	int fieldTFCount=0;  // total term frequency within the fields
        int *fieldTFCounts=NULL; //array to hold individual field counts
        int *fieldTFLocations=NULL; //array to hold individual field positions


	// we unfortunately need to do this for every document - 
	// we don't know if the document is in a different index within
	// the repository...
	indri::collection::Repository::index_state indexes = _repository.indexes();

	// ensure we get the correct internal index
	indri::index::Index* index = _indexWithDocument(indexes,info->docID());
	if (index) {
	  // get the correct internal field ID
	  int fieldID=index->field(theField);

	  // get a vector of the field tags
	  indri::utility::greedy_vector<indri::index::FieldExtent> *aTagVec=getFieldTags(index, info->docID(), fieldID);
	  if (aTagVec) {
	    // get the count (size)
	    fieldCount=aTagVec->size();
	    // get the actual # of fields (total)
	    fieldFreq=getFieldCountOfTerm(index, info->docID(), termid,
					  aTagVec);
	    // get the tf per field...
	    fieldTFCount=getTFFieldTagCount(index, info->docID(), termid,
					    aTagVec);

	    //--- new code

	    // generate the inlink_n_tf and inlink_n_loc_x arrays...
	    if ((fieldTFCount > 0) && (fieldFreq > 0)) {
	      fieldTFCounts=new int[fieldFreq];
	      fieldTFLocations=new int[fieldTFCount];

	      int currentLinkPos=0;
	      int currentLocPos=0;

	      indri::index::TermList *tList=(indri::index::TermList*)index->termList(info->docID());
	      if (!tList) {
		delete fieldTFCounts; fieldTFCounts=NULL;
		delete fieldTFLocations; fieldTFLocations=NULL;
	      } else {
		indri::utility::greedy_vector<int> termVec=tList->terms();
		indri::utility::greedy_vector<indri::index::FieldExtent>::iterator fIter=aTagVec->begin();
		while (fIter!=aTagVec->end()) {
		  // cerr << "field iter...";
		  long fieldStart=(*fIter).begin;
		  bool thisOneHasTerm=false;
		  int  thisFieldTFCount=0;

		  for (long i=fieldStart; i < (*fIter).end; i++) {
		    if (termVec[i]==termid) {
		      if (!thisOneHasTerm) {
			thisOneHasTerm=true;
		      } // end if (!thisOneHasTerm)
		      thisFieldTFCount++;
		      fieldTFLocations[currentLocPos++]=(i-fieldStart);
		    } // end if (termVec[i]==termid)
		  } // end for (long i=anchorFieldStart; i < (*fIter).end; i++)

		  if (thisOneHasTerm) {
		    fieldTFCounts[currentLinkPos++]=thisFieldTFCount;
		  } // end if (thisOneHasTerm)

		  fIter++;
		} // end while (fIter!=anchorFields->end())
	      } // if (!tList)
	    } // if ((anchorTFCount > 0) && (anchorFreq > 0))

	    //--- end new code

	    delete aTagVec;
	  } // end if (aTagVec)
	} // end if (index)

	if (fieldFreq > 0) {

	  int tf=(info->termCount() - fieldFreq);
	  const LOC_T *pos=info->positions();

	  statsString << setw(9) << info->docID()  << "  " << setw(9);

	  for (int i=0; i<tf; i++) {
	    statsString << pos[i] << "  ";
	  }

	  statsString << setw(9) << fieldCount << "  " << setw(9) 
		      << fieldFreq << "  " << setw(9) << fieldTFCount 
		      << "  " << setw(9);

	  if (fieldTFCounts && fieldTFLocations) {
	    int currentTFPos=0;
	    for (int i=0; i < fieldFreq; i++) {
	      int currentTF=fieldTFCounts[i];
	      statsString << currentTF << "  ";
	      for (int j=0; j < currentTF; j++) {
		statsString << fieldTFLocations[currentTFPos++] << "  ";
	      } // end for (int j=0; j < currentTF; j++)
	    } // end for (int i=0; i < fieldFreq; i++)
	  } // end if (fieldTFCounts && fieldTFLocations)
	  statsString << "\n";
	} // end if (fieldFreq > 0)

	if (fieldTFCounts) delete fieldTFCounts;
	if (fieldTFLocations) delete fieldTFLocations;

	fieldTFCounts=NULL;
	fieldTFLocations=NULL;

      } // end if (info)
    } // end while (dlist->hasMore())

    // we're done here - cleanup.
    delete dlist;
  } // end if (!dlist)

  output->displayDataPage(statsString.str(), "Inverted list for " + (*term));

  _repository.close();
  delete db;
#endif
} // end function void getTermInvPosListField(string*)

