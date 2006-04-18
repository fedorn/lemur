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
// ExtentChildNode
//
// 31 Jan 2006 -- pto
//

#include "indri/ExtentChildNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

indri::infnet::ExtentChildNode::ExtentChildNode( const std::string& name, 
                                                           ListIteratorNode* inner, 
                                                           ListIteratorNode* outer,
                                                           DocumentStructureHolderNode & documentStructureHolderNode ) :
  _inner(inner),
  _outer(outer),
  _docStructHolder(documentStructureHolderNode),
  _name(name)
{
}

void indri::infnet::ExtentChildNode::prepare( int documentID ) {
  _extents.clear();

  if( !_inner || !_outer )
    return;

  const indri::utility::greedy_vector<indri::index::Extent>& inExtents = _inner->extents();
  const indri::utility::greedy_vector<indri::index::Extent>& outExtents = _outer->extents();

  indri::utility::greedy_vector<indri::index::Extent>::const_iterator innerIter = inExtents.begin();
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator outerBegin = outExtents.begin();
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator outerEnd = outExtents.end();

  indri::index::DocumentStructure * docStruct = _docStructHolder.getDocumentStructure();  

  // check the inner extents, searching for a parent in outerNodes
  while ( innerIter != inExtents.end() ) {

    if ( innerIter->ordinal == 0 ) {
      _leafs.clear();
      docStruct->findLeafs( &_leafs, innerIter->begin, innerIter->end, true );

      std::set<int>::iterator leafsEnd = _leafs.end();
      std::set<int>::iterator leaf = _leafs.begin();

      bool found = false;
      while ( leaf != leafsEnd && !found) {
        
        indri::utility::greedy_vector<indri::index::Extent>::const_iterator outerIter = outerBegin;
        while ( outerIter != outerEnd && !found ) {

          if ( outerIter->ordinal == 0 ) {
            _ancestors.clear();
            docStruct->findLeafs( &_ancestors, outerIter->begin, outerIter->end, true );
            std::set<int>::iterator ancestor = _ancestors.begin();
            std::set<int>::iterator ancestorsEnd = _ancestors.end();
            while ( ancestor != ancestorsEnd && !found ) {
              if ( *ancestor == docStruct->parent( *leaf ) ) {
                found = true;
                indri::index::Extent extent( innerIter->weight * outerIter->weight, 
                                             innerIter->begin,
                                             innerIter->end,
                                             *leaf );     
                
                _extents.push_back( extent );
              }
              ancestor++;
            }
          } else {

            if ( outerIter->ordinal == docStruct->parent( *leaf ) ) {
              indri::index::Extent extent( innerIter->weight * outerIter->weight, 
                                           innerIter->begin,
                                           innerIter->end,
                                           *leaf );    
              _extents.push_back( extent );
            }
          }
          outerIter++;
        } 
        leaf++;
      }

    } else {
      bool found = false;
      indri::utility::greedy_vector<indri::index::Extent>::const_iterator outerIter = outerBegin;
      while ( outerIter != outerEnd && !found ) {
        
        if ( outerIter->ordinal == 0 ) {
          _ancestors.clear();
          docStruct->findLeafs( &_ancestors, outerIter->begin, outerIter->end, true );
          std::set<int>::iterator ancestor = _ancestors.begin();
          std::set<int>::iterator ancestorsEnd = _ancestors.end();
          while ( ancestor != ancestorsEnd && !found ) {
            if ( *ancestor == docStruct->parent( innerIter->ordinal ) ) {
              found = true;
              indri::index::Extent extent( innerIter->weight * outerIter->weight, 
                                           innerIter->begin,
                                           innerIter->end,
                                           innerIter->ordinal );     
                
              _extents.push_back( extent );
            }
            ancestor++;
          }
        } else {
          if ( outerIter->ordinal == docStruct->parent( innerIter->ordinal ) ) {
            indri::index::Extent extent( innerIter->weight * outerIter->weight, 
                                         innerIter->begin,
                                         innerIter->end,
                                         innerIter->ordinal );    
            _extents.push_back( extent );
          }
        }
        outerIter++;
      } 
    }
    innerIter++;
  }

}

