
/**
 * lemur-compat.hpp
 *
 * The point of this header is to contain the kinds of things
 * that are stored in platform.h on Unix:
 *    * endian constants
 *    * type size constants
 *    * typedefs
 *
 * As this is a Lemur header, it is also appropriate to put general
 * Lemur-specific typedefs here.
 */

#ifndef LEMUR_COMPAT_HPP
#define LEMUR_COMPAT_HPP

#include "lemur-platform.h"

#include <memory>

#ifdef WIN32
  #include <xutility>
  #include <direct.h>
  #define LEMUR_MKDIR_NO_MODE
#else
  #include <utility>
  #include <fstream>
  #include <sys/types.h>
  #include <sys/stat.h>
  #define LEMUR_USES_ENUM_OPENMODE
  #ifdef HOST_OS
    #if HOST_OS == solaris
      #define	BIG_ENDIAN	4321
      #define BYTE_ORDER BIG_ENDIAN
    #endif
  #endif
#endif

#include <stdio.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace lemur_compat {
/// same as std::max, but compatible with VC++ 6
template<typename _Type>
_Type min( _Type x, _Type y ) {
#ifdef LEMUR_BROKEN_MIN
  if( x < y )
    return x;
  else
    return y;
#else
  return std::min<_Type>( x, y );
#endif
}

/// same as std::min, but compatible with VC++ 6
template<typename _Type>
_Type max( _Type x, _Type y ) {
#ifdef LEMUR_BROKEN_MAX  
  if( x > y )
    return x;
  else
    return y;
#else
  return std::max<_Type>( x, y );
#endif
};

/// resets an auto_ptr; same as ptr.reset(), but 
/// compatible with VC++ 6
template<typename _Type>
void autoptr_reset( std::auto_ptr<_Type>& ptr ) {
  #ifdef LEMUR_BROKEN_AUTOPTR
    std::auto_ptr<_Type> garbage;
    garbage = ptr;
  #else
    ptr.reset();
  #endif
};

/// resets an auto_ptr; same as ptr.reset(), but 
/// compatible with VC++ 6
template<typename _Type>
void autoptr_reset( std::auto_ptr<_Type>& ptr, _Type& newValue ) {
  #ifdef LEMUR_BROKEN_AUTOPTR
    ptr = newValue;
  #else
    ptr.reset( newValue );
  #endif
};

/// remove a file (same as ::remove)
inline int remove( const char* fileName ) {
#ifdef LEMUR_NO_REMOVE
  return ::unlink( fileName );
#else
  return ::remove( fileName );
#endif
}

#ifdef LEMUR_USES_ENUM_OPENMODE
inline std::_Ios_Openmode ios_mode_cast( int mode ) {
  return std::_Ios_Openmode(mode);
}
#else
inline int ios_mode_cast( int mode ) {
  return mode;
}
#endif

#ifdef LEMUR_MKDIR_NO_MODE
inline int mkdir( const char* path, int mode ) {
  return ::mkdir(path);
}
#else
inline int mkdir( const char* path, int mode ) {
  return ::mkdir(path, mode);
}
#endif

inline double flipd( double native ) {
  double result;
  const unsigned char* input = (const unsigned char*) &native;
  unsigned char* output = (unsigned char*) &result;

  output[7] = input[0];
  output[6] = input[1];
  output[5] = input[2];
  output[4] = input[3];
  output[3] = input[4];
  output[2] = input[5];
  output[1] = input[6];
  output[0] = input[7];

  return result;
}

inline UINT64 flipll( UINT64 native ) {
  UINT64 result;
  const unsigned char* input = (const unsigned char*) &native;
  unsigned char* output = (unsigned char*) &result;

  output[7] = input[0];
  output[6] = input[1];
  output[5] = input[2];
  output[4] = input[3];
  output[3] = input[4];
  output[2] = input[5];
  output[1] = input[6];
  output[0] = input[7];

  return result;
}

#ifdef WIN32
inline int strncasecmp( const char* one, const char* two, int length ) {
  return ::strnicmp( one, two, length );
}
#else
inline int strncasecmp( const char* one, const char* two, int length ) {
  return ::strncasecmp( one, two, length );
}
#endif

#if defined(BIG_ENDIAN) && (BYTE_ORDER == BIG_ENDIAN)
inline double htond( double native ) {
  return native;
}

inline double ntohd( double native ) {
  return native;
}
#else
inline double htond( double native ) {
  return flipd( native );
}

inline double ntohd( double native ) {
  return flipd( native );
}
#endif 

#if defined(BIG_ENDIAN) && (BYTE_ORDER == BIG_ENDIAN)
inline UINT64 htonll( UINT64 native ) {
  return native;
}

inline UINT64 ntohll( UINT64 native ) {
  return native;
}
#else
inline UINT64 htonll( UINT64 native ) {
  return flipll( native );
}

inline UINT64 ntohll( UINT64 native ) {
  return flipll( native );
}
#endif

void initializeNetwork();
void closesocket( socket_t s );

};

#endif /* LEMUR_COMPAT_HPP */

