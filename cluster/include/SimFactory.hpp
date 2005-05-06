/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

// David Fisher
// init: 02/05/2003
#ifndef _SIMFACTORY_HPP
#define _SIMFACTORY_HPP
// Add new similarity methods here.
#include "CosSim.hpp"
#include "ClusterParam.hpp"
namespace lemur 
{
  namespace cluster
  {
    /// \brief Factory for creating SimilarityMethod objects.
    class SimFactory {
    public:
      static SimilarityMethod * makeSim(const lemur::api::Index &index,
                                        enum ClusterParam::simTypes simType = ClusterParam::COS) {
        switch (simType) {
        case ClusterParam::COS:
          return new CosSim(index);
        default:
          cerr << "Unknown similarity method " << simType << endl;
          return NULL;
        }
      }
    };
  }
}

#endif
