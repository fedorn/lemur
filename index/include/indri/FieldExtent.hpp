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
// FieldExtent
//
// 14 July 2004 -- tds
//

#ifndef INDRI_FIELDEXTENT_HPP
#define INDRI_FIELDEXTENT_HPP

#include "lemur-platform.h"

namespace indri {
  namespace index {
    struct FieldExtent {
      FieldExtent() {};
      FieldExtent( int _id, int _begin, int _end, UINT64 _number ) : id(_id), begin(_begin), end(_end), number(_number) {}
      unsigned int id;
      unsigned int begin;
      unsigned int end;
      UINT64 number;
    };
  }
}

#endif // INDRI_KEYFILEFIELDEXTENT_HPP

