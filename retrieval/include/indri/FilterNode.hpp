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
// FilterNode
//
// 21 July 2004 -- tds
//
// Restricts evaluation to only a subset of the documents
// in the collection.
//

#ifndef INDRI_FILTERNODE_HPP
#define INDRI_FILTERNODE_HPP

class FilterNode : public BeliefNode {
private:
  BeliefNode* _belief;
  std::vector<int> _documents;
  std::string _name;
  int _index;

public:
  FilterNode( const std::string& name, BeliefNode* child, const std::vector<int>& documents )
    :
    _documents(documents)
  {
    _name = name;
    _belief = child;
    _index = 0;
    std::sort( _documents.begin(), _documents.end() );
  }

  int nextCandidateDocument() {
    int childNext = _belief->nextCandidateDocument();

    while( _index < _documents.size() && _documents[_index] < childNext )
      _index++;
    
    if( _index == _documents.size() )
      return MAX_INT32;

    return _documents[_index];
  }

  void annotate( Annotator& annotator, int documentID, int begin, int end ) {
    return _belief->annotate( annotator, documentID, begin, end );
  }

  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength ) {
    return _belief->score( documentID, begin, end, documentLength );
  }

  double maximumScore() {
    return _belief->maximumScore();
  }

  double maximumBackgroundScore() {
    return _belief->maximumBackgroundScore();
  }

  bool hasMatch( int documentID ) {
    return _belief->hasMatch( documentID );
  }

  const std::string& getName() const {
    return _name;
  }
};

#endif // INDRI_FILTERNODE_HPP

