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
// NormalDistribution
//
// 20 July 2004 -- tds
//

#ifndef INDRI_NORMALDISTRIBUTION_HPP
#define INDRI_NORMALDISTRIBUTION_HPP

//
// The normal distribution class is used to generate document priors
// based on some integer attribute of a document.
//

class NormalDistribution {
private:
  double _mu;
  double _sigma;

  double _cdf( double x ) {
    const double a_1 = 0.4361836;
    const double a_2 = -0.1201676;
    const double a_3 = 0.9372980;
    const double p = 0.33267;
    const double pi = 3.1415926535;

    double t = 1./(1.+p*x);
    double zx = ( 1. / sqrt(2*pi*_sigma) ) * exp( pow( -((x-_mu)/_sigma), 2 ) );

    double cdf = 1 - zx * ( a_1*t + a_2*pow(t,2) + a_3*pow(t,3) );
    return cdf;
  }

public:
  NormalDistribution( double mu, double sigma ) {
    _mu = mu;
    _sigma = sigma;
  }

  // The value returned here corresponds to the probability mass
  // on the Gaussian curve between value-0.5 and value+0.5

  double operator () ( INT64 value ) {
    // compute lower bound cdf
    double low = _cdf( value-0.5 );
    double high = _cdf( value+0.5 );
    return high - low;
  }
};

#endif // INDRI_NORMALDISTRIBUTION_HPP

