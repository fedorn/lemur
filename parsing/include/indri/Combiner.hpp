/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// Combiner
//
// 3 June 2004 -- tds
//


#ifndef INDRI_COMBINER_HPP
#define INDRI_COMBINER_HPP

#include <iostream>
#include <vector>
#include <string>
#include "indri/Buffer.hpp"
#include "indri/HashTable.hpp"

class Combiner {
private:
  std::vector< std::ofstream* > _docBucketFiles;
  std::vector< std::ofstream* > _linkBucketFiles;
  std::vector< std::stringstream* > _docBuckets;
  std::vector< std::stringstream* > _linkBuckets;
  int _bins;

  struct strhash {
  public:
    int operator() ( const char* k ) const {
      int hash = 0;
      for( ; *k; k++ ){
        hash *= 7;
        hash += *k;
      }
      return hash;
    }
  };

  struct strcompst {
  public:
    int operator () ( const char* o, const char* t ) const {
      return strcmp( o, t );
    }
  };

  struct url_entry {
    char* url;
    char* corpusPath;
    char* docNo;
    int linkCount;
    Buffer linkinfo;

    void addLink( const char* linkDocUrl,
                  const char* linkText )
    {
      if( linkinfo.position() ) {
        // remove trailing 0
        linkinfo.unwrite(1);
      }

      int docUrlLen = strlen(linkDocUrl);
      int textLen = strlen(linkText);

      int total = docUrlLen + sizeof "LINKFROM=" +
                  textLen + sizeof "TEXT=" + 1;

      sprintf( linkinfo.write(total),
               "LINKFROM=%s\nTEXT=%s\n",
               linkDocUrl,
               linkText );

      linkCount++;
    }
  };

  typedef HashTable<char*, url_entry*, strhash, strcompst> UrlEntryTable;
  typedef HashTable<char*, std::vector<url_entry*>, strhash, strcompst> UrlEntryVectorTable;

  url_entry* _newUrlEntry( char* url, char* corpusPath, char* docNo );
  void _deleteUrlEntry( void* buffer );
  
  void _openWriteBuckets( std::vector<std::stringstream*>& buffers, std::vector<std::ofstream*>& buckets, const std::string& path, int bins );
  void _flushWriteBuffer( std::vector<std::stringstream*>& buffers, std::vector<std::ofstream*>& buckets, bool force, int i );
  void _flushWriteBuffers( std::vector<std::stringstream*>& buffers, std::vector<std::ofstream*>& buckets, bool force );
  void _closeWriteBuckets( std::vector<std::stringstream*>& buffers, std::vector<std::ofstream*>& buckets );
  void _openReadBuckets( std::vector<std::ifstream*>& buckets, const std::string& path, int bins );
  void _readDocBucket( UrlEntryTable& urlTable, std::ifstream& docIn );

  int hashString( const char* str );
  void hashToBuckets( std::ifstream& in, const std::string& path );
  void createBuckets( const std::string& tmpPath );
  void closeBuckets();
  void combineBucket( const std::string& outputPath, const std::string& tmpPath, int bucket );
  void hashToBuckets( const std::string& inputPath );
  void combineRedirectDestinationBucket( const std::string& tmpPath, int i, std::vector<std::stringstream*>& outBuffers, std::vector<std::ofstream*>& outputFiles );

public:
  Combiner( int bins = 10 ) : _bins(bins) {}

  void combineRedirectDestinationBuckets( const std::string& tmpPath );
  void combineBuckets( const std::string& outputPath, const std::string& tmpPath );
  void hashRedirectTargets( const std::string& bucketPath, const std::string& redirectsPath );
  void hashToBuckets( const std::string& bucketPath, const std::string& inputPath );
  void sortCorpusFiles( const std::string& outputPath, const std::string& preSortPath, const std::string& inputPath );
};

#endif // INDRI_COMBINER_HPP

