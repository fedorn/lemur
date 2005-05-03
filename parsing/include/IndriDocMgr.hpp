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

#include "IndexTypes.hpp"
#include "DocumentManager.hpp"
#include "indri/Repository.hpp"
#include "Match.hpp"
namespace lemur 
{
  namespace parse 
  {
    
    /*!
      The IndriDocMgr is able to retrieve a document given its external 
      document id, and additionally to provide the byte offsets of the
      terms within a document @see MatchInfo.
      It can not be built separately from its enclosing Repository, nor
      does it provide a Parser.
    */

    class IndriDocMgr : public lemur::api::DocumentManager {
    public:
      IndriDocMgr(indri::collection::Repository &repository, string name="IndriDocMgr") : 
        _repository(repository), _name(name) {}
      virtual ~IndriDocMgr() {}
      /// IndriDocMgr can only be opend by its enclosing Repository. 
      /// @return true vacuously
      virtual bool open(const string &manname) {
        return true;
      }

      /// returns the document with the given docID (usually in raw format)
      virtual char* getDoc(const lemur::api::EXDOCID_T &docID) const;

      /// get the array of Match entries for the tokens in the document
      /// named docID. The entries are indexed by token position (as is
      /// recorded in a TermInfoList object.
      vector<Match> getOffsets(const lemur::api::EXDOCID_T &docID) const;

  
      /// @return NULL
      virtual lemur::api::Parser* getParser() const {return NULL;}

      /// do nothing
      virtual void buildMgr() {}

      /// get the ID string
      virtual const string &getMyID() const {return _name; }  

    private:
      indri::index::Index* _indexWithDocument( indri::collection::Repository::index_state& indexes, int documentID ) const ;
  
      indri::collection::Repository &_repository;
      string _name;
    };
  }
}

#endif
// _INDRIDOCUMENTMANAGER_HPP
