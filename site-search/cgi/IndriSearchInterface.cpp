#include "IndriSearchInterface.h"
#include <time.h>

IndriSearchInterface::IndriSearchInterface(CGIOutput *_output, lemur::api::Index *_index, indri::api::QueryEnvironment *_queryEnvironment, string _dataRoot) {
  output=_output;
  index=_index;
  queryEnvironment=_queryEnvironment;
  dataRoot=_dataRoot;
}

IndriSearchInterface::~IndriSearchInterface() {
}


void IndriSearchInterface::addToStringVector(std::vector<std::string> *origVec, std::vector<std::string> *addVec) {
  if ((!origVec)||(!addVec)) return;
  std::vector<std::string>::iterator vIter=addVec->begin();
  while (vIter!=addVec->end()) {
    origVec->push_back(*vIter);
    vIter++;
  }
}

std::vector<std::string> IndriSearchInterface::getRawNodes(indri::api::QueryAnnotationNode *node) {
  std::vector<std::string> retArray;
  if (node) {
    if (node->type=="RawScorerNode") {
      retArray.push_back(node->name);
    } else {
      std::vector<indri::api::QueryAnnotationNode*>::iterator vIter=node->children.begin();
      while (vIter!=node->children.end()) {
        std::vector<std::string> childNodeVec=getRawNodes(*vIter);
        addToStringVector(&retArray, &childNodeVec);
        vIter++;
      }
    }
  }
  return retArray;
}

std::vector<IndriSearchInterface::indriTermMatches> IndriSearchInterface::getMatches(int docID, 
                                                                   std::map<std::string, std::vector<indri::api::ScoredExtentResult> > *annotations,
                                                                   std::vector<string> *rawNodes) {
  std::vector<IndriSearchInterface::indriTermMatches> retVec;

  // thin down the annotation matches to just the ones for this document
  std::vector<indri::api::ScoredExtentResult> rawMatches;

  std::vector<std::string>::iterator nIter=rawNodes->begin();
  while (nIter!=rawNodes->end()) {
    std::map<std::string, std::vector<indri::api::ScoredExtentResult> >::iterator iterThisNode=annotations->find(*nIter);
    if (iterThisNode!=annotations->end()) {
      std::vector<indri::api::ScoredExtentResult>::iterator pIter=iterThisNode->second.begin();
      while (pIter!=iterThisNode->second.end()) {
        if ((*pIter).document==docID) {
          rawMatches.push_back(*pIter);
        }
        pIter++;
      }
    }
    nIter++;
  }

  // sort the array
  std::sort(rawMatches.begin(), rawMatches.end(), sortScoredExtentByPosition());
  
  // remove and coalesce duplicates
  int matchSize=rawMatches.size();
  if (matchSize > 0) {
    int begin=rawMatches[0].begin;
    int end=rawMatches[0].end;
    for (int i=1; i < matchSize; i++) {
      if (rawMatches[i].begin > end) {
        retVec.push_back(IndriSearchInterface::indriTermMatches(begin, end));
        begin=rawMatches[i].begin;
      }
      if (end < rawMatches[i].end) {
        end=rawMatches[i].end;
      }
    }
    retVec.push_back(IndriSearchInterface::indriTermMatches(begin, end));
  }

  // return it.
  return retVec;
}

string IndriSearchInterface::stripHtmlTags(string inputString) {
  string retString="";
  string::size_type oPos=inputString.find("<");
  if (oPos==string::npos) {
    return inputString;      
  }

  string::size_type cPos=inputString.find(">", oPos);
  if (cPos==string::npos) {
    return inputString;      
  }

  string::size_type lastPos=0;

  while ((oPos!=string::npos) && (cPos!=string::npos)) {
    retString.append(inputString.substr(lastPos, oPos-lastPos));
    retString.append(" ");
    lastPos=cPos+1;
    oPos=inputString.find("<", cPos);
    cPos=inputString.find(">", oPos);
  }

  return retString;
}

#define INDRI_SNIPPET_MAX_WINDOWSIZE 50

