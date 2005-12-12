

//
// ShrinkageBeliefNode
//
// 29 July 2005 -- pto
//

#include "indri/ShrinkageBeliefNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"


indri::infnet::ShrinkageBeliefNode::ShrinkageBeliefNode( const std::string& name, ListIteratorNode& child, DocumentStructureHolderNode& documentStructureHolderNode, indri::query::TermScoreFunction& scoreFunction, double maximumBackgroundScore, double maximumScore )
  :
  _name(name),
  _scoreFunction(scoreFunction),
  _maximumScore(maximumScore),
  _maximumBackgroundScore(maximumBackgroundScore),
  _list(child),
  _docStructHolder(documentStructureHolderNode),
  _down(0),
  _up(0),
  _base(0),
  _counts(0),
  _documentID(0),
  _parentWeight(0),
  _docWeight(0),
  _recursive(false),
  _queryLevelCombine(false)
{

  _maximumScore = INDRI_HUGE_SCORE;
}

int indri::infnet::ShrinkageBeliefNode::nextCandidateDocument() {
  return _list.nextCandidateDocument();
}

double indri::infnet::ShrinkageBeliefNode::maximumBackgroundScore() {
  return _maximumBackgroundScore;
}

double indri::infnet::ShrinkageBeliefNode::maximumScore() {
  return _maximumScore;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::ShrinkageBeliefNode::score( int documentID, int begin, int end, int documentLength ) {



  indri::index::DocumentStructure * docStruct = _docStructHolder.getDocumentStructure();
  int numNodes = docStruct->nodeCount();


  // build a new cache of scores
  if ( documentID != _documentID ) {


    _counts.clear();
    _base.clear();
    _up.clear();
    _down.clear();

    _counts.resize( numNodes+1 );
    _base.resize( numNodes+1 );
    _up.resize( numNodes+1 );
    _down.resize( numNodes+1 );

  
    // Count up the occurrences terms in the tree.
    // We include all child components - this is a break from the model in my proposal, and is 
    // to allow occurrences of phrases that cross field boundaries.  Otherwise, it is
    // an equivalent reformulation of the original model, but the parameters learned/chosen
    // would be different.

    const indri::utility::greedy_vector<indri::index::Extent> & extents = _list.extents();
    
    int lastEnd = 0;
    // look for all occurrences within bounds and that don't overlap
    for( size_t i=0; i<extents.size(); i++ ) {
      if( extents[i].begin >= lastEnd ) {
	// locate the occurrences in the document tree
	int leaf = docStruct->findLeaf( extents[i].begin, extents[i].end );
	while (leaf >= 1) {
	  _counts[leaf] += extents[i].weight;
	  leaf = docStruct->parent(leaf);
	}
	lastEnd = extents[i].end;
      }
    }
    

    // estimate scores for each of the nodes ( \theta mixed with collection model )
    for( size_t i=1; i<=numNodes; i++ ) {
      int contextSize = docStruct->accumulatedLength(i);
      double occurrences = _counts[i];
      double score = _scoreFunction.scoreOccurrence( occurrences, contextSize );
      if ( !_queryLevelCombine ) {
	score = exp( score );
      }
      _base[i] = score;
    }

    // smooth up doc 
    for( size_t i=numNodes; i>=1;i-- ) {

      // what's left for the original model and the length proportional models
      double remaining = 1;
      
      // for summing probabilities with absolute weights
      double absolute = 0;

      // for summing probabilites with length proportional weights
      int length = docStruct->accumulatedLength( i );
      double divisor = length;
      double relative = _base[ i ] * length;

      indri::index::DocumentStructure::child_iterator kids = docStruct->childrenBegin( i );
      indri::index::DocumentStructure::child_iterator kidsEnd = docStruct->childrenEnd( i );      
      while( kids < kidsEnd ) {
	// check for a smoothing rule for the child
	int kidType = docStruct->type(*kids);
	std::map<int, smoothing_rule, lt_rule>::iterator ruleIter = _ruleMap.find(kidType);
	if( ruleIter != _ruleMap.end() ) {
	  smoothing_rule rule = ruleIter->second;
	  if( rule.lengthProportional ) {
	    double lengthAlpha = rule.weight * docStruct->accumulatedLength( *kids );
	    if( _recursive ) {
	      relative += lengthAlpha * _up[ *kids ];
	    } else {
	      relative += lengthAlpha * _base[ *kids ];
	    }
	    divisor += lengthAlpha;
	  } else {
	    if( _recursive ) {
	      absolute += rule.weight * _up[ *kids ];
	    } else {
	      absolute += rule.weight * _base[ *kids ];
	    }
	    remaining -= rule.weight;
	  }
	}
	kids++;
      }
      relative /= divisor;
      if ( ! isnan( relative ) ) {
	if ( remaining >= 0 ) {
	  _up[ i ] = remaining * relative + absolute;
	} else {
	  // the absolute weights sum to something larger than 1, so we will ignore these 
	  _up[ i ] = relative;
	}
      } else {
	// the divisor was 0, so we can't use the relative weights
	if ( remaining >= 0 ) {
	  _up[ i ] = remaining * _base[ i ] + absolute;
	} else {
	  // the absolute weights sum to something larger than 1, so we will ignore these 
	  _up[ i ] = _base[ i ];
	}
      }

    }

    // smooth down the tree
    _down[1] = _up[1];
    for( size_t i=2; i<=numNodes; i++ ) {
      if( _recursive ) {
	_down[i] = (1.0 - _parentWeight - _docWeight) * _up[i] 
	  + _parentWeight * _up[docStruct->parent(i)]
	  + _docWeight * _up[1];
      } else {
	_down[i] = (1.0 - _parentWeight - _docWeight) * _up[i]
          + _parentWeight * _down[docStruct->parent(i)]
          + _docWeight * _down[1];
      }
    }

        
    _documentID = documentID;

  }

  _results.clear();

  // find the appropriate result 
  int leaf = docStruct->findLeaf( begin, end );

  // output for debugging
  //  std::cout << _list.getName() << "\t\t    leaf: " << docStruct->path(leaf) 
  //	    << "    count: " << _counts[leaf] 
  //	    << "    score: " << _down[leaf] 
  //	    << "    length: " << docStruct->accumulatedLength(leaf)
  //	    << "    coll: " << _scoreFunction.scoreOccurrence( 0, docStruct->accumulatedLength(leaf) )
  //	    << std::endl;
  
  if (docStruct->begin(leaf) == begin && docStruct->end(leaf) == end) {
    double score = _down[leaf];
    if ( !_queryLevelCombine ) {
      score = log( score );
    }
    _results.push_back( indri::api::ScoredExtentResult( score, documentID, 
							begin, end ));
  } 

  return _results;
}

void indri::infnet::ShrinkageBeliefNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {

 
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _list.extents();
  int count = 0;


  // mark the begin and end points for this list
  for( size_t i=0; i<extents.size(); i++ ) {
    if( extents[i].begin >= begin &&
        extents[i].end <= end ) {
      annotator.add( this, documentID, extents[i].begin, extents[i].end );
      _list.annotate( annotator, documentID, extents[i].begin, extents[i].end );
    }
  }
}

bool indri::infnet::ShrinkageBeliefNode::hasMatch( int documentID ) {
  return _list.extents().size() > 0;
}

const indri::utility::greedy_vector<bool>& indri::infnet::ShrinkageBeliefNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& matchExtents ) {

  _matches.clear();
  // Allows matches elsewhere in the document as shrinkage may give a reasonably large belief
  if ( hasMatch( documentID ) ) {
    _matches.resize( matchExtents.size(), true );    
  } else {
    _matches.resize( matchExtents.size(), false );
  }
  /* 
  // Requires a match of extent - may result in incorrect probability estimates

  const indri::utility::greedy_vector<indri::index::Extent>& extents = _list.extents();

  indri::utility::greedy_vector<indri::index::Extent>::const_iterator matchIter = matchExtents.begin();
  indri::utility::greedy_vector<bool>::iterator matchesIter = _matches.begin();
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator extentIter = extents.begin();


  // with field wildcard node possibly in the matches extent list creation process,
  // we can only move the
  // extent iterator when we are sure that it doesn't overlap with the match list
  // MAJOR flaw for now: we are assuming the extent list is nice and does not contain
  // overlapping extents, and is in order of increasing begin
  // we also assume that for any later extent in the match list the begin >= current begin
  while( matchIter != matchExtents.end() && extentIter != extents.end() ) {
    //    std::cout << documentID << " " << matchIter->begin << " " << matchIter->end 
    //    	      << " " << extentIter->begin <<  " " << extentIter->end << "\n";
    if( matchIter->contains( *extentIter ) ) {
      *matchesIter = true;
      matchIter++;
      matchesIter++;
      //      std::cout << "\tmatch, advancing matches\n";
    } else if( extentIter->begin < matchIter->begin ) {
      extentIter++;
      //      std::cout << "\tadvancing extents\n";
    } else {
      matchIter++;
      matchesIter++;
      //      std::cout << "\tadvancing matches\n";
    }
  }

  */
  return _matches;
}

