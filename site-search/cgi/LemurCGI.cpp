#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <map>
using std::map;

#include "LemurSearchCGIConstants.h"
#include "CGIConfiguration.h"
#include "CGIOutput.h"
#include "DictionaryHash.h"
#include "DBInterface.h"

// these are for offline debugging...
// #define OFFLINEDEBUGGING
// #define OFFLINEDEBUGGING_QUERY "va=test"

// our query parameters

typedef struct kvPair_T {
  string key;
  string value;

  kvPair_T(string k, string v) {
    key=k; value=v;
  }

} t_kvPair;

std::vector<t_kvPair*> queryParameters;

string appPath;

/*
 *  Extract a name/value pair from a CGI query string.  Return the
 *  query string with that pair removed.
 */
static void extractNameValue (string *query, string *name, string *value)
{
  size_t i = query->find ("=");
  size_t j = query->find ("&");

  if (j == string::npos)
    j = query->length ();

  *name =  query->substr (0, i);
  *value = query->substr (i+1, (j-i)-1);

  if ((*query)[j] == '&')
    *query = query->substr (j+1);
  else
    *query = query->substr (j);
}

bool getCGIParams(CGIOutput *output, string requestMethod) {
  // only currently support get...
  if (requestMethod=="GET") {

#ifndef OFFLINEDEBUGGING
    string query;
    char *qStringEnv=getenv("QUERY_STRING");
    if (qStringEnv) {
      query=qStringEnv;
    }
#else
    string query(OFFLINEDEBUGGING_QUERY);
#endif

    while (query!="") {
      string tmpName;
      string tmpValue;

      extractNameValue (&query, &tmpName, &tmpValue);
      tmpValue=output->URLDecodeString(tmpValue);

      // TODO : don't use dictionary hash - use vector
      // with structure for key=value!
      // multiple key/value pairs and process them in order...
      // old --> queryParameters.put(tmpName.c_str(), tmpValue.c_str());
      queryParameters.push_back(new t_kvPair(tmpName, tmpValue));
    }

    return true;
  }

  return false;
}

void printOutHelp(CGIOutput *output) {
  if (!output) return;

  output->outputHTTPHeader("text/plain");

  stringstream helpMessage;
  helpMessage << "Lemur CGI - Help:\n\n"
              << "Invoke the CGI interface by "
              << appPath
              << "?name=value&name=value&....\n\n"
              << "Name=value pairs are processed from left to right, in order\n\n"
              << "NAME=VALUE ARGUMENTS\n"
              << "c=term prints corpus statistics for term\n"
              << "d=n sets the database to the n'th database\n"
              << "d=? lists the available databases\n"
              << "D=n displays the statistics for the database ID\n"
              << "e=<string> fetches the document with external id <string>\n"
              << "g=d sets the CGI interface to Diagnostic mode\n"
              << "g=i sets the CGI interface to Interactive mode\n"
              << "g=p sets the CGI interface to Program mode\n"
              << "h=<anything> prints this help message\n"
              << "i=<integer> fetches the document with internal id <integer>\n"
              << "I=<integer> fetches the parsed form of the document with internal id <integer>\n"
              << "m=<string> shows the lexicalized (stopped, stemmed) form of <string>\n"
              << "n=x sets the number of documents to retrieve to x\n"
              << "q=<string> uses the query <string> to search the database\n"
              << "s=n starts the query results at rank n\n"
              << "t=<query_type> tells the CGI which type of query interface to use (currently \"indri\" [default] or \"inquery\")\n"
              << "v=term returns the inverted list for term\n"
              << "V=term returns the inverted list for term with positions\n"
              << "x=false if using indri-style queries, this will not expand the query (expansion is turned on by default)\n";

  if (CGIConfiguration::getInstance().getSupportAnchorText()) {
    helpMessage << "va=term returns the inverted list for term with anchor text term frequencies\n";
    helpMessage << "VA=term returns the inverted list for term with positions and anchor text term frequencies\n";
  }

  if (CGIConfiguration::getInstance().getSupportURLText()) {
    helpMessage << "vu=term returns the inverted list for term with URL text term frequencies\n";
    helpMessage << "VU=term returns the inverted list for term with positions and URL text term frequencies\n";
  }

  helpMessage << "\n";

  output->outputString(helpMessage.str());
  output->writePlaintextFooter();
}

