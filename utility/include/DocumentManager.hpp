/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _DOCUMENTMANAGER_HPP
#define _DOCUMENTMANAGER_HPP

/*!
  Abstract API for document manager interface.  The document manager
  should be able to retrieve a document given its external document id,
  it should also be able to provide a parser that can parse the raw
  document format.
 */

#include "Parser.hpp"

class DocumentManager {
public:

  /// open previously created document manager, return true if opened successfully.  index name should be the full name of the table-of-content file for the manager.  E.g. "manager.flat" for FlattextDocMgr.
  virtual bool open(const char* manname) = 0;

  /// returns the document with the given docID (usually in raw format)
  virtual char* getDoc(const char* docID) = 0;

  /// returns a handle to a Parser object that can handle parsing the raw format of these documents
  virtual Parser* getParser() { return myparser; } ;

  /// returns self cast to TextHandler (FIX INHERITANCE HERE!)
  virtual TextHandler* getTextHandler() { return NULL; } ;  

  /// Construct the data tables for the previously given input files. 
  virtual void buildMgr() = 0;
  /// get the ID string to use for opening this later
  virtual const char* getMyID() = 0;

  virtual ~DocumentManager() {};


protected:
  Parser* myparser;
};

#endif
