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
// DirichletTermScoreFunction
//
// 26 January 2004 - tds
//

#ifndef INDRI_DIRICHLETTERMSCOREFUNCTION_HPP
#define INDRI_DIRICHLETTERMSCOREFUNCTION_HPP

#include <math.h>

class DirichletTermScoreFunction : public TermScoreFunction {
private:
  double _mu;
  double _collectionFrequency;
  double _muTimesCollectionFrequency;

public:
  DirichletTermScoreFunction( double mu, double collectionFrequency ) {
    _collectionFrequency = collectionFrequency;
    _mu = mu;
    _muTimesCollectionFrequency = _mu * _collectionFrequency;
  }

  double scoreOccurrence( int occurrences, int contextSize ) {
    double seen = ( double(occurrences) + _muTimesCollectionFrequency ) / ( double(contextSize) + _mu );
    return log( seen );
  }

  double scoreOccurrence( int occurrences, int contextSize, int documentOccurrences, int documentLength ) {
    // can't two-level smooth with dirichlet
    return scoreOccurrence( occurrences, contextSize );
  }
};

#endif // INDRI_DIRICHLETTERMSCOREFUNCTION_HPP
