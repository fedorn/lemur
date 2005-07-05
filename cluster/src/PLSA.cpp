/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  Author: dmf 2/2003
*/

#include "PLSA.hpp"
#include <float.h> // for -DBL_MAX
#ifdef WIN32
#define drand48() ((rand() % 1000000) / 1000000.0)
#endif

// Probabilistic Latent Semantic Analysis
lemur::cluster::PLSA::PLSA(const lemur::api::Index &dbIndex, int numCats, lemur::utility::HashFreqVector **train, 
                           lemur::utility::HashFreqVector **validate, int numIter, 
                           int numRestarts, double betastart, 
                           double betastop, double anneal, double betaMod) :
  ind(dbIndex), data(train), testData(validate), numberOfIterations(numIter),
  numberOfRestarts(numRestarts), startBeta(betastart), beta(betastart),
  betaMin(betastop), annealcue(anneal), betaModifier(betaMod), 
  sizeZ(numCats) {
  init();
  initR();  
}

void lemur::cluster::PLSA::initR() {
  // initialize R - total count
  // and w->d index
  // 1-based indexing
  invIndex = new set<int, less<int> > [sizeW];
  R = 0;  
  for(int i = 1; i < sizeD; i++) { // DOC
    lemur::utility::HashFreqVector *freqs = data[i];
    freqs->startIteration();
    while (freqs->hasMore()) { // TERM
      lemur::api::TERMID_T termId;
      int freq;
      freqs->nextFreq(termId, freq);
      R += freq;
      invIndex[termId].insert(i);
    }
  }
}

void lemur::cluster::PLSA::init() {
  bestOnly = false;
  ownMem = false;

  sizeD = ind.docCount()  + 1;
  sizeW = ind.termCountUnique() + 1;
  bestA = 0;
  bestTestLL = -DBL_MAX;

  p_z_prev = new double[sizeZ];
  p_z_current = new double[sizeZ];
  p_z_best = new double[sizeZ];

  p_w_z_prev = new double *[sizeW];
  p_w_z_current = new double *[sizeW];
  p_w_z_best = new double *[sizeW];

  p_d_z_prev = new double *[sizeD];
  p_d_z_current = new double *[sizeD];
  p_d_z_best = new double *[sizeD];

  p_w_z_prev[0] = NULL;
  p_w_z_current[0] = NULL;
  p_w_z_best[0] = NULL;

  p_d_z_prev[0] = NULL;
  p_d_z_current[0] = NULL;
  p_d_z_best[0] = NULL;

  // 1-based indexing
  for (int w = 1; w < sizeW; w++) {
    p_w_z_prev[w] = new double [sizeZ];
    p_w_z_current[w] = new double [sizeZ];
    p_w_z_best[w] = new double [sizeZ];
  }
  // 1-based indexing
  for (int d = 1; d < sizeD; d++){
    p_d_z_prev[d] = new double [sizeZ];
    p_d_z_current[d] = new double [sizeZ];
    p_d_z_best[d] = new double [sizeZ];
  }
}

void lemur::cluster::PLSA::selectTestTrain(int testPercentage) {
  // set up data and testData.
  lemur::utility::HashFreqVector **train = new lemur::utility::HashFreqVector *[sizeD];
  lemur::utility::HashFreqVector **test = new lemur::utility::HashFreqVector *[sizeD];
  train[0] = NULL;
  test[0] = NULL;
  lemur::utility::FreqCount ct;  
  for(int i = 1; i < sizeD; i++) { // DOC
    train[i] = new lemur::utility::HashFreqVector;
    test[i] = new lemur::utility::HashFreqVector;
    lemur::api::TermInfoList *tList = ind.termInfoList(i);
    lemur::api::TermInfo *info;
    tList->startIteration();
    while (tList->hasMore()) { // TERM
      info = tList->nextEntry();
      ct.key = info->termID();
      // K % test
      if ((rand() % testPercentage) == 0)
        test[i]->add(ct, info->count());
      else
        train[i]->add(ct, info->count());
    }
    delete tList;
  }
  data = train;
  testData = test; 
  ownMem = true;
}

