/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// QueryEnvironment
//
// 9 March 2004 -- tds
//
#include "indri/indri-platform.h"
#include "indri/QueryEnvironment.hpp"
#include "indri/CompressedCollection.hpp"

#include <vector>
#include <queue>
#include "indri/delete_range.hpp"

#include "indri/InferenceNetwork.hpp"
#include "indri/IndriIndex.hpp"
#include "indri/QuerySpec.hpp"
#include "indri/ScoredExtentResult.hpp"

#include "indri/LocalQueryServer.hpp"
#include "indri/NetworkServerProxy.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkMessageStream.hpp"

#include "indri/QueryLexer.hpp"
#include "indri/QueryParser.hpp"

#include "indri/DagCopier.hpp"
#include "indri/InferenceNetworkBuilder.hpp"
#include "indri/RawScorerNodeExtractor.hpp"
#include "indri/ContextSimpleCountCollectorCopier.hpp"
#include "indri/TreePrinterWalker.hpp"
#include "indri/ContextCountGraphCopier.hpp"
#include "indri/SmoothingAnnotatorWalker.hpp"
#include "indri/ExtentRestrictionModelAnnotatorCopier.hpp"

#include "indri/InferenceNetworkBuilder.hpp"
#include "indri/Packer.hpp"
#include "indri/Unpacker.hpp"
#include "indri/QueryAnnotation.hpp"

#include "indri/XMLReader.hpp"
#include "indri/IndriTimer.hpp"

#include <set>
#include <map>

#include "indri/Appliers.hpp"

// debug code: should be gone soon
#ifdef TIME_QUERIES
#define INIT_TIMER      IndriTimer t; t.start();
#define PRINT_TIMER(s)  { t.printElapsedMicroseconds( std::cout ); std::cout << ": " << s << std::endl; }
#else
#define INIT_TIMER
#define PRINT_TIMER(s)
#endif

// for debugging; this class prints a query tree
class Printer : public indri::lang::Walker {
private:
  int tabs;
public:
  Printer() : tabs(0) {}

  void defaultBefore( indri::lang::Node* n ) {
    for( int i=0; i<tabs; i++ )
      std::cout << "\t";
    std::cout << n->typeName() << " " << n->queryText() << std::endl;
    tabs++;
  }

  void defaultAfter( indri::lang::Node* n ) {
    tabs--;
  }

};

//
// Helper document methods
//

// split a document ID list into sublists, one for each query server
void qenv_scatter_document_ids( const std::vector<DOCID_T>& documentIDs, std::vector< std::vector<DOCID_T> >& docIDLists, std::vector< std::vector<DOCID_T> >& docIDPositions, int serverCount ) {
  docIDLists.resize( serverCount );
  docIDPositions.resize( serverCount );

  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    DOCID_T id = documentIDs[i];
    int serverID = id % serverCount;

    docIDLists[serverID].push_back( id / serverCount );
    docIDPositions[serverID].push_back(i);
  }
}

// retrieve a list of results from each query server, and fold those results into a master list
template<class _ResponseType, class _ResultType>
void qenv_gather_document_results( const std::vector< std::vector<DOCID_T> >& docIDLists,
                                   const std::vector< std::vector<DOCID_T> >& docIDPositions,
                                   greedy_vector<_ResponseType>& responses,
                                   std::vector<_ResultType>& results ) {
  for( unsigned int i=0; i<docIDLists.size(); i++ ) {
    if( docIDLists[i].size() ) {
      std::vector<_ResultType> serverResult = responses[i]->getResults();
      delete responses[i];
      
      // fold the document names back into one large result list
      for( unsigned int j=0; j<docIDLists[i].size(); j++ ) {
        int resultIndex = docIDPositions[i][j];
        results[ resultIndex ] = serverResult[j];
      }
    }
  }
}

//
// QueryEnvironment definition
//

QueryEnvironment::QueryEnvironment() {
}

QueryEnvironment::~QueryEnvironment() {
  close();
}

void QueryEnvironment::setMemory( UINT64 memory ) {
  _parameters.set( "memory", memory );
}

