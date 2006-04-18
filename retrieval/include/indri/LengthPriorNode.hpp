/*==========================================================================
 * Copyright (c) 2006 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// LengthPriorNode
// 
// 9 July 2005 -- pto
//

// Implements an extent length based prior of the form  P(extent) proportional to (end - begin) ^ exponent.
// In terms of actual ranking, exponent * log ( end - begin ) is computed, where INDRI_TINY_SCORE is 
// used for extents of length 0.

#ifndef INDRI_LENGTHPRIORNODE_HPP
#define INDRI_LENGTHPRIORNODE_HPP

#include "indri/BeliefNode.hpp"

namespace indri
{
  namespace infnet
  {
    
    class LengthPriorNode : public BeliefNode {
    private:
      indri::utility::greedy_vector<indri::api::ScoredExtentResult> _scores;
      BeliefNode * _child;
      std::string _name;
      double _exponent;

    public:
      LengthPriorNode( const std::string& name, BeliefNode * child, double exponent );
      ~LengthPriorNode();

      int nextCandidateDocument();
      void indexChanged( indri::index::Index& index );

      bool hasMatch( int documentID );
      const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents );
      const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, indri::index::Extent &extent, int documentLength );
      void annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent );
      double maximumScore();
      double maximumBackgroundScore();
      const std::string& getName() const;
    };
  }
}

#endif // INDRI_LENGTHPRIORNODE_HPP

