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
// KeyfileDocListDiskBuilder.hpp
//

#ifndef INDRI_DOCLISTDISKBUILDER_HPP
#define INDRI_DOCLISTDISKBUILDER_HPP

#include "WriteBuffer.hpp"
#include "RVLCompress.hpp"
#include "ReadBuffer.hpp"
#include "File.hpp"

#include "indri/DocListIterator.hpp"
#include "indri/DocListDiskBlockWriter.hpp"
#include "indri/DocListFileIterator.hpp"
#include "indri/DocListDiskBlockReader.hpp"

namespace indri {
  namespace index {
    class DocListDiskBuilder {
    private:
      File::offset_type _blockCount;
      WriteBuffer& _writeBuffer;
      DocListDiskBlockWriter _block;

    public:
      DocListDiskBuilder( WriteBuffer& buffer );
      void addLocation( int termID, int documentID, int location );
      void addDocument( int termID, int documentID, const int* locations, int locationCount );
      File::offset_type currentBlock();
      void flush();
    };
  }
}

#endif // INDRI_DOCLISTDISKBUILDER_HPP
