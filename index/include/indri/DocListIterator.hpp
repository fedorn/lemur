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


