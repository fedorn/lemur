/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/*! std::min and std::max don't work on VC++ 6.0
*/

#ifndef MINMAX_HPP
#define MINMAX_HPP

#if _MSC_VER == 1200
// this should be VC++ 6.0 
#  define _MAXIM std::_cpp_max
#  define _MINIM std::_cpp_min  
#else
#  define _MAXIM std::max
#  define _MINIM std::min
#endif

#endif
