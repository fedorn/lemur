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

#ifndef _SUMMARIZER_HPP
#define _SUMMARIZER_HPP

#include <stdlib.h> 
#include <vector>
#include <string>
#include "Passage.hpp"

using std::vector;
using std::string;

namespace lemur 
{
  namespace summarization 
  {
#define PSG_LEN  15    
    /*!
      A <code>Summarizer</code> is created in conjunction with a specific <code>Passage</code> implementation.

    */

    class Summarizer {

    public:
      virtual ~Summarizer();

      /// General purpose function that does all summary computation as well as passage marking
      virtual void summDocument(const string &docID, const int optLen, const string &qInfo) = 0;

      /// Score ALL passages currently in this summarizer, optionally given query terms.
      virtual void scorePassages(const string &qInfo) = 0;

      /// Flag <code>optLen</code> passages for inclusion in summary
      virtual void markPassages(int optLen, const string &qInfo) = 0;

      /// Append a new passage to the list the summarizer will use to generate summary
      virtual void addPassage(Passage &psg) = 0;

      /// Clear all scores and passages from the summarizer
      virtual void clear(void) = 0;

      /// Place marked passages into an array
      virtual int fetchPassages(Passage* psgs, int optLen) const = 0;

      /// Get the next passage in iterative manner
      virtual int nextPassage(Passage* psg) const = 0;

      /// Reset to the first summarized passage (in conjunction with <code>nextPassage</code>)
      virtual void iterClear(void) const = 0;

      /// General purpose "send summary to stdout" function
      virtual void outputSumm(void) const = 0;

      /// End of sentence marker.
      static const string EOS;
      
    }; // Summarizer
  }
}

#endif
