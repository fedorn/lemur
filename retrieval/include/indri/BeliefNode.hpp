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


//
// BeliefNode
// 
// 26 January 2004 -- tds
//

#ifndef INDRI_BELIEFNODE_HPP
#define INDRI_BELIEFNODE_HPP

#include "indri/InferenceNetworkNode.hpp"
#include "indri/greedy_vector"
#include "indri/ScoredExtentResult.hpp"
#include <float.h>

#define INDRI_HUGE_SCORE  ( DBL_MAX )
#define INDRI_TINY_SCORE  ( -DBL_MAX )
namespace indri
{
  namespace infnet
  {
    
    class BeliefNode : public InferenceNetworkNode {
    public:
      virtual double maximumBackgroundScore() = 0;
      virtual double maximumScore() = 0;
      virtual const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& score( int documentID, int start, int end, int documentLength ) = 0;
      virtual bool hasMatch( int documentID ) = 0;
      virtual const indri::utility::greedy_vector<bool>& hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) = 0;
      virtual void annotate( class Annotator& annotator, int documentID, int begin, int end ) = 0;
    };
  }
}

#endif // INDRI_BELIEFNODE_HPP

