/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// DiskTermListFileIterator
//
// 25 January 2005 -- tds
//

#ifndef INDRI_DISKTERMLISTFILEITERATOR_HPP
#define INDRI_DISKTERMLISTFILEITERATOR_HPP

#include "indri/TermListFileIterator.hpp"
#include "indri/SequentialReadBuffer.hpp"
#include "indri/TermList.hpp"

namespace indri {
  namespace index {
    class DiskTermListFileIterator : public TermListFileIterator {
    private:
      TermList _termList;
      indri::file::File& _termListFile;
      indri::file::SequentialReadBuffer _buffer;
      UINT64 _fileSize;
      bool _finished;

    public:
      DiskTermListFileIterator( indri::file::File& termListFile );

      void startIteration();
      TermList* currentEntry();
      bool nextEntry();
      bool finished();
    };
  }
}

#endif // INDRI_DISKTERMLISTFILEITERATOR_HPP

