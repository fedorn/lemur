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

