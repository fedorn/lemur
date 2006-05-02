/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _SINGLEREGRMERGEMETHOD_HPP
#define _SINGLEREGRMERGEMETHOD_HPP

#include "Param.hpp"
#include "DistMergeMethod.hpp"
#include "InvFPTypes.hpp"


/// The minimum number of overlap documents (training data points) for a single databases
#define MINOVERLAPDOCNUM 3 
/// The threshold of backing off to CORI results merging algorithm.
#define MINLESSDBPERCEN 0.4
/// Only use the top n documents retrieved from a database to build the regression model.
#define GETTOPDOCSNUM 20

/// Single regression merge method parameters
namespace SingleRegrMergeMethodParameter {

  /// central database index
  static lemur::utility::String csDbDataBaseIndex;

  /// To get the filename of centralized sampling database
  static void get() {
    csDbDataBaseIndex = lemur::api::ParamGetString("csDbDataBaseIndex","");
  }
}

namespace lemur 
{
  namespace distrib
  {

    /// \brief Implementation of regression merging method for single database type case    
    /*! 

      (All the individual databases are using Inquery(CORI) retrieval algorithm.
      Build a single regression model for all the databases.
      When there is not enough training data, the algorithm backs off to CORI results merging algorithm.
      Assume all the documents scores have been normalized 
      See RetrievalMethod for database ranking algorithms (CORIRetMethod).
      See DistSearchMethod for multiple individual database retrieval.
      See CORIMergeMethod for CORI results merging algorithm
    **/
    class SingleRegrMergeMethod : public DistMergeMethod {
    public:
      /// the default values of constA and constB are the values of CORI merging algorithm
      SingleRegrMergeMethod(double constA=0.714, double constB=0.2857);
      ~SingleRegrMergeMethod();

      /// calculate the single regression model for all the databases
      /// indexset are the database scores for selected databases
      /// centralsocres are the central documents scores retrieved by centralized sampling database
      /// scoresset are the distributed documents scores retrieved by individual databases
      void calcRegrParams(const lemur::api::IndexedRealVector &indexset, 
                          const DocScoreVector* centralscores, 
                          const DocScoreVector* const* scoresset);

    protected:
      /// create a score for the merge based on the index ranking score and the document score 
      virtual double score(double dbscore, double docscore) const;

      /// The model parameters.
      double parama;
      double paramb;
    };
  }
}

#endif
