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
// QueryResponsePacker
//
// 23 March 2004 -- tds
//

#ifndef INDRI_QUERYRESPONSEPACKER_HPP
#define INDRI_QUERYRESPONSEPACKER_HPP

#include "indri/InferenceNetwork.hpp"
#include "lemur-compat.hpp"

class QueryResponsePacker {
private:
  InferenceNetwork::MAllResults& _results;

public:
  QueryResponsePacker( InferenceNetwork::MAllResults& results ) :
    _results(results)
  {
  }

  void write( NetworkMessageStream* stream ) {
    InferenceNetwork::MAllResults::iterator iter;
    EvaluatorNode::MResults::iterator nodeIter;

    for( iter = _results.begin(); iter != _results.end(); iter++ ) {
      const std::string& nodeName = iter->first;

      for( nodeIter = iter->second.begin(); nodeIter != iter->second.end(); nodeIter++ ) {
        const std::string& listName = nodeIter->first;
        std::string resultName = nodeName + ":" + listName;
        const std::vector<ScoredExtentResult>& resultList = nodeIter->second;

        // send each chunk of 100 results in a separate chunk
        ScoredExtentResult networkResults[100];
        size_t resultsSent = 0;

        while( resultList.size() > resultsSent ) {
          size_t sendChunk = lemur_compat::min<size_t>( resultList.size() - resultsSent, 100 );

          for( size_t i=0; i<sendChunk; i++ ) {
            networkResults[i].begin = htonl(resultList[i + resultsSent].begin);
            networkResults[i].end = htonl(resultList[i + resultsSent].end);
            networkResults[i].score = lemur_compat::htond( resultList[i + resultsSent].score );
            networkResults[i].document = htonl( resultList[i + resultsSent].document );
          }

          stream->reply( resultName, &networkResults, int(sendChunk * sizeof(ScoredExtentResult)) );
          resultsSent += sendChunk;
        }
      }
    }
    
    stream->replyDone();
  }
};

#endif // INDRI_QUERYRESPONSEPACKER_HPP

