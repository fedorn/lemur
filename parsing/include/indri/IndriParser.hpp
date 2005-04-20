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
// Parser
// 
// 11 May 2004 -- tds
//

#ifndef INDRI_PARSER_HPP
#define INDRI_PARSER_HPP

#include "indri/ObjectHandler.hpp"
#include "indri/UnparsedDocument.hpp"
#include "indri/ParsedDocument.hpp"
#include <map>
#include <vector>

namespace indri {
  namespace parse
  {
      
    class Parser : public ObjectHandler<UnparsedDocument> {
    public:
      virtual ~Parser() {};

      virtual indri::api::ParsedDocument* parse( UnparsedDocument* document ) = 0;
      virtual void setTags( const std::vector<std::string>& include,
                            const std::vector<std::string>& exclude,
                            const std::vector<std::string>& index,
                            const std::vector<std::string>& metadata,
                            const std::map<std::string, std::string>& conflations ) = 0;

      virtual void handle( UnparsedDocument* document ) = 0;
      virtual void setHandler( ObjectHandler<indri::api::ParsedDocument>& handler ) = 0;
    };
  }
}

#endif // INDRI_PARSER_HPP


