/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#ifndef _INDRIDOCUMENTMANAGER_HPP
#define _INDRIDOCUMENTMANAGER_HPP

/*!
  The IndriDocMgr is able to retrieve a document given its external 
  document id, and additionally to provide the byte offsets of the
  terms within a document @see MatchInfo.
  It can not be built separately from its enclosing Repository, nor
  does it provide a Parser.
 */
#include "IndexTypes.hpp"
#include "DocumentManager.hpp"
#include "indri/Repository.hpp"
#include "Match.hpp"

class IndriDocMgr : public DocumentManager {
public:
  IndriDocMgr(Repository &repository, string name="IndriDocMgr") : 
    _repository(repository), _name(name) {}
  virtual ~IndriDocMgr() {}
  /// IndriDocMgr can only be opend by its enclosing Repository. 
  /// @return true vacuously
  virtual bool open(const string &manname) {
    return true;
  }

  /// returns the document with the given docID (usually in raw format)
  virtual char* getDoc(const EXDOCID_T &docID) const;

  /// get the array of Match entries for the tokens in the document
  /// named docID. The entries are indexed by token position (as is
  /// recorded in a TermInfoList object.
  vector<Match> getOffsets(const EXDOCID_T &docID) const;

  
  /// @return NULL
  virtual Parser* getParser() const {return NULL;}

  /// do nothing
  virtual void buildMgr() {}

  /// get the ID string
  virtual const string &getMyID() const {return _name; }  

private:
  Repository &_repository;
  string _name;
};

#endif
// _INDRIDOCUMENTMANAGER_HPP
