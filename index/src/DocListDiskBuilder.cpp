/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
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
