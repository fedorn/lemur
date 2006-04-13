
//
// ExtentChildNode
//
// 31 Jan 2006 - pto
//
//
// Checks to make sure the inner node has an extent that matches the extent
// of a child of nodes whose extents matching the outer extent
//

#ifndef INDRI_EXTENTCHILDNODE_HPP
#define INDRI_EXTENTCHILDNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/DocumentStructureHolderNode.hpp"
#include "indri/greedy_vector"
#include "indri/Extent.hpp"
namespace indri
{
  namespace infnet
  {
    
    class ExtentChildNode : public ListIteratorNode {
      ListIteratorNode* _inner;
      ListIteratorNode* _outer;
      DocumentStructureHolderNode & _docStructHolder;
      indri::utility::greedy_vector<indri::index::Extent> _extents;
      std::string _name;


      std::set<int> _ancestors;
      std::set<int> _leafs;


    public:
      ExtentChildNode( const std::string& name, 
			    ListIteratorNode* inner, 
			    ListIteratorNode* outer,
			    DocumentStructureHolderNode & documentStructureHolderNode  );

      void prepare( int documentID );
      const indri::utility::greedy_vector<indri::index::Extent>& extents();
      int nextCandidateDocument();
      void indexChanged( indri::index::Index& index );

      const std::string& getName() const;
      void annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent );

      const indri::utility::greedy_vector<indri::index::Extent>& matches( indri::index::Extent &extent );
    };
  }
}

#endif // INDRI_EXTENTCHILDNODE_HPP