// Probabilistic Latent Semantic Analysis
lemur::cluster::PLSA::PLSA(const lemur::api::Index &dbIndex, int testPercentage, int numCats, 
                           int numIter, 
                           int numRestarts, double betastart, 
                           double betastop, double anneal, double betaMod) :
  ind(dbIndex), numberOfIterations(numIter),
  numberOfRestarts(numRestarts), startBeta(betastart), beta(betastart),
  betaMin(betastop), annealcue(anneal), betaModifier(betaMod), 
  sizeZ(numCats) {
  init();
  selectTestTrain(testPercentage);
  initR();  
}

/// Make an empty one ready to read in precomputed probability tables.
lemur::cluster::PLSA::PLSA(const lemur::api::Index &dbIndex) : 
  ind(dbIndex), data(NULL), testData(NULL), numberOfIterations(0),
  numberOfRestarts(0), startBeta(0), beta(0),
  betaMin(0), annealcue(0), betaModifier(0), sizeZ(0), sizeW(0),
  sizeD(0) {
  bestOnly = true;
  ownMem = false;
  p_z_prev = NULL;
  p_z_current = NULL;
  p_z_best = NULL;
  p_w_z_prev = NULL;
  p_w_z_current = NULL;
  p_w_z_best = NULL;
  p_d_z_prev = NULL;  
  p_d_z_current = NULL;  
  p_d_z_best = NULL;  
  invIndex = NULL;
  /// pass in a filestem.
  //  if (!readArrays()) cerr << "Failed to Read arrays." << endl;
}

lemur::cluster::PLSA::~PLSA() {
  if (! bestOnly) {
    // pass in a filestem
    writeArrays();
  }
  for (int w = 0; w < sizeW; w++) {
    if (! bestOnly) {
      delete[](p_w_z_prev[w]);
      delete[](p_w_z_current[w]);
    }
    delete[](p_w_z_best[w]);
  }
  for (int d = 0; d < sizeD; d++){
    if (! bestOnly) {
      delete[](p_d_z_prev[d]);
      delete[](p_d_z_current[d]);
    }
    delete[](p_d_z_best[d]);
  }
  if (! bestOnly) {
    delete[](p_z_prev);
    delete[](p_w_z_prev);
    delete[](p_d_z_prev);
    delete[](p_z_current);
    delete[](p_w_z_current);
    delete[](p_d_z_current);
    delete[](invIndex);
  }
  delete[](p_z_best);
  delete[](p_w_z_best);
  delete[](p_d_z_best);
  if (ownMem) {
    for(int i = 1; i < sizeD; i++) {
      delete(data[i]);
      delete(testData[i]);
    }
    delete[](data);
    delete[](testData);
  }
}

