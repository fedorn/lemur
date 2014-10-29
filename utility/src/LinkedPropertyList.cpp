/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
 */

#include "LinkedPropertyList.hpp"

lemur::parse::LinkedPropertyList::LinkedPropertyList(){
  iterator = properties.end();
}

lemur::parse::LinkedPropertyList::~LinkedPropertyList() {
  while (!properties.empty()) {
    Property * prop = properties.front();
    properties.pop_front();
    delete prop;
  }
  while (!cache.empty()) {
    Property * prop = cache.front();
    cache.pop_front();
    delete prop;
  }
}

/// Returns the property with a given name.  Returns
/// NULL if there is no property in the list with that name.
/// Do not deallocate the returned pointer.
const lemur::parse::Property * 
lemur::parse::LinkedPropertyList::getProperty(const string name) const{
  Property * prop = getPropertyInternal(name);
  if (prop != NULL) {
    returnProp.setName(prop->getName());
    returnProp.copyValue(prop);
    prop = &returnProp;
  }
  return prop;
} 

/// Starts iteration over properties.
void 
lemur::parse::LinkedPropertyList::startIteration() const{
  iterator = properties.begin();
}


/// Returns the next Property in the iteration.  Do not
/// deallocate the returned pointer.
const lemur::parse::Property * 
lemur::parse::LinkedPropertyList::nextEntry() const{
  Property * prop = NULL;
  if (hasMore()) {  
    prop = *iterator;
    iterator++;
  }
  if (prop != NULL) {
    returnProp.setName(prop->getName());
    returnProp.copyValue(prop);
    prop = &returnProp;
  }
  return prop;
}


/// Tests to see whether there are more properties in the iteration.
bool 
lemur::parse::LinkedPropertyList::hasMore() const{
  return (iterator != properties.end());
}

  
/// Adds/changes a property into the list.  Overwrites
/// a property with the same value.  The actual property
/// stored is a copy of the property passed in.  Changes
/// after calling this function to the property passed in
/// will not affect the property in the list.
void 
lemur::parse::LinkedPropertyList::setProperty(const Property * property) {
  if ((property != NULL) && (!property->getName().empty())) {
    Property * prop = getPropertyInternal(property->getName());
    if (prop == NULL) {
      prop = getAProperty();
      prop->setName(property->getName());
      properties.push_back(prop);
    } 
    prop->copyValue(property);
  }
}

/// Removes a property from the list with the
/// passed in name.  If no property in the list has
/// the name, the function fails silently.
void 
lemur::parse::LinkedPropertyList::removeProperty(const string name) {
  Property * prop = getPropertyInternal(name);
  // if the property is there, move it to the cache
  if (prop != NULL) {
    properties.remove(prop);
    cache.push_front(prop);
  }
}

/// Clears all properties from the list
void 
lemur::parse::LinkedPropertyList::clear() {
  // move items over to the cache
  while (!properties.empty()) {
    cache.push_front(properties.front());
    properties.pop_front();
  }
}

// checks the cache for a property that 
// can be reused, otherwise creates a new
// property
lemur::parse::Property * 
lemur::parse::LinkedPropertyList::getAProperty() {
  Property * prop = NULL;
  if (!cache.empty()) {
    prop = cache.front();
    cache.pop_front();
  } else {
    prop = new Property();
  }
  return prop;
}

lemur::parse::Property * 
lemur::parse::LinkedPropertyList::getPropertyInternal(const string name) const{
  Property * prop = NULL;
  if (!name.empty()) {
    plist::iterator iter = properties.begin();
    while (iter != properties.end()) {
      if ((*iter)->getName() == name) {
        prop = *iter;
        break;
      }
      iter++;
    }
  }
  return prop;
} 
