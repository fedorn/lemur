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
// Path.cpp
//
// 20 May 2004 -- tds
//

#include <string>
#include "indri/Path.hpp"
#include "Exception.hpp"
#include "lemur-compat.hpp"
#include <sys/stat.h>
#include <errno.h>
#include <iostream>
#include <stack>
#include "indri/DirectoryIterator.hpp"
#include "indri/delete_range.hpp"
#include "indri/XMLNode.hpp"
#include "indri/XMLReader.hpp"

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

static int path_last_separator( const std::string& path ) {
  int i;

  // skip any trailing slashes
  for( i=path.size()-1; i>=0; i-- ) {
    if( path[i] != PATH_SEPARATOR )
      break;
  }

  for( ; i>=0; i-- ) {
    if( path[i] == PATH_SEPARATOR )
      break;
  }

  return i;
}

void Path::create( const std::string& path ) {
  if( lemur_compat::mkdir( path.c_str(), 0777 ) < 0 ) {
    if( errno == EACCES ) {
      LEMUR_THROW( LEMUR_IO_ERROR, "Couldn't create directory: '" + path + "' because of inadequate permissions." );
    } else if( errno == ENOENT ) {
      LEMUR_THROW( LEMUR_IO_ERROR, "Couldn't create directory: '" + path + "' because at least one of the parent directories does not exist." );
    } else if( errno == EEXIST ) {
      LEMUR_THROW( LEMUR_IO_ERROR, "Couldn't create directory: '" + path + "' because something already exists there." );
    }
  }
}

void Path::remove( const std::string& path ) {
  std::stack<DirectoryIterator*> iterators;
  StackDeleter<DirectoryIterator> sd( iterators );
  iterators.push( new DirectoryIterator( path ) );

  while( iterators.size() ) {
    DirectoryIterator* top = iterators.top();
    
    // all done, so go up a level
    if( (*top) == DirectoryIterator::end() ) {
      // release any search handles that may point
      // to this directory
      top->close();

      int result = rmdir( top->base().c_str() );
      if( result != 0 )
        LEMUR_THROW( LEMUR_IO_ERROR, "Path::remove couldn't remove directory '" + top->base() + "'." );

      delete top;
      iterators.pop();
      continue;
    }

    std::string path = **top;
    (*top)++;

    if( Path::isFile( path ) ) {
      int result = lemur_compat::remove( path.c_str() );
      if( result != 0 )
        LEMUR_THROW( LEMUR_IO_ERROR, "Path::remove couldn't remove file '" + path + "'." );
    } else {
      iterators.push( new DirectoryIterator( path ) );
    }
  }
}

std::string Path::trim( const std::string& path ) {
  if( path.size() && path[path.length()-1] == PATH_SEPARATOR ) {
    return path.substr( 0, path.length()-1 );
  }

  return path;
}

std::string Path::relative( const std::string& basePath, const std::string absolutePath ) {
  std::string relativePath = absolutePath.substr( basePath.length() );

  while( relativePath.length() && relativePath[0] == PATH_SEPARATOR )
    relativePath = relativePath.substr(1);

  return relativePath;
}

void Path::make( const std::string& path ) {
  if( !Path::isDirectory( path ) ) {
    std::string parent = Path::directory( path );
    if( path == parent )
      return;

    Path::make( parent );
  }

  lemur_compat::mkdir( path.c_str(), 0755 );
}

char Path::pathSeparator() {
  return PATH_SEPARATOR;
}

bool Path::isDirectory( const std::string& path ) {
  struct stat s;
  int result = stat( path.c_str(), &s );
  bool actualDirectory = (result >= 0) && (s.st_mode & S_IFDIR);

  return actualDirectory;
}

bool Path::exists( const std::string& path ) {
  struct stat s;
  return stat( path.c_str(), &s ) >= 0;
}

bool Path::isFile( const std::string& path ) {
  struct stat s;
  int result = stat( path.c_str(), &s );
  bool actualFile = (result >= 0) && (s.st_mode & S_IFREG);

  return actualFile;
}

std::string Path::combine( const std::string& root, const std::string& addition ) {
  if( !root.size() )
    return addition;

  if( *(root.end()-1) == PATH_SEPARATOR )
    return root + addition;

  return root + PATH_SEPARATOR + addition;
}

std::string Path::directory( const std::string& path ) {
  int last = path_last_separator( path );
  
  if( last > 0 ) {
    return path.substr( 0, last );
  }

  return path;
}

std::string Path::filename( const std::string& path ) {
  int last = path_last_separator( path );

  if( last != std::string::npos ) {
    return path.substr( last+1 );
  }

  return path;
}

std::string Path::extension( const std::string& path ) {
  int last = path_last_separator( path );
  std::string::size_type lastDot = path.find_last_of( '.' );

  if( int(lastDot) > last ) {
    return path.substr( lastDot+1 );
  }

  return std::string();
}
