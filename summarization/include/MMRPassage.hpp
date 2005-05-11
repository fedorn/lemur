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
#ifndef _MMRPASSAGE_HPP
#define _MMRPASSAGE_HPP

#include <iomanip>
#include "Passage.hpp"
#include <vector>

using std::vector;

namespace lemur 
{
  namespace summarization 
  {
    
    /*!
      An implementation of the abstract class <code>Passage</code> meant to work in conjunction with the MMR summarizer <code>MMRSumm</code>

    */

    class MMRPassage : public Passage {

    public:
      double wt;
      double wtSim; 
      double qSim;
      double maxSim;

      MMRPassage(const string &id) {
        docID = id;
        score = -1;
        marked = 0;
        wt = 1.0;
        qSim = 0.0;
        wtSim = 0.0;
        maxSim = -1.0;
      }
      // have to clean up
      ~MMRPassage() {
        //        delete psg;
      }
      
      virtual void clear();

      virtual void addTerm(termCount term);

      virtual void addTerms(const passageVec pv);

      virtual const passageVec* getAsVector(void) const;

      virtual int operator<(const Passage &b) const;

      virtual double computeMMR(double lambda) const;

      virtual double dotProduct(MMRPassage b) const;

      // This is NOT const, it modifies psg.
      //      virtual void scale(double val) const;
      virtual void scale(double val) ;

    };
  }
}

#endif
