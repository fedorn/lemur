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
// LocalQueryServer
//
// 15 March 2004 -- tds
//

#include "indri/LocalQueryServer.hpp"
#include "indri/QuerySpec.hpp"
#include "lemur-platform.h"
#include "lemur-compat.hpp"
#include <vector>

#include "indri/UnnecessaryNodeRemoverCopier.hpp"
#include "indri/ContextSimpleCountCollectorCopier.hpp"
#include "indri/FrequencyListCopier.hpp"
#include "indri/DagCopier.hpp"

#include "indri/InferenceNetworkBuilder.hpp"
#include "indri/InferenceNetwork.hpp"

#include "indri/CompressedCollection.hpp"
#include "indri/delete_range.hpp"
#include "indri/WeightFoldingCopier.hpp"

#include "indri/Appliers.hpp"

//
// Response objects
//

class LocalQueryServerResponse : public QueryServerResponse {
private:
  InferenceNetwork::MAllResults _results;

public:
  LocalQueryServerResponse( const InferenceNetwork::MAllResults& results ) :
    _results(results) {
  }
  
  InferenceNetwork::MAllResults& getResults() {
    return _results;
  }
};

class LocalQueryServerDocumentsResponse : public QueryServerDocumentsResponse {
private:
  std::vector<ParsedDocument*> _documents;

public:
  LocalQueryServerDocumentsResponse( const std::vector<ParsedDocument*>& results )
    :
    _documents(results)
  {
  }

  // caller's responsibility to delete these results
  std::vector<ParsedDocument*>& getResults() {
    return _documents;
  }
};

class LocalQueryServerMetadataResponse : public QueryServerMetadataResponse {
private:
  std::vector<std::string> _metadata;

public:
  LocalQueryServerMetadataResponse( const std::vector<std::string>& metadata ) :
    _metadata(metadata)
  {
  }

  std::vector<std::string>& getResults() {
    return _metadata;
  }
};

class LocalQueryServerVectorsResponse : public QueryServerVectorsResponse {
private:
  std::vector<DocumentVector*> _vectors;

public:
  LocalQueryServerVectorsResponse( int vectorCount ) {
    _vectors.reserve( vectorCount );
  }

  void addVector( DocumentVector* vec ) {
    _vectors.push_back( vec );
  }

  // caller deletes DocumentVector objects
  std::vector<DocumentVector*>& getResults() {
    return _vectors;
  }
};

//
// Class code
//

LocalQueryServer::LocalQueryServer( Repository& repository ) :
  _repository(repository)
{
}

ParsedDocument* LocalQueryServer::document( int documentID ) {
  CompressedCollection* collection = _repository.collection();
  ParsedDocument* document = collection->retrieve( documentID );
  return document;
}

std::string LocalQueryServer::documentMetadatum( int documentID, const std::string& attributeName ) {
  CompressedCollection* collection = _repository.collection();
  return collection->retrieveMetadatum( documentID, attributeName );
}

QueryServerMetadataResponse* LocalQueryServer::documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName ) {
  std::vector<std::string> result;

  std::vector<std::pair<int, int> > docSorted;
  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    docSorted.push_back( std::make_pair( documentIDs[i], i ) );
  }
  std::sort( docSorted.begin(), docSorted.end() );

  for( unsigned int i=0; i<docSorted.size(); i++ ) {
    result.push_back( documentMetadatum(docSorted[i].first, attributeName) );
  }

  std::vector<std::string> actual;
  actual.resize( documentIDs.size() );
  for( unsigned int i=0; i<docSorted.size(); i++ ) {
    actual[docSorted[i].second] = result[i];
  }

  return new LocalQueryServerMetadataResponse( actual );
}

QueryServerDocumentsResponse* LocalQueryServer::documents( const std::vector<int>& documentIDs ) {
  std::vector<ParsedDocument*> result;
  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    result.push_back( document(documentIDs[i]) );
  }
  return new LocalQueryServerDocumentsResponse( result );
}

INT64 LocalQueryServer::termCount() {
  IndriIndex* index = _repository.index();
  return index->termCount();
}

INT64 LocalQueryServer::termCount( int term ) {
  IndriIndex* index = _repository.index();
  return index->termCount( term );
}

INT64 LocalQueryServer::termCount( const std::string& term ) {
  std::string stem = _repository.processTerm( term );
  // stopwords return a string of length 0, causing Keyfile to throw.
  if( stem.length() != 0 ) {
    return stemCount( stem );
  } else {
    return 0;
  }
}