std::string IndriSearchInterface::getScoredExtentSummaryString(const lemur::api::DocumentManager* dm,
                                                      lemur::api::Index *db, lemur::parse::StringQuery* q,
                                                      indri::api::ScoredExtentResult &result, std::vector<string> *nodes,
                                                      std::map< std::string, std::vector<indri::api::ScoredExtentResult> > *annotations,
                                                      string docext) {

  std::vector<lemur::api::DOCID_T> docIDVec;
  int thisDocID=db->document(docext);
  docIDVec.push_back(thisDocID);
  std::vector< indri::api::ParsedDocument*> dVec=queryEnvironment->documents(docIDVec);
  if (dVec.size()==0) return "...";
  indri::utility::greedy_vector<indri::parse::TermExtent> termPositions=dVec[0]->positions;

  string fullDocText="";
  char *fullDocTextChr=dm->getDoc(docext);
  if (fullDocTextChr) {
    fullDocText.append(fullDocTextChr);      
  }


  // get snippet
  std::vector<IndriSearchInterface::indriTermMatches> matches=getMatches(result.document, annotations, nodes);
  if (matches.size()==0) {   dVec.clear(); return "..."; }

  int mw=(INDRI_SNIPPET_MAX_WINDOWSIZE / matches.size());
  if( mw < 7 ) {
    // want at least 7 words around each match
    mw = 7;
  } else if( mw > 30 ) {
    mw = 30;
  }                         

  int matchBegin = matches[0].begin;
  int matchEnd = matches[0].end;
  //$match = array( "begin" => $matchBegin, "end" => $matchEnd );
  int nWords = 0;

  int mwStart=(int)ceil((double)(mw) / 2.0);
  int mwEnd=(int)floor((double)(mw) / 2.0);
	
  int begin = matchBegin - mwStart;
  int end = matchEnd + mwEnd;

  if( begin < result.begin )
    begin=result.begin;
  if( end >= result.end ) 
    end = result.end;

  if( result.end - result.begin <= INDRI_SNIPPET_MAX_WINDOWSIZE ) {
    begin = result.begin;
    end = result.end;
  }

  int numMatches=matches.size();
  IndriSearchInterface::internalPos startInternal;
  startInternal.begin=matchBegin;
  startInternal.end=matchEnd;

  std::vector<IndriSearchInterface::indriTermMatches> segments;
  IndriSearchInterface::indriTermMatches workingSegment(begin, end);
  workingSegment.internalPositions.push_back(startInternal);

  for (int i=1; i < numMatches; i++) {
    IndriSearchInterface::internalPos matchInternal;
    matchInternal.begin=matches[i].begin;
    matchInternal.end=matches[i].end;

    begin=matches[i].begin - mwStart;
    end=matches[i].end + mwEnd;
    if (begin < result.begin) begin=result.begin;
    if (end > result.end) end=result.end;
    if ((nWords + (workingSegment.end-workingSegment.begin)) > INDRI_SNIPPET_MAX_WINDOWSIZE) {
      break;
    }

    if (workingSegment.end >= begin) {
      workingSegment.end=end;
      workingSegment.internalPositions.push_back(matchInternal);
      nWords+=(workingSegment.end-workingSegment.begin);
    } else {
      segments.push_back(workingSegment);
      nWords+=(workingSegment.end-workingSegment.begin);
      workingSegment.begin=begin;
      workingSegment.end=end;
      workingSegment.internalPositions.clear();
      workingSegment.internalPositions.push_back(matchInternal);
    }
    if (nWords > INDRI_SNIPPET_MAX_WINDOWSIZE) {
      break;
    }
  }
  segments.push_back(workingSegment);

  stringstream outString;
  for (int i=0; i < segments.size(); i++) {
    IndriSearchInterface::indriTermMatches thisSegment=segments[i];
    begin=thisSegment.begin;
    end=thisSegment.end;
    
    if ((begin > result.begin) && (i==0))
      outString << "<strong>...</strong>";

    if (((end-1)>=termPositions.size()) || (begin < 0)) {
      continue;
    }

    int beginByte=termPositions[begin].begin;
    int endByte=termPositions[end-1].end;
    int currentByte=beginByte;

    for (int j=0; j < thisSegment.internalPositions.size(); j++) {
      int beginMatch=thisSegment.internalPositions[j].begin;
      int endMatch=thisSegment.internalPositions[j].end;
      
      outString << stripHtmlTags(fullDocText.substr(currentByte, termPositions[beginMatch].begin-currentByte)) << "<strong>";
      outString << stripHtmlTags(fullDocText.substr(termPositions[beginMatch].begin, termPositions[endMatch-1].end-termPositions[beginMatch].begin)) << "</strong>";
      currentByte=termPositions[endMatch-1].end;
    }
    outString << stripHtmlTags(fullDocText.substr(currentByte, endByte-currentByte));

    if (end < (result.end-1)) {
      outString << "<strong>...</strong> ";
    }
  }

  dVec.clear();

  return outString.str();
}

