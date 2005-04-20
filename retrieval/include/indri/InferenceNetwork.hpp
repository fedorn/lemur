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
// InferenceNetwork
//
// 24 February 2004 -- tds
//

#ifndef INDRI_INFERENCENETWORK_HPP
#define INDRI_INFERENCENETWORK_HPP

#include "indri/BeliefNode.hpp"
#include "indri/EvaluatorNode.hpp"
#include "indri/ListIteratorNode.hpp"
#include "indri/TermScoreFunction.hpp"
#include "indri/Repository.hpp"
#include "indri/Index.hpp"
#include "indri/DeletedDocumentList.hpp"

namespace indri
{
  namespace infnet 
  {
    class InferenceNetwork {
    public:
      typedef std::map< std::string, EvaluatorNode::MResults > MAllResults;

      //
      // MAllResults stores results indexed first by node name, then second by the node's 
      // result name.  For instance, to retrieve occurrence counts from a 
      // ContextCountAccumulator named "bd45a", you'd type 'results["bd45a"]["occurrences"]'.
      //

    private:
      std::vector<std::string> _termNames;
      std::vector<std::string> _fieldNames;

      std::vector<class indri::index::DocExtentListIterator*> _fieldIterators;
      std::vector<class indri::index::DocListIterator*> _docIterators;
      std::vector<ListIteratorNode*> _listIteratorNodes;
      std::vector<BeliefNode*> _beliefNodes;
      std::vector<EvaluatorNode*> _evaluators;
      std::vector<EvaluatorNode*> _complexEvaluators;
      std::vector<indri::query::TermScoreFunction*> _scoreFunctions;

      indri::utility::greedy_vector<class indri::index::DocListIterator*> _closeIterators;
      int _closeIteratorBound;

      indri::collection::Repository& _repository;
      MAllResults _results;

      void _indexChanged( indri::index::Index& index );
      void _indexFinished( indri::index::Index& index );

      void _moveToDocument( int candidate );
      void _moveDocListIterators( int candidate );

      int _nextCandidateDocument( indri::index::DeletedDocumentList::read_transaction* deleted );
      void _evaluateDocument( indri::index::Index& index, int document );
      void _evaluateIndex( indri::index::Index& index );

    public:
      InferenceNetwork( indri::collection::Repository& repository );
      ~InferenceNetwork();

      const std::vector<EvaluatorNode*>& getEvaluators() const;

      indri::index::DocListIterator* getDocIterator( int index );
      indri::index::DocExtentListIterator* getFieldIterator( int index );

      int addDocIterator( const std::string& term );
      int addFieldIterator( const std::string& field );
      void addListNode( ListIteratorNode* listNode );
      void addBeliefNode( BeliefNode* beliefNode );
      void addEvaluatorNode( EvaluatorNode* evaluatorNode );
      void addComplexEvaluatorNode( EvaluatorNode* complexEvaluator );
      void addScoreFunction( indri::query::TermScoreFunction* scoreFunction );
      const MAllResults& evaluate();
    };
  }
}

#endif // INDRI_INFERENCENETWORK_HPP

