/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// DirectoryIterator
//
// 26 May 2004 -- tds
//

#include "indri/DirectoryIterator.hpp"
#include "lemur-platform.h"
#include <assert.h>
#include "indri/Path.hpp"
#include <vector>
#include <iostream>

// static construction
DirectoryIterator DirectoryIterator::_end;

void* directoryiterator_init( const std::string& path );
void directoryiterator_destroy(void* opaque);
std::string directoryiterator_current();
bool directoryiterator_next(void* opaque);
bool directoryiterator_done(void* opaque);


#ifdef LEMUR_USING_FINDFIRST

//
// Windows platform-specific directory iterator code
//

struct win_iter_data {
  HANDLE handle;
  WIN32_FIND_DATA data;
  bool done;
};

void* directoryiterator_init( const std::string& path ) {
  win_iter_data* d = new win_iter_data;
  std::string searchPath = Path::combine( path, "*" );

  d->handle = ::FindFirstFile( searchPath.c_str(), &d->data );
  d->done = (d->handle == INVALID_HANDLE_VALUE);

  return d;
}

void directoryiterator_destroy( void* opaque ) {
  win_iter_data* d = (win_iter_data*) opaque;
  ::FindClose( d->handle );
  delete d;
}

std::string directoryiterator_current( void* opaque ) {
  assert( opaque );
  win_iter_data* d = (win_iter_data*) opaque;
  return d->data.cFileName;
}

bool directoryiterator_next( void* opaque ) {
  assert( opaque );
  win_iter_data* d = (win_iter_data*) opaque;

  if( !d->done )
    d->done = (::FindNextFile( d->handle, &d->data ) != TRUE);

  return d->done;
}

bool directoryiterator_done( void* opaque ) {
  assert( opaque );
  return ( (win_iter_data*) opaque )->done;
}

#else

#include <sys/types.h>
#include <dirent.h>
#include <limits.h>

//
// Linux/Unix platform specific directory iterator code
//

struct unix_iter_data {
  DIR* directory;
  bool done;
  struct dirent* entry;
};

void* directoryiterator_init( const std::string& path ) {
  unix_iter_data* d = new unix_iter_data;
  d->directory = opendir( path.c_str() );
  d->done = (d->directory == 0);
  d->entry = (struct dirent*) malloc( sizeof(struct dirent) + PATH_MAX );

  if( !d->done )
    directoryiterator_next(d);

  return d;
}

void directoryiterator_destroy( void* opaque ) {
  unix_iter_data* d = (unix_iter_data*) opaque;
  closedir( d->directory );
  free( d->entry );
  delete d;
}

std::string directoryiterator_current( void* opaque ) {
  unix_iter_data* d = (unix_iter_data*) opaque;
  return d->entry->d_name;
}

bool directoryiterator_next( void* opaque ) {
  unix_iter_data* d = (unix_iter_data*) opaque;
  struct dirent* result = 0;
  readdir_r( d->directory, d->entry, &result );  
  d->done = ( result ? false : true );
  return d->done;
}

bool directoryiterator_done( void* opaque ) {
  unix_iter_data* d = (unix_iter_data*) opaque;
  return d->done;
}

#endif // LEMUR_USING_FINDFIRST

//
// Begin platform independent code
//

const DirectoryIterator& DirectoryIterator::end() {
  return _end;
}

void DirectoryIterator::_copyCurrent() {
  if( _relative ) {
    _current = Path::combine( _path, directoryiterator_current( _platform ) );
  } else {
    _current = directoryiterator_current( _platform );
  }

  _current = Path::trim( _current );
}

void DirectoryIterator::_next() {
  directoryiterator_next( _platform );
  _copyCurrent();

  std::string current = directoryiterator_current( _platform );

  if( !directoryiterator_done( _platform ) &&
      (current == "." || current == "..") ) {
    _next();
  }
}

DirectoryIterator::DirectoryIterator() :
  _relative(false),
  _platform(0)
{
}

DirectoryIterator::DirectoryIterator( const std::string& path, bool relative ) :
  _relative(relative)
{
  _path = Path::trim( path );
  _platform = directoryiterator_init( _path );

  std::string current = directoryiterator_current( _platform );

  if( !directoryiterator_done( _platform ) &&
      (current == "." || current == "..") ) {
    _next();
  }

  _copyCurrent();
}

DirectoryIterator::~DirectoryIterator() {
  close();
}

const std::string& DirectoryIterator::base() const {
  return _path;
}

void DirectoryIterator::close() {
  if( _platform ) {
    directoryiterator_destroy( _platform );
    _platform = 0;
  }
}

void DirectoryIterator::operator ++ () {
  _next();
}

void DirectoryIterator::operator ++ (int) {
  _next();
}

bool DirectoryIterator::operator == ( const DirectoryIterator& other ) {
  // this is a hack, but I think it's decent: we assume
  // the user is comparing against end() in an iteration sense
  return directoryiterator_done( _platform ) && &other == &_end;
}

const std::string& DirectoryIterator::operator * () {
  return _current;
}