std::vector<indri::api::ScoredExtentResult> IndriSearchInterface::indriRemoveDuplicateResults(std::vector<indri::api::ScoredExtentResult> results, lemur::api::Index *db) {
  std::vector<indri::api::ScoredExtentResult> retVector;

  std::vector<indri::api::ScoredExtentResult>::iterator vIter=results.begin();

  std::string lastDocNo="";
  string::size_type lastDocTildePos=string::npos;

  // first - push all results IDs into a map of before the last / and after
  std::map<std::string, std::vector<std::string> > docIDMap;
  while (vIter!=results.end()) {
    std::string thisDocIDString=db->document((*vIter).document);
    // normalize it...
    // should I use URL Decode here???
    output->stringReplaceAll(&thisDocIDString, "%7E", "~");

    string beforeLast="";
    string afterLast="";
    string::size_type lastSlash=thisDocIDString.rfind("/");
    if (lastSlash!=string::npos) {
      beforeLast=thisDocIDString.substr(0, lastSlash+1);
      afterLast=thisDocIDString.substr(lastSlash+1);
    } else {
      beforeLast=thisDocIDString;      
    }
    
    std::map<std::string, std::vector<std::string> >::iterator mIter=docIDMap.find(beforeLast);
    if (mIter!=docIDMap.end()) {
      mIter->second.push_back(afterLast);      
    } else {
      std::vector<std::string> thisMapVec;            
      thisMapVec.push_back(afterLast);
      docIDMap.insert(make_pair(beforeLast, thisMapVec));
    }

    vIter++;
  }

  // now loop and push OK ones into a map...
  std::map<int, int> clearedDocIDs;
  std::map<std::string, std::vector<std::string> >::iterator dmapIter=docIDMap.begin();
  while (dmapIter!=docIDMap.end()) {
    bool foundIndex=false;      
    std::vector<std::string>::iterator mapVecIter=dmapIter->second.begin();
    while (mapVecIter!=dmapIter->second.end()) {
      string thisEnd=(*mapVecIter);
      if ((thisEnd=="index.htm") || (thisEnd=="index.html") || (thisEnd=="index.php") || (thisEnd.length()==0)) {
        if (foundIndex==false) {
          string thisFullDoc=dmapIter->first;      
          thisFullDoc.append(thisEnd);
          clearedDocIDs.insert(make_pair(db->document(thisFullDoc), 0));
          foundIndex=true;
        }
      } else {
        string thisFullDoc=dmapIter->first;      
        thisFullDoc.append(thisEnd);
        clearedDocIDs.insert(make_pair(db->document(thisFullDoc), 0));
      }
      mapVecIter++;      
    }
    dmapIter++;
  }

  // now, go through and generate our output set.
  vIter=results.begin();
  while (vIter!=results.end()) {
    if (clearedDocIDs.find((*vIter).document)!=clearedDocIDs.end()) {
      retVector.push_back(*vIter);      
    }
    vIter++;      
  }
  
  return retVector;
}

