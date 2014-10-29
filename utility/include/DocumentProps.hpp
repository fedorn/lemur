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

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 * ========================================================================
 */

#ifndef _DOCUMENTPROPS_HPP
#define _DOCUMENTPROPS_HPP
namespace lemur 
{
  namespace parse 
  {
    
    //! \brief Class for set of Document Properties

    /*! this is a supporting class for classes that want to pass one object
      around that would have access methods for properties that a document might
      have.  this class should be subclassed, adding access methods for the particular information needs for a particular index.

    */

    class DocumentProps {
    public:
      DocumentProps() { len=0; sid=NULL;};
      ~DocumentProps() {};
  
      /// set document length
      void length(int dl) { len=dl; };

      /// get document length
      int length() const { return len; };

      /// set docID, we're not making a copy
      void stringID(const char* did) { sid = did; };

      /// get docID
      const char* stringID() const { return sid; };

    private:
      int len;
      const char* sid;
    };
  }
}

#endif
