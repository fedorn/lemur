#ifndef _INDRISEARCH_INTERFACE_H
#define _INDRISEARCH_INTERFACE_H

#include <string>
using std::string;

#include "CGIOutput.h"
#include "Index.hpp"
#include "IndexManager.hpp"
#include "Stemmer.hpp"
#include "IndexManager.hpp"
#include "InQueryRetMethod.hpp"
#include "InQueryOpParser.hpp"
#include "Stopper.hpp"
#include "StringQuery.hpp"
#include "ElemDocMgr.hpp"
#include "MatchInfo.hpp"
#include "TextHandlerManager.hpp"
#include "Param.hpp"
#include "LemurIndriIndex.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/QueryAnnotation.hpp"
#include "indri/ScoredExtentResult.hpp"

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

struct sortScoredExtentByPosition {
  bool operator()(const indri::api::ScoredExtentResult &first, const indri::api::ScoredExtentResult &second) {
    return (first.begin < second.begin);
  }
};

/**
 * A separte class for searches via Indri (to clean things up a bit...
 *
 * @author Mark J. Hoy [http://www.cs.cmu.edu/~mhoy/]
 * @version 6/29/06
 */
class IndriSearchInterface {
private:

  struct internalPos {
    int begin;
    int end;
  };

  struct indriTermMatches {
    int begin;
    int end;
    std::vector<internalPos> internalPositions;

    indriTermMatches(int b, int e) {
      begin=b;
      end=e;
    }
    ~indriTermMatches() {
      internalPositions.clear();
    }
  };

  CGIOutput *output;
  lemur::api::Index *index;
  indri::api::QueryEnvironment *queryEnvironment;
  string dataRoot;

  int findOccurrence(char *input, char *chrSet);

  string replaceAllByChr(string inputString, string characters);

  void findAndReplace(std::string &source, const std::string &find, const std::string &replace);

  string stripHtmlTags(string inputString);

  std::string getASCIIFromPercentEncoding(std::string inputSequence);

  std::string normalizeURL(std::string inputURL);

  void addToStringVector(std::vector<std::string> *origVec, std::vector<std::string> *addVec);

  std::vector<std::string> getRawNodes(indri::api::QueryAnnotationNode *node);

  std::vector<IndriSearchInterface::indriTermMatches> getMatches(int docID, std::map<std::string, std::vector<indri::api::ScoredExtentResult> > *annotations, std::vector<string> *rawNodes);

  std::vector<indri::api::ScoredExtentResult> indriRemoveDuplicateResults(std::vector<indri::api::ScoredExtentResult> results, lemur::api::Index *db);

  void displayIndriSearchResults(lemur::api::Index *db, int datasourceID, lemur::parse::StringQuery* q, indri::api::QueryEnvironment *indriEnvironment,
                            std::vector<indri::api::ScoredExtentResult> *results, std::vector<string> *nodes,
                            std::map< std::string, std::vector<indri::api::ScoredExtentResult> > *annotations,
                            int listLength, int rankStart);

  std::string getScoredExtentSummaryString(const lemur::api::DocumentManager* dm, lemur::api::Index *db, lemur::parse::StringQuery* q,
                                           indri::api::ScoredExtentResult &result, std::vector<string> *nodes,
                                           std::map< std::string, std::vector<indri::api::ScoredExtentResult> > *annotations,
                                           string docext);

  std::vector<std::string> getRawScoringNodes(const indri::api::QueryAnnotationNode *node);

  /**
   * Reformulates a free-text query
   * @param origQuery the original query
   * @return the (possibly) modified query string
   */
  string indriDefaultQueryExpansion(string &origQuery, bool usePagerank=false);

public:
  IndriSearchInterface(CGIOutput *_output, lemur::api::Index *_index, indri::api::QueryEnvironment *_queryEnvironment, string _dataRoot);
  ~IndriSearchInterface();

  void performSearch(string &query, int maxNumResults, int indexID, int listLength, int rankStart);



}; // class IndriSearchInterface

#endif // _INDRISEARCH_INTERFACE_H


