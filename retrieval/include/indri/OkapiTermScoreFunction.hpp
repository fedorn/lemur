/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// OkapiTermScoreFunction
//
// 3 February 2004 -- tds
//

#ifndef LEMUR_OKAPITERMSCOREFUNCTION_HPP
#define LEMUR_OKAPITERMSCOREFUNCTION_HPP

#include "indri/TermScoreFunction.hpp"
#include "indri/IndriIndex.hpp"
#include <math.h>

class OkapiTermScoreFunction : public TermScoreFunction {
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
  OkapiTermScoreFunction( IndriIndex& index, int termID, double termWeight, double k1 = 1, double b = 0.5 ) {
    double idfNumerator = index.docCount()+1;
    double idfDenominator = 0.5+index.docCount( termID );

    _inverseDocumentFrequency = log( idfNumerator / idfDenominator );
    _averageDocumentLength = index.docLengthAvg();
    _termWeight = termWeight;

    _k1 = k1;
    _b = b;

    _precomputeConstants();
  }

  OkapiTermScoreFunction( double idf, double averageDocumentLength, double k1, double b ) {
    _inverseDocumentFrequency = idf;
    _averageDocumentLength = averageDocumentLength;

    _k1 = k1;
    _b = b;

    _termWeight = queryTermWeight( 1000, 0 );
    _precomputeConstants();
  }

  double scoreOccurrence( int occurrences, int documentLength ) {
    double numerator = _idfTimesK1PlusOne * occurrences;
    double denominator = occurrences + _k1TimesOneMinusB + _bOverAvgDocLength * documentLength;

    return numerator / denominator;
  }

  double maximumScore( int maximumOccurrences, int minimumDocumentLength ) {
    return scoreOccurrence( maximumOccurrences, minimumDocumentLength );
  }

  double queryTermWeight( double queryK1, double queryB ) {
    return ( _inverseDocumentFrequency * queryK1 ) / ( 1 + queryK1 * ( (1-queryB) + queryB * (1/_averageDocumentLength) ) );
  }
};

#endif // LEMUR_OKAPITERMSCOREFUNCTION_HPP

