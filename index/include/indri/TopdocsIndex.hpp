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
// TopdocsIndex
//
// 29 October 2004 -- tds
//

#ifndef INDRI_TOPDOCSINDEX_HPP
#define INDRI_TOPDOCSINDEX_HPP

#include "Keyfile.hpp"
#include "indri/IndriIndex.hpp"

class TopdocsIndex {
private:
  struct Entry {
  public:
    struct document_less {
      bool operator () ( const Entry& one, const Entry& two ) const {
        return one.documentID < two.documentID;
      }
    };

    struct score_less { 
      bool operator () ( const Entry& one, const Entry& two ) const {
        double oneScore = double(one.count) / double(one.length);
        double twoScore = double(two.count) / double(two.length);
        
        return oneScore < twoScore;
      }
    };

    struct score_greater { 
      bool operator () ( const Entry& one, const Entry& two ) const {
        double oneScore = double(one.count) / double(one.length);
        double twoScore = double(two.count) / double(two.length);
        
        return oneScore > twoScore;
      }
    };

    Entry( int d, int c, int l )
      :
      documentID(d),
      count(c),
      length(l)
    {
    }

    Entry() {}

    int documentID;
    int count;
    int length;
  };

  std::string _pathname;
  Keyfile _lists;
  UINT64 _documents;
  bool _readOnly;
  
  void _getListEntries( std::vector<Entry>& output, Entry& smallest, IndriIndex& index, int term, UINT64 total, UINT64 keep );
  void _writeList( const std::vector<Entry>& input, const Entry& smallest, int term );

public:
  struct TopdocsList {
    std::vector<Entry> entries;
    Entry smallest;
  };

  void create( const std::string& filename );
  void open( const std::string& filename );
  void openRead( const std::string& filename );
  void close();
  void update( IndriIndex& index );
  TopdocsList* TopdocsIndex::fetch( int term );
};

#endif // INDRI_TOPDOCSINDEX_HPP


