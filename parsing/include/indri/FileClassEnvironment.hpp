/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// FileClassEnvironment
//
// 23 August 2004 -- tds
//

#ifndef INDRI_FILECLASSENVIRONMENT_HPP
#define INDRI_FILECLASSENVIRONMENT_HPP

#include "indri/IndriParser.hpp"
#include "indri/DocumentIterator.hpp"
namespace indri
{
  namespace parse
  {
    
    struct FileClassEnvironment {
      FileClassEnvironment() : parser(0), iterator(0) {}
      ~FileClassEnvironment() {
        delete parser;
        delete iterator;
      }

      Parser* parser;
      DocumentIterator* iterator;
    };
  }
}

#endif // INDRI_FILECLASSENVIRONMENT_HPP

