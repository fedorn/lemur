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
// FieldListDiskBuilder.hpp
//
// 5 February 2004 -- tds
//

#ifndef INDRI_KEYFILEFIELDLISTDISKBUILDER_HPP
#define INDRI_KEYFILEFIELDLISTDISKBUILDER_HPP

#include "indri/FieldListDiskBlockWriter.hpp"

namespace indri {
  namespace index {
    class FieldListDiskBuilder {
    private:
      FieldListDiskBlockWriter _block;
      WriteBuffer _writeBuffer;

    public:
      FieldListDiskBuilder( File& outputStream, int size, bool numeric = false ) :
        _writeBuffer(outputStream,size),
        _block(numeric)
      {
      }

      void addExtent( unsigned int documentID, unsigned int begin, unsigned int end, INT64 number ) {
        if( _block.addExtent( documentID, begin, end, number ) )
          return;

        flush();
        _block.addExtent( documentID, begin, end, number );
      }

      void flush() {
        _block.close();
        if( _block.lastDocument() != 0 )
          _writeBuffer.write( _block.data(), _block.dataSize() );
        _block.clear();
      }

      void close() {
        flush();
        _writeBuffer.flush();
      }
    };
  }
}

#endif // INDRI_FIELDLISTDISKBUILDER_HPP
