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
// FileTreeIterator
//
// 26 May 2004 -- tds
//

#include "indri/FileTreeIterator.hpp"
#include "indri/Path.hpp"
#include <iostream>

// static construction
FileTreeIterator FileTreeIterator::_end;

FileTreeIterator::FileTreeIterator() {
}

FileTreeIterator::FileTreeIterator( const std::string& path ) {
  _stack.push( new DirectoryIterator( path ) );

  while( Path::isDirectory( *(*_stack.top()) ) ) {
    _stack.push( new DirectoryIterator( **_stack.top() ) );
  }
}

FileTreeIterator::~FileTreeIterator() {
  while( _stack.size() ) {
    delete _stack.top();
    _stack.pop();
  }
}

void FileTreeIterator::_nextCandidate() {
  // go to the next file.  If the current directory is complete,
  // go up levels until we find a directory with stuff left in it
  while( _stack.size() ) {
    DirectoryIterator& top = (*_stack.top());
    top++;

    if( top == DirectoryIterator::end() ) {
      delete _stack.top();
      _stack.pop();
    } else {
      break;
    }
  }
}

void FileTreeIterator::_next() {
  _nextCandidate();

  // need to make sure we've found a file
  while( _stack.size() ) {
    DirectoryIterator& top = (*_stack.top());
    
    if( top == DirectoryIterator::end() ) {
      _nextCandidate();
      continue;
    } 

    if( Path::isFile( *top ) ) {
      // found a file, so we're done
      break;
    }

    // have to recurse
    DirectoryIterator* child = new DirectoryIterator( *top );
    _stack.push(child);
  }
}

void FileTreeIterator::operator ++ ( int ) {
  _next();
}

void FileTreeIterator::operator ++ () {
  _next();
}

const std::string& FileTreeIterator::operator* () {
  DirectoryIterator& top = (*_stack.top());
  return *top;
}

bool FileTreeIterator::operator== ( const FileTreeIterator& other ) const {
  return ( &other == &_end ) && ( _stack.size() == 0 );
}

bool FileTreeIterator::operator!= ( const FileTreeIterator& other ) const {
  return ! this->operator== ( other );
}

const FileTreeIterator& FileTreeIterator::end() {
  return _end;
}
