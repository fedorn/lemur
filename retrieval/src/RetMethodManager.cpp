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

RetrievalMethod* RetMethodManager::createModel (const Index* ind, 
						ArrayAccumulator* accum, 
						RetModel type) {
  // type set externally as string.
  RetrievalMethod *model = NULL;
  switch (type) {
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
    model = new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, 
				  *accum);
    ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    ((SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);
    break;
  case CORI_CS:
    CORIParameter::get();
    model = new CORIRetMethod(*ind, *accum, CORIParameter::collectionCounts,
			      1);
    ((CORIRetMethod*)model)->setTFFactor(CORIParameter::cstffactor);
    ((CORIRetMethod*)model)->setTFBaseline(CORIParameter::cstfbaseline);
    break;
  case INQUERY:
    CORIParameter::get();
    model = new CORIRetMethod(*ind, *accum, CORIParameter::collectionCounts,
			      0);
    ((CORIRetMethod*)model)->setTFFactor(CORIParameter::doctffactor);
    ((CORIRetMethod*)model)->setTFBaseline(CORIParameter::doctfbaseline);
    break;
  case COS: 
    CosSimParameter::get();
    model = new CosSimRetMethod(*ind, CosSimParameter::L2NormFile, *accum);
    ((CosSimRetMethod *)model)->setFeedbackParam(CosSimParameter::fbPrm);
    break;
  case INQ_STRUCT:
    InQueryParameter::get();
    model = new InQueryRetMethod(*ind, InQueryParameter::defaultBelief,
				 InQueryParameter::fbTermCount,
				 InQueryParameter::fbCoeff,
				 InQueryParameter::cacheIDF);
  }  
  return model;
}

RetrievalMethod* RetMethodManager::createModel (const Index* ind, 
						ArrayAccumulator* accum, 
						string type) {
  RetrievalMethod *mod = NULL;
  if (type.empty()) {
    return mod; // nothing to make, give back NULL.
  }

  // make it all lowercase
  for (int i=0;i<type.length();i++)
    type[i] = tolower(type[i]);

  if (type == "tfidf")
    mod = createModel(ind, accum, TFIDF);
  if (type == "okapi")
    mod = createModel(ind, accum, OKAPI);
  if (type == "inquery")
    mod = createModel(ind, accum, INQUERY);
  if (type == "kl")
    mod = createModel(ind, accum, KL);
  if (type == "cori_cs")
    mod = createModel(ind, accum, CORI_CS);
  if (type == "cos")
    mod = createModel(ind, accum, COS);
  if (type == "inq_struct")
    mod = createModel(ind, accum, INQ_STRUCT);

  return mod;
}

IndexedRealVector* RetMethodManager::runQuery(const string& query, const Index* index, const string& modeltype, const string& stopfile, const string& stemtype, const string& datadir, const string& func) {
  if (!index)
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "cannot run query without an index");

  ArrayAccumulator accum(index->docCount());
  IndexedRealVector* results = NULL;
  RetrievalMethod* model = createModel(index, &accum, modeltype);
  if (!model)
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "could not create retrieval model from given type");
  
  if (TextQueryRetMethod* textmethod = dynamic_cast<TextQueryRetMethod*>(model)) {    
    results = runTextQuery(query, textmethod, stopfile, stemtype, datadir, func);
  } else if (StructQueryRetMethod* structmethod = dynamic_cast<StructQueryRetMethod*>(model)) {
    results = runStructQuery(query, structmethod, stopfile, stemtype, datadir, func);
  } else {
    delete(model);
    LEMUR_THROW(LEMUR_GENERIC_ERROR, "could not handle retrieval model");
  }
  delete(model);
  return results;
}


IndexedRealVector* RetMethodManager::runTextQuery(const string& query, 
						  TextQueryRetMethod* model, 
						  const string& stopfile, 
						  const string& stemtype, 
						  const string& datadir, 
						  const string& func) {
  Stopper* stopper = TextHandlerManager::createStopper(stopfile);
  Stemmer* stemmer = TextHandlerManager::createStemmer(stemtype, datadir, func);
  StringQuery* parser = NULL;
  StringQuery* qryterms;
  TextHandler* chain;

  if (stopper || stemmer) {
    parser = new StringQuery();
    chain = parser;
    qryterms = new StringQuery();

    if (stopper) {
      chain->setTextHandler(stopper);
      chain=stopper;
    }
    
    if (stemmer) {
      chain->setTextHandler(stemmer);    
      chain=stemmer;
    }
    
    chain->setTextHandler(qryterms);
    parser->parse(query);

    delete(parser);
    
  } else {
    qryterms = new StringQuery(query);
  }
  IndexedRealVector* results = new IndexedRealVector();
  QueryRep* qr = model->computeQueryRep(*qryterms);
  model->scoreCollection(*qr, *results);
  results->Sort();

  if (stopper)
    delete(stopper);
  if (stemmer)
    delete(stemmer);
  delete(qryterms);
  delete(qr);

  return results;
}

IndexedRealVector* RetMethodManager::runStructQuery(const string& query, 
						    StructQueryRetMethod* model, 
						    const string& stopfile, 
						    const string& stemtype, 
						    const string& datadir, 
						    const string& func) {
  Stopper* stopper = TextHandlerManager::createStopper(stopfile);
  Stemmer* stemmer = TextHandlerManager::createStemmer(stemtype, datadir, func);
  // make bogus query number for parser
  string structquery = "#q1=" + query;
  InQueryOpParser* parser = new InQueryOpParser();
  StringQuery* qryterms = new StringQuery();
  TextHandler* chain = parser;

  if (stopper) {
    chain->setTextHandler(stopper);
    chain=stopper;
  }
    
  if (stemmer) {
    chain->setTextHandler(stemmer);    
    chain=stemmer;
  }
    
  chain->setTextHandler(qryterms);
  parser->parseBuffer((char*)structquery.c_str(), structquery.length());

  IndexedRealVector* results = new IndexedRealVector();
  QueryRep* qr = model->computeQueryRep(*qryterms);
  model->scoreCollection(*qr, *results);
  results->Sort();

  if (stopper)
    delete(stopper);
  if (stemmer)
    delete(stemmer);
  delete(parser);
  delete(qryterms);
  delete(qr);

  return results;
}
