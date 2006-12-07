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
#include "indri/WildcardNodeCopier.hpp"

#include "indri/Appliers.hpp"
#include "indri/ScopedLock.hpp"

#include "indri/TreePrinterWalker.hpp"

#include "indri/DocumentStructure.hpp"

//
// Response objects
//
namespace indri
{
  namespace server
  {
    
    class LocalQueryServerResponse : public QueryServerResponse {
    private:
      indri::infnet::InferenceNetwork::MAllResults _results;

    public:
      LocalQueryServerResponse( const indri::infnet::InferenceNetwork::MAllResults& results ) :
        _results(results) {
      }
  
      indri::infnet::InferenceNetwork::MAllResults& getResults() {
        return _results;
      }
    };

    class LocalQueryServerDocumentsResponse : public QueryServerDocumentsResponse {
    private:
      std::vector<indri::api::ParsedDocument*> _documents;

    public:
      LocalQueryServerDocumentsResponse( const std::vector<indri::api::ParsedDocument*>& results )
        :
        _documents(results)
      {
      }

      // caller's responsibility to delete these results
      std::vector<indri::api::ParsedDocument*>& getResults() {
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
      std::vector<indri::api::DocumentVector*> _vectors;

    public:
      LocalQueryServerVectorsResponse( int vectorCount ) {
        _vectors.reserve( vectorCount );
      }

      void addVector( indri::api::DocumentVector* vec ) {
        _vectors.push_back( vec );
      }

      // caller deletes indri::api::DocumentVector objects
      std::vector<indri::api::DocumentVector*>& getResults() {
        return _vectors;
      }
    };

    class LocalQueryServerDocumentIDsResponse : public QueryServerDocumentIDsResponse {
    private:
      std::vector<lemur::api::DOCID_T> _documentIDs;

    public:
      LocalQueryServerDocumentIDsResponse( const std::vector<lemur::api::DOCID_T>& documents ) : 
        _documentIDs(documents)
      {
      }

      std::vector<lemur::api::DOCID_T>& getResults() {
        return _documentIDs;
      }
    };
  }
}

//
// Class code
//

indri::server::LocalQueryServer::LocalQueryServer( indri::collection::Repository& repository ) :
  _repository(repository), _maxWildcardMatchesPerTerm(indri::index::DEFAULT_MAX_WILDCARD_TERMS)
{
  // if supplied and false, turn off optimization for all queries.
  _optimizeParameter = indri::api::Parameters::instance().get( "optimize", 1 );

	// get any parameters - right now, its just the max. # of wildcard terms
	indri::collection::Repository::index_state iState=_repository.indexes();
	indri::collection::Repository::index_vector::iterator iIter=iState->begin();
	while (iIter!=iState->end()) {
		indri::index::Index *thisIndex=*iIter;
		if (_maxWildcardMatchesPerTerm > thisIndex->maxWildcardTermCount()) {
			_maxWildcardMatchesPerTerm = thisIndex->maxWildcardTermCount();
		}
		iIter++;
	}
}

//
// _indexWithDocument
//

indri::index::Index* indri::server::LocalQueryServer::_indexWithDocument( indri::collection::Repository::index_state& indexes, int documentID ) {
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    int lowerBound = (*indexes)[i]->documentBase();
    int upperBound = (*indexes)[i]->documentBase() + (*indexes)[i]->documentCount();
    
    if( lowerBound <= documentID && upperBound > documentID ) {
      return (*indexes)[i];
    }
  }
  
  return 0;
}

//
// document
//

indri::api::ParsedDocument* indri::server::LocalQueryServer::document( int documentID ) {
  indri::collection::CompressedCollection* collection = _repository.collection();
  indri::api::ParsedDocument* document = collection->retrieve( documentID );
  return document;
}

std::string indri::server::LocalQueryServer::documentMetadatum( int documentID, const std::string& attributeName ) {
  indri::collection::CompressedCollection* collection = _repository.collection();
  return collection->retrieveMetadatum( documentID, attributeName );
}

indri::server::QueryServerMetadataResponse* indri::server::LocalQueryServer::documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName ) {
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

  return new indri::server::LocalQueryServerMetadataResponse( actual );
}

indri::server::QueryServerDocumentsResponse* indri::server::LocalQueryServer::documents( const std::vector<int>& documentIDs ) {
  std::vector<indri::api::ParsedDocument*> result;
  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    result.push_back( document(documentIDs[i]) );
  }
  return new indri::server::LocalQueryServerDocumentsResponse( result );
}

