

/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "indri/DocumentStructure.hpp"
#include "indri/Index.hpp"

void
indri::index::DocumentStructure::setIndex( indri::index::Index & index ) {
  _index = &index;
}

void
indri::index::DocumentStructure::loadStructure(const indri::utility::greedy_vector<indri::index::FieldExtent> & fields) {

  _numNodes = fields.size();

  _nodes.resize( _numNodes );
  _childrenBuff.resize( _numNodes-1 );

  int i;

  // copy values from field list, 
  for (i = 0; i < _numNodes; i++) {

    int type = fields[i].id;
    int begin = fields[i].begin;
    int end = fields[i].end;
    int length =  end - begin;

    // Node ids are from 1 to _numNodes, but we index them in the 
    // _nodes vector by id - 1
    int id = i+1;
    

    _nodes[i].begin = begin;
    _nodes[i].end = end;
    _nodes[i].id = id;
    _nodes[i].type = type;
    _nodes[i].length = length;
    _nodes[i].totalLength = length;
    _nodes[i].numChildren = 0;

  }
  // establish parent nodes and count children for each node
  for (i = 0; i < _numNodes; i++) {
    int parent = i;
    if (parent > 1) {
      while ( _nodes[parent-1].end <= _nodes[i].begin) {
	if ( _nodes[parent-1].begin <= _nodes[i].begin &&
	     _nodes[parent-1].end >= _nodes[i].end ) break;
	parent = _nodes[parent-1].parent;
      }
    }
    
    _nodes[i].parent = parent;

    // count number of children
    if (parent > 0) {
      _nodes[parent-1].numChildren++;
    }
  }

  // Divy up the children buffer array, and set the children.
  // This also assumes that the parent ids are always smaller than
  // their childrens' ids.
  int cbuffLoc = 0;
  for (i = 0; i < _numNodes; i++ ) {
    if (_nodes[i].numChildren == 0) {
      _nodes[i].children = -1;
    } else {
      _nodes[i].children = cbuffLoc;
      cbuffLoc += _nodes[i].numChildren;
      // reset this counter as we will use it when building the children array
      _nodes[i].numChildren = 0;
    }
    if ( _nodes[i].parent > 0 ) {
      int p = _nodes[i].parent;
      _childrenBuff[ _nodes[p-1].children + _nodes[p-1].numChildren ] = _nodes[i].id;
      _nodes[p-1].numChildren++;
    }
  }

  // subtract away the children total lengths to get the node length
  for (i = 1; i <= _numNodes; i++ ) {
    // int * child = node->children;
    // int * childrenEnd = child + node->numChildren;
    child_iterator child = childrenBegin( i );
    child_iterator childEnd = childrenEnd( i );      
    while (child < childEnd) {
      _nodes[i-1].length -= _nodes[(*child)-1].totalLength;
      child++;
    }
  }
}

indri::index::DocumentStructure::~DocumentStructure() {
}

int 
indri::index::DocumentStructure::parent(int node) {
  if (node > 0 && node <= _numNodes) {
    return _nodes[node-1].parent;
  }
  // cerr << "DocumentStructure: tried to get parent of non-existant node." << endl;
  return 0;
}


int 
indri::index::DocumentStructure::length(int node) {
  if (node > 0 && node <= _numNodes) {
    return _nodes[node-1].length;
  }
  // cerr << "DocumentStructure: tried to get length of non-existant node." << endl;
  return 0;
}


int 
indri::index::DocumentStructure::begin(int node) {
  if (node > 0 && node <= _numNodes) {
    return _nodes[node-1].begin;
  }
  // cerr << "DocumentStructure: tried to get beginning location of non-existant node." << endl;
  return 0;
}

int 
indri::index::DocumentStructure::end(int node) {
  if (node > 0 && node <= _numNodes) {
    return _nodes[node-1].end;
  }
  // cerr << "DocumentStructure: tried to get ending location of non-existant node." << endl;
  return 0;
}


int 
indri::index::DocumentStructure::accumulatedLength(int node) {
  if (node > 0 && node <= _numNodes) {
    return _nodes[node-1].totalLength;
  }
  // cerr << "DocumentStructure: tried to get accumulated length of non-existant node." << endl;
  return 0;
}


int
indri::index::DocumentStructure::type(int node) {
  if (node > 0 && node <= _numNodes) {
    return _nodes[node-1].type;
  }
  // cerr << "DocumentStructure: tried to get type of non-existant node." << endl;
  return 0;
}


