/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _LEMUR_KEYFILE_H
#define _LEMUR_KEYFILE_H

/// Provides C++ interface to the keyfile b-tree package. Arbitrary data can
/// be stored using either int or char * keys. 
class Keyfile {
public:
  /// Random or sequential access b-tree.
  enum access_mode{ random, sequential };

  /// Insert value into b-tree for the given key. Throws an Exception if
  /// the operation fails.
  void put( const char* key, const void* value, int valueSize );
  /// Insert value into b-tree for the given key. Throws an Exception if
  /// the operation fails.
  void put( int key, const void* value, int valueSize );

  /// Retrieve the value in the b-tree for the given key. 
  /// Returns false if key does not exist in the b-tree.
  bool get( const char* key, void* value, int& actualSize, int maxSize );
  /// Retrieve the value in the b-tree for the given key. 
  /// Returns false if key does not exist in the b-tree.
  bool get( const char* key, char** value, int& actualSize );
  /// Retrieve the value in the b-tree for the given key. 
  /// Returns false if key does not exist in the b-tree.
  bool get( int key, void* value, int& actualSize, int maxSize );
  /// Retrieve the value in the b-tree for the given key. 
  /// Returns false if key does not exist in the b-tree.
  bool get( int key, char** value, int& actualSize );

  /// Return the size of the data in the b-tree for the given key. 
  /// Returns -1 if key does not exist in the b-tree.
  int getSize( const char* key );
  /// Return the size of the data in the b-tree for the given key. 
  /// Returns -1 if key does not exist in the b-tree.
  int getSize( int key );

  /// Remove the entry in the b-tree for the given key.
  void remove( const char* key );
  /// Remove the entry in the b-tree for the given key.
  void remove( int key );

  /// Open a keyfile with the given filename, with access_mode of 
  /// random or sequential, and cacheSize (default 1MB).
  void open( const std::string& filename, access_mode mode, 
	     int cacheSize = 1024 * 1024 );
  /// Open a keyfile with the given filename, with access_mode of 
  /// random or sequential, and cacheSize (default 1MB).
  void open( const char* filename, access_mode mode, 
	     int cacheSize = 1024 * 1024 );
  /// Createa keyfile with the given filename, with access_mode of 
  /// random or sequential, and cacheSize (default 1MB).
  void create( const std::string& filename, access_mode mode,
	       int cacheSize = 1024 * 1024 );
  /// Create a keyfile with the given filename, with access_mode of 
  /// random or sequential, and cacheSize (default 1MB).
  void create( const char* filename, access_mode mode, 
	       int cacheSize = 1024 * 1024 );
  /// Close a keyfile.
  void close();
  
private:
  std::auto_ptr<char> _handle; // file control block of the keyfile
  int _handleSize; // sizeof _handle buffer

  void _buildHandle( int cacheSize );
  void _createKey( char* keyBuf, int number );
  int _decodeKey( char* keyBuf );
};

#endif // _LEMUR_KEYFILE_H