// performs one EM iteration, returns log likelihood of training data
double lemur::cluster::PLSA::interleavedIterationEM() {
  int d, z, i, j;
  lemur::api::TERMID_T w;
  
  double* denominatorMaxStep = new double[sizeZ];
  double sum = 0.0, sum_pz = 0;
  for(z = 0; z < sizeZ; z++) { // CAT
    for(d = 1; d < sizeD; d++) {// DOC
      lemur::utility::HashFreqVector *freqs = data[d];
      freqs->startIteration();
      while (freqs->hasMore()) { // TERM
        int freq;
        freqs->nextFreq(w, freq);
        // sum += tf(w) * p(z) * exp(p(d,z)*p(w,z), beta)/jointBeta(d,w)
        sum += freq * p_z_prev[z] * 
          pow(p_d_z_prev[d][z] * p_w_z_prev[w][z], beta) / 
          jointEstimateBeta(d, w);
      }
    }
    denominatorMaxStep[z] = sum;
    sum = 0.0;
  }
  // M step of P(w | z)
  sum = 0.0;
  for(w = 1; w < sizeW; w++) // TERM
    for(z = 0; z < sizeZ; z++) { // CAT
      set<int, less<int> >::iterator it;
      for (it = invIndex[w].begin(); it != invIndex[w].end(); it++) {
        d = *it;
        lemur::utility::HashFreqVector *freqs = data[d]; // DOC
        int freq;
        freqs->find(w, freq);
        // sum += tf(w) * p(z) * exp(p(d,z)*p(w,z), beta)/jointBeta(d,w)
        sum += freq * p_z_prev[z] *
          pow(p_d_z_prev[d][z] * p_w_z_prev[w][z], beta) /
          jointEstimateBeta(d, w);
      }
      p_w_z_current[w][z] = sum/denominatorMaxStep[z];
      sum = 0.0;
    }

  // M step of P(d | z)
  sum = 0.0;
  for(d = 1; d < sizeD; d++) // DOC
    for(z = 0; z < sizeZ; z++) { // CAT
      lemur::utility::HashFreqVector *freqs = data[d];
      freqs->startIteration();
      while (freqs->hasMore()) { // TERM
        int freq;
        freqs->nextFreq(w, freq);
        // sum += tf(w) * p(z) * exp(p(d,z)*p(w,z), beta)/jointBeta(d,w)
        sum += freq * p_z_prev[z] * 
          pow(p_d_z_prev[d][z] * p_w_z_prev[w][z], beta) / 
          jointEstimateBeta(d, w);
      }
      p_d_z_current[d][z] =  sum/denominatorMaxStep[z];
      sum = 0.0;
    }

  // M step of P(z)
  for(z = 0; z < sizeZ; z++) {// CAT
    double val = denominatorMaxStep[z]/R;
    p_z_current[z] = val;
    sum_pz += val; // normalizer
  }

  // normalize parameters
  for(i = 0; i < sizeZ; i++) // CAT
    p_z_current[i] /= sum_pz;
  // ------------------------------
  for(j = 0; j < sizeZ; j++) { // CAT
    sum = 0.0;
    for(i = 1; i < sizeW; i++) // TERM
      sum += p_w_z_current[i][j];
    for(i = 1; i < sizeW; i++) // TERM
      p_w_z_current[i][j] /= sum; 
  }
  // -----------------------------
  for(j = 0; j < sizeZ; j++) { // CAT
    sum = 0.0;
    for(i = 1; i < sizeD; i++) // DOC
      sum += p_d_z_current[i][j];
    for(i = 1; i < sizeD; i++) // DOC
      p_d_z_current[i][j] /= sum; 
  }
  // end normalizing
  delete[] denominatorMaxStep;
  return logLikelihood();
}
    
void lemur::cluster::PLSA::setPrevToCurrent() {
  int i, j;
  for(i = 0; i < sizeZ; i++)
    p_z_prev[i] = p_z_current[i];
  for(i = 1; i < sizeW; i++)
    for(j = 0; j < sizeZ; j++)
      p_w_z_prev[i][j] =  p_w_z_current[i][j];
  for(i = 1; i < sizeD; i++)
    for(j = 0; j < sizeZ; j++)
      p_d_z_prev[i][j] = p_d_z_current[i][j];
}

void lemur::cluster::PLSA::setCurrentToBest() {
  int i, j;
  for(i = 0; i < sizeZ; i++)
    p_z_current[i] = p_z_best[i];
  for(i = 1; i < sizeW; i++)
    for(j = 0; j < sizeZ; j++)
      p_w_z_current[i][j] =  p_w_z_best[i][j];
  for(i = 1; i < sizeD; i++)
    for(j = 0; j < sizeZ; j++)
      p_d_z_current[i][j] = p_d_z_best[i][j];
}