indri::server::QueryServerDocumentsResponse* indri::server::LocalQueryServer::documentsFromMetadata( const std::string& attributeName, const std::vector<std::string>& attributeValues ) {
  indri::collection::CompressedCollection* collection = _repository.collection();
  std::vector<indri::api::ParsedDocument*> result;
  
  for( unsigned int i=0; i<attributeValues.size(); i++ ) {
    std::vector<indri::api::ParsedDocument*> documents = collection->retrieveByMetadatum( attributeName, attributeValues[i] );
    std::copy( documents.begin(), documents.end(), std::back_inserter( result ) );
  }

  return new indri::server::LocalQueryServerDocumentsResponse( result );
}

indri::server::QueryServerDocumentIDsResponse* indri::server::LocalQueryServer::documentIDsFromMetadata( const std::string& attributeName, const std::vector<std::string>& attributeValues ) {
  indri::collection::CompressedCollection* collection = _repository.collection();
  std::vector<lemur::api::DOCID_T> result;
  
  for( unsigned int i=0; i<attributeValues.size(); i++ ) {
    std::vector<lemur::api::DOCID_T> documents = collection->retrieveIDByMetadatum( attributeName, attributeValues[i] );
    std::copy( documents.begin(), documents.end(), std::back_inserter( result ) );
  }

  return new indri::server::LocalQueryServerDocumentIDsResponse( result );
}

INT64 indri::server::LocalQueryServer::termCount() {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  INT64 total = 0;

  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->termCount();
  }

  return total;
}

INT64 indri::server::LocalQueryServer::termCount( const std::string& term ) {
  std::string stem = _repository.processTerm( term );
  // stopwords return a string of length 0, causing Keyfile to throw.
  if( stem.length() != 0 ) {
    return stemCount( stem );
  } else {
    return 0;
  }
}

INT64 indri::server::LocalQueryServer::stemCount( const std::string& stem ) {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  INT64 total = 0;

  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->termCount( stem );
  }

  return total;
}

INT64 indri::server::LocalQueryServer::termFieldCount( const std::string& term, const std::string& field ) {
  std::string stem = _repository.processTerm( term );

  if( stem.length() != 0 ) {
    return stemFieldCount( stem, field );
  } else {
    return 0;
  }
}

INT64 indri::server::LocalQueryServer::stemFieldCount( const std::string& stem, const std::string& field ) {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  INT64 total = 0;

  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->fieldTermCount( field, stem );
  }

  return total;
}

std::string indri::server::LocalQueryServer::termName( int term ) {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  indri::index::Index* index = (*indexes)[0];
  indri::thread::ScopedLock lock( index->statisticsLock() );
  return index->term( term );
}

int indri::server::LocalQueryServer::termID( const std::string& term ) {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  indri::index::Index* index = (*indexes)[0];
  std::string processed = _repository.processTerm( term );
  indri::thread::ScopedLock lock( index->statisticsLock() );

  if( processed.length() != 0 ) {
    return index->term( processed.c_str() );
  } else {
    return 0;
  }
}

std::vector<std::string> indri::server::LocalQueryServer::fieldList() {
  std::vector<std::string> result;
  const std::vector<indri::collection::Repository::Field>& fields = _repository.fields();

  for( int i=0; i<fields.size(); i++ ) {
    result.push_back( fields[i].name );
  }

  return result;
}

int indri::server::LocalQueryServer::documentLength( int documentID ) {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  indri::index::Index* index = _indexWithDocument( indexes, documentID );

  if( index ) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    return index->documentLength( documentID );
  }

  return 0;
}

INT64 indri::server::LocalQueryServer::documentCount() {
  indri::collection::Repository::index_state indexes = _repository.indexes();
  INT64 total = 0;
  
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->documentCount();
  }
  
  return total;
}

INT64 indri::server::LocalQueryServer::documentCount( const std::string& term ) {
  std::string stem = _repository.processTerm( term );
  indri::collection::Repository::index_state indexes = _repository.indexes();
  INT64 total = 0;
  if( stem.length() == 0 ) return total;
  
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    //    total += (*indexes)[i]->documentCount( term );
    total += (*indexes)[i]->documentCount( stem );
  }
  
  return total;
}

