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
  Probabilistic Latent Semantic Analysis 
  Java Reference implementation from Andrew Schein and Alexandrin Popescul
  Author: dmf 2/2003
*/

#ifndef _PLSA_HPP
#define _PLSA_HPP
#include "common_headers.hpp"
#include <cmath>
#include <set>
#include "Index.hpp"
#include "FreqVector.hpp"
namespace lemur 
{
  namespace cluster 
  {
    
    //forward declaration.
    class PLSA;

    // Needed to declare member function parameter (not sure why...).
    /// Joint probability function pointer.
    typedef double (PLSA::*jointfuncType)(int, int);


    /// \brief Probabilistic Latent Semantic Analysis 
    /// Java Reference implementation from Andrew Schein and Alexandrin Popescul
    /// (Penn). PennAspect (GPL).
    // Need both transient and permanent versions (store model components).
    class PLSA  {
    public:
      // need to pass in many parameters. Use sensible defaults...
      // document arguments.
      /// building with provided train/test partitions
      PLSA(const lemur::api::Index &dbIndex, int numCats, 
           lemur::utility::HashFreqVector **train, 
           lemur::utility::HashFreqVector **validate, int numIter, 
           int numRestarts, double betastart, 
           double betastop, double anneal, double betaMod);

      /// building without provided train/test partitions
      PLSA(const lemur::api::Index &dbIndex, int testPercentage, int numCats, int numIter, 
           int numRestarts, double betastart, 
           double betastop, double anneal, double betaMod);
      /// for using prebuilt tables.
      PLSA(const lemur::api::Index &dbIndex);
      virtual ~PLSA();
    
      /// Start things going.
      void iterateWithRestarts();

      /// get the final values
      /// P(z) vector
      double *get_p_z() const   {return p_z_best;}
      /// P(w|z) matrix
      double **get_p_w_z() const {return p_w_z_best;}
      /// P(d|z) matrix
      double **get_p_d_z() const {return p_d_z_best;}
      /// P(d,w) 
      double getProb(int d, int w) const ;
      // get the matrix sizes
      /// number of terms
      int numWords() const {return sizeW;}
      /// number of docs
      int numDocs() const {return sizeD;}
      /// number of categories
      int numCats() const {return sizeZ;}
      /// should these be public?
      /// On ctor if not constructing.
      bool readArrays();

    private:
      // attributes
      /// Index to use
      const lemur::api::Index &ind;
      /// number of categories
      int sizeZ;
      /// number of documents
      int sizeD;
      /// number of words
      int sizeW;

      /// train d->w freq list
      lemur::utility::HashFreqVector **data; // passed in
      /// test (validation) d->w freq list
      lemur::utility::HashFreqVector **testData; // passed in
      /// w->d inverted index for M step of P(w | z)
      set<int, less<int> > *invIndex; // constructed
  
      /// Beta for TEM
      double startBeta, beta, betaMin;
      /// eta for TEM (beta = eta * beta;)
      double betaModifier;
      /// annealcue value (delta)
      double annealcue;
      /// used in M step for p_z
      int R;
      /// How many iterations
      int numberOfIterations;
      /// How many restarts
      int numberOfRestarts;
      /// Best log likelihood on the test data so far.
      double bestTestLL;
      /// Best average log likelihood on the test data so far.
      double bestA;
      /// have we only loaded existing tables from files
      bool bestOnly;
      /// did we allocate the test/train vectors?
      bool ownMem;
      /// P(z) vector current iteration
      double *p_z_current;
      /// P(w|z) matrix current iteration
      double **p_w_z_current;
      /// P(d|z) matrix current iteration
      double **p_d_z_current;

      /// P(z) vector previous iteration
      double *p_z_prev;
      /// P(w|z) matrix previous iteration
      double **p_w_z_prev;
      /// P(d|z) matrix previous iteration
      double **p_d_z_prev;

      /// P(z) vector best iteration
      double *p_z_best;
      /// P(w|z) matrix best iteration
      double **p_w_z_best;
      /// P(d|z) matrix best iteration
      double **p_d_z_best;

      // methods
      /// Copy current iteration data to previous iteration data
      void setPrevToCurrent();
      /// Copy best iteration data to current iteration data
      void setCurrentToBest();
      /// Copy current iteration data to best iteration data
      void setBestToCurrent();
      /// Copy previous iteration data to best iteration data
      void setBestToPrev();
      /// Copy best iteration data to previous iteration data
      void setPrevToBest();

      /// Calculate the average likelihood of an event in the 
      /// test data using the current iteration estimates.
      double getAverageLikelihood();
      /// Calculate the average likelihood of an event in the 
      /// test data using the previous iteration estimates.
      double getAverageLikelihoodPrev();

      /// Estimates P(d,w) using previous parameter estimates
      double jointEstimate (int indexD, int indexW);
      /// Estimates P(d,w) using current parameter estimates
      double jointEstimateCurrent (int indexD, int indexW);
      /// Estimates P(d,w) using best parameter estimates
      double jointEstimateBest (int indexD, int indexW);
      /// Joint estimate using previous parameter estimates where 
      /// the terms in summation are raised to power beta.
      double jointEstimateBeta (int indexD, int indexW);
  
      /// main routine for model training.
      void iterate();
      /// Initialize the prev probability arrays to random values.
      void initializeParameters();

      /// Calculate the log likelihood of a given data set using the supplied
      /// joint estimate method.
      double doLogLikelihood(jointfuncType, lemur::utility::HashFreqVector **&myData);
      /// Calculate the training data log-likelihood using prev parameters
      double logLikelihood();
      /// Calculate the hold out data log-likelihood using prev parameters
      double validateDataLogLikelihood();
      /// Calculate the hold out data log-likelihood using current parameters
      double validateCurrentLogLikelihood();
      /// Calculate the hold out data log-likelihood using the best parameters
      double bestDataLogLikelihood();
      /// performs one EM iteration, returns log likelihood of training data
      double interleavedIterationEM();
      /// Select training/test events
      void selectTestTrain(int testPercent);
      /// Initialize attributes.
      void init();
      /// Initialize R and w->d inverted list.
      void initR();
      /// read/write array options.
      enum pType {P_Z = 0, P_W_Z = 1, P_D_Z = 2};
      /// write out all the arrays to file.
      void writeArrays();  
      /// Read a probability array (matrix) from a file
      bool readArray(ifstream& infile, enum pType which);
      /// Write a probability array (matrix) to a file
      void writeArray(ofstream& ofile, enum pType which);
    };
  }
}

#endif /* _PLSA_HPP */
