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
// TagExtent
//
// 4 February 2004 -- tds
//

#ifndef INDRI_TAGEXTENT_HPP
#define INDRI_TAGEXTENT_HPP

#include "indri/AttributeValuePair.hpp"
#include <string.h>

namespace indri
{
  namespace parse
  {
    
    struct TagExtent {

      // A comparator that sorts by end value, lowest first
      struct lowest_end_first {

	bool operator() ( const indri::parse::TagExtent* x,
			  const indri::parse::TagExtent* y ) const {

	  // returns true if x < y; false otherwise

	  if ( x->end > y->end ) return true;
	  else return false;
	}
      };

      const char* name;
      unsigned int begin;
      unsigned int end;
      INT64 number;
      TagExtent *parent;
      // explicit initial count of two elements.
      indri::utility::greedy_vector<AttributeValuePair, 2> attributes;
    };
  }
}

#include <functional>
namespace std {

  // An STL comparator that implements first-and-longest ordering
  template<>
  struct less<indri::parse::TagExtent*> {

    bool operator() ( const indri::parse::TagExtent* x,
		      const indri::parse::TagExtent* y ) const {

      // returns true if x < y; false otherwise

      if ( x->begin < y->begin ) return true;
      else if ( x->begin > y->begin ) return false;
      else {

	if ( ( x->end - x->begin ) > ( y->end - y->begin ) ) return true;
	else if ( ( x->end - x->begin ) < ( y->end - y->begin ) ) return false;
	else {

	  // Two TagExtents must have same begin and end and name to be
	  // considered equal.

	  if ( strcmp( x->name, y->name ) < 0 ) return true;
	  else return false;
	}	
      }
    }
  };
}

#endif // INDRI_TAGEXTENT_HPP