indri::index::DocumentStructure::child_iterator
indri::index::DocumentStructure::childrenBegin( int node ) {
  if ( node > 0 && node <= _numNodes ) {
    return _childrenBuff.begin() + _nodes[node-1].children;
  } 
  return _childrenBuff.end();
}

indri::index::DocumentStructure::child_iterator
indri::index::DocumentStructure::childrenEnd( int node ) {
  if ( node > 0 && node <= _numNodes ) {
    return _childrenBuff.begin() + _nodes[node-1].children + _nodes[node-1].numChildren;
  } 
  return _childrenBuff.end();
}

bool 
indri::index::DocumentStructure::ancestor(int node, int possibleAncestor) {
  bool anc = false;
  if (node == possibleAncestor) {
    anc = true;
  } else if (node < possibleAncestor) {
    anc = false;
  } else {    
    anc = ancestor(parent(node), possibleAncestor);
  }
  return anc;
}

int
indri::index::DocumentStructure::findLeaf(int b, int e) {


  int leaf = 0;
  // start at root of tree and work way down to leaf
  if ( begin(1) <= b && end(1) >= e ) {
    leaf = 1;
  } 
  int numChildren;
  child_iterator child = childrenBegin( 1 );
  child_iterator childEnd = childrenEnd( 1 );
  while (child < childEnd) {

    if ( begin(*child) <= b && end(*child) >= e ) {
      leaf = *child;
      child = childrenBegin( leaf );
      childEnd = childrenEnd( leaf );
      
    } else {
      child++;
    }
  }
  
  return leaf;
}

std::string
indri::index::DocumentStructure::path(int node) {
  std::stringstream path;
  
  _constructNodePath(path, node);

  return path.str();
}

void
indri::index::DocumentStructure::_constructNodePath(std::stringstream & path, int node) {

  int pid = parent(node);
  if (pid == 0) {
    path << "/" << _index->field( type(node) ) << "[1]";
  } else {

    _constructNodePath(path, pid);

    child_iterator kids = childrenBegin( pid );
    child_iterator kidsEnd = childrenEnd( pid );

    int nodeType = type(node);    
    int sameType = 0;
    int sameTypeLoc = 0;

    while ( kids < kidsEnd ) {
      int sid = *kids;
      if (type(sid) == nodeType) {
	sameType++;
	if (sid == node) {
	  sameTypeLoc = sameType;
	}
      }
      kids++;
    }

    path << "/" << _index->field( nodeType );

    path << "[" << sameTypeLoc << "]";

  }
}

int
indri::index::DocumentStructure::fieldId( const std::string path ) {
  int loc = 0;
  int node = 0;

  child_iterator kids = childrenBegin( 0 );
  child_iterator kidsEnd = childrenEnd( 0 );
  int numKids = 1;

  
  while ( loc >= 0 && loc < path.size() ) {
  
    // seek to first path name
    int typeBegin = path.find_first_not_of( "/", loc );
    // find end of path name
    int typeEnd = path.find_first_of( "[" , typeBegin );
    // extract out the path name
    int typeLength = typeEnd - typeBegin;
    const std::string typeString = path.substr( typeBegin, typeLength );
    int typeId = _index->field( typeString );
    
    // extract count
    int countBegin = path.find_first_not_of( "[", typeEnd );
    int countEnd = path.find_first_of( "]" , typeBegin );
    int countLength = countEnd - countBegin;
    const std::string countString = path.substr( countBegin, countLength );
    int count = 0;
    sscanf( countString.c_str(), "%d", &count );
  
    // find the node
    
    int seen = 0;
    if ( node == 0 ) {
      if ( type( 1 ) == typeId ) {
	seen++;
      } 
      if ( count == seen ) {
	node = 1;
      }
    } else {
      while( kids < kidsEnd ) {
	if( type( *kids ) == typeId ) {
	  seen++;
	}
	if ( count == seen ) {
	  node = *kids;
	  break;
	}
	kids++;
      }
    }
  
    if( count != seen ) {
      std::cerr << "Unable to find child" << std::endl;
    }
    
    // set the children
    kids = childrenBegin( node );
    kidsEnd = childrenEnd( node );
    
    loc = path.find_first_of( "/", countEnd );
  }
  return node;

}
