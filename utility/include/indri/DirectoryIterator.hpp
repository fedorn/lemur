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
// DirectoryIterator
//
// 26 May 2004 -- tds
//

#ifndef INDRI_DIRECTORYITERATOR_HPP
#define INDRI_DIRECTORYITERATOR_HPP

#include <string>
/*! Provides iteration over directory entries. 
 */
class DirectoryIterator {
private:
  static DirectoryIterator _end;

  bool _relative;
  std::string _current;
  void* _platform;
  std::string _path;

  void _copyCurrent();
  void _next();
  
public:
  DirectoryIterator();
  DirectoryIterator( const std::string& path, bool relative = true );
  ~DirectoryIterator();

  void operator ++ (int);
  void operator ++ ();
  bool operator == ( const DirectoryIterator& other );
  const std::string& operator* ();
  const std::string& base() const;
  void close();

  static const DirectoryIterator& end();
};

#endif // INDRI_DIRECTORYITERATOR_HPP