void QueryEnvironment::setScoringRules( const std::vector<std::string>& rules ) {
  _parameters.set("rule","");
  for( unsigned int i=0; i<rules.size(); i++ ) {
    _parameters.append("rule").set( rules[i] );
  }
}

void QueryEnvironment::setStopwords( const std::vector<std::string>& stopwords ) {
  _parameters.set("stopper","");
  Parameters p = _parameters.get("stopper");
  for( unsigned int i=0; i<stopwords.size(); i++ ) {
    _parameters.append("word").set(stopwords[i]);
  }
}

void QueryEnvironment::_copyStatistics( std::vector<indri::lang::RawScorerNode*>& scorerNodes, InferenceNetwork::MAllResults& statisticsResults ) {
  for( unsigned int i=0; i<scorerNodes.size(); i++ ) {
    std::vector<ScoredExtentResult>& occurrencesList = statisticsResults[ scorerNodes[i]->nodeName() ][ "occurrences" ];
    std::vector<ScoredExtentResult>& contextSizeList = statisticsResults[ scorerNodes[i]->nodeName() ][ "contextSize" ];

    std::vector<ScoredExtentResult>& maxOccurrencesList = statisticsResults[ scorerNodes[i]->nodeName() ][ "maxOccurrences" ];
    std::vector<ScoredExtentResult>& maxContextLengthList = statisticsResults[ scorerNodes[i]->nodeName() ][ "maxContextLength" ];
    std::vector<ScoredExtentResult>& minContextLengthList = statisticsResults[ scorerNodes[i]->nodeName() ][ "minContextLength" ];
    std::vector<ScoredExtentResult>& maxContextFractionList = statisticsResults[ scorerNodes[i]->nodeName() ][ "maxContextFraction" ];

    UINT64 occurrences = UINT64(occurrencesList[0].score);
    UINT64 contextSize = UINT64(contextSizeList[0].score);

    UINT64 maxOccurrences = UINT64(maxOccurrencesList[0].score);
    UINT64 maxContextLength = UINT64(maxContextLengthList[0].score);
    UINT64 minContextLength = UINT64(minContextLengthList[0].score);
    double maxContextFraction = maxContextFractionList[0].score;

    scorerNodes[i]->setStatistics( occurrences, contextSize,
                                   maxOccurrences,
                                   minContextLength,
                                   maxContextLength,
                                   maxContextFraction );
  }
}

//
// Runs a query in parallel across all servers, and returns a vector of responses.
// This method will block until all responses have been received.
//

std::vector<QueryServerResponse*> QueryEnvironment::_runServerQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested ) {
  std::vector<QueryServerResponse*> responses;
  
  // this ships out the requests to each server (doesn't necessarily block until they're done)
  for( unsigned int i=0; i<_servers.size(); i++ ) {
    QueryServerResponse* response = _servers[i]->runQuery( roots, resultsRequested, true );
    responses.push_back( response );
  }

  // this just goes through all the results, blocking on each one,
  // making sure they've all arrived
  for( unsigned int i=0; i<_servers.size(); i++ ) {
    responses[i]->getResults();
  }

  return responses;
}

// 
// This method is used for combining raw scores from ContextCounterNodes.
//

void QueryEnvironment::_sumServerQuery( InferenceNetwork::MAllResults& results, std::vector<indri::lang::Node*>& roots, int resultsRequested ) {
  std::vector<QueryServerResponse*> serverResults = _runServerQuery( roots, resultsRequested );
  results.clear();

  InferenceNetwork::MAllResults::iterator nodeIter;
  EvaluatorNode::MResults::iterator listIter;

  for( unsigned int i=0; i<serverResults.size(); i++ ) {
    QueryServerResponse* response = serverResults[i];
    InferenceNetwork::MAllResults& machineResults = response->getResults();
    InferenceNetwork::MAllResults::iterator iter;

    for( nodeIter = machineResults.begin(); nodeIter != machineResults.end(); nodeIter++ ) {
      for( listIter = nodeIter->second.begin(); listIter != nodeIter->second.end(); listIter++ ) {
        std::vector<ScoredExtentResult>& currentResultList = results[ nodeIter->first ][ listIter->first ];
        std::vector<ScoredExtentResult>& machineResultList = listIter->second;

        if( currentResultList.size() == 0 ) {
          currentResultList.assign( machineResultList.begin(), machineResultList.end() );
        } else {
          assert( machineResultList.size() == currentResultList.size() );

          for( unsigned int i=0; i<machineResultList.size(); i++ ) {
            currentResultList[i].score += machineResultList[i].score;
          }
        }
      }
    }
  }

  delete_vector_contents<QueryServerResponse*>( serverResults );
}

