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
// QueryServer
//
// 15 March 2004 -- tds
//

#ifndef INDRI_QUERYSERVER_HPP
#define INDRI_QUERYSERVER_HPP

#include "indri/QuerySpec.hpp"
#include "indri/InferenceNetwork.hpp"
#include "indri/DocumentVector.hpp"
#include <vector>

class QueryServerResponse {
public:
  virtual InferenceNetwork::MAllResults& getResults() = 0;
};

class QueryServerDocumentsResponse {
public:
  virtual std::vector<ParsedDocument*>& getResults() = 0;
};

class QueryServerMetadataResponse {
public:
  virtual std::vector<std::string>& getResults() = 0;
};

class QueryServerVectorsResponse {
public:
  virtual std::vector<DocumentVector*>& getResults() = 0;
};

class QueryServer {
public:
  virtual ~QueryServer() {};
  virtual QueryServerResponse* runQuery( std::vector<indri::lang::Node*>& roots, int resultsRequested, bool optimize ) = 0;
  virtual QueryServerDocumentsResponse* documents( const std::vector<int>& documentIDs ) = 0;
  virtual QueryServerMetadataResponse* documentMetadata( const std::vector<int>& documentIDs, const std::string& attributeName ) = 0;

  // terms
  virtual INT64 termCount() = 0;
  virtual INT64 termCount( int term ) = 0;
  virtual INT64 termCount( const std::string& term ) = 0;
  virtual INT64 stemCount( const std::string& stem ) = 0;

  virtual std::string termName( int term ) = 0;
  virtual int termID( const std::string& term ) = 0;
  
  // fields
  virtual std::vector<std::string> fieldList() = 0;
  virtual INT64 termFieldCount( int term, const std::string& field ) = 0;
  virtual INT64 termFieldCount( const std::string& term, const std::string& field ) = 0;
  virtual INT64 stemFieldCount( const std::string& stem, const std::string& field ) = 0;

  // documents
  virtual int documentLength( int documentID ) = 0;
  virtual INT64 documentCount() = 0;
  
  // document vector
  virtual QueryServerVectorsResponse* documentVectors( const std::vector<int>& documentIDs ) = 0;
};

#endif // INDRI_QUERYSERVER_HPP

