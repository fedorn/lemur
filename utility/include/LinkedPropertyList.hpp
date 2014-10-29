
/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
 */

#ifndef _LINKEDPROPERTYLIST_HPP
#define _LINKEDPROPERTYLIST_HPP

#include "PropertyList.hpp"

#include <list>

using namespace std;

namespace lemur 
{
  namespace parse 
  {
    
    /** A doubly-linked list implementation of the PropertyList interface.
        This makes the getProperty(name) function linear in the number of
        Property objects in the list.  This should not be a problem as
        we are expecting that the number of items in the list is typically
        very small.  Add and remove are also linear operations.

        Note that all calls returning a property object return 
        copies of the internal object.  Changing the returned
        object will not change the property in the list.
        To change a property in the list, you must use setProperty.
    */

    typedef list<Property *> plist;

    class LinkedPropertyList : public PropertyList {

    public:
      LinkedPropertyList();
      virtual ~LinkedPropertyList();

      /// Returns the property with a given name.  Returns
      /// NULL if there is no property in the list with that name.
      /// Do not deallocate the returned pointer.
      virtual const Property * getProperty(const string name) const; 

      /// Starts iteration over properties.
      virtual void startIteration() const;
      /// Returns the next Property in the iteration.  Do not
      /// deallocate the returned pointer.
      virtual const Property * nextEntry() const;
      /// Tests to see whether there are more properties in the iteration.
      virtual bool hasMore() const;

  
      /// Adds/changes a property into the list.  Overwrites
      /// a property with the same value.  The actual property
      /// stored is a copy of the property passed in.  Changes
      /// after calling this function to the property passed in
      /// will not affect the property in the list.
      virtual void setProperty(const Property * property);

      /// Removes a property from the list with the
      /// passed in name.  If no property in the list has
      /// the name, the function fails silently.
      virtual void removeProperty(const string name);
      /// Clears all properties from the list
      virtual void clear();

    protected:  

      // checks the cache for a property that 
      // can be reused, otherwise creates a new
      // property
      virtual Property * getAProperty();
      // finds the property with the given name
      virtual Property * getPropertyInternal(const string name) const;

      // need this mutable for iteration
      mutable plist::iterator iterator;
      mutable plist properties;

      plist cache;

      // We copy values into this property when returning
      // property objects.  This allows control to
      // the linked property list only.
      mutable Property returnProp;
    };
  }
}

#endif //_LINKEDPROPERTYLIST_HPP
