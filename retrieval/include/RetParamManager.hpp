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

#ifndef _RETRIEVALPARAMETER_HPP
#define _RETRIEVALPARAMETER_HPP

/// A package of retrieval parameter definitions and loading procedures
#include "Param.hpp"

#include "TFIDFRetMethod.hpp"
#include "OkapiRetMethod.hpp"
#include "SimpleKLRetMethod.hpp"
#include "CORIRetMethod.hpp"
#include "CosSimRetMethod.hpp"
#include "InQueryRetMethod.hpp"
#include "IndriRetMethod.hpp"

/// General retrieval-related parameters
namespace RetrievalParameter {

  /// @name Common retrieval parameters
  //@{ 
  /// database index
  static lemur::utility::String databaseIndex;
  /// Retrieval method
  static lemur::utility::String retModel;
  /// text query set
  static lemur::utility::String textQuerySet;
  /// result file name
  static lemur::utility::String resultFile;
  /// result file format, one of true for trec, false for 3col.
  static bool TRECresultFileFormat;
  /// number of docs for feedback (0=no feedback)
  static int fbDocCount;
  /// number of docs to return as results
  static int resultCount;
  /// should DocReps be cached across calls to score
  static bool cacheDocReps;
  /// Use a working set? Default is to score the whole collection;
  static bool useWorkingSet;
  /// working set file name
  static lemur::utility::String workSetFile;

  //@}

  static string getLower(char *parm, char *def) {
    std::string tmpString = lemur::api::ParamGetString(parm, def);
    // make it all lowercase
    for (int i = 0; i < tmpString.length(); i++)
      tmpString[i] = tolower(tmpString[i]);
    return tmpString;
  }

  static void get() {
    databaseIndex = lemur::api::ParamGetString("index","");
    retModel = getLower("retModel","kl");
    // backwards compatibility.
    if (retModel == "0") retModel = "tfidf";
    if (retModel == "1") retModel = "okapi";
    if (retModel == "2") retModel = "kl";
    if (retModel == "3") retModel = "inquery";
    if (retModel == "4") retModel = "cori_cs";
    if (retModel == "5") retModel = "cos";
    if (retModel == "6") retModel = "inq_struct";
    if (retModel == "7") retModel = "indri";

    string tmp = getLower("cacheDocReps", "true");
    cacheDocReps = (tmp == "true" || tmp == "1");

    tmp = getLower("useWorkingSet", "false"); 
    useWorkingSet = (tmp == "true" || tmp == "1");
    // working set file name
    workSetFile = lemur::api::ParamGetString("workingSetFile",""); 
    
    textQuerySet = lemur::api::ParamGetString("textQuery","");
    resultFile = lemur::api::ParamGetString("resultFile","");

    tmp = getLower("resultFormat","trec");
    TRECresultFileFormat = (tmp == "trec" || tmp == "1");

    // default being no feedback
    fbDocCount = lemur::api::ParamGetInt("feedbackDocCount",0); 
    resultCount = lemur::api::ParamGetInt("resultCount", 1000); 
    
  }
}

/// TFIDF retrieval method parameters.
namespace TFIDFParameter {

  /// @name TFIDF model parameters
  //@{
  static WeightParam docTFPrm;
  static WeightParam qryTFPrm;
  static FeedbackParam fbPrm;
  //@}
  
  static void get()
  {
    string tfmethod = RetrievalParameter::getLower("doc.tfMethod", "bm25");
    if ((tfmethod == "rawtf") || (tfmethod == "0")) docTFPrm.tf = RAWTF;
    else if ((tfmethod == "logf") || (tfmethod == "1")) docTFPrm.tf = LOGTF;    
    else if ((tfmethod == "bm25") || (tfmethod == "2")) docTFPrm.tf = BM25;

    docTFPrm.bm25K1 = lemur::api::ParamGetDouble("doc.bm25K1",defaultDocK1);
    docTFPrm.bm25B = lemur::api::ParamGetDouble("doc.bm25B",defaultDocB);

    tfmethod = RetrievalParameter::getLower("query.tfMethod", "bm25");
    if ((tfmethod == "rawtf") || (tfmethod == "0")) qryTFPrm.tf = RAWTF;
    else if ((tfmethod == "logf") || (tfmethod == "1")) qryTFPrm.tf = LOGTF;    
    else if ((tfmethod == "bm25") || (tfmethod == "2")) qryTFPrm.tf = BM25;

    qryTFPrm.bm25K1 = lemur::api::ParamGetDouble("query.bm25K1",defaultQryK1);
    qryTFPrm.bm25B = defaultQryB;
    
    fbPrm.howManyTerms = lemur::api::ParamGetInt("feedbackTermCount",defaultHowManyTerms);
    fbPrm.posCoeff = lemur::api::ParamGetDouble("feedbackPosCoeff", defaultPosCoeff); 
  }
}

