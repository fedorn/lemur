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


#ifndef _INVFPDOCINFO_HPP
#define _INVFPDOCINFO_HPP
/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 * tnt 06/2002 - subclass from InvDocInfo
 *
 *========================================================================*/

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


    class InvFPDocInfo: public lemur::api::DocInfo {
    public: 
      InvFPDocInfo() {pos=NULL;}
      ~InvFPDocInfo() {} 
  
      const lemur::api::LOC_T* positions() const {return pos; }
      void positions(const lemur::api::LOC_T* p) {pos = p;}

    private:
      const lemur::api::LOC_T* pos;  // list of positions in this doc (size is count)
    };
  }
}

#endif
