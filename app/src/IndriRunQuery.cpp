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
// runquery
//
// 24 February 2004 -- tds
//
// 18 August 2004 -- dam
// incorporated multiple query, query expansion, and TREC output support 
//
//
// Indri local machine query application
//
/*! \page IndriRunQuery Indri Query Retrieval
<H3>QueryEnvironment Parameters</H3>
<H4>Retrieval Parameters</H4>
<dl>
<dt>memory</dt>
<dd> an integer value specifying the number of bytes to use for the
query retrieval process. The value can include a scaling factor by
adding a suffix. Valid values are (case insensitive) K = 1000, M =
1000000, G = 1000000000. So 100M would be equivalent to 100000000. The
value should contain only decimal digits and the optional
suffix. Specified as &lt;memory&gt;100M&lt;/memory&gt; in the parameter
file and as <tt>-memory=100M</tt> on the command line. </dd> 
<dt>index</dt>
<dd> path to an Indri Repository. Specified as
&lt;index&gt;/path/to/repository&lt;/index&gt; in the parameter file and
as <tt>-index=/path/to/repository</tt> on the command line. This element
can be specified multiple times to combine Repositories.
</dd>
<dt>server</dt>
<dd> hostname of a host running an Indri server (Indrid). Specified as
&lt;server&gt;hostname&lt;/server&gt; in the parameter file and
as <tt>-server=hostname</tt> on the command line. The hostname can
include an optional port number to connect to, using the form
<tt>hostname:portnum</tt>. This element
can be specified multiple times to combine servers.
</dd>
<dt>count</dt>
<dd>an integer value specifying the maximum number of results to
return for a given query. Specified as
&lt;count&gt;number&lt;/count&gt; in the parameter file and
as <tt>-count=number</tt> on the command line. </dd>
<dt>rule</dt>
<dd>specifies the smoothing rule (TermScoreFunction) to apply. Format of
the rule is:<br>

<tt>   ( key ":" value ) [ "," key ":" value ]* </tt>
<p>
Here's an example rule in command line format:<br>

   <tt>-rule=method:linear,collectionLambda:0.2,field:title</tt>
<p> and in parameter file format:<br>
<tt>
&lt;rule&gt;method:linear,collectionLambda:0.2,field:title&lt;/rule&gt;
</tt>
<p>This corresponds to Jelinek-Mercer smoothing with background lambda
equal to 0.2, only for items in a title field. 

<p>If nothing is listed for a key, all values are assumed.
So, a rule that does not specify a field matches all fields.  This makes
<tt>-rule=method:linear,collectionLambda:0.2</tt> a valid rule. 

<p>Valid keys:
<dl>
<dt>   method</dt><dd> smoothing method (text)</dd>
<dt>   field</dt><dd> field to apply this rule to</dd>
<dt>   operator&nbsp;</dt> <dd> type of item in query to apply to { term,
window }</dd>
</dl>

<p>Valid methods:
<dl>
<dt>   dirichlet</dt><dd> (also 'd', 'dir') (default mu=2500)</dd>
<dt>   jelinek-mercer</dt><dd> (also 'jm', 'linear') (default
collectionLambda=0.4, documentLambda=0.0),  collectionLambda is also
known as just "lambda", either will work </dt>
<dt>   twostage</dt><dd> (also 'two-stage', 'two') (default mu=2500,
lambda=0.4)</dd> 
</dl>
If the rule doesn't parse correctly, the default is Dirichlet, mu=2500.
</dd>
<dt>stopper</dt>
<dd>a complex element containing one or more subelements named word,
specifying the stopword list to use. Specified as
&lt;stopper&gt;&lt;word&gt;stopword&lt;/word&gt;&lt;/stopper&gt; and
as <tt>-stopper.word=stopword</tt> on the command line. This is an
optional parameter with the default of no stopping.</dd>
</dl>
<H4>Formatting Parameters</H4>
<dl>
<dt>queryOffset</dt>
<dd>an integer value specifying one less than the starting query number, eg 150 for
TREC formatted output. Specified as
&lt;queryOffset&gt;number&lt;/queryOffset&gt; in the parameter file and
as <tt>-queryOffset=number</tt> on the command line.</dd>
<dt>runID</dt>
<dd>a string specifying the id for a query run, used in TREC scorable
output. Specified as
&lt;runID&gt;someID&lt;/runID&gt; in the parameter file and
as <tt>-runID=someID</tt> on the command line.</dd>
<dt>trecFormat</dt>
<dd>the symbol <tt>true</tt> to produce TREC scorable output, otherwise
the symbol <tt>false</tt>. Specified as
&lt;trecFormat&gt;true&lt;/trecFormat&gt; in the parameter file and
as <tt>-trecFormat=true</tt> on the command line.  Note that <tt>0</tt>
can be used for false, and <tt>1</tt> can be used for true.</dd>
</dl>
<H4>Pseudo-Relevance Feedback Parameters</H4>
<dl>
<dt>fbDocs</dt>
<dd>an integer specifying the number of documents to use for
feedback. Specified as 
&lt;fbDocs&gt;number&lt;/fbDocs&gt; in the parameter file and
as <tt>-fbDocs=number</tt> on the command line.</dd>
<dt>fbTerms</dt>
<dd>an integer specifying the number of terms to use for
feedback. Specified as 
&lt;fbTerms&gt;number&lt;/fbTerms&gt; in the parameter file and
as <tt>-fbTerms=number</tt> on the command line.</dd>
<dt>fbMu</dt>
<dd>a floating point value specifying the value of mu to use for
feedback. [NB: document the feedback formulae]. Specified as
&lt;fbMu&gt;number&lt;/fbMu&gt; in the parameter file and
as <tt>-fbMu=number</tt> on the command line.</dd>
<dt>fbOrigWeight</dt>
<dd>a floating point value in the range [0.0..1.0] specifying the weight
for the original query in the expanded query. Specified as
&lt;fbOrigWeight&gt;number&lt;/fbOrigWeight&gt; in the parameter file and
as <tt>-fbOrigWeight=number</tt> on the command line.</dd>
</dl>
*/
#include <time.h>
#include "indri/QueryEnvironment.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/delete_range.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/NetworkServerProxy.hpp"

