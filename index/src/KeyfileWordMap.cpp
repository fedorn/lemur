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
// 4 February 2004 - tds
//

#include "indri/KeyfileWordMap.hpp"

void indri::file::KeyfileWordMap::open( std::string& path, const std::string& intSuffix, const std::string& wordSuffix ) {
  std::string intName = path + intSuffix;
  std::string wordName = path + wordSuffix;
  
  _wordToInt.open( wordName );
  _intToWord.open( intName );
}

void indri::file::KeyfileWordMap::openRead( std::string& path, const std::string& intSuffix, const std::string& wordSuffix ) {
  std::string intName = path + intSuffix;
  std::string wordName = path + wordSuffix;
  
  _wordToInt.openRead( wordName );
  _intToWord.openRead( intName );
}

void indri::file::KeyfileWordMap::create( std::string& path, const std::string& intSuffix, const std::string& wordSuffix ) {
  std::string intName = path + intSuffix;
  std::string wordName = path + wordSuffix;
  
  _wordToInt.create( wordName );
  _intToWord.create( intName );
}

void indri::file::KeyfileWordMap::close() {
  _wordToInt.close();
  _intToWord.close();
}

int indri::file::KeyfileWordMap::lookupID( const char* word ) {
  int id;
  int actual;

  if( _wordToInt.get( word, &id, actual, sizeof id ) ) {
    return id;
  } else {
    return 0;
  }
}

const char* indri::file::KeyfileWordMap::lookupWord( int id ) {
  int actual;

  if( _intToWord.get( id, _wordBuffer, actual, sizeof _wordBuffer ) ) {
    _wordBuffer[actual] = 0;
    return _wordBuffer;
  } else {
    return 0;
  }
}

void indri::file::KeyfileWordMap::put( int id, const char* word ) {
  _wordToInt.put( word, &id, sizeof id );
  _intToWord.put( id, word, int(strlen(word)) );
}

