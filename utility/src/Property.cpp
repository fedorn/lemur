/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2003, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#include "Property.hpp"
#include "common_headers.hpp"

Property::Property() {
  nameSize = INITNAMESIZE;
  name = new char[nameSize];
  nullName = true;

  buffer = new char[INITBUFFSIZE];
  bufferSize = INITBUFFSIZE;
  dataSize = 0;
  type = INT;
  destructor = 0;
}

Property::Property(char * initName) {
  nameSize = INITNAMESIZE;
  int length = strlen(initName) + 1;
  if (length > nameSize) {
    nameSize = length;
  }
  name = new char[nameSize];
  strcpy(name, initName);
  nullName = false;

  buffer = new char[INITBUFFSIZE];
  bufferSize = INITBUFFSIZE;
  dataSize = 0;
  type = INT;
  destructor = 0;
}

Property::~Property() {
  if (destructor) {
    (this->*destructor)( buffer );
    destructor = 0;
  }

  delete [] name;
  delete [] buffer;
}

void Property::setName(char * newName) {
  if (newName != (char *) NULL) {
    int length = strlen(newName) + 1;
    if (length > nameSize) {
      nameSize = length;
      delete [] name;
      name = new char[nameSize];
    }
    strcpy(name, newName); 
    nullName = false;
  } else {
    nullName = true;
  }
}

void Property::copyValue(Property * property) {
  type = property->getType();
  dataSize = property->getSize();
  if (dataSize > bufferSize) {
    bufferSize = dataSize;
    delete [] buffer;
    buffer = new char[bufferSize];
  }
  if (dataSize > 0) {
    memcpy(buffer, property->getValue(), dataSize);
  }
}

char * Property::getName() {
  return (nullName ? NULL : name);
}

void * Property::getValue() {
  if (dataSize > 0) {
    return buffer;
  } 
  return NULL;
}

int Property::getSize() {
  return dataSize;
}

int Property::getLength() {
  int length = 0;
  switch (type) {
  case INT:
    length = dataSize / (sizeof(int) / sizeof(char));
    break;
  case STRING:
    length = dataSize;
    break;
  }
  return length;
}

Property::DataType Property::getType() {
  return type;
}

void* Property::allocate( int size ) {
  if( destructor ) {
    (this->*destructor)( buffer );
    destructor = 0;
  }

  if( size > bufferSize ) {
    bufferSize = size;
    delete[] buffer;
    buffer = new char[bufferSize];
  }
  return buffer;
}

void Property::setDestructor( void (Property::*destructor)(void* object) ) {
  this->destructor = destructor;
}

void Property::setType( DataType type ) {
  this->type = type;
}