void IndriSearchInterface::displayIndriSearchResults(lemur::api::Index *db, int datasourceID, lemur::parse::StringQuery* q, indri::api::QueryEnvironment *indriEnvironment,
                            std::vector<indri::api::ScoredExtentResult> *results, std::vector<string> *nodes,
                            std::map< std::string, std::vector<indri::api::ScoredExtentResult> > *annotations,
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
    maxResultsToGet=MIN(results->size(), DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE);
  }

  output->setResultStatistics(  datasourceID, rankStart,
                                MIN(rankStart+listLength, maxResultsToGet),
                                maxResultsToGet
                              );

  output->displayResultsPageBeginning();

  stringstream htmlListStart;
  htmlListStart << "<ol type=1 start=\"" << (rankStart + 1) << "\">\n";
  output->outputString(htmlListStart.str());


  for (int i=rankStart;(i<listLength+rankStart) && (i<maxResultsToGet);i++) {

    indri::api::ScoredExtentResult thisResult=(*results)[i];
    //
    // get DocMgr
    //

    const lemur::api::DocumentManager* dm = index->docManager(thisResult.document);

    //
    // fetch possible title
    //
    string docext = index->document(thisResult.document);

    //
    // Get the summary item (if any)
    //
    string buf = getScoredExtentSummaryString(dm, index, q, thisResult, nodes, annotations, docext);

    //
    // if we're using an Indri index - check for metadata fields...
    //
    std::string thisTitle="";
    std::string thisURL="";

    std::vector< lemur::api::DOCID_T > docIDVec;
    docIDVec.clear(); // just to make sure...
    docIDVec.push_back(thisResult.document);
    
    // get a title field (if any)
    std::vector< std::string > titleStrings=queryEnvironment->documentMetadata(docIDVec, "title");
    if (titleStrings.size() > 0) {
      thisTitle=(*(titleStrings.begin()));
    }

    thisURL=docext;

    // should we strip the root path?
    if ((CGIConfiguration::getInstance().getStripRootPathFlag()) && (thisURL.find(dataRoot)==0)) {
      // remove the data root from the docext path...
      thisURL.erase(0,dataRoot.length());
    }

    // depending on if we have a title and/or URL, decide how we want to display it
    if ((thisTitle.length() > 0) && (thisURL.length() > 0)) {
      output->writeSearchResult(thisURL, docext, thisTitle, buf, thisResult.score, datasourceID, thisResult.document);
    } else if (thisTitle.length() > 0) {
      output->writeSearchResult("", docext, thisTitle, buf, thisResult.score, datasourceID, thisResult.document);
    } else {
      output->writeSearchResult("", docext, thisURL, buf, thisResult.score, datasourceID, thisResult.document);
    }
  } // for (int i=rankStart;(i<listLength+rankStart) && (i<results.size());i++)

  output->outputString("</ol>\n");
  
  if (CGIConfiguration::getInstance().getKVItem("displayquerydebug")=="true") {
    output->outputString("<hr />Reformulated Query: " + CGIConfiguration::getInstance().getKVItem("reformulatedQuery") + "<hr />");
  }

  output->displayResultsPageEnding();
}

int IndriSearchInterface::findOccurrence(char *input, char *chrSet) {
  if ((!input) || (!chrSet)) return -1;
  char *tmp=input;
  int cCounter=0;
  while (*tmp) {
    char *cTmp=chrSet;
    while (*cTmp) {
      if (*cTmp==*tmp) {
        return cCounter;
      }
      cTmp++;
    }
    cCounter++;
    tmp++;
  }
  return -1;
}

string IndriSearchInterface::replaceAllByChr(string inputString, string characters) {
  char *inputCopy=strdup(inputString.c_str());
  char *setCopy=strdup(characters.c_str());

  int thisOccurence=findOccurrence(inputCopy, setCopy);
  while (thisOccurence > -1) {
    strcpy(inputCopy+thisOccurence, inputCopy+thisOccurence+1);
    thisOccurence=findOccurrence(inputCopy, setCopy);
  }

  string outputString;
  outputString=inputCopy;
  delete inputCopy;
  return outputString;
}

