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
// InferenceNetworkBuilder
//
// 8 March 2004 -- tds
//

#include "indri/InferenceNetworkBuilder.hpp"

#include "indri/ContextCountAccumulator.hpp"
#include "indri/DocListIteratorNode.hpp"
#include "indri/ExtentInsideNode.hpp"
#include "indri/ExtentAndNode.hpp"
#include "indri/ExtentOrNode.hpp"
#include "indri/OrderedWindowNode.hpp"
#include "indri/UnorderedWindowNode.hpp"
#include "indri/FieldIteratorNode.hpp"
#include "indri/ListBeliefNode.hpp"
#include "indri/ScoredExtentAccumulator.hpp"
#include "indri/WeightedAndNode.hpp"
#include "indri/NullScorerNode.hpp"
#include "indri/OrNode.hpp"
#include "indri/NotNode.hpp"
#include "indri/WeightedAndNode.hpp"
#include "indri/WeightedSumNode.hpp"
#include "indri/FilterRejectNode.hpp"
#include "indri/FilterRequireNode.hpp"
#include "indri/FieldBetweenNode.hpp"
#include "indri/FieldEqualsNode.hpp"
#include "indri/FieldLessNode.hpp"
#include "indri/FieldGreaterNode.hpp"
#include "indri/MaxNode.hpp"
#include "indri/PriorNode.hpp"
#include "indri/ExtentRestrictionNode.hpp"
#include "indri/FilterNode.hpp"
#include "indri/NullListNode.hpp"
#include "indri/TermScoreFunctionFactory.hpp"
#include "indri/TermFrequencyBeliefNode.hpp"
#include "indri/CachedFrequencyBeliefNode.hpp"
#include "indri/BooleanAndNode.hpp"
#include "indri/TopdocsIndex.hpp"

#include <stdexcept>

TermScoreFunction* InferenceNetworkBuilder::_buildTermScoreFunction( const std::string& smoothing, UINT64 occurrences, UINT64 contextSize ) const {
  double collectionFrequency;

  if( occurrences ) {
    collectionFrequency = double(occurrences) / double(contextSize);
  } else {
    // this is something that never happens in our collection, so we assume that it
    // happens somewhat less often than 1./collectionSize.  I picked 1/(2*collectionSize)
    // because it seemed most appropriate
    collectionFrequency = 1.0 / double(contextSize*2.);
  }

  return TermScoreFunctionFactory::get( smoothing, collectionFrequency );
}

InferenceNetworkBuilder::InferenceNetworkBuilder( Repository& repository, ListCache& cache, int resultsRequested ) :
  _repository(repository),
  _cache(cache),
  _network( new InferenceNetwork( repository ) ),
  _resultsRequested( resultsRequested )
{
}

InferenceNetworkBuilder::~InferenceNetworkBuilder() {
  delete _network;
}

InferenceNetwork* InferenceNetworkBuilder::getNetwork() {
  return _network;
}

void InferenceNetworkBuilder::defaultAfter( indri::lang::Node* node ) {
  LEMUR_THROW( LEMUR_INTERNAL_ERROR, "InferenceNetworkBuilder found a node type that it didn't know what to do with" );
}