void QueryEnvironment::_mergeQueryResults( InferenceNetwork::MAllResults& results, std::vector<QueryServerResponse*>& responses ) {
  results.clear();

  InferenceNetwork::MAllResults::iterator nodeIter;
  EvaluatorNode::MResults::iterator listIter;

  // merge all the results from these machines into one master list
  for( unsigned int i=0; i<responses.size(); i++ ) {
    QueryServerResponse* response = responses[i];
    InferenceNetwork::MAllResults& machineResults = response->getResults();

    for( nodeIter = machineResults.begin(); nodeIter != machineResults.end(); nodeIter++ ) {
      EvaluatorNode::MResults& node = nodeIter->second;

      for( listIter = node.begin(); listIter != node.end(); listIter++ ) {
        const std::vector<ScoredExtentResult>& partialResultList = listIter->second;
        std::vector<ScoredExtentResult>& totalResultList = results[ nodeIter->first ][ listIter->first ];

        for( unsigned int j=0; j<partialResultList.size(); j++ ) {
          ScoredExtentResult singleResult = partialResultList[j];
          singleResult.document = (singleResult.document*int(_servers.size())) + i;
          totalResultList.push_back( singleResult );
        }
      }
    }
  }
}

//
// This method is used to merge document results from multiple servers.  It does this
// by reassigning document IDs with the following function:
//      serverCount = _servers.size();
//      cookedDocID = rawDocID * serverCount + docServer;
// So, for document 6 from server 3 (out of 7 servers), the cooked docID would be:
//      (6 * 7) + 3 = 45.
// This function has the nice property that if there is only one server running,
// cookedDocID == rawDocID.
//

void QueryEnvironment::_mergeServerQuery( InferenceNetwork::MAllResults& results, std::vector<indri::lang::Node*>& roots, int resultsRequested ) {
  std::vector<QueryServerResponse*> serverResults = _runServerQuery( roots, resultsRequested );
  results.clear();

  InferenceNetwork::MAllResults::iterator nodeIter;
  EvaluatorNode::MResults::iterator listIter;

  _mergeQueryResults( results, serverResults );

  // now, for each node, sort the result list, and trim off any results past the
  // requested amount
  for( nodeIter = results.begin(); nodeIter != results.end(); nodeIter++ ) {
    for( listIter = nodeIter->second.begin(); listIter != nodeIter->second.end(); listIter++ ) {
      std::vector<ScoredExtentResult>& listResults = listIter->second;
      std::sort( listResults.begin(), listResults.end(), ScoredExtentResult::score_greater() );

      if( int(listResults.size()) > resultsRequested )
        listResults.resize( resultsRequested );
    }
  }

  delete_vector_contents<QueryServerResponse*>( serverResults );
}

void QueryEnvironment::addIndex( const std::string& pathname ) {
  Repository* repository = new Repository();
  repository->openRead( pathname, &_parameters );

  _repositories.push_back( repository );
  _servers.push_back( new LocalQueryServer( *repository ) );
}

void QueryEnvironment::addServer( const std::string& hostname ) {
  NetworkStream* stream = new NetworkStream;
  unsigned int port = INDRID_PORT;
  std::string host = hostname;
  int colon = hostname.find(':');

  if( colon > 0 ) {
    host = hostname.substr( 0, colon );
    port = atoi( hostname.substr( colon+1 ).c_str() );
  }

  if( !stream->connect( host.c_str(), port ) ) {
    delete stream;
    throw Exception( "QueryEnvironment", "Failed to connect to server" );
  }

  _streams.push_back( stream );
  NetworkMessageStream* messageStream = new NetworkMessageStream( stream );
  NetworkServerProxy* proxy = new NetworkServerProxy( messageStream );

  _messageStreams.push_back( messageStream );
  _servers.push_back( proxy );
}

