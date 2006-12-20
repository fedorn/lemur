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
// DiskIndex
//
// 8 December 2004 -- tds
//

#ifndef INDRI_DISKINDEX_HPP
#define INDRI_DISKINDEX_HPP

#include "indri/Index.hpp"
#include "indri/File.hpp"
#include "Keyfile.hpp"
#include "indri/TermData.hpp"
#include "indri/FieldStatistics.hpp"
#include "indri/CorpusStatistics.hpp"
#include "indri/DiskTermData.hpp"
#include <vector>
#include <string>
#include "indri/BulkTree.hpp"
#include "indri/SequentialReadBuffer.hpp"

namespace indri {
  namespace index {
    class DiskIndex : public Index {
    private:
      indri::thread::Mutex _lock;

      std::string _path;

      indri::file::BulkTreeReader _frequentStringToTerm;
      indri::file::BulkTreeReader  _infrequentStringToTerm;

      indri::file::BulkTreeReader _frequentIdToTerm;
      indri::file::BulkTreeReader _infrequentIdToTerm;

      indri::file::File _frequentTermsData;

      indri::file::File _documentLengths;
      indri::file::File _documentStatistics;

      indri::file::File _invertedFile;
      indri::file::File _directFile;
      indri::file::File _fieldsFile;

      indri::file::SequentialReadBuffer _lengthsBuffer;

      std::vector<FieldStatistics> _fieldData;
      int _documentBase;
      int _infrequentTermBase;

      indri::index::DiskTermData* _fetchTermData( int termID );
      indri::index::DiskTermData* _fetchTermData( const char* termString );

      CorpusStatistics _corpusStatistics;
      void _readManifest( const std::string& manifestPath );

    public:
      DiskIndex() : _lengthsBuffer(_documentLengths) {}

      void open( const std::string& base, const std::string& relative );
      void close();

      const std::string& path();
      int documentBase();

      int field( const char* fieldName );
      int field( const std::string& fieldName );
      std::string field( int fieldID );

      int term( const char* term );
      int term( const std::string& term );
      std::string term( int termID );

      int documentLength( int documentID );
      UINT64 documentCount();
      UINT64 documentCount( const std::string& term );
      UINT64 uniqueTermCount();

      UINT64 termCount( const std::string& term );
      UINT64 termCount();

      UINT64 fieldTermCount( const std::string& field );
      UINT64 fieldTermCount( const std::string& field, const std::string& term );

      UINT64 fieldDocumentCount( const std::string& field );
      UINT64 fieldDocumentCount( const std::string& field, const std::string& term );

      //
      // Lists
      //
      
      DocListIterator* docListIterator( int termID );
      DocListIterator* docListIterator( const std::string& term );
      DocListFileIterator* docListFileIterator();
      DocExtentListIterator* fieldListIterator( int fieldID );
      DocExtentListIterator* fieldListIterator( const std::string& field );
      const TermList* termList( int documentID );
      TermListFileIterator* termListFileIterator();

      VocabularyIterator* vocabularyIterator();
      VocabularyIterator* frequentVocabularyIterator();
      VocabularyIterator* infrequentVocabularyIterator();

      DocumentDataIterator* documentDataIterator();

      indri::thread::Lockable* iteratorLock();
      indri::thread::Lockable* statisticsLock();
    };
  }
}

#endif // INDRI_DISKINDEX_HPP
