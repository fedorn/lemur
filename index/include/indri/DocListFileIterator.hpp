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
// DocListFileIterator
//
// 9 January 2004 - tds
//

#ifndef INDRI_DOCLISTFILEITERATOR_HPP
#define INDRI_DOCLISTFILEITERATOR_HPP

#include "File.hpp"
#include "indri/DocListDiskBlockReader.hpp"
#include "ReadBuffer.hpp"

namespace indri {
  namespace index {
    class DocListFileIterator {
      File* _file;
      DocListDiskBlockReader _reader;
      ReadBuffer _readBuffer;
      DocListInfo _info;
      int _segment;
      bool _finished;
      bool _partialDocument;
      File::offset_type _fileOffset; /* starting offset */
      
      bool _skipToNextBlock();
      void _fetchDocument();

    public:
      DocListFileIterator( File* file, int segment, size_t bufferSize, File::offset_type fileOffset = 0 );

      bool finished() const;
      void startIteration();

      bool skip( int termID, int documentID );
      DocListInfo& currentDocument();
      bool nextDocument();

      int document() const;
      int termID() const;
      int segment() const;
      void remove();
    };
  }
}

#endif // INDRI_DOCLISTITERATOR_HPP

