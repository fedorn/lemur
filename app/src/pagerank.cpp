/*==========================================================================
 * Copyright (c) 2002-2008 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
//
// pagerank
//
// 16 July 2005 -- dam
//
// application to compute PageRank using output from harvestlinks application
//
/*! \page PageRank PageRank application

<H3> PageRank parameters (pagerank)</h3>
<dl>
<dt>corpus</dt>
<dd>The pathname of the file or directory containing documents to
index. Specified as
&lt;corpus&gt;/path/to/file_or_directory&lt;/corpus&gt;
in the parameter file and as
<tt>-corpus=/path/to/file_or_directory</tt> on the command line.</dd> 
<dt>links</dt>
<dd>The pathname of the directory containing sorted links data for the 
documents specified in <tt>corpus</tt> produced by the harvestlinks program.
Specified as&lt;links&gt;/path/to/links&lt;/links&gt;
in the parameter file and as
<tt>-links=/path/to/links</tt> on the command line.</dd> 

<dt>output</dt>
<dd>basename for the output files.</dd>
<dt>docs</dt>
<dd>Number of documents to process per iteration. Default 1000.</dd>

<dt>iters</dt>
<dd>Number of iterations to use estimating the PageRank. Default 10</dd>

<dt>c</dt>
<dd> Dampening parameter. Default 0.5</dd>

<dt>writeRaw</dt>
<dd>Write the raw PageRank scores to &lt;output&gt;.raw </dd>
<dt>writeRanks</dt>
<dd>Write the integer PageRank scores [1..10] to &lt;output&gt;.ranks </dd>
<dt>writePriors</dt>
<dd>Write the log probability PageRank scores to &lt;output&gt;.priors.
This data file is suitable for input to the makeprior application.</dd>
</dl>
*/

#include <time.h>
#include "indri/Parameters.hpp"

#include "lemur/Exception.hpp"
#include "indri/PageRank.hpp"

int main( int argc, char** argv ) {
  try {
    indri::api::Parameters& parameters = indri::api::Parameters::instance();
    parameters.loadCommandLine( argc, argv );

    if( parameters.get( "version", false ) ) {
      std::cout << INDRI_DISTRIBUTION << std::endl;
    }

    std::string corpusPath = parameters[ "corpus" ];
    std::string linkPath = parameters[ "links" ];
    std::string outputFile = parameters[ "output" ];

    int docsPerIter = parameters.get( "docs", 1000 );
    int maxIters = parameters.get( "iters", 10 );
    double c = parameters.get( "c", 0.5 );

    indri::parse::PageRank pr( corpusPath, linkPath );
    pr.computePageRank( outputFile, maxIters, docsPerIter, c );

    if( parameters.get( "writeRaw", false ) ) {
      std::string rawFile = outputFile + ".raw";
      pr.writeRaw( outputFile, rawFile );
    }
    // default is to produce a prior file for makeprior.
    if( parameters.get( "writePriors", true ) ) {
      std::string priorFile = outputFile + ".prior";
      pr.writePriors( outputFile, priorFile );
    }
    if( parameters.get( "writeRanks", false ) ) {
      std::string ranksFile = outputFile + ".ranks";
      pr.writeRanks( outputFile, ranksFile );
    }
    // don't really neat the outputFile
    ::remove(outputFile.c_str());
  }
  catch( lemur::api::Exception& e ) {
    LEMUR_ABORT(e);
  }
}