/// Okapi retrieval method parameters.
namespace OkapiParameter {

  /// @name Okapi model parameters
  //@{
  static TFParam tfPrm;
  static FeedbackParam fbPrm;
  //@}


  static void get()
  {
    tfPrm.k1 = lemur::api::ParamGetDouble("BM25K1",defaultK1);
    tfPrm.b =  lemur::api::ParamGetDouble("BM25B",defaultB);
    tfPrm.k3 = lemur::api::ParamGetDouble("BM25K3", defaultK3);
    fbPrm.expQTF = lemur::api::ParamGetDouble("BM25QTF", defaultExpQTF);
    fbPrm.howManyTerms = lemur::api::ParamGetInt("feedbackTermCount",defaultHowManyTerms);
    
  }
}
/// Simple KL retrieval method parameters.
namespace SimpleKLParameter {
  /// @name Simple KL model parameters
  //@{
  static SimpleKLParameter::DocSmoothParam docPrm;
  static SimpleKLParameter::QueryModelParam qryPrm;
  static lemur::utility::String smoothSupportFile;
  //@}
    
  static void get()
  {
    smoothSupportFile = lemur::api::ParamGetString("smoothSupportFile", "");

    string tmpString = RetrievalParameter::getLower("adjustedScoreMethod", 
                                                    "negativekld");
    if (tmpString == "querylikelihood" || tmpString == "ql") {
      qryPrm.adjScoreMethod = SimpleKLParameter::QUERYLIKELIHOOD;
    } else if (tmpString == "crossentropy" ||tmpString == "ce") {
      qryPrm.adjScoreMethod = SimpleKLParameter::CROSSENTROPY;
    } else if (tmpString == "negativekld" || tmpString == "-d") {
      qryPrm.adjScoreMethod = SimpleKLParameter::NEGATIVEKLD;
    } else {
      cerr << "Unknown scoreMethod " << tmpString << ". Using NEGATIVEKLD" 
           << endl;
      qryPrm.adjScoreMethod = SimpleKLParameter::NEGATIVEKLD;
    }

    tmpString = RetrievalParameter::getLower("smoothMethod", 
                                             "dirichletprior");
    if (tmpString == "jelinikmercer" || tmpString == "jm" || tmpString == "0")
      docPrm.smthMethod = SimpleKLParameter::JELINEKMERCER;
    else if (tmpString == "dirichletprior" || tmpString == "dir" || 
             tmpString == "1")
      docPrm.smthMethod = SimpleKLParameter::DIRICHLETPRIOR;
    else if (tmpString == "absolutediscount" || tmpString == "ad" || 
             tmpString == "2")
      docPrm.smthMethod = SimpleKLParameter::ABSOLUTEDISCOUNT;
    else if (tmpString == "twostage" || tmpString == "2s" || tmpString == "3")
      docPrm.smthMethod = SimpleKLParameter::TWOSTAGE;
    else {
      cerr << "Unknown smoothMethod " << tmpString << ". Using DIRICHLET" 
           << endl;
      docPrm.smthMethod = SimpleKLParameter::defaultSmoothMethod;
    }
    

    tmpString = RetrievalParameter::getLower("smoothStrategy", "interpolate");
    if (tmpString == "interpolate" || tmpString == "int" || tmpString == "0")
      docPrm.smthStrategy= SimpleKLParameter::INTERPOLATE;
    else if (tmpString == "backoff" || tmpString == "bo" || tmpString == "1")
      docPrm.smthStrategy= SimpleKLParameter::BACKOFF;
    else {
      cerr << "Unknown smoothStrategy " << tmpString << ". Using INTERPOLATE" 
           << endl;
      docPrm.smthStrategy= SimpleKLParameter::defaultSmoothStrategy;
    }
    

    docPrm.ADDelta = lemur::api::ParamGetDouble("discountDelta",defaultADDelta);
    docPrm.JMLambda = lemur::api::ParamGetDouble("JelinekMercerLambda",defaultJMLambda);
    docPrm.DirPrior = lemur::api::ParamGetDouble("DirichletPrior",defaultDirPrior);
    
    tmpString = RetrievalParameter::getLower("queryUpdateMethod", "mixture");

    if (tmpString == "mixture" || tmpString == "mix" || tmpString == "0")
      qryPrm.fbMethod = SimpleKLParameter::MIXTURE;
    else if (tmpString == "divmin" || tmpString == "div" || tmpString == "1")
      qryPrm.fbMethod = SimpleKLParameter::DIVMIN;
    else if (tmpString == "markovchain" || tmpString == "mc" || 
             tmpString == "2")
      qryPrm.fbMethod = SimpleKLParameter::MARKOVCHAIN;
    else if (tmpString == "relevancemodel1" || tmpString == "rm1" || 
             tmpString == "3")
      qryPrm.fbMethod = SimpleKLParameter::RM1;
    else if (tmpString == "relevancemodel2" || tmpString == "rm2" || 
             tmpString == "4")
      qryPrm.fbMethod = SimpleKLParameter::RM2;
    else {
      cerr << "Unknown queryUpdateMethod " << tmpString 
           << ". Using MIXTURE" 
           << endl;
      qryPrm.fbMethod = SimpleKLParameter::MIXTURE;
    }
    

    qryPrm.fbCoeff = lemur::api::ParamGetDouble("feedbackCoefficient", defaultFBCoeff);
    qryPrm.fbPrTh = lemur::api::ParamGetDouble("feedbackProbThresh", defaultFBPrTh);
    qryPrm.fbPrSumTh = lemur::api::ParamGetDouble("feedbackProbSumThresh",
                                                  defaultFBPrSumTh);
    qryPrm.fbTermCount = lemur::api::ParamGetInt("feedbackTermCount", defaultFBTermCount);
    qryPrm.fbMixtureNoise = lemur::api::ParamGetDouble("feedbackMixtureNoise",
                                                       defaultFBMixNoise);
    qryPrm.emIterations = lemur::api::ParamGetInt("emIterations", defaultEMIterations);
                                              
  }
}
/// CORI retrieval method parameters.
namespace CORIParameter {
  static lemur::utility::String collectionCounts;
  static double cstffactor;
  static double cstfbaseline;
  static double doctffactor;
  static double doctfbaseline;
  static void get() {
    collectionCounts = lemur::api::ParamGetString("collCounts", "USE_INDEX_COUNTS");
    cstffactor = lemur::api::ParamGetDouble("CSCTF_factor", 150);
    cstfbaseline = lemur::api::ParamGetDouble("CSCTF_baseline", 50);
    doctffactor = lemur::api::ParamGetDouble("DOCCTF_factor", 1.5);
    doctfbaseline = lemur::api::ParamGetDouble("DOCCTF_baseline", 0.5);
  }
}
/// Cosine similarity retrieval method parameters.
namespace CosSimParameter {

