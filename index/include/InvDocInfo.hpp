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


#ifndef _INVDOCINFO_HPP
#define _INVDOCINFO_HPP
/*
 * NAME DATE - COMMENTS
 * tnt 03/2002 - created
 *
 *========================================================================*/

#include "DocInfoList.hpp"
#include "InvFPTypes.hpp"

namespace lemur 
{
  namespace index 
  {
    
    /*!

    This class contains document information for a particular term
    All methods are in reference to a particular term.  For information
    about a document in whole, see the class DocumentProps.hpp
    */

    class InvDocInfo: public lemur::api::DocInfo {
    public: 
      friend class InvDocList;

      InvDocInfo() {};
      ~InvDocInfo() {}; 
  
      lemur::api::DOCID_T docID() const{ return id; };
      lemur::api::COUNT_T termCount() const{ return count; };
 
    protected:
      lemur::api::DOCID_T id;    // id of this doc
      lemur::api::COUNT_T count; // count of this term in this doc
    };
  }
}
#endif
