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
// DocListIterator
//
// 9 January 2004 - tds
//

#ifndef INDRI_DOCLISTITERATOR_HPP
#define INDRI_DOCLISTITERATOR_HPP

#include "indri/DocPositionInfoList.hpp"
#include "indri/DocListFileIterator.hpp"
#include "indri/DocListInfo.hpp"

#define KEYFILE_DOCLISTITERATOR_DEFAULT_BUFFERSIZE   (128*1024)
#define KEYFILE_DOCLISTITERATOR_MAX_BUFFERSIZE      (1024*1024)

namespace indri {
  namespace index {
    class DocListIterator : public DocPositionInfoList {
    private:
      mutable DocListFileIterator _fileIterator;  // a file iterator that is iterating over all term/document postings
      int _termID;  // termID of the term for which we're interested in reading document positions
      mutable bool _atNext; // if true, this means that the file iterator already points to the next document
      mutable bool _finished;  // if true, then we have read all postings for this term already

    public:
      DocListIterator( File& segment,
                              int termID,
                              File::offset_type seekLocation = 0,
                              INT64 estimatedDataLength = KEYFILE_DOCLISTITERATOR_DEFAULT_BUFFERSIZE );
      ~DocListIterator();

      // get the iterator ready to return data; call this before calling currentEntry or nextEntry
      void startIteration() const;

      /// Important note: In this implementation, hasMore() invalidates any
      /// structures returned by nextEntry() or nextEntry( docID ).
      bool hasMore() const;

      // move to the next document in the list; return null if there are no more valid documents
      DocInfo* nextEntry() const ;

      // find the first document that contains this term that has an id >= documentID.
      // returns null if no such document exists.
      DocInfo* nextEntry( DOCID_T documentID );

      // return the current document entry if we're not finished, null otherwise.
      DocInfo* currentEntry();

      // iterator support functions
      virtual DocInfo* newElement() const { return new DocListInfo(); }
      virtual void assignElement(DocInfo* to, DocInfo* from) const { 
        DocListInfo *tmp = dynamic_cast<DocListInfo *>(to);
        DocListInfo *cur = dynamic_cast<DocListInfo*>(from);
        *tmp = *cur;
      }

      /// set element from position, returns pointer to the element
      virtual DocInfo* getElement(DocInfo* elem, POS_T position) const;
      /// Position at beginning of list
      virtual POS_T beginPosition() const;
      /// Position at end of list
      virtual POS_T endPosition() const;
      /// Advance to next position
      virtual POS_T nextPosition(POS_T position) const;
    };
  }
}

#endif // INDRI_DOCLISTITERATOR_HPP


