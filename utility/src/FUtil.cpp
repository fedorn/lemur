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

#include "FUtil.hpp"
#include "common_headers.hpp"

bool fileExist(const string &name)
{
  ifstream ifs;
  ifs.open(name.c_str());
  if (!ifs.fail()) {
    ifs.close();
    return true;
  } 
  return  false;
}

