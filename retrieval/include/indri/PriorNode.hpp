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
// PriorNode
//
// 27 April 2004 -- tds
//

#ifndef INDRI_PRIORNODE_HPP
#define INDRI_PRIORNODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/FieldIteratorNode.hpp"
#include "indri/QuerySpec.hpp"
namespace indri
{
  namespace infnet
  {
    
    class PriorNode : public BeliefNode {
    private:
      FieldIteratorNode* _field;
      std::map<int, indri::lang::PriorNode::tuple_type> _table;
      indri::utility::greedy_vector<bool> _matches;
      indri::utility::greedy_vector<indri::api::ScoredExtentResult> _scores;
      std::string _name;

    public:
      PriorNode( const std::string& name, FieldIteratorNode* field, const std::map<int, indri::lang::PriorNode::tuple_type>& table );
      ~PriorNode();

      int nextCandidateDocument();
      void indexChanged( indri::index::Index& index );

      bool hasMatch( int documentID );
      const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents );
      const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
      void annotate( class Annotator& annotator, int documentID, int begin, int end );
      double maximumScore();
      double maximumBackgroundScore();
      const std::string& getName() const;
    };
  }
}

#endif // INDRI_PRIORNODE_HPP

