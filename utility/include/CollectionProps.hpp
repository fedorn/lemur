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

#ifndef _COLLECTIONPROPS_HPP
#define _COLLECTIONPROPS_HPP


/*
 * NAME DATE - COMMENTS
 * tnt 03/01 - created
 ==========================================================================*/
namespace lemur 
{
  namespace parse 
  {
    
    //! this Abstract Class for set of Collection Properties

    /*! this is a supporting class for classes that want to pass one object
      around that would have access methods for properties that a collectiont might
      have.  this class should be subclassed, adding access methods for the particular information needs of a particular index.

      See BasicCollectionProps for example

    */
    class CollectionProps {
    public:
      virtual ~CollectionProps() {};

    };
  }
}

#endif
