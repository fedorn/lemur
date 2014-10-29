/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _DOCUMENTREP_HPP
#define _DOCUMENTREP_HPP

#include "DocInfoList.hpp"

namespace lemur 
{
  namespace api
  {
    
    //------------------------------------------------------------
    //      Abstract Interface for Document Representation 
    //------------------------------------------------------------

    /// Representation of documents in a collection for efficient inverted index scoring

    class DocumentRep {
    public:
      DocumentRep(DOCID_T docID, int dl = 1) : id(docID), docLength(dl) {}
      virtual ~DocumentRep() {}

      /// term weighting function @see TextQueryRetMethod
      virtual double termWeight(TERMID_T termID, const DocInfo *info) const = 0;

      /// doc-specific constant term in the scoring formula @see TextQueryRetMethod 
      virtual double scoreConstant() const = 0 ;
      /// set the length of the document. Used by PassageRep.
      /// @param len the new length.
      void setDocLength(int len) { docLength = len;}

      /// get the length of the document. Used by PassageRep.
      /// @return the new length.
      int getDocLength() const { return docLength;}
      /// get the document id. Used by PassageRep.
      /// @return the id.
      DOCID_T getID() const { return id;}
  
    protected:  
      DOCID_T id;
      int docLength;
    };
  }
}

#endif /* _DOCUMENTREP_HPP */
