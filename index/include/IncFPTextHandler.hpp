/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  author: dmf
*/

#ifndef _INCFPTEXTHANDLER_HPP
#define _INCFPTEXTHANDLER_HPP
#include "InvFPTextHandler.hpp"
#include "IncFPPushIndex.hpp"

///  IncFPTextHandler builds an InvFPIndex using IncFPPushIndex.
///  This class is a destination TextHandler.  
///

class IncFPTextHandler : public InvFPTextHandler {
public:
  IncFPTextHandler() { }
  /// Create a IncFPTextHandler with index name filename 
  /// (minus the .ifp extension) and specified buffer size.
  IncFPTextHandler(const string &filename, int bufferSize, 
		   bool countStopWds = false, int ind = 1);
};

#endif