void lemur::cluster::PLSA::setBestToCurrent() {
  int i, j;
  for(i = 0; i < sizeZ; i++)
    p_z_best[i] = p_z_current[i];
  for(i = 1; i < sizeW; i++)
    for(j = 0; j < sizeZ; j++)
      p_w_z_best[i][j]  = p_w_z_current[i][j];
  for(i = 1; i < sizeD; i++)
    for(j = 0; j < sizeZ; j++)
      p_d_z_best[i][j] = p_d_z_current[i][j];
}
    
void lemur::cluster::PLSA::setBestToPrev() {
  int i, j;
  for(i = 0; i < sizeZ; i++)
    p_z_best[i]  =  p_z_prev[i];
  for(i = 1; i < sizeW; i++)
    for(j = 0; j < sizeZ; j++)
      p_w_z_best[i][j]  = p_w_z_prev[i][j];
  for(i = 1; i < sizeD; i++)
    for(j = 0; j < sizeZ; j++)
      p_d_z_best[i][j] = p_d_z_prev[i][j];
}

void lemur::cluster::PLSA::setPrevToBest() {
  int i, j;
  for(i = 0; i < sizeZ; i++)
    p_z_prev[i] = p_z_best[i];
  for(i = 1; i < sizeW; i++)
    for(j = 0; j < sizeZ; j++)
      p_w_z_prev[i][j] = p_w_z_best[i][j];
  for(i = 1; i < sizeD; i++)
    for(j = 0; j < sizeZ; j++)
      p_d_z_prev[i][j] =  p_d_z_best[i][j];
}

// Calculate the average likelihood of an event
double lemur::cluster::PLSA::getAverageLikelihood() {
  double answer = 0.0;
  int d;
  lemur::api::TERMID_T w;
  int events = 0;
  for (d = 1; d < sizeD; d++) {
    lemur::utility::HashFreqVector *freqs = testData[d];
    freqs->startIteration();
    while (freqs->hasMore()) { // TERM
      int freq;
      freqs->nextFreq(w, freq);
      answer += jointEstimateCurrent(d, w);
      events++;
    }
  }
  answer /= events;
  return answer;
}

// Calculate the average likelihood of an event using prev parameter values
double lemur::cluster::PLSA::getAverageLikelihoodPrev() {
  double answer = 0.0;
  int d;
  lemur::api::TERMID_T w;
  int events = 0;
  for (d = 1; d < sizeD; d++) {
    lemur::utility::HashFreqVector *freqs = testData[d];
    freqs->startIteration();
    while (freqs->hasMore()) { // TERM
      int freq;
      freqs->nextFreq(w, freq);
      answer += jointEstimate(d, w);
      events++;
    }
  }
  answer /= events;
  return answer;
}
    
// Calculates likelihood using previous parameter estimates
double lemur::cluster::PLSA::jointEstimate (int indexD, int indexW) {
  int z;
  double sum = 0.0;
  for(z = 0; z < sizeZ; z++)
    sum += p_z_prev[z]*p_w_z_prev[indexW][z]*p_d_z_prev[indexD][z];
  return sum;
}

// Calculates likelihood using current parameter estimates
double lemur::cluster::PLSA::jointEstimateCurrent (int indexD, int indexW) {
  int z;
  double sum = 0.0;
  for(z = 0; z < sizeZ; z++)
    sum += p_z_current[z]*p_w_z_current[indexW][z]*p_d_z_current[indexD][z];
  return sum;
}

// Calculates likelihood using best parameter estimates found 
// (over all restarts)
double lemur::cluster::PLSA::jointEstimateBest(int indexD, int indexW) {
  int z;
  double sum = 0.0;
  for(z = 0; z < sizeZ; z++)
    sum += p_z_best[z]*p_w_z_best[indexW][z]*p_d_z_best[indexD][z];
  return sum;
}