void QueryEnvironment::close() {
  delete_vector_contents<QueryServer*>( _servers );
  delete_vector_contents<NetworkMessageStream*>( _messageStreams );
  delete_vector_contents<NetworkStream*>( _streams );
  delete_vector_contents<Repository*>( _repositories );
}

std::vector<std::string> QueryEnvironment::documentMetadata( const std::vector<DOCID_T>& documentIDs, const std::string& attributeName ) {
  std::vector< std::vector<DOCID_T> > docIDLists;
  docIDLists.resize( _servers.size() );
  std::vector< std::vector<DOCID_T> > docIDPositions;
  docIDPositions.resize( _servers.size() );
  std::vector< std::string > results;
  results.resize( documentIDs.size() );

  // split document numbers into lists for each query server
  qenv_scatter_document_ids( documentIDs, docIDLists, docIDPositions, _servers.size() );

  greedy_vector<QueryServerMetadataResponse*> responses;

  // send out requests for execution
  for( unsigned int i=0; i<docIDLists.size(); i++ ) {
    QueryServerMetadataResponse* response = 0;
    
    if( docIDLists[i].size() )
      response = _servers[i]->documentMetadata( docIDLists[i], attributeName );
    
    responses.push_back(response);
  }

  // fold the results back into one master list (this method will delete the responses)
  qenv_gather_document_results( docIDLists, docIDPositions, responses, results );

  return results;
}

std::vector<std::string> QueryEnvironment::documentMetadata( const std::vector<ScoredExtentResult>& results, const std::string& attributeName ) {
  // copy into an int vector
  std::vector<DOCID_T> documentIDs;
  documentIDs.reserve(results.size());

  for( unsigned int i=0; i<results.size(); i++ ) {
    documentIDs.push_back( results[i].document );
  }

  return documentMetadata( documentIDs, attributeName );
}

std::vector<ParsedDocument*> QueryEnvironment::documents( const std::vector<DOCID_T>& documentIDs ) {
  std::vector< std::vector<DOCID_T> > docIDLists;
  std::vector< std::vector<DOCID_T> > docIDPositions;
  std::vector< ParsedDocument* > results;
  results.resize( documentIDs.size() );

  // split document numbers into lists for each query server
  qenv_scatter_document_ids( documentIDs, docIDLists, docIDPositions, _servers.size() );

  greedy_vector<QueryServerDocumentsResponse*> responses;

  // send out requests for processing
  for( unsigned int i=0; i<docIDLists.size(); i++ ) {
    QueryServerDocumentsResponse* response = 0;

    if( docIDLists[i].size() ) {
      response = _servers[i]->documents( docIDLists[i] );
    } 

    responses.push_back(response);
  }

  // fold the results back into one master list (this method will delete the responses)
  qenv_gather_document_results( docIDLists, docIDPositions, responses, results );

  return results;
}

// fetch the document names for a list of document IDs
std::vector<ParsedDocument*> QueryEnvironment::documents( const std::vector<ScoredExtentResult>& results ) {
  // copy into an int vector
  std::vector<DOCID_T> documentIDs;
  documentIDs.reserve(results.size());

  for( unsigned int i=0; i<results.size(); i++ ) {
    documentIDs.push_back( results[i].document );
  }

  return documents( documentIDs );
}