void processRequest(CGIOutput *output) {
  if (!output) return;

  if (queryParameters.size()==0) {
    // no parameters - display search box
    output->displayDefaultSearchPage();
    return;
  }

  // set up some default items...

  // create our database interface...
  DBInterface db(output);

  if ((CGIConfiguration::getInstance().getStripRootPathFlag()) && (CGIConfiguration::getInstance().getNumRootPaths())) {
    // for now, only set the first root path...
    db.setDataRoot(CGIConfiguration::getInstance().getRootPath(0));
  }

  // default is interactive mode...
  output->setOutputMode(CGI_OUTPUT_INTERACTIVE);

  int datasourceToUse=0;  // set default index path...
  db.setIndexPath(CGIConfiguration::getInstance().getIndexPath(datasourceToUse));

  // set max # documents to retrieve...
  int maxDocsToRetrievePerPage=DEFAULT_RESULTS_PER_PAGE;

  // query results start rank...
  int startRank=0;

  // check to make sure we've outputted something
  bool hasOutput=false;

  // set query expansion for indri on by default
  CGIConfiguration::getInstance().putKVItem("expandindriquery", "true");

  DBInterface::QUERY_INTERFACE_TYPE currentQueryType=DBInterface::QUERY_INTERFACE_INDRI;

  std::vector<t_kvPair*>::iterator vIter=queryParameters.begin();
  while (vIter!=queryParameters.end()) {

    string thisKey=(*vIter)->key;
    string thisVal=(*vIter)->value;

    if (thisKey.length()==0) {
      // should never happen... but just in case...
      vIter++;
      continue;
    }

    char firstKeyChar=thisKey[0];

    switch (firstKeyChar) {
      case 'h': {
                  // print help

                  printOutHelp(output);
                  hasOutput=true;
                } break;
      case 'x': {
                  CGIConfiguration::getInstance().putKVItem("expandindriquery", thisVal);
                } break;
      case 'g': {
                  // set interface mode
                  if (thisVal.length() > 0) {
                    switch (thisVal[0]) {
                    case 'd':
                      output->setOutputMode(CGI_OUTPUT_DIAGNOSTIC);
                      break;
                    case 'p':
                      output->setOutputMode(CGI_OUTPUT_PROGRAM);
                      break;
                    case 'i':
                      output->setOutputMode(CGI_OUTPUT_INTERACTIVE);
                      break;
                    default:
                      // do nothing.
                      break;
                    } // end switch (thisVal[0])
                  }
                } break;
      case 'd': {
                  // select datasource
                  if (thisVal.length() > 0) {
                    if (thisVal[0]=='?') {
                      // display datasource list and exit...
                      output->displayIndexListingPage();
                      return;
                    }

                    datasourceToUse=atoi(thisVal.c_str());
                    if (datasourceToUse >= CGIConfiguration::getInstance().getNumIndices()) {
                      // error out - unknown datasource...
                      stringstream tmpDSource;
                      tmpDSource << "Unknown datasource value: d=" << datasourceToUse;
                      output->writeErrorMessage("Unknown datasource.", tmpDSource.str());
                      return;
                    }

                    db.setIndexPath(CGIConfiguration::getInstance().getIndexPath(datasourceToUse));
                  }
                } break;
      case 'D': {
                  // get database statistics
                  if (thisVal.length() > 0) {
                    if (thisVal[0]=='?') {
                      // display datasource list and exit...
                      output->displayIndexListingPage();
		      hasOutput=true;
                      break;
                    }

                    int whichDatasource=atoi(thisVal.c_str());

                    if (whichDatasource >= CGIConfiguration::getInstance().getNumIndices()) {
                      // error out - unknown datasource...
                      stringstream tmpDSource;
                      tmpDSource << "Unknown datasource value: D=" << whichDatasource;
                      output->writeErrorMessage("Unknown datasource.", tmpDSource.str());
                      return;
                    }

                    db.setIndexPath(CGIConfiguration::getInstance().getIndexPath(whichDatasource));
                    db.displayIndexStatistics(whichDatasource);
                    db.setIndexPath(CGIConfiguration::getInstance().getIndexPath(datasourceToUse));
		    hasOutput=true;
                  }
                } break;
      case 't': {
                  // set query type...
                  if (thisVal.compare("indri")==0) {
                    currentQueryType=DBInterface::QUERY_INTERFACE_INDRI;
                  } else if (thisVal.compare("inquery")==0) {
                    currentQueryType=DBInterface::QUERY_INTERFACE_INQUERY;
                  } else {
                    // ignore it...
                  }
                }
      case 'n': {
                  // set max # of documents to retrieve per page
                  maxDocsToRetrievePerPage=atoi(thisVal.c_str());
                  if (maxDocsToRetrievePerPage < 1) {
                    maxDocsToRetrievePerPage=DEFAULT_RESULTS_PER_PAGE;
                  }
                } break;
      case 's': {
                  // query results start rank...
                  startRank=atoi(thisVal.c_str());
                  if (startRank < 0) {
                    startRank=0;
                  }

                  if (DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE!=0) {
                    if (startRank > DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE) {
                      startRank=DEFAULT_MAX_DOCUMENTS_TO_RETRIEVE-1;
                    }
                  }
                } break;
      case 'q': {
                  // query
                  if (thisVal=="") {
                    output->displayDefaultSearchPage();      
                  } else {
                    db.search(datasourceToUse, thisVal, maxDocsToRetrievePerPage, startRank, currentQueryType);
                  }  
                  hasOutput=true;
                } break;
      case 'Q': {
                  // query w/ extra debugging info
                  CGIConfiguration::getInstance().putKVItem("displayquerydebug", "true");
                  if (thisVal=="") {
                    output->displayDefaultSearchPage();      
                  } else {
                    db.search(datasourceToUse, thisVal, maxDocsToRetrievePerPage, startRank, currentQueryType);
                  }  
                  hasOutput=true;
                } break;  
      case 'e': {
                  // get document from ID string
                  db.getDocXID(&thisVal);
                  hasOutput=true;
                } break;
      case 'i': {
                  // get document from internal ID
                  long internalID=atol(thisVal.c_str());
                  db.getDocIID(internalID);
                  hasOutput=true;
                } break;
      case 'I': {
                  // get parsed document
                  long internalID=atol(thisVal.c_str());
                  db.getParsedDoc(internalID);
                  hasOutput=true;
                } break;
      case 'c': {
                  // term corpus statistics
                  db.getTermCorpusStats(&thisVal);
                  hasOutput=true;
                } break;
      case 'm': {
                  // show lexicalized version...
                  db.getWordStem(&thisVal);
                  hasOutput=true;
                } break;
      case 'v': {
                  // get inverted term list
                  if (CGIConfiguration::getInstance().getSupportAnchorText()) {
                    if ((thisKey.length()==2) && (thisKey[1]=='a')) {
                      db.getTermInvListWithAnchor(&thisVal);
                      hasOutput=true;
                      break;
                    }
                  } // end if (CGIConfiguration::getInstance().getSupportAnchorText())

                  if (CGIConfiguration::getInstance().getSupportURLText()) {
                    if ((thisKey.length()==2) && (thisKey[1]=='u')) {
                      db.getTermInvListWithURL(&thisVal);
                      hasOutput=true;
                      break;
                    }
                  } // end if (CGIConfiguration::getInstance().getSupportURLText())

				  db.getTermInvList(&thisVal);
                  hasOutput=true;
                } break;
      case 'V': {
                  // get inverted term list w/ position
                  if (CGIConfiguration::getInstance().getSupportAnchorText()) {
                    if ((thisKey.length()==2) && (thisKey[1]=='A')) {
                      db.getTermInvPosListWithAnchor(&thisVal);
                      hasOutput=true;
                      break;
                    }
                  } // end if (CGIConfiguration::getInstance().getSupportAnchorText())

                  if (CGIConfiguration::getInstance().getSupportURLText()) {
                    if ((thisKey.length()==2) && (thisKey[1]=='U')) {
                      db.getTermInvPosListWithURL(&thisVal);
                      hasOutput=true;
                      break;
                    }
                  } // end if (CGIConfiguration::getInstance().getSupportURLText())

                  db.getTermInvPosList(&thisVal);
                  hasOutput=true;
                } break;
      default:
        // do nothing - skip this one...
        break;
     } // end switch (firstKeyChar)
    vIter++;
  } // end while (vIter!=queryParameters.end())
  if (!hasOutput) {
    // nothing was output? Do a default page      
    output->displayDefaultSearchPage();
  }
}

