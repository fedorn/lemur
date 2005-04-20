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
// atomic
//
// 15 December 2004 -- tds
//

#ifndef INDRI_ATOMIC_HPP
#define INDRI_ATOMIC_HPP

#ifndef WIN32
#include <bits/atomicity.h>
#endif

namespace indri {
  /*! Atomic actions for thread support */
  namespace atomic {
#ifdef WIN32
    typedef volatile LONG value_type;

    inline void increment( value_type& variable ) {
      ::InterlockedIncrement( &variable );
    }

    inline void decrement( value_type& variable ) {
      ::InterlockedDecrement( &variable );
    }
#elif defined(__APPLE__)
    typedef _Atomic_word value_type;

    inline void increment( value_type& variable ) {
      __atomic_add( &variable, 1 );
    }

    inline void decrement( value_type& variable ) {
      __atomic_add( &variable, -1 );
    }
#elif defined(HOST_OS) && (HOST_OS == solaris)
    typedef _Atomic_word value_type;

    inline void increment( value_type& variable ) {
      __atomic_add( &variable, 1 );
    }

    inline void decrement( value_type& variable ) {
      __atomic_add( &variable, -1 );
    }
#else
    typedef _Atomic_word value_type;

    inline void increment( value_type& variable ) {
      __gnu_cxx::__atomic_add( &variable, 1 );
    }

    inline void decrement( value_type& variable ) {
      __gnu_cxx::__atomic_add( &variable, -1 );
    }
#endif
  };
}

#endif // INDRI_ATOMIC_HPP

