/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _RETRIEVALPARAMETER_HPP
#define _RETRIEVALPARAMETER_HPP

/// A package of retrieval parameter definitions and loading procedures
#include "Param.hpp"

#include "TFIDFRetMethod.hpp"
#include "OkapiRetMethod.hpp"
#include "SimpleKLRetMethod.hpp"
#include "CORIRetMethod.hpp"

/// General retrieval-related parameters
namespace RetrievalParameter {

  /// @name Common retrieval parameters
  //@{ 
  /// database index
  static String databaseIndex;
  /// text query set
  static String textQuerySet;
  /// result file name
  static String resultFile;
  /// number of docs for feedback (0=no feedback)
  static int fbDocCount;
  /// number of docs to return as results
  static int resultCount;
  //@}

  static void get() {
    databaseIndex = ParamGetString("index","");
    textQuerySet = ParamGetString("textQuery","");
    resultFile = ParamGetString("resultFile","");
    fbDocCount = ParamGetInt("feedbackDocCount",0); // default being no feedback
    resultCount = ParamGetInt("resultCount", 1000); 
  }
};


namespace TFIDFParameter {

  /// @name TFIDF model parameters
  //@{
  static WeightParam docTFPrm;
  static WeightParam qryTFPrm;
  static FeedbackParam fbPrm;
  //@}
  
  static void get()
  {
    
    docTFPrm.tf = (TFMethod)ParamGetInt("doc.tfMethod",BM25);
    docTFPrm.bm25K1 = ParamGetDouble("doc.bm25K1",defaultDocK1);
    docTFPrm.bm25B = ParamGetDouble("doc.bm25B",defaultDocB);
  
    qryTFPrm.tf = (TFMethod)ParamGetInt("query.tfMethod",BM25);
    qryTFPrm.bm25K1 = ParamGetDouble("query.bm25K1",defaultQryK1);
    qryTFPrm.bm25B = defaultQryB;
    
    fbPrm.howManyTerms = ParamGetInt("feedbackTermCount",defaultHowManyTerms); 
    fbPrm.posCoeff = ParamGetDouble("feedbackPosCoeff", defaultPosCoeff); 
    
  }

};

namespace OkapiParameter {

  /// @name Okapi model parameters
  //@{
  static TFParam tfPrm;
  static FeedbackParam fbPrm;
  //@}


  static void get()
  {
    tfPrm.k1 = ParamGetDouble("BM25K1",defaultK1);
    tfPrm.b =  ParamGetDouble("BM25B",defaultB);
    tfPrm.k3 = ParamGetDouble("BM25K3", defaultK3);
    fbPrm.expQTF = ParamGetDouble("BM25QTF", defaultExpQTF);
    fbPrm.howManyTerms = ParamGetInt("feedbackTermCount",defaultHowManyTerms);
    
  }
};

namespace SimpleKLParameter {
  /// @name Simple KL model parameters
  //@{
  static SimpleKLParameter::DocSmoothParam docPrm;
  static SimpleKLParameter::QueryModelParam qryPrm;
  static String smoothSupportFile;
  //@}


  static void get()
  {
    
    smoothSupportFile = ParamGetString("smoothSupportFile");
    
    docPrm.smthMethod = (SmoothMethod) ParamGetInt("smoothMethod", DIRICHLETPRIOR);
    docPrm.smthStrategy= (SmoothStrategy) ParamGetInt("smoothStrategy", INTERPOLATE);
    docPrm.ADDelta = ParamGetDouble("discountDelta",defaultADDelta);
    docPrm.JMLambda = ParamGetDouble("JelinekMercerLambda",defaultJMLambda);
    docPrm.DirPrior = ParamGetDouble("DirichletPrior",defaultDirPrior);
    
    qryPrm.fbMethod = (QueryUpdateMethod)ParamGetInt("queryUpdateMethod",MIXTURE);
    qryPrm.fbCoeff = ParamGetDouble("feedbackCoefficient",defaultFBCoeff);
    qryPrm.fbPrTh = ParamGetDouble("feedbackProbThresh",defaultFBPrTh);
    qryPrm.fbPrSumTh = ParamGetDouble("feedbackProbSumThresh",defaultFBPrSumTh);
    qryPrm.fbTermCount = ParamGetInt("feedbackTermCount",defaultFBTermCount);
    qryPrm.fbMixtureNoise = ParamGetDouble("feedbackMixtureNoise",defaultFBMixNoise);
    qryPrm.emIterations = ParamGetInt("emIterations",defaultEMIterations);
					      
  }
};

namespace CORIParameter {
  static String collectionCounts;
  static double tffactor;
  static double tfbaseline;
  static void get() {
    collectionCounts = ParamGetString("collCounts");
    tffactor = ParamGetDouble("TF_factor", 150);
    tfbaseline = ParamGetDouble("TF_baseline", 50);
  }
};

#endif
