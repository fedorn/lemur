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
// DocListFrequencyIterator
//
// 24 August 2004 -- tds
//

#ifndef INDRI_DOCLISTFREQUENCYITERATOR_HPP
#define INDRI_DOCLISTFREQUENCYITERATOR_HPP

#include "indri/DocListDiskBlockReader.hpp"
#include "indri/greedy_vector"
#include "ReadBuffer.hpp"
#include "indri/DocListIterator.hpp"
#include "indri/DocumentCount.hpp"

namespace indri {
  namespace index {
    class DocListFrequencyIterator {
    private:
      int _termID;
      ReadBuffer* _buffer;
      indri::index::DocListDiskBlockReader _reader;
      greedy_vector<DocumentCount> _entries;
      greedy_vector<DocumentCount>::iterator _end;
      greedy_vector<DocumentCount>::iterator _iterator;
      File::offset_type _seekLocation;
      bool _finished;

      void _fetchNextBlock( int documentID );
      void _fillVector( int documentID );

    public:
      DocListFrequencyIterator( File& segment,
                                int termID,
                                File::offset_type seekLocation,
                                INT64 estimatedDataLength = KEYFILE_DOCLISTITERATOR_DEFAULT_BUFFERSIZE );
      ~DocListFrequencyIterator();

      void startIteration();
      bool hasMore();
      void nextEntry();
      void nextEntry( int documentID );
      const DocumentCount* currentEntry();
    };
  }
}

#endif // INDRI_DOCLISTFREQUENCYITERATOR_HPP