const std::string& indri::infnet::ShrinkageBeliefNode::getName() const {
  return _name;
}

void indri::infnet::ShrinkageBeliefNode::indexChanged( indri::index::Index& index ) {

  _ruleMap.clear();
  
  std::set<smoothing_rule, lt_rule>::iterator ruleIter = _ruleSet.begin();
  while( ruleIter != _ruleSet.end() ) {
    int field = index.field( ruleIter->fieldName );
    _ruleMap[ field ] = *ruleIter;
    ruleIter++;
  }

}


void indri::infnet::ShrinkageBeliefNode::addShrinkageRule( std::string ruleText ) {


  int nextComma = 0;
  int nextColon = 0;
  int location = 0;
  
  smoothing_rule rule;
  rule.weight = 0;
  rule.lengthProportional = false;
  rule.fieldName = "";

  for( location = 0; location < ruleText.length(); ) {
    nextComma = ruleText.find( ',', location );
    nextColon = ruleText.find( ':', location );
    
    std::string key = ruleText.substr( location, nextColon-location );
    std::string value = ruleText.substr( nextColon+1, nextComma-nextColon-1 );

    if( key == "parentWeight" ) {
      _parentWeight = atof( value.c_str() );	    
    } else if( key == "docWeight" ) {
      _docWeight = atof( value.c_str() );
    } else if( key == "recursive" ) {
      _recursive = (value == "true");
    } else if( key == "queryLevelCombine" ) {
      _queryLevelCombine = (value == "true");
    } else if( key == "field" ) {
      rule.fieldName = value;
    } else if( key == "weight" ) {
      rule.weight = atof( value.c_str() );
    } else if( key == "length") {
      rule.lengthProportional = (value == "true");
    }
    if( nextComma > 0 )
      location = nextComma+1;
    else
      location = ruleText.size();
  }
  if ( rule.fieldName != "" ) {
    _ruleSet.insert( rule );
  }

}

