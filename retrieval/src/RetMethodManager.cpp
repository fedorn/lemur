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

#include "RetMethodManager.hpp"
#include "BasicCollectionProps.hpp"

lemur::api::RetrievalMethod* lemur::api::RetMethodManager::createModel (const Index* ind, 
                                                                        ScoreAccumulator* accum, 
                                                                        RetModel type) {
  // type set externally as string.
  RetrievalMethod *model = NULL;
  switch (type) {
  case TFIDF:
    model = new lemur::retrieval::TFIDFRetMethod(*ind, *accum);
    TFIDFParameter::get();
    ((lemur::retrieval::TFIDFRetMethod *)model)->setDocTFParam(TFIDFParameter::docTFPrm);
    ((lemur::retrieval::TFIDFRetMethod *)model)->setQueryTFParam(TFIDFParameter::qryTFPrm);
    ((lemur::retrieval::TFIDFRetMethod *)model)->setFeedbackParam(TFIDFParameter::fbPrm);
    break;
  case OKAPI:
    model = new lemur::retrieval::OkapiRetMethod(*ind, *accum);
    OkapiParameter::get();
    ((lemur::retrieval::OkapiRetMethod *)model)->setTFParam(OkapiParameter::tfPrm);
    ((lemur::retrieval::OkapiRetMethod *)model)->setFeedbackParam(OkapiParameter::fbPrm);
    break;
  case KL:
    SimpleKLParameter::get();
    model = new lemur::retrieval::SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, 
                                                    *accum);
    ((lemur::retrieval::SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    ((lemur::retrieval::SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);
    break;
  case CORI_CS:
    CORIParameter::get();
    model = new lemur::retrieval::CORIRetMethod(*ind, *accum, CORIParameter::collectionCounts,
                                                1);
    ((lemur::retrieval::CORIRetMethod*)model)->setTFFactor(CORIParameter::cstffactor);
    ((lemur::retrieval::CORIRetMethod*)model)->setTFBaseline(CORIParameter::cstfbaseline);
    break;
  case INQUERY:
    CORIParameter::get();
    model = new lemur::retrieval::CORIRetMethod(*ind, *accum, CORIParameter::collectionCounts,
                                                0);
    ((lemur::retrieval::CORIRetMethod*)model)->setTFFactor(CORIParameter::doctffactor);
    ((lemur::retrieval::CORIRetMethod*)model)->setTFBaseline(CORIParameter::doctfbaseline);
    break;
  case COS: 
    CosSimParameter::get();
    model = new lemur::retrieval::CosSimRetMethod(*ind, CosSimParameter::L2NormFile, *accum);
    ((lemur::retrieval::CosSimRetMethod *)model)->setFeedbackParam(CosSimParameter::fbPrm);
    break;
  case INQ_STRUCT:
    InQueryParameter::get();
    model = new lemur::retrieval::InQueryRetMethod(*ind, InQueryParameter::defaultBelief,
                                                   InQueryParameter::fbTermCount,
                                                   InQueryParameter::fbCoeff,
                                                   InQueryParameter::cacheIDF);
    break;
  case INDRI:
    IndriParameter::get();
    model = new lemur::retrieval::IndriRetMethod(*ind);
    lemur::retrieval::IndriRetMethod * indri = dynamic_cast<lemur::retrieval::IndriRetMethod *>(model);
    // set up the parameters
    indri->setParams(&IndriParameter::params);
    // then add the stopwords.
    indri->setStopwords(IndriParameter::stopwords);
  }  
  return model;
}

lemur::api::RetrievalMethod* lemur::api::RetMethodManager::createModel (const Index* ind, 
                                                                        ScoreAccumulator* accum, 
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
  if (type == "indri")
    mod = createModel(ind, accum, INDRI);

  return mod;
}

lemur::api::IndexedRealVector* lemur::api::RetMethodManager::runQuery(const string& query, const Index* index, const string& modeltype, const string& stopfile, const string& stemtype, const string& datadir, const string& func) {
  if (!index)
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "cannot run query without an index");

  lemur::retrieval::ArrayAccumulator accum(index->docCount());
  IndexedRealVector* results = NULL;
  RetrievalMethod* model = createModel(index, &accum, modeltype);
  if (!model)
    LEMUR_THROW(LEMUR_MISSING_PARAMETER_ERROR, "could not create retrieval model from given type");
  
  if (stopfile.empty() || stemtype.empty()) {
    const lemur::parse::BasicCollectionProps* props = dynamic_cast<const lemur::parse::BasicCollectionProps*> (index->collectionProps());
    if (props) {
      const lemur::parse::Property* p = NULL;
      props->startIteration();
      while (props->hasMore()) {
        p = props->nextEntry();
        if (p->getType() == lemur::parse::Property::STRING) 
          ParamSet(p->getName(),(char*)p->getValue());
      }
    }
  }
  
  if (TextQueryRetMethod* textmethod = dynamic_cast<TextQueryRetMethod*>(model)) {    
    results = runTextQuery(query, textmethod, stopfile, stemtype, datadir, func);
  } else if (StructQueryRetMethod* structmethod = dynamic_cast<StructQueryRetMethod*>(model)) {
    results = runStructQuery(query, structmethod, stopfile, stemtype, datadir, func);
  } else if (lemur::retrieval::IndriRetMethod* indrimethod = dynamic_cast<lemur::retrieval::IndriRetMethod*>(model)) {
    results = new IndexedRealVector();
    // add the stopwords.
    indrimethod->setStopwords(stopfile);
    indrimethod->scoreCollection(query, *results);
    results->Sort();
  } else {
    delete(model);
    LEMUR_THROW(LEMUR_GENERIC_ERROR, "could not handle retrieval model");
  }
  delete(model);
  return results;
}


lemur::api::IndexedRealVector* lemur::api::RetMethodManager::runTextQuery(const string& query, 
                                                                          TextQueryRetMethod* model, 
                                                                          const string& stopfile, 
                                                                          const string& stemtype, 
                                                                          const string& datadir, 
                                                                          const string& func) {
  Stopper* stopper = TextHandlerManager::createStopper(stopfile);
  Stemmer* stemmer = TextHandlerManager::createStemmer(stemtype, datadir, func);
  lemur::parse::StringQuery* parser = NULL;
  lemur::parse::StringQuery* qryterms;
  TextHandler* chain;

  if (stopper || stemmer) {
    parser = new lemur::parse::StringQuery();
    chain = parser;
    qryterms = new lemur::parse::StringQuery();

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
    qryterms = new lemur::parse::StringQuery(query);
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

lemur::api::IndexedRealVector* lemur::api::RetMethodManager::runStructQuery(const string& query, 
                                                                            StructQueryRetMethod* model, 
                                                                            const string& stopfile, 
                                                                            const string& stemtype, 
                                                                            const string& datadir, 
                                                                            const string& func) {
  Stopper* stopper = TextHandlerManager::createStopper(stopfile);
  Stemmer* stemmer = TextHandlerManager::createStemmer(stemtype, datadir, func);

  // make bogus query number for parser
  string structquery = "#q1=" + query;
  lemur::parse::InQueryOpParser* parser = new lemur::parse::InQueryOpParser();
  lemur::parse::StringQuery* qryterms = new lemur::parse::StringQuery();
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