string IndriSearchInterface::indriDefaultQueryExpansion(string &origQuery, bool usePagerank) {

  if (CGIConfiguration::getInstance().getKVItem("expandindriquery")!="true") {
    return origQuery;
  }

  // don't reformulate if this query contains a query operator
  if (origQuery.find("#")!=string::npos) {
    return origQuery;
  }

  // first - remove any bad possible punctuation
  // this includes ?;:!,.+-
  origQuery=replaceAllByChr(output->URLDecodeString(origQuery), "?;:!,.+-<>/{}[]_=|\\\'");

  // also - don't reformulate if we find any quotes...
  if ((origQuery.find("\"")!=string::npos) || (origQuery.find("%22")!=string::npos)) {
    // however, if we do find quotes - wrap the terms in a #1(...)
    // replace any %22 with actual quotes to make it easier...
    string::size_type tPos=origQuery.find("%22");
    while (tPos!=string::npos) {
      origQuery.replace(tPos, 3, "\"");
      tPos=origQuery.find("%22");
    }

    char *qCopy=strdup(origQuery.c_str());
    char *tmpPlace=qCopy;
    // replace quotes with what needs to be there...
    // 0x01 for the start quote - 0x02 for an end quote
    bool hasStarted=false;
    while (*tmpPlace) {
      if ((*tmpPlace)=='\"') {
        if (hasStarted) {
          *tmpPlace=0x02;
        } else {
          *tmpPlace=0x01;
        }
        hasStarted=!hasStarted;
      }
      tmpPlace++;
    }
    
    stringstream wrappedString;
    wrappedString << "#combine( ";

    // ok - now wrap the items.
    tmpPlace=qCopy;
    while (*tmpPlace) {
      if (*tmpPlace==0x01) {
        wrappedString << " #1( ";
      } else if (*tmpPlace==0x02) {
        wrappedString << " ) ";
      } else {
        wrappedString << (*tmpPlace);
      }
      tmpPlace++;
    }
    wrappedString << " )";

    // return the new string
    delete qCopy;
    if (usePagerank) {
      stringstream toWrap;
      toWrap << "#weight( 0.1 #weight( 1.0 #prior(pagerank) ) 1.0 " << wrappedString.str() << " )";
      return toWrap.str();
    }
    return wrappedString.str();
  }

  stringstream retString;
  retString << "#combine( " << origQuery << " )";
  if (usePagerank) {
    stringstream toWrap;
    toWrap << "#weight( 0.1 #weight( 1.0 #prior(pagerank) ) 1.0 " << retString.str() << " )";
    return toWrap.str();
  }
  return retString.str();

/*
  // wrap a weighted operator around each term
  // first, split it via whitespace
  std::vector<char*> queryTermVector;
  string queryCopy;
  char *stringCopy=strdup(origQuery.c_str());
  char *thisToken=strtok(stringCopy, " \t\n\r\"");
  while (thisToken) {
    if (strlen(thisToken)) {      
      char *thisString=strdup(thisToken);
      if (thisString) {
        queryTermVector.push_back(thisString);
      }  
    }
    thisToken=strtok(NULL, " \t\n\r\"");
  }

  delete stringCopy;

  // if we've got more than 4 terms, don't build a complex
  // query... wrap it with a combine operator and return
  if ((queryTermVector.size() > 4) || (queryTermVector.size()==0)) {
    queryTermVector.clear();      
    stringstream retString;
    retString << "#combine( " << origQuery << " )";
    return retString.str();
  }

  int numTerms=queryTermVector.size();

  stringstream queryT;
  stringstream queryO;
  stringstream queryU;

  for (int startPos=0; startPos < numTerms; startPos++) {
    for (int endPos=startPos; endPos < numTerms; endPos++) {
      if (startPos==endPos) {
        queryT << "#wsum( 1.0 " <<  queryTermVector[startPos] << ".(title) 3.0 " << queryTermVector[startPos] << " 1.0 " << queryTermVector[startPos] << ".(heading) ) ";
        continue;
      }

      for (int i=startPos; i <= endPos; i++) {
        stringstream w;
        w << "#1( " << queryTermVector[i] << " )";
        queryO << "#wsum( 1.0 " << w.str() << ".(title) 3.0 " << w.str() << " 1.0 " << w.str() << ".(heading) ) ";

        stringstream uw;
        uw << " #uw" << (4*( endPos - startPos + 1 )) << "( " << queryTermVector[i] << " )";
        queryU << "#wsum( 1.0 " << uw.str() << ".(title) 3.0 " << uw.str() + " 1.0 " << uw.str() + ".(heading) ) ";
      } // end for (int i=startPos; i <= endPos; i++)
    } // end for (int endPos=startPos; endPos < numTerms; endPos++)
  } // end for (int startPos=0; startPos < numTerms; startPos++)

  stringstream retString;
  retString << "#weight( ";
  if (queryT.str().length() > 0) {
    retString << "0.8 #combine(" << queryT.str() << ") ";
  }
  if (queryO.str().length() > 0) {
    retString << "0.1 #combine(" << queryO.str() << ") ";
  }
  if (queryU.str().length() > 0) {
    retString << "0.1 #combine(" << queryU.str() << ") ";
  }
  retString << ")";

  return retString.str();
  */
}


