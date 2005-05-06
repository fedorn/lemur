/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _BASICCOLLECTIONPROPS_HPP
#define _BASICCOLLECTIONPROPS_HPP
#include "CollectionProps.hpp"
#include "LinkedPropertyList.hpp"
/// namespaces within the lemur system.
namespace lemur 
{
  namespace parse 
  {
    
    /*! this is a supporting class for classes that want to pass one object
      around that would have access methods for properties that a collection might
      have.  
    */


    class BasicCollectionProps : public CollectionProps, public LinkedPropertyList {

    public:
      BasicCollectionProps() {};
      virtual ~BasicCollectionProps() {};

      /// get me the value for the property with this name
      string getStringProperty(const string& name) const;

    protected:

    };
  }
}

#endif
