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
// ShrinkageBeliefNode
//
// 29 June 2005 - pto
//

#ifndef INDRI_SHRINKAGEBELIEFNODE_HPP
#define INDRI_SHRINKAGEBELIEFNODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/TermScoreFunction.hpp"
#include "indri/greedy_vector"
#include "indri/ScoredExtentResult.hpp"
#include "indri/DocumentStructureHolderNode.hpp"
#include <vector>
#include <set>
#include <map>

namespace indri
{
  namespace infnet
  {
    
    class ShrinkageBeliefNode : public BeliefNode {
    protected:
      ListIteratorNode& _list;
      DocumentStructureHolderNode& _docStructHolder;
      double _maximumScore;
      double _maximumBackgroundScore;
      indri::query::TermScoreFunction& _scoreFunction;
      indri::utility::greedy_vector<indri::api::ScoredExtentResult> _results;
      indri::utility::greedy_vector<bool> _matches;
      std::string _name;

      std::vector<double> _down;
      std::vector<double> _up;
      std::vector<double> _base;
      std::vector<double> _counts;
      int _documentID;


      typedef struct sr {
	std::string fieldName;
	double weight;
	bool lengthProportional;	
      } smoothing_rule;

      struct lt_rule {
	bool operator()( smoothing_rule r1, smoothing_rule r2 ) const {
	  int cmpVal = strcmp( r1.fieldName.c_str(), r2.fieldName.c_str() );
	  if ( cmpVal != 0 ) {
	    return cmpVal < 0;
	  }
	  if ( r1.weight != r2.weight ) {
	    return r1.weight < r2.weight;
	  } 
	  if ( r1.lengthProportional != r2.lengthProportional) {
	    return r1.lengthProportional;
	  }
	  return false;
	}
      };

      std::set<smoothing_rule, lt_rule> _ruleSet;
      std::map<int, smoothing_rule> _ruleMap;

      double _parentWeight;
      double _docWeight;
      bool _recursive;
      bool _queryLevelCombine;
      

    public:
      ShrinkageBeliefNode( const std::string& name,
			   ListIteratorNode& child,
			   DocumentStructureHolderNode& documentStructureHolderNode,
			   indri::query::TermScoreFunction& scoreFunction,
			   double maximumBackgroundScore,
			   double maximumScore );

      int nextCandidateDocument();
      void indexChanged( indri::index::Index& index );
      double maximumBackgroundScore();
      double maximumScore();
      const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
      void annotate( class Annotator& annotator, int documentID, int begin, int end );
      bool hasMatch( int documentID );
      const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents );
      const std::string& getName() const;
      void addShrinkageRule( std::string rule );
    };
  }
}

#endif // INDRI_SHRINKAGEBELIEFNODE_HPP