void IndriSearchInterface::performSearch(string &query, int maxNumResults, int indexID, int listLength, int rankStart) {
  if (!output) return;
  if (!index) {
    output->writeErrorMessage("No index?", "No index set on call to IndriSearchInterface::performSearch()");
    return;
  }
  if (!queryEnvironment) {
    output->writeErrorMessage("No Query Environment?", "No Query Environment set on call to IndriSearchInterface::performSearch()");
    return;
  }

  FILE *oQueryLog=NULL;
 
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  if (CGIConfiguration::getInstance().useQueryLogging()) {
    oQueryLog=fopen(CGIConfiguration::getInstance().getQueryLogPath().c_str(), "a");
  }

  std::vector<std::string> scoringRules;
  scoringRules.push_back("method:dirichlet,mu:250,field:mainbody,operator:term");
  scoringRules.push_back("method:dirichlet,mu:1000,field:mainbody,operator:window");
  scoringRules.push_back("method:dirichlet,mu:100,field:inlink,operator:term");
  scoringRules.push_back("method:dirichlet,mu:100,field:inlink,operator:window");
  scoringRules.push_back("method:dirichlet,mu:10,field:title,operator:term");
  scoringRules.push_back("method:dirichlet,mu:5,field:title,operator:window");
  scoringRules.push_back("method:dirichlet,mu:40,field:heading,operator:term");
  scoringRules.push_back("method:dirichlet,mu:80,field:heading,operator:window");
  queryEnvironment->setScoringRules(scoringRules);

  string origQueryCopy="";
  origQueryCopy.append(query);
  origQueryCopy=output->URLDecodeString(origQueryCopy);

  if (oQueryLog) {
    string logTimeString;
    logTimeString.append(asctime(timeinfo));
    output->stringReplaceAll(&logTimeString, "\n", "");

    fprintf(oQueryLog, "[%s] Query: %s : ", logTimeString.c_str(), origQueryCopy.c_str());      
  }

  string reformulatedQuery=indriDefaultQueryExpansion(query, CGIConfiguration::getInstance().getSupportPageRankPrior());
  CGIConfiguration::getInstance().putKVItem("reformulatedQuery", reformulatedQuery);

  // indri::api::QueryAnnotation *qaResults=NULL;
	std::vector< indri::api::ScoredExtentResult> qResults;
  try {
    // qaResults=queryEnvironment->runAnnotatedQuery(reformulatedQuery, maxNumResults);
		qResults=queryEnvironment->runQuery(reformulatedQuery, maxNumResults);
  } catch (...) {
    output->resetResultsPage();
    output->setResultQuery(origQueryCopy);
    output->setResultStatistics(0, 0, 0, 0);
    output->displayResultsPageBeginning();
    output->outputString("The query &quot;&nbsp;<strong>" + origQueryCopy + "</strong>&nbsp;&quot; could not be properly parsed. Please restate the query.");
    output->displayResultsPageEnding();
    if (oQueryLog) {
      fprintf(oQueryLog, "(exception thrown)\n");      
      fclose(oQueryLog);
    }
    return;
  }

  //std::vector<indri::api::ScoredExtentResult> finalResults=indriRemoveDuplicateResults(qaResults->getResults(), index);
	std::vector<indri::api::ScoredExtentResult> finalResults=indriRemoveDuplicateResults(qResults, index);

  lemur::parse::StringQuery* q = new lemur::parse::StringQuery(query.c_str());
  // reset out results page to initialize it...
  output->resetResultsPage();
  output->setResultQuery(origQueryCopy);

  if (oQueryLog) {
    fprintf(oQueryLog, "(%d results)\n", finalResults.size());      
    fclose(oQueryLog);
  }

  // Display results
  //
  if (finalResults.size() == 0) {
    output->setResultStatistics(0, 0, 0, 0);
    output->displayResultsPageBeginning();
    output->outputString("No results.");
    if (CGIConfiguration::getInstance().getKVItem("displayquerydebug")=="true") {
      output->outputString("<hr />Reformulated Query: " + reformulatedQuery + "<hr />"); 
    }
    output->displayResultsPageEnding();
  } else {
		
		// get annotations for only those nodes we will be displaying...
		// create a docID vector...
		std::vector<lemur::api::DOCID_T> displayNodes;
		int startItem=rankStart;
		int endItem=(rankStart+listLength);
		if (endItem > finalResults.size()) { endItem=finalResults.size(); }
		for (int i=startItem; i < endItem; i++) {
			displayNodes.push_back(finalResults[i].document);
		}

		indri::api::QueryAnnotation *qaResults=queryEnvironment->runAnnotatedQuery(reformulatedQuery, displayNodes, maxNumResults);

    std::vector<string> scoreNodes=getRawNodes((indri::api::QueryAnnotationNode*)qaResults->getQueryTree());
    std::map< std::string, std::vector<indri::api::ScoredExtentResult> > resultAnnotations=qaResults->getAnnotations();
    displayIndriSearchResults(index, indexID, q, queryEnvironment, &finalResults, &scoreNodes, &resultAnnotations, listLength, rankStart);
  } // end [else] if (results.size() == 0)

  delete(q);
  
  finalResults.clear();
}