void InferenceNetworkBuilder::after( indri::lang::IndexTerm* term ) {
  if( _nodeMap.find( term ) == _nodeMap.end() ) {
    bool stopword = false;
    std::string processed = term->getText();
    int termID = 0;
    ListIteratorNode* iteratorNode = 0;
    
    // stem and stop the word
    if( term->getStemmed() == false ) {
      processed = _repository.processTerm( term->getText() );
      stopword = processed.length() == 0;
    }

    // if it isn't a stopword, we can try to get it from the index
    if( !stopword ) {
      termID = _repository.index()->term( processed.c_str() );
    
      if( termID != 0 ) {
        DocPositionInfoList* positionList = _repository.index()->docPositionInfoList( termID );

        _network->addDocIterator( positionList );
        iteratorNode = new DocListIteratorNode( term->nodeName(), positionList );
      }
    }
    
    // if we didn't find anything at the index, make a placeholder
    if( !iteratorNode ) {
      iteratorNode = new NullListNode( term->nodeName(), stopword );
    }

    _network->addListNode( iteratorNode );
    _nodeMap[term] = iteratorNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::Field* field ) {
  if( _nodeMap.find( field ) == _nodeMap.end() ) {
    int fieldID = _repository.index()->field( field->getFieldName().c_str() );
    indri::index::FieldListIterator* iterator = 0;
    ListIteratorNode* iteratorNode = 0;

    if( fieldID != 0 ) {
      iterator = _repository.index()->fieldPositionListIterator( fieldID );
      _network->addFieldIterator( iterator );
      
      iteratorNode = new FieldIteratorNode( field->nodeName(), iterator );
    } else {
      iteratorNode = new NullListNode( field->nodeName(), false );
    }

    _network->addListNode( iteratorNode );
    _nodeMap[field] = iteratorNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::ExtentRestriction* erNode ) {
  if( _nodeMap.find( erNode ) == _nodeMap.end() ) {
    BeliefNode* childNode = dynamic_cast<BeliefNode*>(_nodeMap[erNode->getChild()]);
    ListIteratorNode* fieldNode = dynamic_cast<ListIteratorNode*>(_nodeMap[erNode->getField()]);
    ExtentRestrictionNode* extentRestriction = new ExtentRestrictionNode( erNode->nodeName(), childNode, fieldNode );

    _network->addBeliefNode( extentRestriction );
    _nodeMap[erNode] = extentRestriction;
  }
}

void InferenceNetworkBuilder::after( indri::lang::ExtentAnd* extentAnd ) {
  if( _nodeMap.find( extentAnd ) == _nodeMap.end() ) {
    std::vector<ListIteratorNode*> translation = _translate<ListIteratorNode>( extentAnd->getChildren() );
    ExtentAndNode* extentAndNode = new ExtentAndNode( extentAnd->nodeName(), translation );

    _network->addListNode( extentAndNode );
    _nodeMap[extentAnd] = extentAndNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::ExtentOr* extentOr ) {
  if( _nodeMap.find( extentOr ) == _nodeMap.end() ) {
    std::vector<ListIteratorNode*> translation = _translate<ListIteratorNode>( extentOr->getChildren() );
    ExtentOrNode* extentOrNode = new ExtentOrNode( extentOr->nodeName(), translation );

    _network->addListNode( extentOrNode );
    _nodeMap[extentOr] = extentOrNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::ExtentInside* extentInside ) {
  if( _nodeMap.find( extentInside ) == _nodeMap.end() ) {
    ExtentInsideNode* extentInsideNode = new ExtentInsideNode( 
      extentInside->nodeName(),
      dynamic_cast<ListIteratorNode*>(_nodeMap[extentInside->getInner()]),
      dynamic_cast<ListIteratorNode*>(_nodeMap[extentInside->getOuter()]) );

    _network->addListNode( extentInsideNode );
    _nodeMap[extentInside] = extentInsideNode;
  }
}

static bool inferencenetworkbuilder_is_stopword( ListIteratorNode* node ) {
  NullListNode* nullNode = dynamic_cast<NullListNode*>(node);
  return nullNode && nullNode->isStopword();
}

static bool inferencenetworkbuilder_contains_stopwords( const std::vector<ListIteratorNode*>& nodes ) {
  // scan for stopwords
  int stopwords = 0;

  for( unsigned int i=0; i<nodes.size(); i++ ) {
    if( inferencenetworkbuilder_is_stopword(nodes[i]) )
      stopwords++;
  }

  return stopwords != 0;
}

static int inferencenetworkbuilder_find_stopwords_left( const std::vector<ListIteratorNode*>& nodes ) {
  int begin;

  for( begin=0; begin<nodes.size(); begin++ ) {
    if( inferencenetworkbuilder_is_stopword(nodes[begin]) )
      continue;

    break;
  }

  return begin;
}

static int inferencenetworkbuilder_find_stopwords_right( const std::vector<ListIteratorNode*>& nodes ) {
  int end;

  if( nodes.size() == 0 )
    return 0;

  for( end=nodes.size()-1; end>=0; end-- ) {
    if( inferencenetworkbuilder_is_stopword(nodes[end]) )
      continue;

    break;
  }

  return end+1;
}

static void inferencenetworkbuilder_find_stopwords_stretch( const std::vector<ListIteratorNode*>& nodes, int& begin, int& end ) {
  for( begin=0; begin<int(nodes.size()); begin++ ) {
    if( inferencenetworkbuilder_is_stopword(nodes[begin]) )
      break;
  }

  for( end=begin+1; end<int(nodes.size()); end++ ) {
    if( !inferencenetworkbuilder_is_stopword(nodes[end]) )
      break;
  }
}

static ListIteratorNode* inferencenetworkbuilder_build_child_window( InferenceNetwork* network, const std::vector<ListIteratorNode*>& nodes, int windowSize ) {
  assert( nodes.size() ); // we should have nodes
  assert( !inferencenetworkbuilder_is_stopword(nodes.front()) ); // the front must not be a stopword
  assert( !inferencenetworkbuilder_is_stopword(nodes.back()) );  // the back must not be a stopword

  std::string nodeName = "unknown";

  if( ! inferencenetworkbuilder_contains_stopwords( nodes ) ) {
    ListIteratorNode* total = 0;
    
    if( nodes.size() == 1 ) {
      total = nodes.front();
    } else {
      total = new OrderedWindowNode( nodeName, nodes, windowSize );
      network->addListNode( total );
    }

    return total;
  } else {
    int begin;
    int end;

    inferencenetworkbuilder_find_stopwords_stretch( nodes, begin, end );

    std::vector<ListIteratorNode*> leftNodes;
    std::vector<ListIteratorNode*> rightNodes;
    leftNodes.assign( nodes.begin(), nodes.begin()+begin );
    rightNodes.assign( nodes.begin()+end, nodes.end() );

    ListIteratorNode* left = 0; 
    ListIteratorNode* right = 0;

    if( leftNodes.size() == 1 )
      left = leftNodes.front();
    else
      left = inferencenetworkbuilder_build_child_window( network, leftNodes, windowSize );
    
    if( rightNodes.size() == 1 )
      right = rightNodes.front();
    else if( rightNodes.size() > 1 )
      right = inferencenetworkbuilder_build_child_window( network, rightNodes, windowSize );

    std::vector<ListIteratorNode*> totalNodes;

    if( left )
      totalNodes.push_back( left );
    if( right )
      totalNodes.push_back( right );

    assert( totalNodes.size() >= 1 );
    ListIteratorNode* total = 0;

    if( totalNodes.size() > 1 ) {
      total = new OrderedWindowNode( nodeName, totalNodes, windowSize*(end-begin+1) );
      network->addListNode( total );
    } else {
      total = totalNodes.front();
    }

    return total;
  }
}

static ListIteratorNode* inferencenetworkbuilder_build_ordered_window( InferenceNetwork* network, const std::string& nodeName, const std::vector<ListIteratorNode*>& nodes, int windowSize ) {
  // trim stopwords from beginning and ending
  int begin = inferencenetworkbuilder_find_stopwords_left( nodes );
  int end = inferencenetworkbuilder_find_stopwords_right( nodes );
  ListIteratorNode* result = 0;

  // We can't just ignore stopwords, because they'll cause this window to not match unnecessarily.
  // Here's the plan: if we find a stopword, we throw it out, but we build a two-level ordered window in its place,
  // like this: (assuming "the" is a stopword)
  // #1(jumped over the lazy dog) = #2( #1(jumped over) #1(lazy dog) )

  if( end <= begin ) {
    // window contains no non-stopwords
    NullListNode* nullNode = new NullListNode( nodeName, true );
    result = nullNode;
    network->addListNode( result );
  } else {
    // window contains some non-stopwords
    std::vector<ListIteratorNode*> inner;
    inner.assign( nodes.begin()+begin, nodes.begin()+end );

    if( inferencenetworkbuilder_contains_stopwords( inner ) ) {
      // window contains some stopwords in the middle, sandwitched between non-stopwords
      inferencenetworkbuilder_find_stopwords_stretch( inner, begin, end );

      std::vector<ListIteratorNode*> leftNodes;
      std::vector<ListIteratorNode*> rightNodes;
      leftNodes.assign( inner.begin(), inner.begin()+begin );
      rightNodes.assign( inner.begin()+end, inner.end() );

      ListIteratorNode* left = inferencenetworkbuilder_build_child_window( network, leftNodes, windowSize );
      ListIteratorNode* right = inferencenetworkbuilder_build_child_window( network, rightNodes, windowSize );

      std::vector<ListIteratorNode*> totalNodes;
      totalNodes.push_back( left );
      totalNodes.push_back( right );

      OrderedWindowNode* total = new OrderedWindowNode( nodeName, totalNodes, windowSize*(end-begin+1) );
      result = total;
      network->addListNode( result );
    } else if( inner.size() > 1 ) {
      // no stopwords left, more than 1 word
      OrderedWindowNode* odNode = new OrderedWindowNode( nodeName, inner, windowSize );
      result = odNode;
      network->addListNode( result );
    } else if( inner.size() == 1 ) {
      // no stopwords left, only one word
      result = inner.front();
    } else {
      // no stopwords left, no words
      assert( false && "shouldn't ever get here, handled by top case" );
    }
  }

  return result;
}

void InferenceNetworkBuilder::after( indri::lang::ODNode* odNode ) {
  if( _nodeMap.find( odNode ) == _nodeMap.end() ) {
    std::vector<ListIteratorNode*> translation = _translate<ListIteratorNode>( odNode->getChildren() );
    ListIteratorNode* orderedNode = inferencenetworkbuilder_build_ordered_window( _network, odNode->nodeName(), translation, odNode->getWindowSize() ); 

    _nodeMap[odNode] = orderedNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::UWNode* uwNode ) {
  if( _nodeMap.find( uwNode ) == _nodeMap.end() ) {
    std::vector<ListIteratorNode*> translation = _translate<ListIteratorNode>( uwNode->getChildren() );
    std::vector<ListIteratorNode*> nullsRemoved;
    nullsRemoved.reserve( translation.size() );

    for( unsigned int i=0; i<translation.size(); i++ ) {
      NullListNode* nullNode = dynamic_cast<NullListNode*>(translation[i]);
      // we want to remove words that are stopwords (either query or index) because they'll cause this window not to match
      if( !(nullNode && nullNode->isStopword()) ) 
        nullsRemoved.push_back( translation[i] );
    }

    ListIteratorNode* unorderedNode = 0; 
    
    if( nullsRemoved.size() == 0 ) {
      unorderedNode = new NullListNode( uwNode->nodeName(), true );
      _network->addListNode( unorderedNode );
    } else if( nullsRemoved.size() == 1 ) {
      unorderedNode = nullsRemoved.front();
    } else {
      unorderedNode = new UnorderedWindowNode( uwNode->nodeName(), nullsRemoved, uwNode->getWindowSize() );
      _network->addListNode( unorderedNode );
    }

    _nodeMap[uwNode] = unorderedNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::BAndNode* bandNode ) {
  if( _nodeMap.find( bandNode ) == _nodeMap.end() ) {
    std::vector<ListIteratorNode*> translation = _translate<ListIteratorNode>( bandNode->getChildren() );
    BooleanAndNode* booleanAndNode = new BooleanAndNode( bandNode->nodeName(),
                                                         translation );

    _network->addListNode( booleanAndNode );
    _nodeMap[bandNode] = booleanAndNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FilRejNode* filRejNode ) {
  if( _nodeMap.find( filRejNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedFilter = _nodeMap[ filRejNode->getFilter() ];
    InferenceNetworkNode* untypedDisallowed = _nodeMap[ filRejNode->getDisallowed() ];
    BeliefNode* filterRejectNode = 0;
    
    if( untypedFilter ) {
      // normal case
      filterRejectNode = new FilterRejectNode( filRejNode->nodeName(),
                                              dynamic_cast<ListIteratorNode*>(untypedFilter),
                                              dynamic_cast<BeliefNode*>(untypedDisallowed) );
      _network->addBeliefNode( filterRejectNode );
    } else {
      // the filter term doesn't exist in the collection, so therefore, it never matches
      // therefore, we can promote the disallowed node up the chain
      filterRejectNode = dynamic_cast<BeliefNode*>(untypedDisallowed);
    }

    _nodeMap[filRejNode] = filterRejectNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FilReqNode* filReqNode ) {
  if( _nodeMap.find( filReqNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedFilter = _nodeMap[ filReqNode->getFilter() ];
    InferenceNetworkNode* untypedRequired = _nodeMap[ filReqNode->getRequired() ];
    FilterRequireNode* filterRequireNode = 0;
    
    if( untypedFilter && untypedRequired ) {
      filterRequireNode = new FilterRequireNode( filReqNode->nodeName(),
                                                 dynamic_cast<ListIteratorNode*>(untypedFilter),
                                                 dynamic_cast<BeliefNode*>(untypedRequired) );
      _network->addBeliefNode( filterRequireNode );
    }

    _nodeMap[filReqNode] = filterRequireNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FilterNode* fNode ) {
  if( _nodeMap.find( fNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedChild = _nodeMap[ fNode->getChild() ];
    FilterNode* filterNode = new FilterNode( fNode->nodeName(),
                                             dynamic_cast<BeliefNode*>(untypedChild),
                                             fNode->getDocuments() );

    _network->addBeliefNode( filterNode );
    _nodeMap[fNode] = filterNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FieldLessNode* flNode ) {
  if( _nodeMap.find( flNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedFieldNode = _nodeMap[ flNode->getField() ];
    FieldLessNode* fieldLessNode = 0;
    
    if( untypedFieldNode ) {
      fieldLessNode = new FieldLessNode( flNode->nodeName(),
                                         dynamic_cast<FieldIteratorNode*>( untypedFieldNode ),
                                         flNode->getConstant() );
      _network->addListNode( fieldLessNode );
    }

    _nodeMap[flNode] = fieldLessNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FieldGreaterNode* fgNode ) {
  if( _nodeMap.find( fgNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedFieldNode = _nodeMap[ fgNode->getField() ];
    FieldGreaterNode* fieldGreaterNode = 0;
    
    if( untypedFieldNode ) {
      fieldGreaterNode = new FieldGreaterNode( fgNode->nodeName(),
                                               dynamic_cast<FieldIteratorNode*>( untypedFieldNode ),
                                               fgNode->getConstant() );
      _network->addListNode( fieldGreaterNode );
    }

    _nodeMap[fgNode] = fieldGreaterNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FieldBetweenNode* fbNode ) {
  if( _nodeMap.find( fbNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedFieldNode = _nodeMap[ fbNode->getField() ];
    FieldBetweenNode* fieldBetweenNode = 0;
      
    if( untypedFieldNode ) {
      fieldBetweenNode = new FieldBetweenNode( fbNode->nodeName(),
                                               dynamic_cast<FieldIteratorNode*>( untypedFieldNode ),
                                               fbNode->getLow(),
                                               fbNode->getHigh() );
      _network->addListNode( fieldBetweenNode );
    }

    _nodeMap[fbNode] = fieldBetweenNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::FieldEqualsNode* feNode ) {
  if( _nodeMap.find( feNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedFieldNode = _nodeMap[ feNode->getField() ];
    FieldEqualsNode* fieldEqualsNode = 0;
      
    if( untypedFieldNode ) {
      fieldEqualsNode = new FieldEqualsNode( feNode->nodeName(),
                                             dynamic_cast<FieldIteratorNode*>( untypedFieldNode ),
                                             feNode->getConstant() );
      _network->addListNode( fieldEqualsNode );
    }

    _nodeMap[feNode] = fieldEqualsNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::ContextCounterNode* contextCounterNode ) {
  if( _nodeMap.find( contextCounterNode ) == _nodeMap.end() ) {
    InferenceNetworkNode* untypedRawExtent = _nodeMap[ contextCounterNode->getRawExtent() ];
    InferenceNetworkNode* untypedContext = _nodeMap[ contextCounterNode->getContext() ];
    ContextCountAccumulator* contextCount = 0;

    if( contextCounterNode->hasMaxScore() ) {
      // the preprocessor filled in these counts, so we'll put in a placeholder,
      // but won't mark it as complex (so it won't be evaluated on each doc in the collection)
      // it also managed to get us some max-score stats, so we'll pass those along
      contextCount = new ContextCountAccumulator(
        contextCounterNode->nodeName(),
        contextCounterNode->getOccurrences(),
        contextCounterNode->getContextSize(),
        contextCounterNode->getMaximumOccurrences(),
        contextCounterNode->getMinimumContextLength(), 
        contextCounterNode->getMaximumContextLength(),
        contextCounterNode->getMaximumContextFraction() );
    } else if( contextCounterNode->hasCounts() ) {
      // the preprocessor filled in these counts, so we'll put in a placeholder,
      // but won't mark it as complex (so it won't be evaluated on each doc in the collection)
      contextCount = new ContextCountAccumulator(
        contextCounterNode->nodeName(),
        contextCounterNode->getOccurrences(),
        contextCounterNode->getContextSize() );
    } else if( contextCounterNode->hasContextSize() ) {
      // the preprocessor was able to compute the context size, but not the
      // occurrences for this term
      ListCache::CachedList* list = new ListCache::CachedList;
      contextCounterNode->getRawExtent()->copy( list->raw );
      if( contextCounterNode->getContext() )
        contextCounterNode->getContext()->copy( list->context );
        
      contextCount = new ContextCountAccumulator(
        contextCounterNode->nodeName(),
        &_cache,
        list,
        dynamic_cast<ListIteratorNode*>(untypedRawExtent),
        contextCounterNode->getContextSize(),
        _repository.index()->maxDocumentLength() );
      _network->addComplexEvaluatorNode( contextCount );
    } else {
      ListCache::CachedList* list = new ListCache::CachedList;
      contextCounterNode->getRawExtent()->copy( list->raw );
      if( contextCounterNode->getContext() )
        contextCounterNode->getContext()->copy( list->context );

      // the preprocessing step wasn't able to calculate counts for this
      // node, so it's a complex evaluator (needs to be run on each document in the collection)
      contextCount = new ContextCountAccumulator(
        contextCounterNode->nodeName(),
        &_cache,
        list,
        dynamic_cast<ListIteratorNode*>(untypedRawExtent),
        dynamic_cast<ListIteratorNode*>(untypedContext) );
      _network->addComplexEvaluatorNode( contextCount );
    }

    _network->addEvaluatorNode( contextCount );
    _nodeMap[ contextCounterNode ] = contextCount;
  }
}

void InferenceNetworkBuilder::after( indri::lang::ScoreAccumulatorNode* scoreAccumulatorNode ) {
  if( _nodeMap.find( scoreAccumulatorNode ) == _nodeMap.end() ) {
    indri::lang::Node* c = scoreAccumulatorNode->getChild();
    BeliefNode* child = dynamic_cast<BeliefNode*>(_nodeMap[c]);
    ScoredExtentAccumulator* accumulator = new ScoredExtentAccumulator( scoreAccumulatorNode->nodeName(), child, _resultsRequested );

    _network->addEvaluatorNode( accumulator );
    _network->addComplexEvaluatorNode( accumulator );
    _nodeMap[ scoreAccumulatorNode ] = accumulator;
  }
}

void InferenceNetworkBuilder::after( indri::lang::AnnotatorNode* annotatorNode ) {
  if( _nodeMap.find( annotatorNode ) == _nodeMap.end() ) {
    indri::lang::Node* c = annotatorNode->getChild();
    BeliefNode* child = dynamic_cast<BeliefNode*>(_nodeMap[c]);
    Annotator* annotator = new Annotator( annotatorNode->nodeName(), child );

    _network->addEvaluatorNode( annotator );
    _network->addComplexEvaluatorNode( annotator );
    _nodeMap[ annotatorNode ] = annotator;
  }
}

void InferenceNetworkBuilder::after( indri::lang::CachedFrequencyScorerNode* cachedScorerNode ) {
  if( _nodeMap.find( cachedScorerNode ) == _nodeMap.end() ) {
    ListCache::CachedList* list = (ListCache::CachedList*) cachedScorerNode->getList();

    if( !list )
      LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Trying to expand a CachedFrequencyScoredNode, but the cached list is gone" );

    BeliefNode* belief = 0;
    TermScoreFunction* function = 0;

    function = _buildTermScoreFunction( cachedScorerNode->getSmoothing(),
                                        list->occurrences,
                                        list->contextSize );

    if( list->occurrences ) {
      UINT64 maxOccurrences = UINT64( ceil( double( list->maximumContextSize ) * list->maximumContextFraction ) );

      double maximumScore = function->scoreOccurrence( maxOccurrences, list->maximumContextSize );
      double maximumBackgroundScore = function->scoreOccurrence( 0, list->minimumContextSize );
      
      belief = new CachedFrequencyBeliefNode( cachedScorerNode->nodeName(),
                                              list,
                                              *function,
                                              maximumBackgroundScore,
                                              maximumScore );
    } else {
      belief = new NullScorerNode( cachedScorerNode->nodeName(), *function );
    }

    _network->addScoreFunction( function );
    _network->addBeliefNode( belief );
    _nodeMap[cachedScorerNode] = belief;
  }
}

void InferenceNetworkBuilder::after( indri::lang::TermFrequencyScorerNode* termScorerNode ) {
  if( _nodeMap.find( termScorerNode ) == _nodeMap.end() ) {
    BeliefNode* belief = 0;
    TermScoreFunction* function = 0;

    function = _buildTermScoreFunction( termScorerNode->getSmoothing(),
                                        termScorerNode->getOccurrences(),
                                        termScorerNode->getContextSize() );

    if( termScorerNode->getOccurrences() ) {
      bool stopword = false;
      std::string processed = termScorerNode->getText();
      int termID = 0;
    
      // stem and stop the word
      if( termScorerNode->getStemmed() == false ) {
        processed = _repository.processTerm( termScorerNode->getText() );
        stopword = processed.length() == 0;
      }

      // if it isn't a stopword, we can try to get it from the index
      if( !stopword ) {
        termID = _repository.index()->term( processed.c_str() );

        if( termID != 0 ) {
          indri::index::DocListFrequencyIterator* frequencyList = _repository.index()->docFrequencyInfoList( termID );
          _network->addFrequencyIterator( frequencyList );

          UINT64 maxOccurrences;
          double maximumScore;
          double maximumBackgroundScore;
          double maximumFraction;

          TopdocsIndex::TopdocsList* topdocs = 0;

          if( Parameters::instance().get( "topdocs", 1 ) )
            topdocs = _repository.topdocs()->fetch( termID );

          if( topdocs ) {
            // this is the maximum fraction *not* in the topdocs list
            maximumFraction = double(topdocs->smallest.count) / double(topdocs->smallest.length);
          } else {
            maximumFraction = termScorerNode->getMaxContextFraction();
          }

          maxOccurrences = UINT64( ceil( double(termScorerNode->getMaxContextLength()) * maximumFraction ) );

          maximumScore = function->scoreOccurrence( maxOccurrences, termScorerNode->getMaxContextLength() );
          maximumBackgroundScore = function->scoreOccurrence( 0, termScorerNode->getMinContextLength() );

          belief = new TermFrequencyBeliefNode( termScorerNode->nodeName(), *frequencyList, topdocs, *function, maximumBackgroundScore, maximumScore );
        }
      }
    }

    // either there's no list here, or there aren't any occurrences
    // in the local collection, so just use a NullScorerNode in place
    if( !belief ) {
      belief = new NullScorerNode( termScorerNode->nodeName(), *function );
    }

    _network->addScoreFunction( function );
    _network->addBeliefNode( belief );
    _nodeMap[termScorerNode] = belief;
  }
}

void InferenceNetworkBuilder::after( indri::lang::RawScorerNode* rawScorerNode ) {
  if( _nodeMap.find( rawScorerNode ) == _nodeMap.end() ) {
    BeliefNode* belief;
    InferenceNetworkNode* untypedRawExtentNode = _nodeMap[rawScorerNode->getRawExtent()];
    InferenceNetworkNode* untypedContextNode = _nodeMap[rawScorerNode->getContext()];
    ListIteratorNode* iterator = dynamic_cast<ListIteratorNode*>(untypedRawExtentNode);

    TermScoreFunction* function = 0;

    function = _buildTermScoreFunction( rawScorerNode->getSmoothing(),
                                        rawScorerNode->getOccurrences(),
                                        rawScorerNode->getContextSize() );

    if( rawScorerNode->getOccurrences() && iterator != 0 ) {
      ListIteratorNode* rawIterator = 0;
      ListIteratorNode* context = dynamic_cast<ListIteratorNode*>(untypedContextNode);

      if( context ) {
        rawIterator = iterator;
        iterator = new ExtentInsideNode( "", rawIterator, context );
        _network->addListNode( iterator );
      }
      
      // this is here to turn max-score off for this term
      // only frequency lists are "max-scored"
      double maximumScore = INDRI_HUGE_SCORE;
      double maximumBackgroundScore = INDRI_TINY_SCORE;
      
      belief = new ListBeliefNode( rawScorerNode->nodeName(), *iterator, context, rawIterator, *function, maximumBackgroundScore, maximumScore );
    } else {
      belief = new NullScorerNode( rawScorerNode->nodeName(), *function );
    }

    _network->addScoreFunction( function );
    _network->addBeliefNode( belief );
    _nodeMap[rawScorerNode] = belief;
  }
}

void InferenceNetworkBuilder::after( indri::lang::PriorNode* pNode ) {
  if( _nodeMap.find( pNode ) == _nodeMap.end() ) {
    FieldIteratorNode* field = dynamic_cast<FieldIteratorNode*>(_nodeMap[pNode->getField()]);
    PriorNode* priorNode = new PriorNode( pNode->nodeName(), field, pNode->getTable() );

    _network->addBeliefNode( priorNode );
    _nodeMap[pNode] = priorNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::WeightNode* weightNode ) {
  if( _nodeMap.find( weightNode ) == _nodeMap.end() ) {
    const std::vector< std::pair<double, indri::lang::ScoredExtentNode*> >& children = weightNode->getChildren();
    WeightedAndNode* wandNode = new WeightedAndNode( weightNode->nodeName() );
    
    double totalWeights = 0;
    for( unsigned int i=0; i<children.size(); i++ ) {
      totalWeights += children[i].first;
    }

    for( unsigned int i=0; i<children.size(); i++ ) {
      wandNode->addChild( children[i].first / totalWeights,
        dynamic_cast<BeliefNode*>( _nodeMap[children[i].second] ) );
    }

    wandNode->doneAddingChildren();

    _network->addBeliefNode( wandNode );
    _nodeMap[weightNode] = wandNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::WSumNode* wsumNode ) {
  if( _nodeMap.find( wsumNode ) == _nodeMap.end() ) {
    const std::vector< std::pair<double, indri::lang::ScoredExtentNode*> >& children = wsumNode->getChildren();
    WeightedSumNode* weightedSumNode = new WeightedSumNode( wsumNode->nodeName() );

    double totalWeights = 0;
    for( unsigned int i=0; i<children.size(); i++ ) {
      totalWeights += children[i].first;
    }

    for( unsigned int i=0; i<children.size(); i++ ) {
      weightedSumNode->addChild( children[i].first / totalWeights,
        dynamic_cast<BeliefNode*>( _nodeMap[children[i].second] ) );
    }

    _network->addBeliefNode( weightedSumNode );
    _nodeMap[wsumNode] = weightedSumNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::WAndNode* wandNode ) {
  if( _nodeMap.find( wandNode ) == _nodeMap.end() ) {
    const std::vector< std::pair<double, indri::lang::ScoredExtentNode*> >& children = wandNode->getChildren();
    WeightedAndNode* weightedAndNode = new WeightedAndNode( wandNode->nodeName() );

    double totalWeights = 0;
    for( unsigned int i=0; i<children.size(); i++ ) {
      totalWeights += children[i].first;
    }

    for( unsigned int i=0; i<children.size(); i++ ) {
      weightedAndNode->addChild( children[i].first / totalWeights,
        dynamic_cast<BeliefNode*>( _nodeMap[children[i].second] ) );
    }

    weightedAndNode->doneAddingChildren();

    _network->addBeliefNode( weightedAndNode );
    _nodeMap[wandNode] = weightedAndNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::OrNode* orSpecNode ) {
  if( _nodeMap.find( orSpecNode ) == _nodeMap.end() ) {
    std::vector<BeliefNode*> translation = _translate<BeliefNode>( orSpecNode->getChildren() );
    OrNode* orNode = new OrNode( orSpecNode->nodeName(), translation );

    _network->addBeliefNode( orNode );
    _nodeMap[orSpecNode] = orNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::NotNode* notSpecNode ) {
  if( _nodeMap.find( notSpecNode ) == _nodeMap.end() ) {
    BeliefNode* child = dynamic_cast<BeliefNode*>( _nodeMap[notSpecNode->getChild()] );
    NotNode* notNode = new NotNode( notSpecNode->nodeName(), child );

    _network->addBeliefNode( notNode );
    _nodeMap[notSpecNode] = notNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::MaxNode* maxSpecNode ) {
  if( _nodeMap.find( maxSpecNode ) == _nodeMap.end() ) {
    std::vector<BeliefNode*> translation = _translate<BeliefNode>( maxSpecNode->getChildren() );
    MaxNode* maxNode = new MaxNode( maxSpecNode->nodeName(), translation );

    _network->addBeliefNode( maxNode );
    _nodeMap[maxSpecNode] = maxNode;
  }
}

void InferenceNetworkBuilder::after( indri::lang::CombineNode* combineNode ) {
  if( _nodeMap.find( combineNode ) == _nodeMap.end() ) {
    const std::vector<indri::lang::ScoredExtentNode*>& children = combineNode->getChildren();
    double weight = 1. / double(children.size());

    std::vector<BeliefNode*> translation = _translate<BeliefNode,indri::lang::ScoredExtentNode>( children );
    WeightedAndNode* wandNode = new WeightedAndNode( combineNode->nodeName() );

    for( unsigned int i=0; i<children.size(); i++ ) {
      wandNode->addChild( weight, translation[i] );
    }

    wandNode->doneAddingChildren();

    _network->addBeliefNode( wandNode );
    _nodeMap[combineNode] = wandNode;
  }
}