char* getFileFromPath(const char *filePath) {
  if ((!filePath) || (!strlen(filePath)))
    return NULL;

  char *fPtr=(char*)(filePath+strlen(filePath)-1);
  //accounts for *nix, Windows/MS-DOS, & Macintosh path names
  while ((fPtr!=filePath) && ((*fPtr)!='/') && ((*fPtr)!='\\') && ((*fPtr)!=':')) {
    fPtr--;
  }
  if (fPtr!=filePath) {
    return fPtr+1;
  } else {
    if ((*fPtr=='/') || (*fPtr=='\\') || (*fPtr==':'))
      return fPtr+1;
    else
      return fPtr;
  }
}

int main (int argc, char **argv) {

  // right off the bat - get the request method and our pathf
#ifndef OFFLINEDEBUGGING
  string cgi_request_method(getenv("REQUEST_METHOD"));
#else
  string cgi_request_method("GET");
#endif

  appPath = argv[0];

  // strip the path off the appPath so we're left with the filename
  appPath=getFileFromPath(appPath.c_str());

  // get our configuration
  //config=new CGIConfiguration(LEMURCGI_CONFIG_PATH);
  CGIConfiguration::getInstance().loadConfiguration(LEMURCGI_CONFIG_PATH);

  // create our output object
  CGIOutput output(appPath);

  // get the parameters
  if (!getCGIParams(&output, cgi_request_method)) {
    output.writeErrorMessage("Unsuppored Request Method.", "Error:  Unsupported request method. Please notify Web site administrator.");
  } else {
    processRequest(&output);
  }

  queryParameters.clear();

  // all finished... exit.
  return 0;
}

