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

#ifndef _BASICPASSAGE_HPP
#define _BASICPASSAGE_HPP

#include <iomanip>
#include <vector>
#include <string>
#include "Passage.hpp"
using std::vector;
using std::string;

namespace lemur
{
  /// Document summarization components.
  namespace summarization
  {
    
    /*!
      A simple implementation of the abstract class <code>Passage</code> meant to work in conjunction with the simple sentence selection summarizer <code>BasicSumm</code>

    */

    class BasicPassage : public Passage {

    public:
      BasicPassage(const string &id) {
        docID = id;
        score = -1;
        marked = 0;
      } 
      
      virtual void clear() ;

      virtual void addTerm(termCount term) ;

      virtual void addTerms(const passageVec pv) ;

      virtual const passageVec* getAsVector(void) const ;

      virtual int operator<(const Passage &b) const ;

    };
  }
}

#endif
