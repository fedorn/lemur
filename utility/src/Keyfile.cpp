
//
// Keyfile.cpp
//
// 18 September 2003 - tds
// 30 March 2004 - dmf -- update to new 8 bit clean keyfile api.
//

extern "C" {
  #include "keyref.h"
  #include "keyerr.h"
}

#include <cassert>
#include <memory>
#include <string>

#include "Keyfile.hpp"
#include "Exception.hpp"
#include "minmax.hpp"
#include <sstream>

void Keyfile::_buildHandle( int cacheSize ) {
  int blocks = _MAXIM( (cacheSize - min_fcb_lc) / buffer_lc, 0 );
  _handleSize = min_fcb_lc + blocks * buffer_lc;
//  _handle.reset( new char[ _handleSize ] );
//  _handle = std::auto_ptr<char>(new char[ _handleSize ]);
  _handle = new char[ _handleSize ];
}

void Keyfile::open( const char* filename, int cacheSize ) {
  _buildHandle( cacheSize );
  
  int error = open_key( _handle, const_cast<char*>(filename), 
			_handleSize);

  if( error )
    throw Exception( "Keyfile::open", "Error opening keyfile" );
}

void Keyfile::open( const std::string& filename, int cacheSize ) {
  open( filename.c_str(), cacheSize );
}

void Keyfile::create( const char* filename, int cacheSize ) {
  _buildHandle( cacheSize );
  
  int error = create_key( _handle, const_cast<char*>(filename), 
			  _handleSize );

  if( error )
    throw Exception( "Keyfile::create", "Error creating keyfile" );
}

void Keyfile::create( const std::string& filename, int cacheSize ) {
  create( filename.c_str(), cacheSize );
}

void Keyfile::close() {
//  assert( _handle );
  // don't close an unopened key.
  if (_handle != NULL) close_key( _handle);
  delete[](_handle);
  _handle = NULL;
}

bool Keyfile::get( const char* key, char** value, int& actualSize ) {
  //  assert( key && "key cannot be null" );
  //  assert( value && "value cannot be null" );
  //  assert( _handle && "call open() or create() first" );

  char *buffer; 

  // clear parameters in case of size <= 0
  *value = 0;
  actualSize = 0;

  int size = getSize( key );

  if( size > 0 ) {
    // make a buffer to handle the record
    buffer = new char[size];
    get(key, buffer, actualSize, size);
    *value = buffer; 
  } 

  return size > 0;
}

bool Keyfile::get( const char* key, void* value, int& actualSize, 
		      int maxSize ) {
  //  assert( key && "key cannot be null" );
  //  assert( value && "value cannot be null" );
  //  assert( _handle && "call open() or create() first" );
  //  assert( maxSize > 0 && "maxSize must be positive" );
  int len = strlen(key); // fix for UTF-8
  
  int error = get_rec( _handle, const_cast<char*>(key), len,
		       (char*)value, &actualSize, maxSize );

  if( error && error != getnokey_err )
    throw Exception( "Keyfile::get", "Internal error getting record" );

  return error != getnokey_err;
}

void Keyfile::put( const char* key, const void* value, int valueSize ) {
  //  assert( key && "key cannot be null" );
  //  assert( value && "value cannot be null" );
  int len = strlen(key); // fix for UTF-8
  int error = put_rec( _handle,
                       const_cast<char*>(key), len,
                       static_cast<char*>(const_cast<void*>(value)),
                       valueSize );

  if( error ) {
    std::ostringstream errorString;
    errorString << "Internal error putting record: " << "(Error:" 
		<< error << ")";
    Exception e( "Keyfile::put", 
		 const_cast<char*>(errorString.str().c_str()) );
    throw e;
  }
}

void Keyfile::remove( const char* key ) {
  //  assert( key && "key cannot be null" );
  //  assert( _handle && "call open() or create() first" );
  int len = strlen(key); // fix for UTF-8
  int error = delete_rec( _handle, const_cast<char*>(key), len );

  if( error )
    throw Exception( "Keyfile::remove", "Internal error deleting record" );
}

