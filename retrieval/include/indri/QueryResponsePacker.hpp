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