void QueryEnvironment::_scoredQuery( InferenceNetwork::MAllResults& results, indri::lang::Node* queryRoot, std::string& accumulatorName, int resultsRequested, const std::vector<DOCID_T>* documentSet ) {
  // add a FilterNode, unique to each server
  // send off each query for evaluation
  std::vector< std::vector<DOCID_T> > docIDLists;
  std::vector< std::vector<DOCID_T> > docIDPositions;

  // scatter the document IDs out to the servers
  if( documentSet )
    qenv_scatter_document_ids( *documentSet, docIDLists, docIDPositions, _servers.size() );

  accumulatorName = "";

  // For each server, make a FilterNode and an AccumulatorNode, then run the query.
  // The filter node makes sure that we only score the documents that are interesting
  // so we don't waste too much time here.  
  std::vector<indri::lang::Node*> nodes;
  VectorDeleter<indri::lang::Node*> nd(nodes);
  std::string filterName;

  std::vector<QueryServerResponse*> queryResponses;
  VectorDeleter<QueryServerResponse*> qd(queryResponses);

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    indri::lang::ScoredExtentNode* scoredRoot = dynamic_cast<indri::lang::ScoredExtentNode*>(queryRoot);
    indri::lang::FilterNode* filterNode = 0;
    indri::lang::ScoreAccumulatorNode* accumulatorNode = 0;
    
    if( documentSet ) {
      filterNode = new indri::lang::FilterNode( scoredRoot, docIDLists[i] );
      accumulatorNode = new indri::lang::ScoreAccumulatorNode( filterNode );
      nodes.push_back( filterNode );
      nodes.push_back( accumulatorNode );
    } else {
      accumulatorNode = new indri::lang::ScoreAccumulatorNode( scoredRoot );
      nodes.push_back( accumulatorNode );
    }

    // name all the nodes the same thing
    if( filterNode ) {
      if( filterName.length() > 0 )
        filterNode->setNodeName( filterName );
      else
        filterName = filterNode->nodeName();
    }

    if( accumulatorNode ) {
      if( accumulatorName.length() > 0 ) {
        accumulatorNode->setNodeName( accumulatorName );
      } else {
        accumulatorName = accumulatorNode->nodeName();
      }
    }

    std::vector<indri::lang::Node*> root;
    root.push_back( accumulatorNode );

    // don't optimize these queries, otherwise we won't be able to distinguish some annotations from others
    QueryServerResponse* response = _servers[i]->runQuery( root, resultsRequested, true );
    queryResponses.push_back(response);
  }

  // now, gather up all the responses, merge them into some kind of output structure, and return them
  _mergeQueryResults( results, queryResponses );
}

void QueryEnvironment::_annotateQuery( InferenceNetwork::MAllResults& results,
                                       const std::vector<DOCID_T>& documentSet,
                                       std::string& annotatorName,
                                       indri::lang::Node* queryRoot ) {
  // add a FilterNode, unique to each server
  // send off each query for evaluation
  std::vector< std::vector<DOCID_T> > docIDLists;
  std::vector< std::vector<DOCID_T> > docIDPositions;

  // scatter the document IDs out to the servers
  qenv_scatter_document_ids( documentSet, docIDLists, docIDPositions, _servers.size() );

  // For each server, make a FilterNode and an AnnotatorNode, then run the query.
  // The filter node makes sure that we only annotate the documents that are interesting
  // so we don't waste too much time here.  The AnnotatorNode collects the annotations.
  std::vector<indri::lang::FilterNode*> filterNodes;
  std::vector<indri::lang::AnnotatorNode*> annotatorNodes;
  std::vector<QueryServerResponse*> queryResponses;
  VectorDeleter<indri::lang::FilterNode*> fd(filterNodes);
  VectorDeleter<indri::lang::AnnotatorNode*> ad(annotatorNodes);
  VectorDeleter<QueryServerResponse*> qd(queryResponses);

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    indri::lang::ScoredExtentNode* scoredRoot = dynamic_cast<indri::lang::ScoredExtentNode*>(queryRoot);
    indri::lang::FilterNode* filterNode = new indri::lang::FilterNode( scoredRoot, docIDLists[i] );
    indri::lang::AnnotatorNode* annotatorNode = new indri::lang::AnnotatorNode( filterNode );

    // name all the nodes the same thing
    if( i != 0 ) {
      filterNode->setNodeName( filterNodes[0]->nodeName() );
      annotatorNode->setNodeName( annotatorNodes[0]->nodeName() );
    }

    filterNodes.push_back( filterNode );
    annotatorNodes.push_back( annotatorNode );

    std::vector<indri::lang::Node*> root;
    root.push_back( annotatorNode );

    // don't optimize these queries, otherwise we won't be able to distinguish some annotations from others
    QueryServerResponse* response = _servers[i]->runQuery( root, docIDLists[i].size(), false );
    queryResponses.push_back(response);
  }

  // now, gather up all the responses, merge them into some kind of output structure, and return them
  _mergeQueryResults( results, queryResponses );
  
  if( annotatorNodes.size() )
    annotatorName = annotatorNodes[0]->nodeName();
}


