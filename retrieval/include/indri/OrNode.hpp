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
// OrNode
//
// 31 March 2004 -- tds
//
// Implements the InQuery #or node.
//
// Note that this class transforms the probabilities
// out of log space and back into log space, which
// could cause a (catastrophic) loss of precision.
//

#ifndef INDRI_ORNODE_HPP
#define INDRI_ORNODE_HPP

#include <math.h>
#include "indri/BeliefNode.hpp"
#include <vector>
#include "indri/greedy_vector"

class OrNode : public BeliefNode {
private:
  std::vector<BeliefNode*> _children;
  greedy_vector<ScoredExtentResult> _scores;
  std::string _name;

public:
  OrNode( const std::string& name );
  OrNode( const std::string& name, const std::vector<BeliefNode*>& children );

  const greedy_vector<ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
  double maximumScore();
  double maximumBackgroundScore();
  
  bool hasMatch( int documentID );
  int nextCandidateDocument();
  const std::string& getName() const;
};

#endif // INDRI_ORNODE_HPP

