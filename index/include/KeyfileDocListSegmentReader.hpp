/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// KeyfileDocListSegmentReader
//
// tds - 13 November 2003
//

#ifndef LEMUR_KEYFILEDOCLISTSEGMENTREADER_HPP
#define LEMUR_KEYFILEDOCLISTSEGMENTREADER_HPP

#include "InvFPDocList.hpp"
#include "ReadBuffer.hpp"
#include "File.hpp"
namespace lemur 
{
  namespace file 
  {
    
    /// Provides a reader for DocInfoLists from a give segment in a File.
    class KeyfileDocListSegmentReader {
    private:
      File* _stream;
      ReadBuffer* _file;
      lemur::index::InvFPDocList* _top;
      int _segment;
      std::string _name;

    public:
      /// Instantiate for a given file
      KeyfileDocListSegmentReader( File* stream, std::string& baseName, 
                                   int segment, int readBufferSize );
      ~KeyfileDocListSegmentReader();
      /// Return the File this reader reads.
      File* file();
      /// Return the next DocInfoList in the segment
      lemur::index::InvFPDocList* next();
      /// Compare two Readers for merging
      bool operator<( const KeyfileDocListSegmentReader& other ) const;

      /// Return the current DocInfoList in the segment  
      const lemur::index::InvFPDocList* top() const;
      /// Return the current DocInfoList in the segment  
      lemur::index::InvFPDocList* top();

      /// set the current DocInfoList to the next in the segment  
      void pop();
      /// Return which segment this reader reads.
      int segment() const;
      /// Return the base name of the File this reader reads.
      const std::string& name() const;
    };
  }
}

#endif // LEMUR_KEYFILEDOCLISTSEGMENTREADER_HPP