// jointestimateBeta is joint estimate where terms in summation 
// were raised to power beta.
double lemur::cluster::PLSA::jointEstimateBeta (int indexD, int indexW) {
  int z;
  double sum = 0.0;
  for(z = 0; z < sizeZ; z++)
    sum += p_z_prev[z]*pow(p_w_z_prev[indexW][z]*p_d_z_prev[indexD][z], beta);
  return sum;
}


// This routine gets the ball rolling
void lemur::cluster::PLSA::iterateWithRestarts() {
  iterate();
  cerr << "RESTART\tLogLikelihood\tTestDataLogLikelihood\n";
  cerr << 0 << "\t" << logLikelihood() << "\t" << validateDataLogLikelihood()
       << endl;
  for(int count = 0; count < numberOfRestarts - 1 ; count++) {
    iterate();
    cerr << "RESTART\tLogLikelihood\tTestDataLogLikelihood\n";
    cerr << (count + 1) << "\t" << logLikelihood() << "\t" 
         << validateDataLogLikelihood() << endl;
  }
  setPrevToBest();      
  setCurrentToBest();
  cerr << " FINAL LOG-LIKELIHOOD " << validateDataLogLikelihood() 
       << " Avg. Likelihood: " << getAverageLikelihood() << endl;
}

// This is the main routine of the model training
void lemur::cluster::PLSA::iterate() {
  double lastA;  // These two variables help check for
  double newA = 1;   // numerical errors due to overfitting
  int iterNum = 0;
  initializeParameters();
  beta = startBeta;
  lastA = getAverageLikelihoodPrev();
  cerr << "Starting average likelihood: " << lastA << endl;
  cerr << "iteration\ttLL\ttestLL\tAvgLL" << endl;      
  double LL = -DBL_MAX;
  double prevTestLL = validateDataLogLikelihood();
  double currentTestLL = -DBL_MAX;
  newA = 1;
       
  while(iterNum < numberOfIterations && beta > betaMin) {
    LL = interleavedIterationEM();
    currentTestLL = validateCurrentLogLikelihood();
    newA = getAverageLikelihood();
    cerr << iterNum <<  "\t" << LL << "\t" <<  currentTestLL << "\t" 
         << newA << endl;
    if((currentTestLL - annealcue)  < prevTestLL || newA < lastA) {
      currentTestLL = prevTestLL;
      beta *= betaModifier;
      iterNum--;
    } else {
      lastA = newA;
      setPrevToCurrent();
      prevTestLL = currentTestLL;
    }
    iterNum++;
  }
  // Only update best parameters if there is clear 
  // improvement
  // Do we want to look at current?
  if (newA > lastA && newA > bestA && currentTestLL > prevTestLL) {
    if (currentTestLL > bestTestLL) {
      setBestToCurrent();
      bestTestLL = currentTestLL;
      bestA = newA;
    }
  } else { 
    if (lastA > bestA) {
      if (prevTestLL    > bestTestLL ) {
        bestTestLL = prevTestLL;
        setBestToPrev();
        bestA = lastA;
      } 
    }
  }
}

void lemur::cluster::PLSA::initializeParameters() {
  int i, j;
  double sum;
  // initializing prev
  // Make sure we can use drand48 w/ win...
  // initialize p_z_prev
  sum = 0.0;
  for(i = 0; i < sizeZ; i++) {
    p_z_prev[i] = drand48()*drand48();
    sum += p_z_prev[i];
  }
  // normalize:
  for(i = 0; i < sizeZ; i++)
    p_z_prev[i] = p_z_prev[i]/sum;
  // initialize p_w_z_prev
  for(j = 0; j < sizeZ; j++) {
    sum = 0.0;
    double rnd;
    for(i = 1; i < sizeW; i++) {
      rnd = drand48()*drand48();
      sum += rnd;
      p_w_z_prev[i][j] = rnd;
    }
    for(i = 1; i < sizeW; i++)
      p_w_z_prev[i][j] = p_w_z_prev[i][j]/sum; 
  }
  // initialize p_d_z_prev
  for(j = 0; j < sizeZ; j++) {
    sum = 0.0;
    double rnd;
    for(i = 1; i < sizeD; i++) {
      rnd = drand48()*drand48();
      sum += rnd;
      p_d_z_prev[i][j] = rnd;
    }
    for(i = 1; i < sizeD; i++)
      p_d_z_prev[i][j] =  p_d_z_prev[i][j]/sum; 
  }
}