#include "indri/ListIteratorNode.hpp"
#include "indri/ExtentInsideNode.hpp"
#include "indri/DocListIteratorNode.hpp"
#include "indri/FieldIteratorNode.hpp"

#include "indri/Parameters.hpp"

#include "indri/ParsedDocument.hpp"
#include "indri/Collection.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/TaggedDocumentIterator.hpp"
#include "indri/XMLNode.hpp"

#include "indri/QueryExpander.hpp"
#include "indri/RMExpander.hpp"
#include "indri/PonteExpander.hpp"

#include "indri/IndriTimer.hpp"

#ifndef WIN32
#include <sys/time.h>
#endif

void open_indexes( indri::api::Parameters& params, indri::api::QueryEnvironment& env ) {
  if( !params.exists("index") && !params.exists("server") ) 
    LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a server or index to query against." );

  if( params.exists( "index" ) ) {
    indri::api::Parameters indexes = params["index"];

    for( unsigned int i=0; i < indexes.size(); i++ ) {
      env.addIndex( std::string(indexes[i]) );
    }
  }

  if( params.exists( "server" ) ) {
    indri::api::Parameters servers = params["server"];

    for( unsigned int i=0; i < servers.size(); i++ ) {
      env.addServer( std::string(servers[i]) );
    }
  }
}

static bool copy_parameters_to_string_vector( std::vector<std::string>& vec, indri::api::Parameters p, const std::string& parameterName ) {
  if( !p.exists(parameterName) )
    return false;

  indri::api::Parameters slice = p[parameterName];
  
  for( int i=0; i<slice.size(); i++ ) {
    vec.push_back( slice[i] );
  }

  return true;
}

