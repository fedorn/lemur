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



#ifndef _PASSAGE_HPP
#define _PASSAGE_HPP

#include <string>
#include <vector>
using std::vector;
using std::string;
namespace lemur 
{
  namespace summarization 
  {
    
    class termCount {
    public:
      int termID;
      int tf;
      double val;
    };

    typedef vector<termCount> passageVec;

    /*!
      A <code>Passage</code> is created in conjunction with a specific <code>Summarizer</code> implementation.  This is an abstract base class.

    */

    class Passage {
      
    public:
      double score;
      int marked;
      string docID;
      passageVec psg;

      virtual ~Passage();

      /// Clear a passage (make empty and clear score, etc.)
      virtual void clear() = 0;

      /// Append a term to this passage
      virtual void addTerm(termCount term) = 0;

      /// Take a vector of term/scores and make it a passage
      virtual void addTerms(const passageVec pv) = 0;

      /// Return the passage terms in vector form for easy access
      virtual const passageVec* getAsVector(void) const = 0;

      /// For comparison with other passages for sorting by score
      virtual int operator<(const Passage &b) const = 0;

    };
  }
}

#endif
