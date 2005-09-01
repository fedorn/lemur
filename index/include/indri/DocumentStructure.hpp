


#ifndef INDRI_DOCUMENTSTRUCTURE_HPP
#define INDRI_DOCUMENTSTRUCTURE_HPP

#include "indri/FieldExtent.hpp"
#include "indri/greedy_vector"
#include <sstream>
#include <vector>

typedef struct ninf {
  int id;
  int parent;
  int type;

  int begin;
  int end;
  
  int length;
  int totalLength;

  int numChildren;
  int children;

} nodeinfo_t;


namespace indri {
  namespace index {
    
    class Index;

class DocumentStructure {

public:

  DocumentStructure(Index & index, const indri::utility::greedy_vector<indri::index::FieldExtent> & fields) 
    : _index( &index ) ,
      _numNodes( 0 ),
      _nodes( ),
      _childrenBuff( )
  {     
    loadStructure(fields);
  }

  DocumentStructure(Index & index) :
    _index( &index ) ,
    _numNodes( 0 ),
    _nodes( ),
    _childrenBuff( )
  { 
  }

  DocumentStructure() :
    _index( 0 ) ,
    _numNodes( 0 ),
    _nodes( ),
    _childrenBuff( )
  { 
  }

  void loadStructure(const indri::utility::greedy_vector<indri::index::FieldExtent> & fields);
  void setIndex( indri::index::Index & index );

  ~DocumentStructure();

  int parent(int node);
  int length(int node);
  int begin(int node);
  int end(int node);
  int accumulatedLength(int node);
  int type(int node);

  typedef indri::utility::greedy_vector<int>::iterator child_iterator;
  child_iterator childrenBegin( int node );
  child_iterator childrenEnd( int node );
  

  int nodeCount() { return _numNodes; }


  int findLeaf(int begin, int end);

  // returns true if node == possibleAncestor or
  // if possibleAncestor is a parent or parent's parent ... of node
  bool ancestor(int node, int possibleAncestor);

  std::string path(int node);
  int fieldId( const std::string path );


protected:

  int _numNodes;

  indri::utility::greedy_vector<nodeinfo_t> _nodes;
  indri::utility::greedy_vector<int> _childrenBuff;
  
  Index * _index;
  void _constructNodePath(std::stringstream & path, int node);
  
};

  }
}
#endif
