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

lemur::parse::Property::Property() {
  name = "";
  nullName = true;

  buffer = new char[INITBUFFSIZE];
  bufferSize = INITBUFFSIZE;
  dataSize = 0;
  type = INT;
  destructor = 0;
}

lemur::parse::Property::Property(const string initName) {
  name = initName;
  nullName = false;

  buffer = new char[INITBUFFSIZE];
  bufferSize = INITBUFFSIZE;
  dataSize = 0;
  type = INT;
  destructor = 0;
}

lemur::parse::Property::~Property() {
  if (destructor) {
    (this->*destructor)( buffer );
    destructor = 0;
  }

  delete [] buffer;
}

void lemur::parse::Property::setName(const string newName) {
  if (! newName.empty() ) {
    name = newName;
    nullName = false;
  } else {
    nullName = true;
  }
}

void lemur::parse::Property::copyValue(const Property * property) {
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

const string lemur::parse::Property::getName() const{
  return (nullName ? NULL : name);
}

const void * lemur::parse::Property::getValue() const{
  if (dataSize > 0) {
    return buffer;
  } 
  return NULL;
}

int lemur::parse::Property::getSize() const{
  return dataSize;
}

int lemur::parse::Property::getLength() const{
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

lemur::parse::Property::DataType lemur::parse::Property::getType() const{
  return type;
}

void* lemur::parse::Property::allocate( int size ) {
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

void lemur::parse::Property::setDestructor( void (lemur::parse::Property::*destructor)(void* object) ) {
  this->destructor = destructor;
}

void lemur::parse::Property::setType( DataType type ) {
  this->type = type;
}