indri::server::QueryServerResponse* indri::server::LocalQueryServer::runQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested, bool optimize ) {

  indri::lang::TreePrinterWalker printer;

	// here, walk through the parse tree and get any 
	// potential wildcard nodes and transform them into synonym lists
	// default=100, 2nd parameter - need to get dynamically...
	if (_maxWildcardMatchesPerTerm < 0) {
		// if it's less than 0, reset it to the default.
		_maxWildcardMatchesPerTerm=indri::index::DEFAULT_MAX_WILDCARD_TERMS;
	}

	std::vector<indri::lang::Node*> wildcardTransformRoots;

	// apply the wildcard copier...
	for (std::vector<indri::lang::Node*>::iterator rIter=roots.begin(); rIter!=roots.end(); rIter++) {
		indri::lang::Node* transformedRootNode=findAndTransformWildcardNodes(*rIter);
		wildcardTransformRoots.push_back(transformedRootNode);
	}

  // use UnnecessaryNodeRemover to get rid of window nodes, ExtentAnd nodes and ExtentOr nodes
  // that only have one child and LengthPrior nodes where the exponent is zero
  indri::lang::ApplyCopiers<indri::lang::UnnecessaryNodeRemoverCopier> unnecessary( wildcardTransformRoots );

  // run the contextsimplecountcollectorcopier to gather easy stats
  indri::lang::ApplyCopiers<indri::lang::ContextSimpleCountCollectorCopier> contexts( unnecessary.roots(), _repository );

  // use frequency-only nodes where appropriate
  indri::lang::ApplyCopiers<indri::lang::FrequencyListCopier> frequency( contexts.roots(), _cache );

  // fold together any nested weight nodes
  indri::lang::ApplyCopiers<indri::lang::WeightFoldingCopier> weight( frequency.roots() );

  // make all this into a dag
  indri::lang::ApplySingleCopier<indri::lang::DagCopier> dag( weight.roots(), _repository );

  std::vector<indri::lang::Node*>& networkRoots = dag.roots();
  // turn off optimization if called with optimize == false
  // turn off optimization if called the Parameter optimize == false
  if( !optimize || !_optimizeParameter ) {
    // we may be asked not to perform optimizations that might
    // drastically change the structure of the tree; for instance,
    // annotation queries may ask for this
    networkRoots = contexts.roots();
  }
  /*
  indri::lang::TreePrinterWalker printer;
  indri::lang::ApplyWalker<indri::lang::TreePrinterWalker> printTree(networkRoots, &printer);
  */

  // build an inference network
  indri::infnet::InferenceNetworkBuilder builder( _repository, _cache, resultsRequested );
  indri::lang::ApplyWalker<indri::infnet::InferenceNetworkBuilder> buildWalker( networkRoots, &builder );

  indri::infnet::InferenceNetwork* network = builder.getNetwork();
  indri::infnet::InferenceNetwork::MAllResults result;
  result = network->evaluate();

  return new indri::server::LocalQueryServerResponse( result );
}

indri::server::QueryServerVectorsResponse* indri::server::LocalQueryServer::documentVectors( const std::vector<int>& documentIDs ) {
  indri::server::LocalQueryServerVectorsResponse* response = new indri::server::LocalQueryServerVectorsResponse( documentIDs.size() );
  indri::collection::Repository::index_state indexes = _repository.indexes();
  std::map<int, std::string> termIDStringMap;

  for( size_t i=0; i<documentIDs.size(); i++ ) {
    indri::index::Index* index = _indexWithDocument( indexes, documentIDs[i] );

    {
      indri::thread::ScopedLock lock( index->statisticsLock() );
  
      const indri::index::TermList* termList = index->termList( documentIDs[i] );
      indri::api::DocumentVector* result = new indri::api::DocumentVector( index, termList, termIDStringMap );
      delete termList;
      response->addVector( result );
    }
  }

  return response;
}