/* logLikelihood calculates the log-likelihood of the training data using the "previous" parameter estimates. Note that events with probability zero are thrown out of the likelihood calculation. This is useful when we have a word in our dictionary that doesn't occur in any document.  

This technique of not using zero probability events in the log-likelihood is dangerous, since we can overfit so much that some events get zero probability when they shouldn't. This artificially lowers the log-likelihood.   We use an additional metric, the Breese R value to catch this case and monitor for potential overfitting. "*/  

// Calculate the log likelihood of a given data set using the supplied
// joint estimate method.
double lemur::cluster::PLSA::doLogLikelihood(jointfuncType jointFunc, 
                                             lemur::utility::HashFreqVector **&myData) {
  int d;
  lemur::api::TERMID_T w;  
  double sum = 0.0;
  double logJoint = 0.0;
  for (d = 1; d < sizeD; d++) {
    lemur::utility::HashFreqVector *freqs = myData[d];
    freqs->startIteration();
    while (freqs->hasMore()) { // TERM
      int freq;
      freqs->nextFreq(w, freq);
      logJoint = log((this->*jointFunc)(d, w));
      if (logJoint > -HUGE_VAL) {       
        sum += freq * logJoint;
      }
    }
  }
  return sum;
}

// Calculate the training data log-likelihood using prev parameters
double lemur::cluster::PLSA::logLikelihood() {
  return doLogLikelihood(&lemur::cluster::PLSA::jointEstimate, data);
}

// Calculate the hold out data log-likelihood using prev parameters
double lemur::cluster::PLSA::validateDataLogLikelihood() {
  return doLogLikelihood(&lemur::cluster::PLSA::jointEstimate, testData);
}

// Calculate the held out data log-liklihood using current parameters
double lemur::cluster::PLSA::validateCurrentLogLikelihood() {
  return doLogLikelihood(&lemur::cluster::PLSA::jointEstimateCurrent, testData);
}

// Calculate the hold out data log-likelihood using the best parameters
double lemur::cluster::PLSA::bestDataLogLikelihood() {
  return doLogLikelihood(&lemur::cluster::PLSA::jointEstimateBest, testData);
}

bool lemur::cluster::PLSA::readArrays() {
  /// filestem passed in!
  bool res = true;
  ifstream if1("p_z.bin", ios_base::in | ios_base::binary);
  ifstream if2("p_w_z.bin", ios_base::in | ios_base::binary);
  ifstream if3("p_d_z.bin", ios_base::in | ios_base::binary);
  res &= readArray(if1, lemur::cluster::PLSA::P_Z);
  res &= readArray(if2, lemur::cluster::PLSA::P_W_Z);
  res &= readArray(if3, lemur::cluster::PLSA::P_D_Z);  
  if1.close();
  if2.close();
  if3.close();
  return res;
}

