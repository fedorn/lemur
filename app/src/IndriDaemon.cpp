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
// indrid
//
// 23 March 2004 -- tds
//
// Listens on a socket (on a port defined with the -port parameter)
// for requests from an Indri query client, and processes them as they arrive.
//
/*! \page IndriDaemon IndriDaemon query server
<p> This application opens an Indri Repository and listens on a socket
for requests from an Indri query client, and processes them as they arrive. 

<H3>IndriDaemon Parameters</H3>
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
<dd> path to the Indri Repository to act as server for. Specified as
&lt;index&gt;/path/to/repository&lt;/index&gt; in the parameter file and
as <tt>-index=/path/to/repository</tt> on the command line.
</dd>
<dt>port</dt>
<dd> an integer value specifying the port number to use.Specified as
&lt;port&gt;number&lt;/port&gt; in the parameter file and as
<tt>-port=number</tt> on the command line. </dd> 
</dl>

 */
#include "indri/indri-platform.h"
#include "lemur-compat.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkListener.hpp"
#include "indri/NetworkServerStub.hpp"
#include "indri/Parameters.hpp"
#include "Exception.hpp"

int main( int argc, char* argv[] ) {
  try {
    Parameters& parameters = Parameters::instance();
    parameters.loadCommandLine( argc, argv );

    NetworkListener listener;
    int port = parameters.get( "port", INDRID_PORT );
    std::string repositoryPath = parameters["index"];

    // wrap the index in a local server that the stub can talk to
    Repository* repository = new Repository();
    repository->openRead( repositoryPath );
    LocalQueryServer server( *repository );

    // open for business
    listener.listen( port );
    NetworkStream* connection;

    // this handles the threading issue by only allowing one
    // connection at a time; for our current uses this is fine
    while( connection = listener.accept() ) {
      NetworkMessageStream messageStream( connection );
      NetworkServerStub stub( &server, &messageStream );
      stub.run();
      delete connection;
    }

    repository->close();
    delete repository;
    return 0;
  } catch( Exception& e ) {
    LEMUR_ABORT(e);
  }
}


