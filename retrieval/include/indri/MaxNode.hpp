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
// MaxNode
//
// 1 April 2004 -- tds
//

#ifndef INDRI_MAXNODE_HPP
#define INDRI_MAXNODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/ScoredExtentResult.hpp"
#include "indri/Annotator.hpp"
namespace indri
{
  namespace infnet
  {
    
    class MaxNode : public BeliefNode {
    private:
      std::vector<BeliefNode*> _children;
      indri::utility::greedy_vector<indri::api::ScoredExtentResult> _scores;
      indri::utility::greedy_vector<bool> _matches;
      std::string _name;

    public:
      MaxNode( const std::string& name ) : _name(name) {}
      MaxNode( const std::string& name, const std::vector<BeliefNode*>& children ) :
        _children( children ),
        _name( name )
      {
        // if we have children - set the sibling flag...
        if (_children.size() > 1) {
          for (int i=0; i < _children.size(); i++) {
            if (_children[i]) {
              _children[i]->setSiblingsFlag(1);
            }
          }
        }
      }

      int nextCandidateDocument() {
        int candidate = MAX_INT32;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          candidate = lemur_compat::min<int>( candidate, _children[i]->nextCandidateDocument() );
        }

        return candidate;
      }

      indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, indri::index::Extent &extent, int documentLength ) {
        double maxScore = INDRI_TINY_SCORE;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& childResults = _children[i]->score( documentID, extent, documentLength );

          for( unsigned int j=0; j<childResults.size(); j++ ) {
            maxScore = lemur_compat::max<double>( maxScore, childResults[j].score );
          }
        }

        _scores.clear();
        _scores.push_back( indri::api::ScoredExtentResult( maxScore, documentID, extent.begin, extent.end ) );

        return _scores;
      }

      void annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent ) {
        annotator.add( this, documentID, extent);

        // find the maximum score here, then descend only into that one
        double maxScore = INDRI_TINY_SCORE;
        int maxI = -1;
        int maxJ = -1;
        int maxBegin = -1;
        int maxEnd = -1;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& childResults = _children[i]->score( documentID, extent, extent.end);

          for( unsigned int j=0; j<childResults.size(); j++ ) {
            if ( maxScore < childResults[j].score ) {
              maxScore = childResults[j].score;
              maxI = i;
              maxJ = j;
              maxBegin = childResults[j].begin;
              maxEnd = childResults[j].end;
            }
          }
        }

        indri::index::Extent childExtent;
        childExtent.begin=maxBegin;
        childExtent.end=maxEnd;
        _children[maxI]->annotate( annotator, documentID, childExtent );
      }

      double maximumScore() {
        double maxScore = INDRI_TINY_SCORE;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          maxScore = lemur_compat::max<double>( maxScore, _children[i]->maximumScore() );  
        }

        return maxScore;
      }

      double maximumBackgroundScore() {
        double maxScore = INDRI_TINY_SCORE;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          maxScore = lemur_compat::max<double>( maxScore, _children[i]->maximumBackgroundScore() );  
        }

        return maxScore;
      }

      bool hasMatch( int documentID ) {
        for( unsigned int i=0; i<_children.size(); i++ ) {
          if( _children[i]->hasMatch( documentID ) )
            return true;
        }

        return false;
      }

      const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
        _matches.clear();
        _matches.resize( extents.size(), false );

        for( unsigned int i=0; i<_children.size(); i++ ) {
          const indri::utility::greedy_vector<bool>& kidMatches = _children[i]->hasMatch( documentID, extents );

          for( unsigned int j=0; j<kidMatches.size(); j++ ) {
            if( kidMatches[j] ) {
              _matches[j] = true;
            }
          }
        }

        return _matches;
      }

      void indexChanged( indri::index::Index& index ) {
        // do nothing
      }

      const std::string& getName() const {
        return _name;
      }
    };
  }
}

#endif // INDRI_MAXNODE_HPP

