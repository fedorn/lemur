
/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#ifndef _PROPERTYLIST_HPP
#define _PROPERTYLIST_HPP

#include "Property.hpp"

/** A PropertyList class is used 
    to pass token information down a TextHandler chain during
    parsing and tokenization.  The PropertyList class is expected
    to "own" and provide memory management of properties stored
    in its list.  The setProperty function should copy the 
    Property name/value into its own internal Property object.
    Calls to nextElement and getProperty should not create
    new Property objects, but reuse the internal objects.
    On destruction of the PropertyList, the object should free
    any Property objects the PropertyList has created.
 */
class PropertyList {

public:
  virtual ~PropertyList() {}

  /// Returns the property with a given name.  Returns
  /// NULL if there is no property in the list with that name.
  /// Do not deallocate the returned pointer.
  virtual const Property * getProperty(const char * name) const= 0;

  /// Starts iteration over properties.
  virtual void startIteration() const= 0;
  /// Returns the next Property in the iteration.  Do not
  /// deallocate the returned pointer.
  virtual const Property * nextEntry() const= 0;
  /// Tests to see whether there are more properties in the iteration.
  virtual bool hasMore() const= 0;

  
  /// Adds/changes a property into the list.  Overwrites
  /// a property with the same value.  The actual property
  /// stored is a copy of the property passed in.  Changes
  /// after calling this function to the property passed in
  /// will not affect the property in the list.
  virtual void setProperty(const Property * property) = 0;

  /// Removes a property from the list with the
  /// passed in name.  If no property in the list has
  /// the name, the function fails silently.
  virtual void removeProperty(const char * name) = 0;

  /// Clears all properties from the list
  virtual void clear() = 0;
  

};

#endif //_PROPERTYLIST_HPP
