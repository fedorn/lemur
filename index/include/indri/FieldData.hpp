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
// FieldData.hpp
//
// 5 February 2004 -- tds
//

#ifndef INDRI_FIELDDATA_HPP
#define INDRI_FIELDDATA_HPP

#define INDRI_FIELD_BUFFERSIZE (1024*1024)

#include "indri/FieldListDiskBuilder.hpp"
#include "indri/FieldStatistics.hpp"

namespace indri {
  namespace index {
    struct FieldData {
      FieldStatistics statistics;
      FieldListDiskBuilder* list;
      File* listFile;

      FieldData( const std::string& fieldName, bool isNumeric ) : statistics(fieldName, isNumeric) {
        listFile = 0;
        list = 0;
      }

      ~FieldData() {
        close();
      }

      void create( const std::string& name ) {
        close();

        listFile = new File();
        listFile->open( name, std::fstream::out | std::fstream::binary | std::fstream::trunc );
        list = new FieldListDiskBuilder( *listFile, INDRI_FIELD_BUFFERSIZE, statistics.isNumeric );
      }

      void open( const std::string& name ) {
        close();

        listFile = new File();
        listFile->open( name, std::fstream::in | std::fstream::out | std::fstream::binary );
        listFile->seekp( 0, std::fstream::end );
        list = new FieldListDiskBuilder( *listFile, INDRI_FIELD_BUFFERSIZE, statistics.isNumeric );
      }

      void openRead( const std::string& name ) {
        close();

        listFile = new File();
        listFile->open( name, std::fstream::in | std::fstream::binary );
        listFile->seekp( 0, std::fstream::end );
        list = new FieldListDiskBuilder( *listFile, INDRI_FIELD_BUFFERSIZE, statistics.isNumeric );
      }

      void close() {
        if( list )
          list->close();
        delete list;
        list = 0;

        if( listFile )
          listFile->close();
        delete listFile;
        listFile = 0;
      }
    };
  }
}

#endif // INDRI_FIELDDATA_HPP
