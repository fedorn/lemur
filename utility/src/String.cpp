/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "String.hpp"

ostream &operator<<(ostream &os, const String &ts) 
{ os << ts.c_str(); return os; }

istream &operator>>(istream &is, String &ts) 
{ string tmp; is >> tmp; ts.assign(tmp); return is; }

