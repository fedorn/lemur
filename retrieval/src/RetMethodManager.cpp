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

#include "RetMethodManager.hpp"

RetrievalMethod* RetMethodManager::createModel (Index* ind, ArrayAccumulator* accum, RetModel def) 
{
  static enum RetModel mod = (RetModel) ParamGetInt("retModel", def);
  RetrievalMethod* model;
  
  switch (mod) {
  case TFIDF:
    model = new TFIDFRetMethod(*ind, *accum);
    TFIDFParameter::get();
    ((TFIDFRetMethod *)model)->setDocTFParam(TFIDFParameter::docTFPrm);
    ((TFIDFRetMethod *)model)->setQueryTFParam(TFIDFParameter::qryTFPrm);
    ((TFIDFRetMethod *)model)->setFeedbackParam(TFIDFParameter::fbPrm);
    break;
  case OKAPI:
    model = new OkapiRetMethod(*ind, *accum);
    OkapiParameter::get();
    ((OkapiRetMethod *)model)->setTFParam(OkapiParameter::tfPrm);
    ((OkapiRetMethod *)model)->setFeedbackParam(OkapiParameter::fbPrm);
    break;
  case KL:
    SimpleKLParameter::get();
    model = new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, *accum);
    ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    ((SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);
    break;
  case CORI_CS:
    CORIParameter::get();
    model = new CORIRetMethod(*ind, *accum, CORIParameter::collectionCounts,1);
    ((CORIRetMethod*)model)->setTFFactor(CORIParameter::cstffactor);
    ((CORIRetMethod*)model)->setTFBaseline(CORIParameter::cstfbaseline);
    break;
  case CORI_DOC:
    CORIParameter::get();
    model = new CORIRetMethod(*ind, *accum, CORIParameter::collectionCounts,0);
    ((CORIRetMethod*)model)->setTFFactor(CORIParameter::doctffactor);
    ((CORIRetMethod*)model)->setTFBaseline(CORIParameter::doctfbaseline);
    break;
  case COS: 
    CosSimParameter::get();
    model = new CosSimRetMethod(*ind, CosSimParameter::L2NormFile, *accum);
    ((CosSimRetMethod *)model)->setFeedbackParam(CosSimParameter::fbPrm);
    break;
  default:
    throw Exception("RetrievalExp", "unknown retModel parameter");
    break;
    
    }  
  return model;
}