indri::server::QueryServerMetadataResponse* indri::server::LocalQueryServer::pathNames( const std::vector<int>& documentIDs, const std::vector<int>& pathBegins, const std::vector<int>& pathEnds ) {

  int lastDoc = 0;
  indri::index::DocumentStructure docStruct;
  std::vector<std::string> result;

  std::vector<std::pair<int, int> > docSorted;
  for( unsigned int i=0; i<documentIDs.size(); i++ ) {
    docSorted.push_back( std::make_pair( documentIDs[i], i ) );
  }
  std::sort( docSorted.begin(), docSorted.end() );

  for( unsigned int i=0; i<docSorted.size(); i++ ) {
    indri::collection::Repository::index_state indexes = _repository.indexes();
    bool docStructLoaded = true;
    int documentID = docSorted[i].first;
    if ( documentID != lastDoc ) {
      indri::index::Index * index = _indexWithDocument(indexes, documentID);
      const indri::index::TermList * termList = index->termList( documentID );
      if ( termList != 0 ) {
        docStruct.setIndex( *index );
        docStruct.loadStructure( termList->fields() );
        delete termList;
        lastDoc = docStructLoaded;
      } else {
        docStructLoaded = false;
      }       
    }

    std::string path = "";
    if ( docStructLoaded ) {
      path = docStruct.path( docStruct.findLeaf( pathBegins[docSorted[i].second], 
                                                 pathEnds[docSorted[i].second] ) );
    }
    result.push_back( path );
  }

  std::vector<std::string> actual;
  actual.resize( documentIDs.size() );
  for( unsigned int i=0; i<docSorted.size(); i++ ) {
    actual[docSorted[i].second] = result[i];
  }

  return new indri::server::LocalQueryServerMetadataResponse( actual );
}

// mhoy - added 10/23/06 for wildcard support

//
// findAndTransformWildcardNodes
//
//
// finds any potential wildcard nodes and transforms them into synonym lists
// @param currentNode the current node to start with (for recursion)
// @param maxItemsPerNode the maximum number of synonyms that can be generated before an exception is thrown
//
indri::lang::Node* indri::server::LocalQueryServer::findAndTransformWildcardNodes(indri::lang::Node* currentNode) {
	if (!currentNode) return currentNode;
	if (_maxWildcardMatchesPerTerm < 0) return currentNode;

	// use a copier to transform the wildcard nodes to #syn() nodes with 
	indri::lang::WildcardNodeCopier wildcardCopier(this);
	return (currentNode->copy(wildcardCopier));
}

//
// getWildcardTermList
//
// Gets a list of terms from the opened index(es) that match the wildcardTerm and
// returns them as a vector
//
std::vector<std::string> indri::server::LocalQueryServer::getWildcardTermList(std::string wildcardTerm, int maxTermsToGet) {
	std::vector<std::string> retVec;

	// if there's no set amount of max terms to get
	// set the default
	if (maxTermsToGet==0) {
		maxTermsToGet=_maxWildcardMatchesPerTerm;
	}

	// first - ensure there's a wildcard in the term...
	std::string::size_type wildcardPos=wildcardTerm.find("*");
	if (wildcardPos==std::string::npos) {
		// no wildcard character? just return the term.
		retVec.push_back(wildcardTerm);
		return retVec;
	}

	std::string theTerm=wildcardTerm.substr(0, wildcardPos);
	const char *theTermChar=theTerm.c_str();
	// why is there a copy here?
	// const char *termCharCopy=strdup(theTermChar);
  
	indri::collection::Repository::index_state state = _repository.indexes();

	// get the index.
	indri::index::Index* index = (*state)[0];

	// get the vocab. iterator
	indri::index::VocabularyIterator* iter = index->vocabularyIterator();
	iter->startIteration();

	// get the next entry that corresponds to our term
	while (iter->nextEntry(theTermChar)) {
		// get the term
		indri::index::DiskTermData* entry = iter->currentEntry();
		if (entry) {
			indri::index::TermData* termData = entry->termData;

			if (strstr(termData->term,theTermChar)==termData->term) {
				if (retVec.size()==maxTermsToGet) {
					char maxTermExString[256];
					sprintf(maxTermExString, "Error in parsing wildcard terms. Too many terms matched %s. Limit is %d.", wildcardTerm.c_str(), maxTermsToGet);
					LEMUR_THROW( LEMUR_PARSE_ERROR, maxTermExString);
				} // end if (retVec.size()==maxTermsToGet)

				retVec.push_back(termData->term);
			} // end if (strstr(termData->term,theTermChar)==termData->term)
		} // end if (entry)
	} // end while (iter->nextEntry(theTermChar))

	// delete the iterator!
	// will cause thread locking issues in pthreads
	// for DiskKeyfileVocabIter if the mutex is not released...
	delete iter;

	// if (termCharCopy)	delete termCharCopy;

	return retVec;
}

//
// setMaxWildcardTerms
//
void indri::server::LocalQueryServer::setMaxWildcardTerms(int maxTerms) {
	_maxWildcardMatchesPerTerm = maxTerms;
}
