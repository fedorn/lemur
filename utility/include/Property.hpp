/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2003, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#ifndef _PROPERTY_HPP
#define _PROPERTY_HPP

#define INITBUFFSIZE 128
#define INITNAMESIZE 32

/** A Property class is used 
    to pass token information down a TextHandler chain during
    parsing and tokenization.  The Property class is expected
    to "own" and provide memory management of the name and 
    value stored in the Property.  The setName and setValue 
    functions should copy the name/value into its own 
    storage.
    On destruction of the Property, the object should free
    any memory that it has created.
    Caller should not free memory returned by getValue
 */

#include <cstring>

class Property {
public:
  enum DataType {EMPTY = 0, INT = 1, STRING = 2, DOUBLE = 3, UNKNOWN = 4};

  Property();
  Property(const char * name);
  ~Property();

  void setName(const char * name);
  void copyValue(const Property * property);
  /// get this property's name
  const char * getName() const;
  /// get the value of this property. do not free memory.
  const void * getValue() const;
  /// enumerated datatype for this property
  DataType getType() const;
  int getSize() const;
  int getLength() const;

  /// set the value for this Property to be given object. proper destructor will
  /// get called. ** Don't use this for primary data types. For that, 
  /// use setFundamentalValue  
  template<typename T>
  void setObjectValue( const T& value ) {
    dataSize = sizeof(T);
    void* memory = allocate( dataSize );
    setDestructor( &Property::destruct<T> );
    new(memory) T(value);
    setType( Property::UNKNOWN );
  }

  /// set a primary type that does not have a destructor
  template<typename T>
  void setFundamentalValue( T value, DataType t ) {
    dataSize = sizeof(T);
    void* memory = allocate( sizeof(T) );
    setDestructor( 0 );
    memcpy( memory, &value, sizeof(T) );
    setType( t );
  }

  /// set value for array of type t with length (length is number of objects)
  template<typename T>
  void setArrayValue( T* value, DataType t, int length ) {
    dataSize = sizeof(T) * length;
    void* memory = allocate( dataSize );
    setDestructor( 0 );
    memcpy( memory, value, length );
    setType( t );
  }

  /// set the value for this Property to be given object. proper destructor will
  /// get called. can use this for any object without needing to modify this 
  /// class to add type but type will be returned as UNKNOWN. 
  /// ** Don't use this for primary data types. For that, add new setValue or 
  /// use setFundamentalValue
  template<typename T>
  void setValue( T value ) {
    setObjectValue( value );
  }

  /// set string value to object  
  void setValue( const char* value ) {
    setArrayValue( value, STRING, strlen(value)+1 );
  }

  /// set string value to object  
  void setValue( char* value ) {
    setArrayValue( value, STRING, strlen(value)+1 );
  }

  /// set int value to object  
  void setValue( int value ) {
    setFundamentalValue( value, INT );
  }

  /// set double value to object  
  void setValue( double value ) {
    setFundamentalValue( value, DOUBLE );
  }

  template<class T>
  void destruct( void* value ) {
    ( (T*) value )->~T();
  }

protected:
  void (Property::*destructor)( void* object );
  /// allocate memory for the buffer, deleting old memory if necessary
  void* allocate( int size );
  void setDestructor( void (Property::*destructor)(void* object) );
  void setType( DataType type );

  DataType type;
  int bufferSize;
  int dataSize;
  char * buffer;

  int nameSize;
  char * name;
  bool nullName;
};


#endif //_PROPERTY_HPP
