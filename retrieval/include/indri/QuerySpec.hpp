/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


#ifndef INDRI_QUERYSPEC_HPP
#define INDRI_QUERYSPEC_HPP

#include <vector>
#include <string>
#include <sstream>
#include <indri/greedy_vector>
#include <algorithm>
#include "lemur-platform.h"

#include "indri/Walker.hpp"
#include "indri/Copier.hpp"
#include "indri/Packer.hpp"
#include "indri/Unpacker.hpp"

#include "Exception.hpp"

template<class T>
bool equal( const std::vector<T>& one, const std::vector<T>& two ) {
  if( one.size() != two.size() )
    return false;

  for( unsigned int i=0; i<one.size(); i++ ) {
    if( *one[i] == *two[i] )
      continue;

    return false;
  }

  return true;
}

template<class T>
bool unordered_equal( std::vector<T>& one, std::vector<T>& two ) {
  if( one.size() != two.size() )
    return false;

  std::vector<T> one_copy;
  for( unsigned int i=0; i<one.size(); i++ ) {
    one_copy.push_back( one[i] );
  }

  // this algorithm is n^2 as opposed to n log n if
  // we sorted things, but windows tend to be short
  for( unsigned int i=0; i<two.size(); i++ ) {
    for( unsigned int j=0; j<one_copy.size(); j++ ) {
      if( *one_copy[j] == *two[i] ) {
        // we remove each match--if they all match, the array will be empty
        one_copy.erase( one_copy.begin() + j );
        break;
      }
    }
  }

  return one_copy.size() == 0;
}

namespace indri {
  namespace lang {
    /* abstract */ class Node {
    protected:
      std::string _name;

    public:
      Node() {
        std::stringstream s;
        s << PTR_TO_INT(this);
        _name = s.str();
      }

      virtual ~Node() {
      }
      
      void setNodeName( const std::string& name ) {
        _name = name;
      }

      const std::string& nodeName() const {
        return _name;
      }

      virtual std::string typeName() const {
        return "Node";
      }

      virtual std::string queryText() const = 0;

      virtual bool operator < ( Node& other ) {
        // TODO: make this faster
        if( typeName() != other.typeName() )
          return typeName() < other.typeName();

        return queryText() < other.queryText();
      }
     
      virtual bool operator== ( Node& other ) {
        return &other == this; 
      }
      
      virtual void pack( Packer& packer ) = 0;
      virtual void walk( Walker& walker ) = 0;
      virtual Node* copy( Copier& copier ) = 0;
    };

    /* abstract */ class RawExtentNode : public Node {};
    /* abstract */ class ScoredExtentNode : public Node {};
    /* abstract */ class AccumulatorNode : public Node {};
    
    class IndexTerm : public RawExtentNode {
    private:
      std::string _text;
      bool _stemmed;

    public:
      IndexTerm( const std::string& text, bool stemmed = false ) : _text(text), _stemmed(stemmed)
      {
      }

      IndexTerm( Unpacker& unpacker ) {
        _text = unpacker.getString( "termName" );
        _stemmed = unpacker.getBoolean( "stemmed" );
      }

      const std::string& getText() { return _text; }

      bool operator==( Node& node ) {
        IndexTerm* other = dynamic_cast<IndexTerm*>(&node);

        if( !other )
          return false;

        if( other == this )
          return true;
        
        return other->_text == _text;
      }

      std::string typeName() const {
        return "IndexTerm";
      }

      std::string queryText() const {
        std::stringstream qtext;

        if( _stemmed ) {
          qtext << '"' << _text << '"';
        } else {
          qtext << _text;
        }

        return qtext.str();
      }

      void setStemmed(bool stemmed) {
        _stemmed = stemmed;
      }

      bool getStemmed() const {
        return _stemmed;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "termName", _text );
        packer.put( "stemmed", _stemmed );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        IndexTerm* termCopy = new IndexTerm(*this);
        return copier.after(this, termCopy);
      }
    };

    class Field : public RawExtentNode {
    private:
      std::string _fieldName;

    public:
      Field( const std::string& name ) : _fieldName(name)
      {
      }

      Field( Unpacker& unpacker ) {
        _fieldName = unpacker.getString( "fieldName" );
      }

      const std::string& getFieldName() const { return _fieldName; }

      std::string typeName() const {
        return "Field";
      }

      std::string queryText() const {
        return _fieldName;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "fieldName", _fieldName );
        packer.after(this);
      }
      
      void walk( Walker& walker ) {
        walker.before(this);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        Field* newField = new Field(*this);
        return copier.after(this, newField);
      }

