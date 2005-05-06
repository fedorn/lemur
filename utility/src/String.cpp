/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#include "String.hpp"

ostream &lemur::utility::operator<<(ostream &os, const lemur::utility::String &ts) 
{ os << ts.c_str(); return os; }

istream &lemur::utility::operator>>(istream &is, lemur::utility::String &ts) 
{ string tmp; is >> tmp; ts.assign(tmp); return is; }