const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::ExtentChildNode::extents() {
  return _extents;
}

int indri::infnet::ExtentChildNode::nextCandidateDocument() {
  return lemur_compat::max( _inner->nextCandidateDocument(), _outer->nextCandidateDocument() );
}

const std::string& indri::infnet::ExtentChildNode::getName() const {
  return _name;
}

void indri::infnet::ExtentChildNode::annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  annotator.addMatches( _extents, this, documentID, extent );

  indri::index::Extent range( extent.begin, extent.end );
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
  iter = std::lower_bound( _extents.begin(), _extents.end(), range, indri::index::Extent::begins_before_less() );
  
  for( unsigned int i = iter-_extents.begin(); i<_extents.size() && _extents[i].begin <= extent.end; i++ ) {
    _inner->annotate( annotator, documentID, (indri::index::Extent &)_extents[i] );
    _outer->annotate( annotator, documentID, (indri::index::Extent &)_extents[i] );
  }
}

void indri::infnet::ExtentChildNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}


const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::ExtentChildNode::matches( indri::index::Extent &extent ) {

  _matches.clear();
  
  int begin = extent.begin;
  int end = extent.end;
  
  indri::index::DocumentStructure * docStruct = _docStructHolder.getDocumentStructure();  

  if ( extent.ordinal == 0 ) {
    _ancestors.clear();
    docStruct->findLeafs( &_ancestors, begin, end, true);

    std::set<int>::iterator ancestorsBegin = _ancestors.begin();
    std::set<int>::iterator ancestorsEnd = _ancestors.end();

    for( size_t i = 0; i < _extents.size(); i++ ) {    

      bool match = false;
      int innerBegin = _extents[i].begin;
      int innerEnd = _extents[i].end;
      
      if ( _extents[i].ordinal == 0 ) {
              _leafs.clear();
              docStruct->findLeafs( &_leafs, innerBegin, innerEnd, true);

              std::set<int>::iterator leafsBegin = _leafs.begin();
              std::set<int>::iterator leafsEnd = _leafs.end();

              std::set<int>::iterator ancestor = ancestorsBegin;
              while ( !match && ancestor != ancestorsEnd ) { 
                std::set<int>::iterator leaf = leafsBegin;
        
                while ( !match && leaf != leafsEnd ) {
                  if ( *ancestor == docStruct->parent(*leaf) ) {
                    match = true;
                  }
                  leaf++;
                } 
                ancestor++;
              }      
      } else {
              std::set<int>::iterator ancestor = ancestorsBegin;
              while ( !match && ancestor != ancestorsEnd ) { 
                if ( *ancestor == docStruct->parent( _extents[i].ordinal ) ) {
                  match = true;
                }
                ancestor++;
              }
      }
      if ( match ) {
            _matches.push_back(_extents[i]);
      }
    }

  } else {

    for( size_t i = 0; i < _extents.size(); i++ ) {    
      int innerBegin = _extents[i].begin;
      int innerEnd = _extents[i].end;
      
      bool match = false;
      if ( _extents[i].ordinal == 0 ) {
              _leafs.clear();
              docStruct->findLeafs( &_leafs, innerBegin, innerEnd, true);
        
              std::set<int>::iterator leafsBegin = _leafs.begin();
              std::set<int>::iterator leafsEnd = _leafs.end();  
              std::set<int>::iterator leaf = leafsBegin;
        
              while ( !match && leaf != leafsEnd ) {
                if ( extent.ordinal == docStruct->parent(*leaf) ) {
                  match = true;
                }
                leaf++;
              }             
      } else {
              if ( extent.ordinal == docStruct->parent( _extents[i].ordinal ) ) {
                match = true;
              }      
      }
      if ( match ) {
        _matches.push_back(_extents[i]);
      }
    }
  }

  return _matches;
}