  /// @name CosSim model parameters
  //@{
  static FeedbackParam fbPrm;
  static lemur::utility::String L2NormFile;
  //@}
  
  static void get()
  {
    fbPrm.howManyTerms = lemur::api::ParamGetInt("feedbackTermCount",defaultHowManyTerms);
    fbPrm.posCoeff = lemur::api::ParamGetDouble("feedbackPosCoeff", defaultPosCoeff); 
    L2NormFile = lemur::api::ParamGetString("L2File", defaultL2File);
  }
}
/// Parameters used in the InQuery structured query retrieval method
namespace InQueryParameter {
  /// @name InQuery structured query parameters
  /// default feedback coefficient (0.5).
  static double fbCoeff = 0.5;
  /// default number of feedback terms (50).
  static int fbTermCount = 50;
  /// default belief value (score) for a node (0.4).
  static double defaultBelief = 0.4;
  /// cache term idf values
  static bool cacheIDF = false;
  static void get()
  {
    defaultBelief = lemur::api::ParamGetDouble("defaultBelief", defaultBelief);
    fbCoeff = lemur::api::ParamGetDouble("feedbackPosCoeff", fbCoeff);
    fbTermCount = lemur::api::ParamGetInt("feedbackTermCount", fbTermCount);
    string tmpString = RetrievalParameter::getLower("cacheIDF", "true");
    cacheIDF = (tmpString == "true" || tmpString == "1");
  }
}

/// Parameters used in the Indri query language
namespace IndriParameter {
  /// @name Indri query language parameters
  /// Parameters object passed into the IndriRetMethod
  static indri::api::Parameters params;
  /// For query time stopping.
  static string stopwords;
  /// get parameters.
  static void get()
  {
    RetrievalParameter::get();
    // stopwords
    stopwords = lemur::api::ParamGetString("stopwords", "");

    // RM expansion parameters
    int fbTerms = lemur::api::ParamGetInt("feedbackTermCount", 10);
    double fbOrigWt = lemur::api::ParamGetDouble("fbOrigWt", 0.5);
    double fbMu = lemur::api::ParamGetDouble("fbMu", 0);
    params.set( "fbDocs" , RetrievalParameter::fbDocCount);
    params.set( "fbTerms" , fbTerms );
    params.set( "fbOrigWt", fbOrigWt);
    params.set( "fbMu", fbMu );
    // set retrieval rules -- NB limit to one.
    string rule = lemur::api::ParamGetString("rule", "method:dirichlet,mu:2500");
    params.set("rule", rule);
    // results count
    params.set( "count", RetrievalParameter::resultCount );
  }
}

#endif
