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
// DocumentVector
//
// An independent representation of a document term list,
// suitable for network transmission.
//

#ifndef INDRI_DOCUMENTVECTOR_HPP
#define INDRI_DOCUMENTVECTOR_HPP

#include <string>
#include <map>
#include <vector>
#include "indri/TermListBuilder.hpp"

class DocumentVector {
public:
  struct Field {
    std::string name;
    int begin;
    int end;
    INT64 number;
  };

private:
  std::vector<std::string> _stems;
  std::vector<int> _positions;
  std::vector<Field> _fields;

  void _init( class IndriIndex* index, const class indri::index::TermListBuilder* termList, std::map<int,std::string>* termStringMap );

public:
  DocumentVector();
  DocumentVector( class IndriIndex* index, const class indri::index::TermListBuilder* termList );
  DocumentVector( class IndriIndex* index, const class indri::index::TermListBuilder* termList, std::map<int,std::string>& termStringMap );

  std::vector<std::string>& stems();
  const std::vector<std::string>& stems() const;

  std::vector<int>& positions();
  const std::vector<int>& positions() const;

  std::vector<Field>& fields();
  const std::vector<Field>& fields() const;
};

#endif // INDRI_DOCUMENTVECTOR_HPP