int Keyfile::getSize( const char* key ) {
  //  assert( key && "key cannot be null" );
  //  assert( _handle && "call open() or create() first" );
  char pointer[buffer_lc];
  int size;
  int len = strlen(key); // fix for UTF-8
  int error = get_ptr( _handle, const_cast<char*>(key), len, pointer );

  if( error ) {
    if( error == getnokey_err ) {
      size =  -1;
    } else {
      throw Exception( "Keyfile::getSize", "Error getting size of record" );
    }
  } else {
      size = keyrec_lc( pointer );
  }

  return size;
}

//
// keyfile_create_key
//
// Converts an integer into a null-terminated string so
// that it can be used as a keyfile key.
//    keyBuf - character buffer to receive string key (must be at least 6 bytes long)
//    number - integer key to convert
//

#define KEYFILE_KEYBUF_SIZE (7)
#define KEYFILE_BUFFER_SHIFT(num,dig) (num >> ((5-dig)*6))
#define KEYFILE_BUFFER_DIGIT(num,dig)  ( (KEYFILE_BUFFER_SHIFT(num,dig) | 1<<6) & ~(1<<7) )

inline void Keyfile::_createKey( char* keyBuf, int number ) {
   keyBuf[6] = 0;
   keyBuf[5] = KEYFILE_BUFFER_DIGIT( number, 5 );
   keyBuf[4] = KEYFILE_BUFFER_DIGIT( number, 4 );
   keyBuf[3] = KEYFILE_BUFFER_DIGIT( number, 3 );
   keyBuf[2] = KEYFILE_BUFFER_DIGIT( number, 2 );
   keyBuf[1] = KEYFILE_BUFFER_DIGIT( number, 1 );
   keyBuf[0] = KEYFILE_BUFFER_DIGIT( number, 0 );
}

inline int Keyfile::_decodeKey( char* keyBuf ) {
  return ((keyBuf[5] & 0x3f) << 6*0) |
         ((keyBuf[4] & 0x3f) << 6*1) |
         ((keyBuf[3] & 0x3f) << 6*2) |
         ((keyBuf[2] & 0x3f) << 6*3) |
         ((keyBuf[1] & 0x3f) << 6*4) |
         ((keyBuf[0] & 0x3f) << 6*5);
}

void Keyfile::put( int key, const void* value, int valueLength ) {
  // have to handle the 0 key. Bleah.
  char keyBuf[KEYFILE_KEYBUF_SIZE];
  _createKey( keyBuf, key );
  put( keyBuf, value, valueLength );
}

bool Keyfile::get( int key, void* value, int& actualSize, int maxSize ) {
  char keyBuf[KEYFILE_KEYBUF_SIZE];
  _createKey( keyBuf, key );
  return get( keyBuf, value, actualSize, maxSize );
}


bool Keyfile::get( int key, char** value, int& actualSize ) {
  char keyBuf[KEYFILE_KEYBUF_SIZE];
  _createKey( keyBuf, key );
  return get( keyBuf, value, actualSize );
}

void Keyfile::remove( int key ) {
  char keyBuf[KEYFILE_KEYBUF_SIZE];
  _createKey( keyBuf, key );
  remove( keyBuf );
}

int Keyfile::getSize( int key ) {
  char keyBuf[KEYFILE_KEYBUF_SIZE];
  _createKey( keyBuf, key );
  return getSize( keyBuf );
}

void Keyfile::setFirst() {
  set_bof(_handle);
}

bool Keyfile::getNext( char* key, int maxKeySize, void* value, 
		       int& actualSize, int maxValueSize ) {
  int len; // for key length
  int result = next_rec( _handle, key, &len, maxKeySize,
			 value, &actualSize, maxValueSize );
  return !result;
}

bool Keyfile::getNext( int& key, void* value, int& actualSize, 
		       int maxValueSize ) {
  char keyBuf[KEYFILE_KEYBUF_SIZE];
  bool result = getNext( keyBuf, KEYFILE_KEYBUF_SIZE, 
			 value, actualSize, maxValueSize );
  
  if( result )
    key = _decodeKey( keyBuf );

  return result;
}