int main(int argc, char * argv[]) {
  try {
    indri::api::Parameters& param = indri::api::Parameters::instance();
    param.loadCommandLine( argc, argv );

    if( param.get( "version", 0 ) ) {
      std::cout << INDRI_DISTRIBUTION << std::endl;
    }

    if( !param.exists( "query" ) )
      LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify at least one query." );

    std::string queryString = param["query"];
    int resultsRequested = param.get( "count", 1000 );
    int initialRequested = param.get( "fbDocs", resultsRequested );
    int fbDocs = param.get( "fbDocs", 0 );
    string runID = param.get( "runID", "indri" );
    int queryOffset = param.get( "queryOffset" , 0 );
    int trecFormat = param.get( "trecFormat" , 0 );
    bool printQuery = param.get( "printQuery", false );

    indri::api::Parameters queries = param[ "query" ];

    indri::api::QueryEnvironment env;
    open_indexes( param, env );

    env.setMemory( param.get("memory", 100*1024*1024) );

    std::vector<std::string> stopwords;
    if( copy_parameters_to_string_vector( stopwords, param, "stopper.word" ) )
      env.setStopwords(stopwords);
    
    std::vector<std::string> smoothingRules;
    if( copy_parameters_to_string_vector( smoothingRules, param, "rule" ) )
      env.setScoringRules( smoothingRules );

    indri::query::QueryExpander * qe = NULL;

    if( fbDocs ) {
      qe = new indri::query::RMExpander( &env, param );
    }
    // actually perform the query (in parallel for servers, in series for
    // the local databases)
    std::vector<indri::api::ScoredExtentResult> results;

    // fetch document information
    std::vector<std::string> documentNames;
    std::vector<indri::api::ParsedDocument*> documents;

    for( int query = 0; query < queries.size(); query++ ) {
      std::string queryString = std::string( queries[ query ] );
      std::string expandedQuery;

      if( printQuery ) std::cout << "# query: " << queryString << std::endl;

      results = env.runQuery( queryString, initialRequested );

      if( qe ) {
        expandedQuery = qe->expand( queryString, results );
        if( printQuery ) std::cout << "# expanded: " << expandedQuery << std::endl;
        results = env.runQuery( expandedQuery, resultsRequested );
      }

      if( param.get( "printDocuments", 0 ) || param.get( "printPassages", 0 ) ) {
        documents = env.documents( results );
        documentNames.clear();

        for( unsigned int i=0; i<results.size(); i++ ) {
          indri::api::ParsedDocument* doc = documents[i];
          std::string documentName;

          indri::utility::greedy_vector<indri::parse::MetadataPair>::iterator iter = std::find_if( documents[i]->metadata.begin(),
            documents[i]->metadata.end(),
            indri::parse::MetadataPair::key_equal( "docno" ) );

          if( iter != documents[i]->metadata.end() )
            documentName = (char*) iter->value;

          documentNames.push_back( documentName );
        }
      } else {
        documentNames = env.documentMetadata( results, "docno" );
      }

      // print results: <score> <name> <beginExtent> <endExtent>
      for( unsigned int i=0; i<results.size(); i++ ) {

        if( trecFormat ) {
          std::cout << ( query + queryOffset + 1 ) << " "
            << "Q0 "
            << documentNames[i] << " "
            << ( i + 1 ) << " "
            << results[ i ].score << " "
            << runID << std::endl;
        }
        else {
          std::cout << results[i].score << "\t"
            << documentNames[i] << "\t"
            << results[i].begin << "\t"
            << results[i].end << std::endl;
        }

        if( param.get( "printDocuments", 0 ) ) {
          std::cout << documents[i]->text << std::endl;
        }

        if( param.get( "printPassages", 0 ) ) {
          // we'll print the text from the beginning of the first word
          int byteBegin = documents[i]->positions[ results[i].begin ].begin;
          int byteEnd = documents[i]->positions[ results[i].end-1 ].end;
          std::cout.write( documents[i]->text + byteBegin, byteEnd - byteBegin );
          std::cout << std::endl;
        }

        if( documents.size() )
          delete documents[i];
      }
    }

    delete qe;
    env.close();
  } catch( Exception& e ) {
    LEMUR_ABORT(e);
  } catch( ... ) {
    std::cout << "Caught unhandled exception" << std::endl;
    return -1;
  }

  return 0;
}


