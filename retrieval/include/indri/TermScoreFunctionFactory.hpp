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
// TermScoreFunctionFactory
//
// 19 August 2004 -- tds
//

#ifndef INDRI_TERMSCOREFUNCTIONFACTORY_HPP
#define INDRI_TERMSCOREFUNCTIONFACTORY_HPP

#include "indri/TermScoreFunction.hpp"
#include <string>

class TermScoreFunctionFactory {
public:
  static TermScoreFunction* get( const std::string& spec, double collectionFrequency );
};

#endif // INDRI_TERMSCOREFUNCTIONFACTORY_HPP

