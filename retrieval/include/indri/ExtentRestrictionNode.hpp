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
// ExtentRestrictionNode
//
// 6 July 2004 -- tds
//

#ifndef INDRI_EXTENTRESTRICTIONNODE_HPP
#define INDRI_EXTENTRESTRICTIONNODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/FieldIteratorNode.hpp"
#include "indri/greedy_vector"
#include "indri/Extent.hpp"

class ExtentRestrictionNode : public BeliefNode {
private:
  BeliefNode* _child;
  ListIteratorNode* _field;
  greedy_vector<ScoredExtentResult> _scores;
  std::string _name;

public:
  ExtentRestrictionNode( const std::string& name, BeliefNode* child, ListIteratorNode* field );

  int nextCandidateDocument();
  double maximumScore();
  double maximumBackgroundScore();

  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
  bool hasMatch( int documentID );
  const std::string& getName() const;
};

#endif // INDRI_EXTENTRESTRICTIONNODE_HPP