INT64 LocalQueryServer::stemCount( const std::string& stem ) {
  IndriIndex* index = _repository.index();
  int termID = index->term( stem.c_str() );
  return termCount(termID);
}

INT64 LocalQueryServer::termFieldCount( int term, const std::string& field ) {
  IndriIndex* index = _repository.index();
  int fieldid = index->field( field.c_str() );

  return index->fieldTermCount( term, fieldid );
}

INT64 LocalQueryServer::termFieldCount( const std::string& term, const std::string& field ) {
  std::string stem = _repository.processTerm( term );
  // stopwords return a string of length 0, causing Keyfile to throw.
  if( stem.length() != 0 ) {
    return stemFieldCount( stem, field );
  } else {
    return 0;
  }
}

INT64 LocalQueryServer::stemFieldCount( const std::string& stem, const std::string& field ) {
  IndriIndex* index = _repository.index();
  int termid = index->term( stem.c_str() );
  return termFieldCount( termid, field );
}

std::string LocalQueryServer::termName( int term ) {
  IndriIndex* index = _repository.index();
  return index->term( term );
}

int LocalQueryServer::termID( const std::string& term ) {
  IndriIndex* index = _repository.index();
  std::string processed = _repository.processTerm( term );
  // stopwords return a string of length 0, causing Keyfile to throw.
  if( processed.length() != 0 ) {
    return index->term( processed.c_str() );
  } else {
    return 0;
  }
}

std::vector<std::string> LocalQueryServer::fieldList() {
  IndriIndex* index = _repository.index();
  std::vector<std::string> results;

  for( unsigned int i=1; ; i++ ) {
    const char* fieldName = index->field(i);
    
    if( !strcmp( fieldName, "[OOV]" ) )
      break;

    results.push_back( std::string(fieldName) );
  }

  return results;
}

int LocalQueryServer::documentLength( int documentID ) {
  IndriIndex* index = _repository.index();
  return index->docLength( documentID );
}

INT64 LocalQueryServer::documentCount() {
  IndriIndex* index = _repository.index();
  return index->docCount();
}

INT64 LocalQueryServer::documentCount( const std::string& term ) {
  IndriIndex* index = _repository.index();
  int termid = this->termID( term );
  return index->docCount( termid );
}

QueryServerResponse* LocalQueryServer::runQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested, bool optimize ) {
  // use UnnecessaryNodeRemover to get rid of window nodes, ExtentAnd nodes and ExtentOr nodes
  // that only have one child
  ApplyCopiers<UnnecessaryNodeRemoverCopier> unnecessary( roots );

  // run the contextsimplecountcollectorcopier to gather easy stats
  ApplyCopiers<ContextSimpleCountCollectorCopier> contexts( unnecessary.roots(), _repository, _cache );

  // use frequency-only nodes where appropriate
  ApplyCopiers<FrequencyListCopier> frequency( contexts.roots(), _cache );

  // fold together any nested weight nodes
  ApplyCopiers<WeightFoldingCopier> weight( frequency.roots() );

  // make all this into a dag
  ApplySingleCopier<DagCopier> dag( weight.roots(), _repository );

  std::vector<indri::lang::Node*>& networkRoots = dag.roots();

  if( !optimize ) {
    // we may be asked not to perform optimizations that might
    // drastically change the structure of the tree; for instance,
    // annotation queries may ask for this
    networkRoots = contexts.roots();
  }

  // build an inference network
  InferenceNetworkBuilder builder( _repository, _cache, resultsRequested );
  ApplyWalker<InferenceNetworkBuilder> buildWalker( networkRoots, &builder );

  InferenceNetwork* network = builder.getNetwork();
  InferenceNetwork::MAllResults result;
  result = network->evaluate();

  return new LocalQueryServerResponse( result );
}

QueryServerVectorsResponse* LocalQueryServer::documentVectors( const std::vector<int>& documentIDs ) {
  LocalQueryServerVectorsResponse* response = new LocalQueryServerVectorsResponse( documentIDs.size() );
  std::map<int, std::string> termIDStringMap;

  for( size_t i=0; i<documentIDs.size(); i++ ) {
    indri::index::TermListBuilder* termList = _repository.index()->termPositionList( documentIDs[i] );
    DocumentVector* result = new DocumentVector( _repository.index(), termList, termIDStringMap );
    delete termList;
    response->addVector( result );
  }

  return response;
}
