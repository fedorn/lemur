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