// run a query (Indri query language)
std::vector<ScoredExtentResult> QueryEnvironment::_runQuery( InferenceNetwork::MAllResults& results,
                                                             const std::string& q,
                                                             int resultsRequested,
                                                             const std::vector<DOCID_T>* documentSet,
                                                             QueryAnnotation** annotation ) {
  INIT_TIMER

  std::istringstream query(q);
  indri::lang::QueryLexer lexer( query );
  indri::lang::QueryParser parser( lexer );
  
  // this step is required to initialize some internal
  // parser variables, since ANTLR grammars can't add things
  // to the constructor
  parser.init( &_priorFactory, &lexer );
  lexer.init();

  PRINT_TIMER( "Initialization complete" );

  indri::lang::ScoredExtentNode* rootNode;

  try {
    rootNode = parser.query();
  } catch( antlr::ANTLRException exception ) {
    LEMUR_THROW( LEMUR_PARSE_ERROR, "Couldn't understand this query: " + exception.toString() );
  }
  
  PRINT_TIMER( "Parsing complete" );

  // push down language models from ExtentRestriction nodes
  ExtentRestrictionModelAnnotatorCopier restrictionCopier;
  rootNode = dynamic_cast<indri::lang::ScoredExtentNode*>(rootNode->copy(restrictionCopier));
  
  // extract the raw scorer nodes from the query tree
  RawScorerNodeExtractor extractor;
  rootNode->walk(extractor);

  // copy out a new graph that has context counters in it -- this will be evaluated
  // so that we can get counts for everything in the query.  We need those counts
  // so that we can score the query terms correctly.
  std::vector<indri::lang::RawScorerNode*>& scorerNodes = extractor.getScorerNodes();
  ApplyCopiers<ContextCountGraphCopier, indri::lang::RawScorerNode> graph( scorerNodes );

  InferenceNetwork::MAllResults statisticsResults;
  _sumServerQuery( statisticsResults, graph.roots(), 1000 );

  PRINT_TIMER( "Statistics complete" );

  // feed the statistics we found back into the query network
  _copyStatistics( scorerNodes, statisticsResults );

  // annotate the graph with smoothing parameters
  SmoothingAnnotatorWalker smoother( _parameters );
  rootNode->walk(smoother);

  // run a scored query (possibly including a document set)
  std::string accumulatorName;
  _scoredQuery( results, rootNode, accumulatorName, resultsRequested, documentSet );
  std::vector<ScoredExtentResult> queryResults = results[accumulatorName]["scores"];
  std::sort( queryResults.begin(), queryResults.end() );
  if( queryResults.size() > resultsRequested )
    queryResults.resize( resultsRequested );

  PRINT_TIMER( "Query complete" );

  if( annotation ) {
    std::string annotatorName;
    std::vector<DOCID_T> documentSet;

    for( size_t i=0; i<queryResults.size(); i++ ) {
      documentSet.push_back( queryResults[i].document );
    }

    _annotateQuery( results, documentSet, annotatorName, rootNode );
    *annotation = new QueryAnnotation( rootNode, results[annotatorName], queryResults );
  }

  PRINT_TIMER( "Annotation complete" );

  return queryResults;
}

std::vector<ScoredExtentResult> QueryEnvironment::runQuery( const std::string& query, int resultsRequested ) {
  InferenceNetwork::MAllResults results;
  std::vector<ScoredExtentResult> queryResult = _runQuery( results, query, resultsRequested, 0, 0 );
  return queryResult;
}

std::vector<ScoredExtentResult> QueryEnvironment::runQuery( const std::string& query, const std::vector<DOCID_T>& documentSet, int resultsRequested ) {
  InferenceNetwork::MAllResults results;
  std::vector<ScoredExtentResult> queryResult = _runQuery( results, query, resultsRequested, &documentSet, 0 );
  return queryResult;
}

