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
// TFIDFScoreFunction
//
// 23 January 2004 -- tds
//

#ifndef INDRI_TFIDFTERMSCOREFUNCTION_HPP
#define INDRI_TFIDFTERMSCOREFUNCTION_HPP

#include "indri/TermScoreFunction.hpp"
#include "indri/IndriIndex.hpp"
#include <math.h>

class TFIDFTermScoreFunction : public TermScoreFunction {
private:
  /// inverse document frequency (IDF) for this term
  double _inverseDocumentFrequency; 
  /// average document length in the collection
  double _averageDocumentLength;

  double _termWeight;

  // These are BM25 parameters
  double _k1;
  double _b;

  // The following values are precomputed so that score computation will go faster
  double _bOverAvgDocLength;
  double _k1TimesOneMinusB;
  double _idfTimesK1PlusOne;
  double _k1TimesBOverAvgDocLength;
  double _termWeightTimesIDFTimesK1;

  void _precomputeConstants() {
    _idfTimesK1PlusOne = _inverseDocumentFrequency * ( _k1 + 1 );
    _k1TimesOneMinusB = _k1 * (1-_b);
    _bOverAvgDocLength = _b / _averageDocumentLength;
    _k1TimesBOverAvgDocLength = _k1 * _bOverAvgDocLength;
    _termWeightTimesIDFTimesK1 = _termWeight * _inverseDocumentFrequency * _k1;
  }

public:
  TFIDFTermScoreFunction( IndriIndex& index, int termID, double termWeight, double k1 = 1, double b = 0.5 ) {
    double idfNumerator = index.docCount()+1;
    double idfDenominator = 0.5+index.docCount( termID );

    _inverseDocumentFrequency = log( idfNumerator / idfDenominator );
    _averageDocumentLength = index.docLengthAvg();
    _termWeight = termWeight;

    _k1 = k1;
    _b = b;

    _precomputeConstants();
  }

  TFIDFTermScoreFunction( double idf, double averageDocumentLength, double k1, double b ) {
    _inverseDocumentFrequency = idf;
    _averageDocumentLength = averageDocumentLength;

    _k1 = k1;
    _b = b;

    _termWeight = queryTermWeight( 1000, 0 );
    _precomputeConstants();
  }

  double scoreOccurrence( int occurrences, int documentLength ) {
    //
    // Score function is:
    //                                                   K1 * occurrences
    // score = termWeight * IDF * ------------------------------------------------------------------
    //                             occurrences + K1 * ( (1-B) + B * ( documentLength / avgDocLength) )
    //
    // Factored for constants:
    //                        (termWeight * IDF * K1) * occurrences
    // score = ------------------------------------------------------------------------
    //          occurrences + (K1 * (1-B)) + (K1 * B * 1/avgDocLength) * documentLength
    //

    double numerator = _termWeightTimesIDFTimesK1 * occurrences;
    double denominator = occurrences + _k1TimesOneMinusB + _k1TimesBOverAvgDocLength * documentLength;

    return numerator / denominator;
  }

  double scoreOkapiOccurrence( int occurrences, int documentLength ) {
    double numerator = _idfTimesK1PlusOne * occurrences;
    double denominator = occurrences + _k1TimesOneMinusB + _bOverAvgDocLength * documentLength;

    return numerator / denominator;
  }

  double maximumScore( int minimumDocumentLength, int maximumOccurrences ) {
    return scoreOccurrence( maximumOccurrences, minimumDocumentLength );
  }

  double queryTermWeight( double queryK1, double queryB ) {
    return ( _inverseDocumentFrequency * queryK1 ) / ( 1 + queryK1 * ( (1-queryB) + queryB * (1/_averageDocumentLength) ) );
  }
};

#endif // TFIDF_TERMSCOREFUNCTION_HPP