      bool operator== ( Node& other ) {
        Field* otherField = dynamic_cast<Field*>(&other);

        if( !otherField )
          return false;

        return otherField->getFieldName() == getFieldName();
      }
    };

    class ExtentInside : public RawExtentNode {
    private:
      RawExtentNode* _inner;
      RawExtentNode* _outer;

    public:
      ExtentInside( RawExtentNode* inner, RawExtentNode* outer ) :
        _inner(inner),
        _outer(outer)
      {
      }

      ExtentInside( Unpacker& unpacker ) {
        _inner = unpacker.getRawExtentNode( "inner" );
        _outer = unpacker.getRawExtentNode( "outer" );
      }

      bool operator== ( Node& o ) {
        ExtentInside* other = dynamic_cast<ExtentInside*>(&o);
  
        return other &&
               *_inner == *other->_inner &&
               *_outer == *other->_outer;
      }
      
      std::string typeName() const {
        return "ExtentInside";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << _inner->queryText()
              << "."
              << _outer->queryText();

        return qtext.str();
      }

      RawExtentNode* getInner() {
        return _inner;
      }

      RawExtentNode* getOuter() {
        return _outer;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "inner", _inner );
        packer.put( "outer", _outer );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _inner->walk(walker);
        _outer->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        
        RawExtentNode* newInner = dynamic_cast<RawExtentNode*>(_inner->copy(copier));
        RawExtentNode* newOuter = dynamic_cast<RawExtentNode*>(_outer->copy(copier));
        ExtentInside* extentInsideCopy = new ExtentInside( newInner, newOuter );
        extentInsideCopy->setNodeName( nodeName() );

        return copier.after(this, extentInsideCopy);
      }
    };

    class ExtentOr : public RawExtentNode {
    private:
      std::vector<RawExtentNode*> _children;

    public:
      ExtentOr() {}
      ExtentOr( const std::vector<RawExtentNode*>& children ) :
        _children(children)
      {
      }

      ExtentOr( Unpacker& unpacker ) {
        _children = unpacker.getRawExtentVector( "children" );
      } 

      std::string typeName() const {
        return "ExtentOr";
      }

      std::string queryText() const {
        std::stringstream qtext;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          if(i>0) qtext << " ";
          qtext << _children[i]->queryText();
        }

        return qtext.str();
      }

      void addChild( RawExtentNode* node ) {
        _children.push_back(node);
      }

      std::vector<RawExtentNode*>& getChildren() {
        return _children;
      }

      bool operator== ( Node& node ) {
        ExtentOr* other = dynamic_cast<ExtentOr*>(&node);

        if( other == this )
          return true;

        if( !other )
          return false;

        return unordered_equal( other->_children, _children );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "children", _children );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        for( unsigned int i=0; i<_children.size(); i++ ) {
          _children[i]->walk(walker);
        }
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        
        ExtentOr* duplicate = new ExtentOr();
        duplicate->setNodeName( nodeName() );
        for( unsigned int i=0; i<_children.size(); i++ ) {
          indri::lang::Node* childNode = _children[i]->copy(copier);
          duplicate->addChild( dynamic_cast<RawExtentNode*>(childNode) );
        }

        return copier.after(this, duplicate);
      }
    };

    class ExtentAnd : public RawExtentNode {
    private:
      std::vector<RawExtentNode*> _children;

    public:
      ExtentAnd() {}
      ExtentAnd( const std::vector<RawExtentNode*>& children ) :
        _children(children)
      {
      }

      ExtentAnd( Unpacker& unpacker ) {
        _children = unpacker.getRawExtentVector( "children" );
      }

      std::string typeName() const {
        return "ExtentAnd";
      }

      std::string queryText() const {
        std::stringstream qtext;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          if(i>0) qtext << ",";
          qtext << _children[i]->queryText();
        }

        return qtext.str();
      }

      void addChild( RawExtentNode* node ) {
        _children.push_back(node);
      }

      std::vector<RawExtentNode*>& getChildren() {
        return _children;
      }

      bool operator== ( Node& node ) {
        ExtentAnd* other = dynamic_cast<ExtentAnd*>(&node);

        if( other == this )
          return true;

        if( !other )
          return false;

        return unordered_equal( other->_children, _children );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "children", _children );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        for( unsigned int i=0; i<_children.size(); i++ ) {
          _children[i]->walk(walker);
        }
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);

        ExtentAnd* duplicate = new ExtentAnd();
        duplicate->setNodeName( nodeName() );
        for( unsigned int i=0; i<_children.size(); i++ ) {
          Node* child = _children[i]->copy(copier);
          duplicate->addChild( dynamic_cast<RawExtentNode*>(child) );
        }

        return copier.after(this, duplicate);
      }
    };

    class BAndNode : public RawExtentNode {
    private:
      std::vector<RawExtentNode*> _children;

    public:
      BAndNode() {}

      BAndNode( Unpacker& unpacker ) {
        _children = unpacker.getRawExtentVector( "children" );
      }

      std::string typeName() const {
        return "BAndNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#band(";
        for( unsigned int i=0; i<_children.size(); i++ ) {
          qtext << _children[i]->queryText() << " ";
        }
        qtext << ")";

        return qtext.str();
      } 

      const std::vector<RawExtentNode*>& getChildren() const {
        return _children;
      }

      void addChild( RawExtentNode* node ) {
        _children.push_back( node );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "children", _children );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        for( int i=0; i<_children.size(); i++ ) {
          _children[i]->walk(walker);
        }
        walker.after(this);
      }
      
      Node* copy( Copier& copier ) {
        copier.before(this);
        BAndNode* duplicate = new BAndNode();

        duplicate->setNodeName( nodeName() );
        for(unsigned int i=0; i<_children.size(); i++) {
          Node* child = _children[i]->copy(copier);
          duplicate->addChild( dynamic_cast<RawExtentNode*>(child) );
        }

        return copier.after(this, duplicate);
      }
    };

    class UWNode : public RawExtentNode {
    private:
      std::vector<RawExtentNode*> _children;
      int _windowSize;

    public:
      UWNode() :
        _windowSize(-1) // default is unlimited window size
      {
      }

      UWNode( int windowSize, std::vector<RawExtentNode*>& children ) :
        _windowSize(windowSize),
        _children(children)
      {
      }

      UWNode( Unpacker& unpacker ) {
        _windowSize = (int) unpacker.getInteger( "windowSize" );
        _children = unpacker.getRawExtentVector( "children" );
      }

      std::string typeName() const {
        return "UWNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        
        if( _windowSize >= 0 )
          qtext << "#uw" << _windowSize << "( ";
        else
          qtext << "#uw( ";
          
        for( unsigned int i=0; i<_children.size(); i++ ) {
          qtext << _children[i]->queryText() << " ";
        }
        qtext << ")";
        return qtext.str();
      }

      void setWindowSize( int windowSize ) {
        _windowSize = windowSize;
      }

      void setWindowSize( const std::string& windowSize ) {
        setWindowSize( atoi( windowSize.c_str() ) );
      }

      int getWindowSize() const {
        return _windowSize;
      }

      const std::vector<RawExtentNode*>& getChildren() const {
        return _children;
      }

      void addChild( RawExtentNode* node ) {
        _children.push_back( node );
      }

      bool operator== ( Node& node ) {
        UWNode* other = dynamic_cast<UWNode*>(&node);

        if( !other )
          return false;

        if( other == this )
          return true;

        if( other->_windowSize != _windowSize ) {
          return false;
        }

        return unordered_equal( _children, other->_children );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "windowSize", _windowSize );
        packer.put( "children", _children );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        for(unsigned int i=0; i<_children.size(); i++) {
          _children[i]->walk(walker);
        }
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        
        UWNode* duplicate = new UWNode();
        duplicate->setNodeName( nodeName() );
        duplicate->setWindowSize( _windowSize );
        for(unsigned int i=0; i<_children.size(); i++) {
          Node* child = _children[i]->copy(copier);
          duplicate->addChild( dynamic_cast<RawExtentNode*>(child) );
        }

        return copier.after(this, duplicate);
      }
    };

    class ODNode : public RawExtentNode {
    private:
      int _windowSize;
      std::vector<RawExtentNode*> _children;

    public:
      ODNode( int windowSize, std::vector<RawExtentNode*>& children ) :
        _windowSize(windowSize),
        _children(children)
      {
      }

      ODNode() :
        _windowSize(-1) // default is unlimited window size
      {
      }

      ODNode( Unpacker& unpacker ) {
        _windowSize = (int) unpacker.getInteger( "windowSize" );
        _children = unpacker.getRawExtentVector( "children" );
      }

      std::string typeName() const {
        return "ODNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        if( _windowSize >= 0 )
          qtext << "#" << _windowSize << "( ";
        else
          qtext << "#od( ";

        for( unsigned int i=0; i<_children.size(); i++ ) {
          qtext << _children[i]->queryText() << " ";
        }
        qtext << ")";
        return qtext.str();
      }

      const std::vector<RawExtentNode*>& getChildren() const {
        return _children;
      }

      void setWindowSize( int windowSize ) {
        _windowSize = windowSize;
      }

      void setWindowSize( const std::string& windowSize ) {
        setWindowSize( atoi( windowSize.c_str() ) );
      }

      int getWindowSize() const {
        return _windowSize;
      }

      void addChild( RawExtentNode* node ) {
        _children.push_back( node );
      }

      bool operator== ( Node& node ) {
        ODNode* other = dynamic_cast<ODNode*>(&node);

        if( ! other )
          return false;

        if( other == this )
          return true;

        if( other->_windowSize != _windowSize )
          return false;

        if( _children.size() != other->_children.size() )
          return false;

        return equal( _children, other->_children );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "windowSize", _windowSize );
        packer.put( "children", _children );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        for(unsigned int i=0; i<_children.size(); i++) {
          _children[i]->walk(walker);
        }
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        
        ODNode* duplicate = new ODNode();
        duplicate->setWindowSize( _windowSize );
        for(unsigned int i=0; i<_children.size(); i++) {
          Node* child = _children[i]->copy(copier);
          duplicate->addChild( dynamic_cast<RawExtentNode*>(child) );
        }

        return copier.after(this, duplicate);
      }
    };

    class FilReqNode : public RawExtentNode {
    private:
      RawExtentNode* _filtered;
      RawExtentNode* _required;

    public:
      FilReqNode( RawExtentNode* filtered, RawExtentNode* required ) {
        _filtered = filtered;
        _required = required;
      }

      FilReqNode( Unpacker& unpacker ) {
        _filtered = unpacker.getRawExtentNode( "filtered" );
        _required = unpacker.getRawExtentNode( "required" );
      }

      std::string typeName() const {
        return "FilReqNode";
      }

      std::string queryText() const {
        std::stringstream qtext;

        qtext << "#filreq("
              << _filtered->queryText()
              << " "
              << _required->queryText()
              << ")";
        return qtext.str();
      }

      RawExtentNode* getFiltered() {
        return _filtered;
      }

      RawExtentNode* getRequired() {
        return _required;
      }

      bool operator== ( Node& node ) {
        FilReqNode* other = dynamic_cast<FilReqNode*>(&node);

        if( !other )
          return false;

        return (*_filtered) == (*other->getFiltered()) &&
               (*_required) == (*other->getRequired());
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("filtered", _filtered);
        packer.put("required", _required);
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _filtered->walk(walker);
        _required->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* filteredDuplicate = dynamic_cast<RawExtentNode*>(_filtered->copy(copier));
        RawExtentNode* requiredDuplicate = dynamic_cast<RawExtentNode*>(_required->copy(copier));
        FilReqNode* duplicate = new FilReqNode( filteredDuplicate, requiredDuplicate );
        return copier.after(this, duplicate);
      }
    };

    class FilRejNode : public RawExtentNode {
    private:
      RawExtentNode* _filtered;
      RawExtentNode* _disallowed;

    public:
      FilRejNode( RawExtentNode* filtered, RawExtentNode* disallowed ) {
        _filtered = filtered;
        _disallowed = disallowed;
      }

      FilRejNode( Unpacker& unpacker ) {
        _filtered = unpacker.getRawExtentNode( "filtered" );
        _disallowed = unpacker.getRawExtentNode( "disallowed" );
      }

      std::string typeName() const {
        return "FilRejNode";
      }

      std::string queryText() const {
        std::stringstream qtext;

        qtext << "#filrej("
              << _filtered->queryText()
              << " "
              << _disallowed->queryText()
              << ")";

        return qtext.str();
      }

      RawExtentNode* getFiltered() {
        return _filtered;
      }

      RawExtentNode* getDisallowed() {
        return _disallowed;
      }

      bool operator== ( Node& node ) {
        FilRejNode* other = dynamic_cast<FilRejNode*>(&node);

        if( !other )
          return false;

        return (*_filtered) == (*other->getFiltered()) &&
               (*_disallowed) == (*other->getDisallowed());
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("filtered", _filtered);
        packer.put("disallowed", _disallowed);
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _filtered->walk(walker);
        _disallowed->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* filteredDuplicate = dynamic_cast<RawExtentNode*>(_filtered->copy(copier));
        RawExtentNode* disallowedDuplicate = dynamic_cast<RawExtentNode*>(_disallowed->copy(copier));
        FilRejNode* duplicate = new FilRejNode( filteredDuplicate, disallowedDuplicate );
        return copier.after(this, duplicate);
      }
    };

    class FieldLessNode : public RawExtentNode {
    private:
      RawExtentNode* _field;
      INT64 _constant;

    public:
      FieldLessNode( RawExtentNode* field, INT64 constant ) :
        _field(field),
        _constant(constant) {
      }
      
      FieldLessNode( Unpacker& unpacker ) {
        _field = unpacker.getRawExtentNode( "field" );
        _constant = unpacker.getInteger("constant");
      }

      std::string typeName() const {
        return "FieldLessNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#less(" << _field->queryText() << " " << _constant << ")";
        return qtext.str();
      }

      INT64 getConstant() const {
        return _constant;
      }

      RawExtentNode* getField() {
        return _field;
      }

      bool operator== ( Node& node ) {
        FieldLessNode* other = dynamic_cast<FieldLessNode*>(&node);

        return other &&
               other->getConstant() == _constant &&
               *other->getField() == *_field;
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* fieldDuplicate = dynamic_cast<RawExtentNode*>(_field->copy(copier));
        FieldLessNode* duplicate = new FieldLessNode( fieldDuplicate, _constant );
        return copier.after(this, duplicate);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _field->walk(walker);
        walker.after(this);
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("field", _field);
        packer.put("constant", _constant);
        packer.after(this);
      }
    };

    class FieldGreaterNode : public RawExtentNode {
    private:
      RawExtentNode* _field;
      INT64 _constant;

    public:
      FieldGreaterNode( RawExtentNode* field, INT64 constant ) :
        _field(field),
        _constant(constant) {
      }
      
      FieldGreaterNode( Unpacker& unpacker ) {
        _field = unpacker.getRawExtentNode( "field" );
        _constant = unpacker.getInteger("constant");
      }

      std::string typeName() const {
        return "FieldGreaterNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#greater(" << _field->queryText() << " " << _constant << ")";
        return qtext.str();
      }

      INT64 getConstant() const {
        return _constant;
      }

      RawExtentNode* getField() {
        return _field;
      }

      bool operator== ( Node& node ) {
        FieldGreaterNode* other = dynamic_cast<FieldGreaterNode*>(&node);

        return other &&
               other->getConstant() == _constant &&
               *other->getField() == *_field;
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* fieldDuplicate = dynamic_cast<RawExtentNode*>(_field->copy(copier));
        FieldGreaterNode* duplicate = new FieldGreaterNode( fieldDuplicate, _constant );
        return copier.after(this, duplicate);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _field->walk(walker);
        walker.after(this);
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("field", _field);
        packer.put("constant", _constant);
        packer.after(this);
      }
    };

    class FieldBetweenNode : public RawExtentNode {
    private:
      RawExtentNode* _field;
      INT64 _low;
      INT64 _high;

    public:
      FieldBetweenNode( RawExtentNode* field, INT64 low, INT64 high ) :
        _field(field),
        _low(low),
        _high(high) {
      }
      
      FieldBetweenNode( Unpacker& unpacker ) {
        _field = unpacker.getRawExtentNode( "field" );
        _low = unpacker.getInteger("low");
        _high = unpacker.getInteger("high");
      }

      std::string typeName() const {
        return "FieldBetweenNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#between(" << _field->queryText() << " " << _low << " " << _high << ")";
        return qtext.str();
      }

      INT64 getLow() const {
        return _low;
      }

      INT64 getHigh() const {
        return _high;
      }

      RawExtentNode* getField() {
        return _field;
      }

      bool operator== ( Node& node ) {
        FieldBetweenNode* other = dynamic_cast<FieldBetweenNode*>(&node);

        return other &&
               other->getLow() == _low &&
               other->getHigh() == _high &&
               *other->getField() == *_field;
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* fieldDuplicate = dynamic_cast<RawExtentNode*>(_field->copy(copier));
        FieldBetweenNode* duplicate = new FieldBetweenNode( fieldDuplicate, _low, _high );
        return copier.after(this, duplicate);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _field->walk(walker);
        walker.after(this);
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("field", _field);
        packer.put("low", _low);
        packer.put("high", _high);
        packer.after(this);
      }
    };

    class FieldEqualsNode : public RawExtentNode {
    private:
      RawExtentNode* _field;
      INT64 _constant;

    public:
      FieldEqualsNode( RawExtentNode* field, INT64 constant ) :
        _field(field),
        _constant(constant) {
      }
      
      FieldEqualsNode( Unpacker& unpacker ) {
        _field = unpacker.getRawExtentNode("field");
        _constant = unpacker.getInteger("constant");
      }

      std::string typeName() const {
        return "FieldEqualsNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#equals(" << _field->queryText() << " " << _constant << ")";
        return qtext.str();
      }

      INT64 getConstant() const {
        return _constant;
      }

      RawExtentNode* getField() {
        return _field;
      }

      bool operator== ( Node& node ) {
        FieldEqualsNode* other = dynamic_cast<FieldEqualsNode*>(&node);

        return other &&
               other->getConstant() == _constant &&
               *other->getField() == *_field;
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* fieldDuplicate = dynamic_cast<RawExtentNode*>(_field->copy(copier));
        FieldEqualsNode* duplicate = new FieldEqualsNode( fieldDuplicate, _constant );
        return copier.after(this, duplicate);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _field->walk(walker);
        walker.after(this);
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("field", _field);
        packer.put("constant", _constant);
        packer.after(this);
      }
    };

    class RawScorerNode : public ScoredExtentNode {
    private:
      UINT64 _occurrences; // number of occurrences within this context
      UINT64 _contextSize; // number of terms that occur within this context
      UINT64 _maximumContextLength;
      UINT64 _minimumContextLength;
      UINT64 _maximumOccurrences;
      double _maximumContextFraction;

      RawExtentNode* _raw;
      RawExtentNode* _context;
      std::string _smoothing;

    public:
      RawScorerNode( RawExtentNode* raw, RawExtentNode* context, std::string smoothing = "method:dirichlet,mu:2500" ) {
        _raw = raw;
        _context = context;

        _occurrences = 0;
        _contextSize = 0;
        _maximumContextLength = MAX_INT32;
        _maximumOccurrences = MAX_INT32;
        _minimumContextLength = 1;
        _maximumContextFraction = 1;
        _smoothing = smoothing;
      }

      RawScorerNode( Unpacker& unpacker ) {
        _raw = unpacker.getRawExtentNode( "raw" );
        _context = unpacker.getRawExtentNode( "context" );

        _occurrences = unpacker.getInteger( "occurrences" );
        _contextSize = unpacker.getInteger( "contextSize" );
        _maximumContextLength = unpacker.getInteger( "maximumContextLength" );
        _minimumContextLength = unpacker.getInteger( "minimumContextLength" );
        _maximumOccurrences = unpacker.getInteger( "maximumOccurrences" );
        _maximumContextFraction = unpacker.getDouble( "maximumContextFraction" );
        _smoothing = unpacker.getString( "smoothing" );
      }

      std::string typeName() const {
        return "RawScorerNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        
        qtext << _raw->queryText();
        if( _context ) {
          // if we haven't added a period yet, put one in
          int dot = qtext.str().find('.');
          if( dot < 0 )
            qtext << '.';

          qtext << "(" << _context->queryText() << ")";
        }

        return qtext.str();
      }

      UINT64 getOccurrences() const {
        return _occurrences;
      }

      UINT64 getContextSize() const {
        return _contextSize;
      }

      const std::string& getSmoothing() const {
        return _smoothing;
      }

      UINT64 getMaxContextLength() const {
        return _maximumContextLength;
      }

      UINT64 getMinContextLength() const {
        return _minimumContextLength;
      }

      UINT64 getMaxOccurrences() const {
        return _maximumOccurrences;
      }

      double getMaxContextFraction() const {
        return _maximumContextFraction;
      }

      void setStatistics( UINT64 occurrences, UINT64 contextSize ) {
        _occurrences = occurrences;
        _contextSize = contextSize;
      }

      void setStatistics( UINT64 occurrences, UINT64 contextSize,
                          UINT64 maxOccurrences, UINT64 minContextLength, UINT64 maxContextLength,
                          double maxContextFraction ) {
        _occurrences = occurrences;
        _contextSize = contextSize;

        _maximumOccurrences = maxOccurrences;
        _minimumContextLength = minContextLength;
        _maximumContextLength = maxContextLength;
        _maximumContextFraction = maxContextFraction;
      }

      void setContext( RawExtentNode* context ) {
        _context = context;
      }

      void setRawExtent( RawExtentNode* rawExtent ) {
        _raw = rawExtent;
      }

      void setSmoothing( const std::string& smoothing ) {
        _smoothing = smoothing;
      }

      RawExtentNode* getContext() {
        return _context;
      }

      RawExtentNode* getRawExtent() {
        return _raw;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "raw", _raw );
        packer.put( "context", _context );

        packer.put( "occurrences", _occurrences );
        packer.put( "contextSize", _contextSize );
        packer.put( "minimumContextLength", _minimumContextLength );
        packer.put( "maximumContextLength", _maximumContextLength );
        packer.put( "maximumOccurrences", _maximumOccurrences );
        packer.put( "maximumContextFraction", _maximumContextFraction );
        packer.put( "smoothing", _smoothing );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        if( _raw )
          _raw->walk(walker);
        if( _context )
          _context->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);

        RawExtentNode* duplicateContext = _context ? dynamic_cast<RawExtentNode*>(_context->copy(copier)) : 0;
        RawExtentNode* duplicateRaw = _raw ? dynamic_cast<RawExtentNode*>(_raw->copy(copier)) : 0;
        RawScorerNode* duplicate = new RawScorerNode(*this);
        duplicate->setRawExtent( duplicateRaw );
        duplicate->setContext( duplicateContext );

        return copier.after(this, duplicate);
      }
    };

    class TermFrequencyScorerNode : public ScoredExtentNode {
    private:
      UINT64 _occurrences; // number of occurrences within this context
      UINT64 _contextSize; // number of terms that occur within this context
      UINT64 _maximumContextLength;
      UINT64 _minimumContextLength;
      UINT64 _maximumOccurrences;
      double _maximumContextFraction;

      std::string _text;
      std::string _smoothing;
      bool _stemmed;

    public:
      TermFrequencyScorerNode( const std::string& text, bool stemmed ) {
        _occurrences = 0;
        _contextSize = 0;
        _maximumContextLength = MAX_INT32;
        _maximumOccurrences = MAX_INT32;
        _minimumContextLength = 1;
        _maximumContextFraction = 1;
        _smoothing = "";
        _text = text;
        _stemmed = stemmed;
      }

      TermFrequencyScorerNode( Unpacker& unpacker ) {
        _occurrences = unpacker.getInteger( "occurrences" );
        _contextSize = unpacker.getInteger( "contextSize" );
        _maximumContextLength = unpacker.getInteger( "maximumContextLength" );
        _minimumContextLength = unpacker.getInteger( "minimumContextLength" );
        _maximumOccurrences = unpacker.getInteger( "maximumOccurrences" );
        _maximumContextFraction = unpacker.getDouble( "maximumContextFraction" );
        _smoothing = unpacker.getString( "smoothing" );
        _text = unpacker.getString( "text" );
        _stemmed = unpacker.getBoolean( "stemmed" );
      }
      
      const std::string& getText() const {
        return _text;
      }

      bool getStemmed() const {
        return _stemmed;
      }

      std::string typeName() const {
        return "TermFrequencyScorerNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        
        if( !_stemmed )
          qtext << _text;
        else
          qtext << "\"" << _text << "\"";

        return qtext.str();
      }

      UINT64 getOccurrences() const {
        return _occurrences;
      }

      UINT64 getContextSize() const {
        return _contextSize;
      }

      const std::string& getSmoothing() const {
        return _smoothing;
      }

      UINT64 getMaxContextLength() const {
        return _maximumContextLength;
      }

      UINT64 getMinContextLength() const {
        return _minimumContextLength;
      }

      UINT64 getMaxOccurrences() const {
        return _maximumOccurrences;
      }

      double getMaxContextFraction() const {
        return _maximumContextFraction;
      }

      void setStatistics( UINT64 occurrences, UINT64 contextSize,
                          UINT64 maxOccurrences,
                          UINT64 minContextLength, UINT64 maxContextLength,
                          double maxContextFraction ) {
        _occurrences = occurrences;
        _contextSize = contextSize;

        _maximumOccurrences = maxOccurrences;
        _minimumContextLength = minContextLength;
        _maximumContextLength = maxContextLength;
        _maximumContextFraction = maxContextFraction;
      }

      void setSmoothing( const std::string& smoothing ) {
        _smoothing = smoothing;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "occurrences", _occurrences );
        packer.put( "contextSize", _contextSize );
        packer.put( "minimumContextLength", _minimumContextLength );
        packer.put( "maximumContextLength", _maximumContextLength );
        packer.put( "maximumOccurrences", _maximumOccurrences );
        packer.put( "maximumContextFraction", _maximumContextFraction );
        packer.put( "text", _text );
        packer.put( "stemmed", _stemmed );
        packer.put( "smoothing", _smoothing );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        TermFrequencyScorerNode* duplicate = new TermFrequencyScorerNode(*this);
        return copier.after(this, duplicate);
      }
    };

    // The CachedFrequencyScorerNode should only be used on a local machine;
    // it should not be transferred across the network
    class CachedFrequencyScorerNode : public indri::lang::ScoredExtentNode {
    private:
      indri::lang::Node* _raw;
      indri::lang::Node* _context;
      std::string _smoothing;
      void* _list;
    
    public:
      CachedFrequencyScorerNode( indri::lang::Node* raw, indri::lang::Node* context )
        :
        _raw(raw),
        _context(context),
        _list(0)
      {
      }

      CachedFrequencyScorerNode( Unpacker& unpacker ) {
        LEMUR_THROW( LEMUR_RUNTIME_ERROR, "CachedFrequencyScorerNode should not be used on the network" );

        _raw = unpacker.getRawExtentNode( "raw" );
        _context = unpacker.getRawExtentNode( "context" );
        _smoothing = unpacker.getString( "smoothing" );
      }

      void setList( void* list ) {
        _list = list;
      }

      void* getList() {
        return _list;
      }

      std::string typeName() const {
        return "CachedFrequencyScorerNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        
        qtext << _raw->queryText();
        if( _context ) {
          // if we haven't added a period yet, put one in
          int dot = qtext.str().find('.');
          if( dot < 0 )
            qtext << '.';

          qtext << "(" << _context->queryText() << ")";
        }

        return qtext.str();
      }

      void setSmoothing( const std::string& smoothing ) {
        _smoothing = smoothing;
      }

      const std::string& getSmoothing() const {
        return _smoothing;
      }

      indri::lang::Node* getRaw() {
        return _raw;
      }

      indri::lang::Node* getContext() {
        return _context;
      }
      
      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "raw", _raw );
        packer.put( "context", _context );
        packer.put( "smoothing", _smoothing );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _raw->walk( walker );
        if( _context )
          _context->walk( walker );
        walker.after(this);
      }

      indri::lang::Node* copy( Copier& copier ) {
        copier.before(this);

        indri::lang::RawExtentNode* duplicateRaw = dynamic_cast<indri::lang::RawExtentNode*>(_raw->copy(copier));
        indri::lang::RawExtentNode* duplicateContext = 0;

        if( _context ) 
          duplicateContext = dynamic_cast<indri::lang::RawExtentNode*>(_context->copy(copier));

        CachedFrequencyScorerNode* duplicate = new CachedFrequencyScorerNode( duplicateRaw,
                                                                              duplicateContext );
        duplicate->setNodeName( nodeName() );
        duplicate->setSmoothing( _smoothing );
        duplicate->setList( getList() );

        return copier.after( this, duplicate );
      }
    };

    class PriorNode : public ScoredExtentNode {
    public:
      struct tuple_type {
        int begin;
        int end;
        double score;
      };

    private:
      std::map<int,tuple_type> _table;
      Field* _field;
      std::string _fieldName;

    public:
      PriorNode() :
        _field(0)
      {
      }

      PriorNode( indri::lang::Field* field, const std::map<int, tuple_type>& table ) :
        _table(table),
        _field(field)
      {
        _fieldName = _field->getFieldName();  
      }

      PriorNode( std::string& fieldName, const std::map<int, tuple_type>& table ) :
        _fieldName(fieldName),
        _field(0),
        _table(table)
      {
      }

      std::string queryText() const {
        std::stringstream qtext;
        // with the definition of priors somewhat in flux, it's
        // hard to know what would be good to put here.
        qtext << "#prior(" << _fieldName << ")";
        return qtext.str();
      }

      PriorNode( Unpacker& unpacker ) {
        std::vector<int> beginList = unpacker.getIntVector( "begin" );
        std::vector<int> endList = unpacker.getIntVector( "end" );
        std::vector<double> scoreList = unpacker.getDoubleVector( "score" );
        assert( beginList.size() == endList.size() );
        assert( scoreList.size() == endList.size() );

        for( size_t i=0; i<beginList.size(); i++ ) {
          tuple_type t;
          t.begin = beginList[i];
          t.end = endList[i];
          t.score = scoreList[i];

          _table[ beginList[i] ] = t;
        }
 
        _field = dynamic_cast<Field*>(unpacker.getRawExtentNode( "field" ));
      }

      std::string nodeType() {
        return "PriorNode";
      }
      
      const std::map<int,tuple_type>& getTable() const {
        return _table;
      }

      const std::string& getFieldName() const {
        return _fieldName;
      }

      indri::lang::Field* getField() const {
        return _field;
      }

      void setField( indri::lang::Field* field ) {
        _field = field;
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _field->walk(walker);
        walker.after(this);
      }

      indri::lang::Node* copy( Copier& copier ) {
        copier.before(this);
        Field* duplicateField = dynamic_cast<Field*>(_field->copy(copier));
        PriorNode* duplicate = new PriorNode( duplicateField, getTable() );
        return copier.after(this, duplicate);
      }

      void pack( Packer& packer ) {
        packer.before(this);

        std::vector<int> beginList;
        std::vector<int> endList;
        std::vector<double> scoreList;

        for( std::map<int,tuple_type>::iterator iter;
             iter != _table.end();
             iter++ )
        {
          beginList.push_back( (*iter).second.begin );
          endList.push_back( (*iter).second.end );
          scoreList.push_back( (*iter).second.score );
        }

        packer.put( "begin", beginList );
        packer.put( "end", endList );
        packer.put( "score", scoreList );
        packer.put( "field", _field );
        packer.after(this);
      }
    };

    /* abstract */ class UnweightedCombinationNode : public ScoredExtentNode {
    protected:
      std::vector<ScoredExtentNode*> _children;

      void _unpack( Unpacker& unpacker ) {
        _children = unpacker.getScoredExtentVector( "children" );
      }

      template<class _ThisType>
      void _walk( _ThisType* ptr, Walker& walker ) {
        walker.before(ptr);

        for( unsigned int i=0; i<_children.size(); i++ ) {
          _children[i]->walk(walker);
        }
        
        walker.after(ptr);
      }

      template<class _ThisType>
      Node* _copy( _ThisType* ptr, Copier& copier ) {
        copier.before(ptr);
        
        _ThisType* duplicate = new _ThisType();
        duplicate->setNodeName( nodeName() );
        for( unsigned int i=0; i<_children.size(); i++ ) {
          duplicate->addChild( dynamic_cast<ScoredExtentNode*>(_children[i]->copy(copier)) );
        } 

        return copier.after(ptr, duplicate);
      }

      void _childText( std::stringstream& qtext ) const {
        for( unsigned int i=0; i<_children.size(); i++ ) {
          if(i>0) qtext << " ";
          qtext << _children[i]->queryText();
        }
      }

    public:
      const std::vector<ScoredExtentNode*>& getChildren() {
        return _children;
      }

      void addChild( ScoredExtentNode* scoredNode ) {
        _children.push_back( scoredNode );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "children", _children );
        packer.after(this);
      }
    };

    /* abstract */ class WeightedCombinationNode : public ScoredExtentNode {
    protected:
      std::vector< std::pair<double, ScoredExtentNode*> > _children;

      void _unpack( Unpacker& unpacker ) {
        std::vector<double> weights = unpacker.getDoubleVector( "weights" );
        std::vector<ScoredExtentNode*> nodes = unpacker.getScoredExtentVector( "children" );

        for( unsigned int i=0; i<weights.size(); i++ ) {
          _children.push_back( std::make_pair( weights[i], nodes[i] ) );
        }
      }

      template<class _ThisType>
      void _walk( _ThisType* ptr, Walker& walker ) {
        walker.before(ptr);
        for( unsigned int i=0; i<_children.size(); i++ ) {
          _children[i].second->walk(walker);
        }
        walker.after(ptr);
      }

      template<class _ThisType>
      Node* _copy( _ThisType* ptr, Copier& copier ) {
        copier.before(ptr);

        _ThisType* duplicate = new _ThisType;
        duplicate->setNodeName( nodeName() );
        for( unsigned int i=0; i<_children.size(); i++ ) {
          double childWeight = _children[i].first;
          Node* childCopy = _children[i].second->copy( copier );

          duplicate->addChild( childWeight, dynamic_cast<ScoredExtentNode*>(childCopy) );
        }
        return copier.after(ptr, duplicate);
      }

      void _childText( std::stringstream& qtext ) const {
        for( unsigned int i=0; i<_children.size(); i++ ) {
          if(i>0) qtext << " ";
          qtext << _children[i].first
                << " "
                << _children[i].second->queryText();
        }
      }

    public:
      const std::vector< std::pair<double, ScoredExtentNode*> >& getChildren() {
        return _children;
      }

      void addChild( double weight, ScoredExtentNode* scoredNode ) {
        _children.push_back( std::make_pair( weight, scoredNode) );
      }

      void addChild( const std::string& weight, ScoredExtentNode* scoredNode ) {
        addChild( atof( weight.c_str() ), scoredNode );
      }

      void pack( Packer& packer ) {
        packer.before(this);
        
        std::vector<double> weights;
        std::vector<ScoredExtentNode*> nodes;

        for( unsigned int i=0; i<_children.size(); i++ ) {
          weights.push_back( _children[i].first );
          nodes.push_back( _children[i].second );
        }

        packer.put( "weights", weights );
        packer.put( "children", nodes );
        packer.after(this);
      }
    };

    class OrNode : public UnweightedCombinationNode {
    public:
      OrNode() {}
      OrNode( Unpacker& unpacker ) {
        _unpack( unpacker );
      }

      std::string typeName() const {
        return "OrNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#or(";
        _childText(qtext);
        qtext << ")";

        return qtext.str();
      } 

      void walk( Walker& walker ) {
        _walk( this, walker );
      }
      
      Node* copy( Copier& copier ) {
        return _copy( this, copier );
      }
    };

    class NotNode : public ScoredExtentNode {
    private:
      ScoredExtentNode* _child;

    public:
      NotNode() {
        _child = 0;
      }

      NotNode( ScoredExtentNode* child ) {
        _child = child;
      }

      NotNode( Unpacker& unpacker ) {
        _child = unpacker.getScoredExtentNode( "child" );
      }

      std::string typeName() const {
        return "NotNode";
      }

      ScoredExtentNode* getChild() {
        return _child;
      }

      void setChild( ScoredExtentNode* child ) {
        _child = child;
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#not(";
        qtext << _child->queryText();
        qtext << ")";

        return qtext.str();
      } 

      void walk( Walker& walker ) {
        walker.before(this);
        _child->walk(walker);
        walker.after(this);
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "child", _child );
        packer.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before( this );
        ScoredExtentNode* childDuplicate = dynamic_cast<ScoredExtentNode*>(_child->copy(copier));
        NotNode* duplicate = new NotNode( childDuplicate );
        duplicate->setNodeName( nodeName() );
        return copier.after( this, duplicate );
      }
    };

    class MaxNode : public UnweightedCombinationNode {
    public:
      MaxNode() {}
      MaxNode( Unpacker& unpacker ) {
        _unpack( unpacker );
      }

      std::string typeName() const {
        return "MaxNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#max(";
        _childText(qtext);
        qtext << ")";

        return qtext.str();
      } 


      void walk( Walker& walker ) {
        _walk(this, walker);
      }

      Node* copy( Copier& copier ) {
        return _copy( this, copier );
      }
    };

    class CombineNode : public UnweightedCombinationNode {
    public:
      CombineNode() {}
      CombineNode( Unpacker& unpacker ) {
        _unpack( unpacker );
      }

      std::string typeName() const {
        return "CombineNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#combine(";
        _childText(qtext);
        qtext << ")";

        return qtext.str();
      } 

      void walk( Walker& walker ) {
        _walk( this, walker );
      }
      
      Node* copy( Copier& copier ) {
        return _copy( this, copier );
      }
    };

    class WAndNode : public WeightedCombinationNode {
    public:
      WAndNode() {}
      WAndNode( Unpacker& unpacker ) {
        _unpack( unpacker );
      }

      std::string typeName() const {
        return "WAndNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#wand(";
        _childText(qtext);
        qtext << ")";

        return qtext.str();
      } 

      void walk( Walker& walker ) {
        _walk( this, walker );
      }

      Node* copy( Copier& copier ) {
        return _copy( this, copier );
      }
    };

    class WSumNode : public WeightedCombinationNode {
    public:
      WSumNode() {}
      WSumNode( Unpacker& unpacker ) {
        _unpack( unpacker );
      }

      std::string typeName() const {
        return "WSumNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#wsum(";
        _childText(qtext);
        qtext << ")";

        return qtext.str();
      } 

      void walk( Walker& walker ) {
        _walk( this, walker );
      }

      Node* copy( Copier& copier ) {
        return _copy( this, copier );
      }
    };

    class WeightNode : public WeightedCombinationNode {
    public:
      WeightNode() {}
      WeightNode( Unpacker& unpacker ) {
        _unpack( unpacker );
      }

      std::string typeName() const {
        return "WeightNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        qtext << "#weight(";
        _childText(qtext);
        qtext << ")";

        return qtext.str();
      } 

      void walk( Walker& walker ) {
        _walk( this, walker );
      }

      Node* copy( Copier& copier ) {
        return _copy( this, copier );
      }
    };

    class ExtentRestriction : public ScoredExtentNode {
    private:
      ScoredExtentNode* _child;
      RawExtentNode* _field;

    public:
      ExtentRestriction( Unpacker& unpacker ) {
        _child = unpacker.getScoredExtentNode("child");
        _field = unpacker.getRawExtentNode("field");
      }

      ExtentRestriction( ScoredExtentNode* child, RawExtentNode* field ) :
        _child(child),
        _field(field)
      {
      }

      std::string typeName() const {
        return "ExtentRestriction";
      }

      std::string queryText() const {
        std::stringstream qtext;
        // this extent restriction is almost certainly because of some #combine or #max operator
        // in the _child position.  We look for the first parenthesis (e.g. #combine(dog cat)) and
        // insert the brackets in.
        
        std::string childText = _child->queryText();
        std::string::size_type pos = childText.find( '(' );

        if( pos != std::string::npos ) {
          qtext << childText.substr(0,pos) 
                << "["
                << _field->queryText()
                << "]"
                << childText.substr(pos);
        } else {
          // couldn't find a parenthesis, so we'll tack the [field] on the front
          qtext << "["
                << _field->queryText()
                << "]"
                << childText;
        }

        return qtext.str();
      } 

      ScoredExtentNode* getChild() {
        return _child;
      }

      RawExtentNode* getField() {
        return _field;
      }

      void setChild( ScoredExtentNode* child ) {
        _child = child;
      }

      void setField( RawExtentNode* field ) {
        _field = field;
      }
      
      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("child", _child);
        packer.put("field", _field);
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _child->walk(walker);
        _field->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);

        ScoredExtentNode* duplicateChild = dynamic_cast<indri::lang::ScoredExtentNode*>(_child->copy(copier));
        RawExtentNode* duplicateField = dynamic_cast<indri::lang::RawExtentNode*>(_field->copy(copier));
        ExtentRestriction* duplicate = new ExtentRestriction( duplicateChild, duplicateField );
        duplicate->setNodeName( nodeName() );
        
        return copier.after(this, duplicate);
      }
    };

    class FilterNode : public ScoredExtentNode {
    private:
      ScoredExtentNode* _child;
      std::vector<int> _documents;

    public:
      FilterNode( ScoredExtentNode* child, std::vector<int>& documents ) : 
        _child(child),
        _documents(documents)
      {
      }

      FilterNode( Unpacker& unpacker ) {
        _child = unpacker.getScoredExtentNode( "child" );
        _documents = unpacker.getIntVector( "documents" );
      }

      std::string typeName() const {
        return "FilterNode";
      }

      ScoredExtentNode* getChild() {
        return _child;
      }

      const std::vector<int>& getDocuments() const {
        return _documents;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put("child", _child);
        packer.put("documents", _documents);
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _child->walk(walker);
        walker.after(this);
      }

      std::string queryText() const {
        // for now, we'll let the filter be anonymous, since it can never
        // be typed by the user
        return _child->queryText();
      }

      Node* copy( Copier& copier ) {
        copier.before(this);

        ScoredExtentNode* duplicateChild = dynamic_cast<ScoredExtentNode*>(_child->copy(copier));
        FilterNode* duplicate = new FilterNode( duplicateChild, _documents );
        duplicate->setNodeName( nodeName() );

        return copier.after(this, duplicate);
      }
    };

    class ContextCounterNode : public AccumulatorNode {
    private:
      RawExtentNode* _raw;
      RawExtentNode* _context;
      bool _hasCounts;
      bool _hasContextSize;
      bool _hasMaxScore;
      UINT64 _occurrences;
      UINT64 _contextSize;

      UINT64 _minimumContextLength;
      UINT64 _maximumContextLength;
      UINT64 _maximumOccurrences;
      double _maximumContextFraction;

    public:
      ContextCounterNode( RawExtentNode* raw, RawExtentNode* context ) :
         _hasCounts(false),
         _hasContextSize(false),
         _hasMaxScore(false),
         _occurrences(0),
         _contextSize(0),
         _minimumContextLength(1),
         _maximumContextLength(MAX_INT32),
         _maximumOccurrences(MAX_INT32),
         _maximumContextFraction(1.0)
      {
        _raw = raw;
        _context = context;
      }

      ContextCounterNode( Unpacker& unpacker ) {
        _raw = unpacker.getRawExtentNode( "raw" );
        _context = unpacker.getRawExtentNode( "context" );
        _occurrences = unpacker.getInteger( "occurrences" );
        _contextSize = unpacker.getInteger( "contextSize" );

        _minimumContextLength = unpacker.getInteger( "minimumContextLength" );
        _maximumContextLength = unpacker.getInteger( "maximumContextLength" );
        _maximumOccurrences = unpacker.getInteger( "maximumOccurrences" );
        _maximumContextFraction = unpacker.getDouble( "maximumContextFraction" );

        _hasCounts = unpacker.getBoolean( "hasCounts" );
        _hasContextSize = unpacker.getBoolean( "hasContextSize" );
        _hasMaxScore = unpacker.getBoolean( "hasMaxScore" );
      }

      std::string typeName() const {
        return "ContextCounterNode";
      }

      std::string queryText() const {
        std::stringstream qtext;
        
        if( _raw )
          qtext << _raw->queryText();

        if( _context ) {
          // if we haven't added a period yet, put one in
          int dot = qtext.str().find('.');
          if( dot < 0 )
            qtext << '.';

          qtext << "(" << _context->queryText() << ")";
        }

        return qtext.str();
      }

      RawExtentNode* getContext() {
        return _context;
      }

      RawExtentNode* getRawExtent() {
        return _raw;
      }

      void setRawExtent( RawExtentNode* rawExtent ) {
        _raw = rawExtent;
      }

      void setContext( RawExtentNode* context ) {
        _context = context;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "raw", _raw );
        packer.put( "context", _context );
        packer.put( "occurrences", _occurrences );
        packer.put( "contextSize", _contextSize );

        packer.put( "maximumOccurrences", _maximumOccurrences );
        packer.put( "maximumContextFraction", _maximumContextFraction );
        packer.put( "maximumContextLength", _maximumContextLength );
        packer.put( "minimumContextLength", _minimumContextLength );

        packer.put( "hasCounts", _hasCounts );
        packer.put( "hasContextSize", _hasContextSize );
        packer.put( "hasMaxScore", _hasMaxScore );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        if( _raw ) _raw->walk(walker);
        if( _context ) _context->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        RawExtentNode* duplicateRaw = _raw ? dynamic_cast<RawExtentNode*>(_raw->copy(copier)) : 0;
        RawExtentNode* duplicateContext = _context ? dynamic_cast<RawExtentNode*>(_context->copy(copier)) : 0;
        ContextCounterNode* duplicate = new ContextCounterNode(*this);
        duplicate->setContext(duplicateContext);
        duplicate->setRawExtent(duplicateRaw);
        return copier.after(this, duplicate);
      }

      bool hasCounts() const {
        return _hasCounts;
      }

      bool hasContextSize() const {
        return _hasContextSize;
      }

      bool hasMaxScore() const {
        return _hasMaxScore;
      }

      UINT64 getOccurrences() const {
        return _occurrences;
      }

      UINT64 getContextSize() const {
        return _contextSize;
      }

      UINT64 getMaximumOccurrences() const {
        return _maximumOccurrences;
      }

      UINT64 getMinimumContextLength() const {
        return _minimumContextLength;
      }

      UINT64 getMaximumContextLength() const {
        return _maximumContextLength;
      }

      double getMaximumContextFraction() const {
        return _maximumContextFraction;
      }

      void setContextSize( UINT64 contextSize ) {
        _contextSize = contextSize;
        _hasContextSize = true;
      }

      void setCounts( UINT64 occurrences,
                      UINT64 contextSize ) {
        _hasCounts = true;
        _occurrences = occurrences;
        setContextSize( contextSize );
      }

      void setCounts( UINT64 occurrences,
                      UINT64 contextSize,
                      UINT64 maximumOccurrences, 
                      UINT64 minimumContextLength, 
                      UINT64 maximumContextLength,
                      double maximumContextFraction )
      {
        setCounts( occurrences, contextSize );
        _hasMaxScore = true;
        _maximumOccurrences = maximumOccurrences;
        _minimumContextLength = minimumContextLength;
        _maximumContextLength = maximumContextLength;
        _maximumContextFraction = maximumContextFraction;
      }
    };

    class ScoreAccumulatorNode : public AccumulatorNode {
    private:
      ScoredExtentNode* _scoredNode;

    public:
      ScoreAccumulatorNode( ScoredExtentNode* scoredNode ) :
        _scoredNode(scoredNode)
      {
      }

      ScoreAccumulatorNode( Unpacker& unpacker ) {
        _scoredNode = unpacker.getScoredExtentNode( "scoredNode" );
      }

      std::string typeName() const {
        return "ScoreAccumulatorNode";
      }

      std::string queryText() const {
        // anonymous
        return _scoredNode->queryText();
      }

      ScoredExtentNode* getChild() {
        return _scoredNode;
      }

      void pack( Packer& packer ) {
        packer.before(this);
        packer.put( "scoredNode", _scoredNode );
        packer.after(this);
      }

      void walk( Walker& walker ) {
        walker.before(this);
        _scoredNode->walk(walker);
        walker.after(this);
      }

      Node* copy( Copier& copier ) {
        copier.before(this);
        ScoredExtentNode* duplicateChild = dynamic_cast<ScoredExtentNode*>(_scoredNode->copy(copier));
        ScoreAccumulatorNode* duplicate = new ScoreAccumulatorNode(duplicateChild);
        duplicate->setNodeName( nodeName() );
        return copier.after(this, duplicate);
      }
    };

    class AnnotatorNode : public AccumulatorNode {
      private:
        ScoredExtentNode* _scoredNode;

      public:
        AnnotatorNode( ScoredExtentNode* scoredNode ) :
          _scoredNode(scoredNode)
        {
        }

        AnnotatorNode( Unpacker& unpacker ) {
          _scoredNode = unpacker.getScoredExtentNode( "scoredNode" );
        }

        std::string typeName() const {
          return "AnnotatorNode";
        }

        std::string queryText() const {
          return _scoredNode->queryText();
        }

        ScoredExtentNode* getChild() {
          return _scoredNode;
        }

        void pack( Packer& packer ) {
          packer.before(this);
          packer.put( "scoredNode", _scoredNode );
          packer.after(this);
        }

        void walk( Walker& walker ) {
          walker.before(this);
          _scoredNode->walk(walker);
          walker.after(this);
        }

        Node* copy( Copier& copier ) {
          copier.before(this);
          ScoredExtentNode* duplicateChild = dynamic_cast<ScoredExtentNode*>(_scoredNode->copy(copier));
          AnnotatorNode* duplicate = new AnnotatorNode(duplicateChild);
          duplicate->setNodeName( nodeName() );
          return copier.after(this, duplicate);
        }
    };
  }
}

#endif // INDRI_QUERYSPEC_HPP



