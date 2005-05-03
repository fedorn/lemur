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
// KeyfileTextHandler.hpp
//

#ifndef LEMUR_KEYFILE_TEXT_HANDLER_HPP
#define LEMUR_KEYFILE_TEXT_HANDLER_HPP

#include "TextHandler.hpp"
#include "DocumentProps.hpp"
namespace lemur
{
  namespace index 
  {
    class KeyfileIncIndex;
  }
}
namespace lemur 
{
  namespace parse 
  {
    
    ///  \brief KeyfileTextHandler pushes terms into a KeyFileIncIndex.
    ///  This class is a destination TextHandler.  
    class KeyfileTextHandler : public lemur::api::TextHandler {
    public:
      /// Instantiate with index to push terms into.
      KeyfileTextHandler( class lemur::index::KeyfileIncIndex* index, bool countStops=false);
      ~KeyfileTextHandler();
      /// add a new document
      char * handleDoc(char * docno);
      /// add or update a term
      char * handleWord(char * word);
      /// set the document manager id in the index.
      void setDocManager(const string &mgrID);

    private:
      void endDoc();
      void endCollection();

      int docLength;
      DocumentProps* dp;
      bool first;
      int pos;
      bool countStopWds;
      class lemur::index::KeyfileIncIndex* _index;
    };
  }
}

#endif // LEMUR_KEYFILE_TEXT_HANDLER_HPP
