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

#include "IncFPTextHandler.hpp"

IncFPTextHandler::IncFPTextHandler(char * filename, int bufferSize,
				   bool countStopWords, int ind) {
  index = new IncFPPushIndex(filename, bufferSize);

  dp = new DocumentProps();
  term = new InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  pos = 0;
  // set state that is on first doc
  first = true;
}
