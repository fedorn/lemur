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
// DocListDiskBuilder.cpp
// 

#include "indri/DocListDiskBuilder.hpp"
#include "indri/DocListDiskBlockReader.hpp"

using namespace indri::index;

DocListDiskBuilder::DocListDiskBuilder( WriteBuffer& buffer ) : _writeBuffer( buffer ) {
  _blockCount = 0;
}

void DocListDiskBuilder::addDocument( int termID, int documentID, const int* locations, int locationCount ) {
  int added;
  int storedLocations = 0;

  storedLocations = _block.addDocument( termID,
                                        documentID,
                                        locations,
                                        locationCount );

  while( storedLocations != locationCount ) {
    flush();
    added = _block.addDocument( termID,
                                documentID,
                                locations + storedLocations,
                                locationCount - storedLocations );
    storedLocations += added;
  }
}

void DocListDiskBuilder::addLocation( int termID, int documentID, int location ) {
  if( _block.addPosition( termID, documentID, location ) )
    return;

  flush();
  _block.addPosition( termID, documentID, location );
}

void DocListDiskBuilder::flush() {
  _blockCount++;
  _block.close();
  _writeBuffer.write( _block.data(), _block.dataSize() );
  _block.clear();
}

File::offset_type DocListDiskBuilder::currentBlock() {
  return _blockCount;
}
