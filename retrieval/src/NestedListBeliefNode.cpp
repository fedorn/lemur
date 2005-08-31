

//
// NestedListBeliefNode
//
// 30 Aug 2005
//

#include "indri/NestedListBeliefNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

// computes the length of the scored context
int indri::infnet::NestedListBeliefNode::_contextLength( int begin, int end ) {
  //
  // There are two possible contexts at work here.  Consider the query
  // #combine[sentence]( dog.(paragraph) )
  //
  // In this case, the context for scoring is text in a paragraph field, but
  // this text will be scored for every sentence.  The paragraph field will
  // be represented by <_context>, and the sentence to be scored
  // will be represented by the term offsets <begin> and <end>.
  //

  if( !_context )
    return end - begin;

  int contextLength = 0;
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _context->extents();

  for( size_t i=0; i<extents.size(); i++ ) {
    if( extents[i].begin > end )
      break;

    if( extents[i].end < begin )
      continue; 

    // make sure to restrict the extents here to match the scored region
    int extentBegin = lemur_compat::max( extents[i].begin, begin );
    int extentEnd = lemur_compat::min( extents[i].end, end );

    contextLength += extentEnd - extentBegin;
  }

  return contextLength;
}

double indri::infnet::NestedListBeliefNode::_contextOccurrences( int begin, int end ) {
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _list.extents();
  double count = 0;
  int lastEnd = 0;

  // look for all occurrences within bounds and that don't overlap
  for( size_t i=0; i<extents.size(); i++ ) {
    if( extents[i].begin >= begin &&
        extents[i].end <= end &&
        extents[i].begin >= lastEnd ) {
      count += extents[i].weight;
      lastEnd = extents[i].end;
    }
  }

  return count;
}

double indri::infnet::NestedListBeliefNode::_documentOccurrences() {
  assert( _raw ); // score() maintains this invariant
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _raw->extents();
  double count = 0;
  int lastEnd = 0;

  // look for all occurrences within bounds and that don't overlap
  for( size_t i=0; i<extents.size(); i++ ) {
    if( extents[i].begin >= lastEnd ) {
      count += extents[i].weight;
      lastEnd = extents[i].end;
    }
  }

  return count;
}

indri::infnet::NestedListBeliefNode::NestedListBeliefNode( const std::string& name, ListIteratorNode& child, ListIteratorNode* context, ListIteratorNode* raw, indri::query::TermScoreFunction& scoreFunction, double maximumBackgroundScore, double maximumScore )
  :
  _name(name),
  _scoreFunction(scoreFunction),
  _maximumScore(maximumScore),
  _maximumBackgroundScore(maximumBackgroundScore),
  _documentSmoothing(false),
  _context(context),
  _raw(raw),
  _list(child)
{
  _maximumScore = INDRI_HUGE_SCORE;
}

int indri::infnet::NestedListBeliefNode::nextCandidateDocument() {
  return _list.nextCandidateDocument();
}

double indri::infnet::NestedListBeliefNode::maximumBackgroundScore() {
  return _maximumBackgroundScore;
}

double indri::infnet::NestedListBeliefNode::maximumScore() {
  return _maximumScore;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::NestedListBeliefNode::score( int documentID, int begin, int end, int documentLength ) {
  int contextSize = _contextLength( begin, end );
  double occurrences = _contextOccurrences( begin, end );
  double documentOccurrences = _raw ? _documentOccurrences() : occurrences;
  double score = 0;
  
  score = _scoreFunction.scoreOccurrence( occurrences, contextSize, documentOccurrences, documentLength );

  _scores.clear();
  _scores.push_back( indri::api::ScoredExtentResult( score, documentID, begin, end ) );

  return _scores;
}

void indri::infnet::NestedListBeliefNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _list.extents();

  indri::index::Extent range( begin, end );
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
  iter = std::lower_bound( extents.begin(), extents.end(), range, indri::index::Extent::begins_before_less() );

  // mark the begin and end points for this list
  for( size_t i = iter - extents.begin(); i < extents.size() && extents[i].begin <= end; i++ ) {
    if( extents[i].begin >= begin &&
        extents[i].end <= end ) {
      annotator.add( this, documentID, extents[i].begin, extents[i].end );
      _list.annotate( annotator, documentID, extents[i].begin, extents[i].end );
    }
  }
}

bool indri::infnet::NestedListBeliefNode::hasMatch( int documentID ) {
  return _list.extents().size() > 0;
}

const indri::utility::greedy_vector<bool>& indri::infnet::NestedListBeliefNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& matchExtents ) {
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _list.extents();
  _matches.clear();
  _matches.resize( matchExtents.size(), false );

  size_t i=0;
  size_t j=0; 

  // with field wildcard node possibly in the matches extent list creation process,
  // we can only move the
  // extent iterator when we are sure that it doesn't overlap with the match list
  // MAJOR flaw for now: we are assuming the extent list is nice and does not contain
  // overlapping extents, and is in order of increasing begin
  // we also assume that for any later extent in the match list the begin >= current begin
  while( i < extents.size() && j < matchExtents.size() ) {
    if( matchExtents[j].contains( extents[i] ) ) {
      // found a match
      _matches[j] = true;
      j++;
    } else if( matchExtents[j].begin > extents[i].begin ) {
      // Safe to increment pointer in extents list as 
      // all extents in match list will have larger begin value than current one
      i++;
    } else {
      // Either matchExtents[j].begin <= extents[i].begin or
      // extents[i].contains( matchExtents[j] ).
      // This means it is safe to increment j and check the next matchExtent.    
      j++;
    }
  }

  return _matches;
}

const std::string& indri::infnet::NestedListBeliefNode::getName() const {
  return _name;
}

void indri::infnet::NestedListBeliefNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}


