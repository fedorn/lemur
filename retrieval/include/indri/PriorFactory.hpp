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
// PriorFactory
//
// 4 May 2004 -- tds
//

#ifndef INDRI_PRIORFACTORY_HPP
#define INDRI_PRIORFACTORY_HPP

#include "indri/QuerySpec.hpp"
#include "indri/Parameters.hpp"
namespace indri
{
  namespace query
  {
    
    class PriorFactory {
    private:
      struct prior_data {
        std::string field;
        std::map<int, indri::lang::PriorNode::tuple_type> tuples;
      };

      indri::api::Parameters _parameters;
      std::map<std::string, prior_data> _data;

      void _readPriorFile( prior_data& data, const std::string& fileName );
      void _readParamValues( prior_data& data, class indri::api::Parameters& range );

    public:
      PriorFactory();
      PriorFactory( indri::api::Parameters& parameters );
      void initialize();
      indri::lang::PriorNode* create( const std::string& name );
    };
  }
}

#endif // INDRI_PRIORFACTORY_HPP
