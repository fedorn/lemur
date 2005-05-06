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
// KeyfileWordMap
//
// 4 February 2004 -- tds
//
// A simple wrapper around Keyfile to provide a
// (word <-> int) mapping.
//

#ifndef INDRI_KEYFILEWORDMAP_HPP
#define INDRI_KEYFILEWORDMAP_HPP

#include <string>
#include "Keyfile.hpp"
namespace indri
{
  /// filesystem interaction components
  namespace file
  {
    
    class KeyfileWordMap {
    private:
      lemur::file::Keyfile _wordToInt;
      lemur::file::Keyfile _intToWord;
      char _wordBuffer[ 1024 ];

    public:
      void create( std::string& path, const std::string& intSuffix, const std::string& wordSuffix );
      void open( std::string& path, const std::string& intSuffix, const std::string& wordSuffix );
      void openRead( std::string& path, const std::string& intSuffix, const std::string& wordSuffix );
      void close();
      int lookupID( const char* word );
      const char* lookupWord( int id );
      void put( int id, const char* word );
    };
  }
}

#endif // INDRI_KEYFILEWORDMAP_HPP
