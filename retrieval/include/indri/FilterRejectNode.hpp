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
// FilterRejectNode
//
// 13 April 2004 -- tds
//

#ifndef INDRI_FILTERREJECTNODE_HPP
#define INDRI_FILTERREJECTNODE_HPP

#include "indri/greedy_vector"
#include "indri/Extent.hpp"
#include "indri/BeliefNode.hpp"
#include "indri/ListIteratorNode.hpp"

class FilterRejectNode : public BeliefNode {
private:
  greedy_vector<ScoredExtentResult> _extents;
  ListIteratorNode* _filter;
  BeliefNode* _disallowed;
  std::string _name;

public:
  FilterRejectNode( const std::string& name, ListIteratorNode* filter, 
		    BeliefNode* disallowed );

  double maximumBackgroundScore();
  double maximumScore();
  bool hasMatch( int documentID );
  const greedy_vector<ScoredExtentResult>& score( int documentID, int start, int end, int documentLength );
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_FILTERREJECTNODE_HPP