bool lemur::cluster::PLSA::readArray(ifstream& infile, enum pType which) {
  bool retVal = true;
  // initialize contents of best
  int dim1, dim2;
  int w, j;
  switch (which) {
  case P_Z:
    infile.read((char*) &dim1, sizeof(int));
    if (!(infile.gcount() == sizeof(int)))
      return false;
    p_z_best = new double[dim1];
    sizeZ = dim1;
    infile.read((char*) p_z_best, sizeof(double) * dim1);
    if (!(infile.gcount() == (sizeof(double) * dim1))) {
      delete[](p_z_best);
      p_z_best = NULL;
      return false;
    }
    break;
  case P_W_Z:
    infile.read((char*) &dim1, sizeof(int));
    if (!(infile.gcount() == sizeof(int)))
      return false;
    infile.read((char*) &dim2, sizeof(int));
    if (!(infile.gcount() == sizeof(int)))
      return false;
    p_w_z_best = new double *[dim1];
    sizeW = dim1;
    sizeZ = dim2;
    p_w_z_best[0] = NULL;
    // 1-based indexing
    for (w = 1; w < dim1; w++) {
      p_w_z_best[w] = new double [dim2];
      infile.read((char*) p_w_z_best[w], sizeof(double) * dim2);
      if (!(infile.gcount() == (sizeof(double) * dim2))) {
        for (j = 1; j <= w; j++)
          delete[](p_w_z_best[j]);
        delete[](p_w_z_best);
        p_w_z_best = NULL;
        return false;
      }
    }
    break;
  case P_D_Z:
    infile.read((char*) &dim1, sizeof(int));
    if (!(infile.gcount() == sizeof(int)))
      return false;
    infile.read((char*) &dim2, sizeof(int));
    if (!(infile.gcount() == sizeof(int)))
      return false;
    sizeD = dim1;
    sizeZ = dim2;
    p_d_z_best = new double *[dim1];
    p_d_z_best[0] = NULL;
    // 1-based indexing
    for (w = 1; w < dim1; w++) {
      p_d_z_best[w] = new double [dim2];
      infile.read((char*) p_d_z_best[w], sizeof(double) * dim2);
      if (!(infile.gcount() == (sizeof(double) * dim2))) {
        for (j = 1; j <= w; j++)
          delete[](p_d_z_best[j]);
        delete[](p_d_z_best);
        p_d_z_best = NULL;
        return false;
      }
    }
    break;
  default:
    retVal = false;
    break;
  }
  return retVal;
}

void lemur::cluster::PLSA::writeArrays() {
  // pass in a filestem
  ofstream of1("p_z.bin", ios_base::out | ios_base::binary);
  ofstream of2("p_w_z.bin", ios_base::out | ios_base::binary);
  ofstream of3("p_d_z.bin", ios_base::out | ios_base::binary);
  writeArray(of1, lemur::cluster::PLSA::P_Z);
  writeArray(of2, lemur::cluster::PLSA::P_W_Z);
  writeArray(of3, lemur::cluster::PLSA::P_D_Z);  
  of1.close();
  of2.close();
  of3.close();
}

void lemur::cluster::PLSA::writeArray(ofstream& ofile, enum pType which) {
  int w;
  switch (which) {
  case P_Z:
    ofile.write((const char*) &sizeZ, sizeof(int));
    ofile.write((const char*) p_z_best, sizeof(double) * sizeZ);
    break;
  case P_W_Z:
    ofile.write((const char*) &sizeW, sizeof(int));
    ofile.write((const char*) &sizeZ, sizeof(int));
    for (w = 1; w < sizeW; w++) {
      ofile.write((const char*) p_w_z_best[w], sizeof(double) * sizeZ);
    }
    break;
  case P_D_Z:
    ofile.write((const char*) &sizeD, sizeof(int));
    ofile.write((const char*) &sizeZ, sizeof(int));
    for (w = 1; w < sizeD; w++) {
      ofile.write((const char*) p_d_z_best[w], sizeof(double) * sizeZ);
    }
    break;
  default:
    break;
  }
}

// getProb(int d, int w) calculates \Pr(d,w)
double lemur::cluster::PLSA::getProb(int d, int w) const {
  double total = 0.0;
  for (int z = 0; z < sizeZ; z++) {
    total += p_z_best[z] * p_d_z_best[d][z] * p_w_z_best[w][z];
  }
  return total;
}
