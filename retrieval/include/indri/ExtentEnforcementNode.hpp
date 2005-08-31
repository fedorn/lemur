

//
// ExtentEnforcementNode
//
// 31 Aug 2005 -- pto
//

// Behaves like extent restriction, but preserves the child's extents
// rather than using the field extents.  It merely enforces that
// the child extents be contained within the field extents.

#ifndef INDRI_EXTENTENFORCEMENTNODE_HPP
#define INDRI_EXTENTENFORCEMENTNODE_HPP

#include "indri/ExtentRestrictionNode.hpp"

namespace indri
{
  namespace infnet
  {
    
    class ExtentEnforcementNode : public BeliefNode {
    private:
      BeliefNode* _child;
      ListIteratorNode* _field;
      indri::utility::greedy_vector<indri::api::ScoredExtentResult> _scores;
      indri::utility::greedy_vector<bool> _matches;
      std::string _name;

    public:
      ExtentEnforcementNode( const std::string& name, BeliefNode* child, ListIteratorNode* field );

      int nextCandidateDocument();
      void indexChanged( indri::index::Index& index );

      double maximumScore();
      double maximumBackgroundScore();

      const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, int begin, int end, int documentLength );
      void annotate( class Annotator& annotator, int documentID, int begin, int end );
      bool hasMatch( int documentID );
      const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents );
      const std::string& getName() const;
    };
  }
}

#endif // INDRI_EXTENTENFORCEMENTNODE_HPP
