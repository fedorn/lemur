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
// FixedPassageNode
//
// 23 February 2005 -- tds
//

#ifndef INDRI_FIXEDPASSAGENODE_HPP
#define INDRI_FIXEDPASSAGENODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/FieldIteratorNode.hpp"
#include "indri/greedy_vector"
#include "indri/Extent.hpp"
namespace indri
{
  namespace infnet
  {
    
    class FixedPassageNode : public BeliefNode {
    private:
      BeliefNode* _child;
      indri::utility::greedy_vector<indri::api::ScoredExtentResult> _scores;
      indri::utility::greedy_vector<bool> _matches;
      indri::utility::greedy_vector<indri::index::Extent> _subextents;

      std::string _name;

      int _windowSize;
      int _increment;

    public:
      FixedPassageNode( const std::string& name, BeliefNode* child, int windowSize, int increment );

      int nextCandidateDocument();
      void indexChanged( indri::index::Index& index );

      double maximumScore();
      double maximumBackgroundScore();

      const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, indri::index::Extent &extent, int documentLength );
      void annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent );

      const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents );
      bool hasMatch( int documentID );
      const std::string& getName() const;
    };
  }
}


#endif // INDRI_FIXEDPASSAGENODE_HPP