QueryAnnotation* QueryEnvironment::runAnnotatedQuery( const std::string& query, int resultsRequested ) {
  InferenceNetwork::MAllResults results;
  QueryAnnotation* annotation = 0;
  
  _runQuery( results, query, resultsRequested, 0, &annotation );
  return annotation;
}

QueryAnnotation* QueryEnvironment::runAnnotatedQuery( const std::string& query, const std::vector<DOCID_T>& documentSet, int resultsRequested ) {
  InferenceNetwork::MAllResults results;
  QueryAnnotation* annotation = 0;
  
  _runQuery( results, query, resultsRequested, &documentSet, &annotation );
  return annotation;
}

//
// Term counts
//

INT64 QueryEnvironment::termCount() {
  // note that we should probably send these requests asynchronously
  INT64 totalTermCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalTermCount += _servers[i]->termCount();
  }

  return totalTermCount;
}

INT64 QueryEnvironment::termCount( const std::string& term ) {
  // note that we should probably send these requests asynchronously
  INT64 totalTermCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalTermCount += _servers[i]->termCount( term );
  }

  return totalTermCount;
}

INT64 QueryEnvironment::stemCount( const std::string& stem ) {
  // note that we should probably send these requests asynchronously
  INT64 totalTermCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalTermCount += _servers[i]->stemCount( stem );
  }

  return totalTermCount;
}

// 
// Field counts
//

INT64 QueryEnvironment::termFieldCount( const std::string& term, const std::string& field ) {
  INT64 totalTermCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalTermCount += _servers[i]->termFieldCount( term, field );
  }

  return totalTermCount;
}

INT64 QueryEnvironment::stemFieldCount( const std::string& stem, const std::string& field ) {
  INT64 totalTermCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalTermCount += _servers[i]->stemFieldCount( stem, field );
  }

  return totalTermCount;
}

//
// Field list
//

std::vector<std::string> QueryEnvironment::fieldList() {
  std::vector<std::string> result;
  std::vector<std::string> machineResult;
  std::set<std::string> fieldSet;
  
  for( unsigned int i=0; i<_servers.size(); i++ ) {
    machineResult = _servers[i]->fieldList();

    for( unsigned int j=0; j<machineResult.size(); j++ ) {
      fieldSet.insert( machineResult[j] );
    }
  }

  std::set<std::string>::iterator iter;

  for( iter = fieldSet.begin(); iter != fieldSet.end(); iter++ ) {
    result.push_back( *iter );
  }
  
  return result;
}

//
// Documents
//

INT64 QueryEnvironment::documentCount() {
  INT64 totalDocumentCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalDocumentCount += _servers[i]->documentCount();
  }

  return totalDocumentCount;
}

INT64 QueryEnvironment::documentCount( const std::string& term ) {
  INT64 totalDocumentCount = 0;

  for( unsigned int i=0; i<_servers.size(); i++ ) {
    totalDocumentCount += _servers[i]->documentCount( term );
  }

  return totalDocumentCount;
}

std::vector<DocumentVector*> QueryEnvironment::documentVectors( const std::vector<DOCID_T>& documentIDs ) {
  std::vector< std::vector<DOCID_T> > docIDLists;
  docIDLists.resize( _servers.size() );
  std::vector< std::vector<DOCID_T> > docIDPositions;
  docIDPositions.resize( _servers.size() );
  std::vector< DocumentVector* > results;
  results.resize( documentIDs.size() );

  // split document numbers into lists for each query server
  qenv_scatter_document_ids( documentIDs, docIDLists, docIDPositions, _servers.size() );

  greedy_vector<QueryServerVectorsResponse*> responses;

  // send out requests for processing
  for( unsigned int i=0; i<docIDLists.size(); i++ ) {
    QueryServerVectorsResponse* response = 0;

    if( docIDLists[i].size() )
      response = _servers[i]->documentVectors( docIDLists[i] );
    
    responses.push_back(response);
  }

  // fold the results back into one master list (this method will delete the responses)
  qenv_gather_document_results( docIDLists, docIDPositions, responses, results );

  return results;
}

