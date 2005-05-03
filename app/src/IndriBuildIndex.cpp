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
// buildindex
//
// 10 February 2004 -- tds
//
// Indri index build driver application
//

/*! \page IndriParameters Indri Parameter Files
<P>The indri applications, IndriBuildIndex, IndriDaemon, and IndriRunQuery accept
parameters from either the command line or from a file. The parameter
file uses an XML format. The command line uses dotted path notation. The
top level element in the parameters file is named <em>parameters</em>.

<H3> Repository construction parameters</h3>
<dl>
<dt>memory</dt>
<dd> an integer value specifying the number of bytes to use for the
indexing process. The value can include a scaling factor by adding a
suffix. Valid values are (case insensitive) K = 1000, M = 1000000, G =
1000000000. So 100M would be equivalent to 100000000. The value should
contain only decimal digits and the optional suffix. Specified as
&lt;memory&gt;100M&lt;/memory&gt; in the parameter file and as
<tt>-memory=100M</tt> on the command line. </dd> 
<dt>corpus</dt>
<dd>a complex element containing parameters related to a corpus. This
element can be specified multiple times. The parameters are 
<dl>
<dt>path</dt>
<dd>The pathname of the file or directory containing documents to
index. Specified as
&lt;corpus&gt;&lt;path&gt;/path/to/file_or_directory&lt;/path&gt;&lt;/corpus&gt;
in the parameter file and as
<tt>-corpus.path=/path/to/file_or_directory</tt> on the command
line.</dd> 
<dt>class</dt>
<dd>The FileClassEnviroment of the file or directory containing
documents to index. Specified as
&lt;corpus&gt;&lt;class&gt;trecweb&lt;/class&gt;&lt;/corpus&gt; in the
parameter file and as <tt>-corpus.class=trecweb</tt> on the command
line. The known classes are: 
<ul>
<li>html -- web page data.
<li>trecweb -- TREC web format, eg terabyte track.
<li>trectext -- TREC format, eg TREC-3 onward.
<li>doc -- Microsoft Word format (windows platform only).
<li>ppt -- Microsoft Powerpoint format (windows platform only).
<li>pdf --  Adobe PDF format.
<li>txt --  Plain text format.
</ul>
</dd>
Combining each of these elements, the paramter file would contain:
<br>
&lt;corpus&gt;<br>
&nbsp;&nbsp;&lt;path&gt;/path/to/file_or_directory&lt;/path&gt;<br>
&nbsp;&nbsp;&lt;class&gt;trecweb&lt;/class&gt;<br>
&lt;/corpus&gt;
</dd>
</dl>
<dt>metadata</dt>
<dd>a complex element containing one or more <tt>field</tt> entry
specifying the metadata fields to index, eg DOCNO. Specified as
&lt;metadata&gt;&lt;field&gt;fieldname&lt;/field&gt;&lt;/metadata&gt; in
the parameter file and as <tt>metadata.field=fieldname</tt> on the
command line.</dd> 
<dt>field</dt>
<dd>a complex element specifying the fields to index as data, eg
TITLE. This parameter can appear multiple times in a parameter file. 
<b>If provided on the command line, only the first field specified will 
be indexed</b>. The subelements are:  
<dl>
<dt>name</dt><dd>the field name, specified as
&lt;field&gt;&lt;name&gt;fieldname&lt;/name&gt;&lt;/field&gt; in the
parameter file and as <tt>-field.name=fieldname</tt> on the command
line.</dd> 
<dt>numeric</dt><dd>the symbol <tt>true</tt> if the field contains
numeric data, otherwise the symbol <tt>false</tt>, specified as
&lt;field&gt;&lt;numeric&gt;true&lt;/numeric&gt;&lt;/field&gt; in the
parameter file and as <tt>-field.numeric=true</tt> on the command
line. This is an optional parameter, defaulting to false. Note that <tt>0</tt>
can be used for false and <tt>1</tt> can be used for true. </dd>
</dl> 
</dd>
<dt>stemmer</dt>
<dd>a complex element specifying the stemming algorithm to use in the
subelement name. Valid options are Porter or Krovetz (case
insensitive). Specified as
&lt;stemmer&gt;&lt;name&gt;stemmername&lt;/name&gt;&lt;/stemmer&gt; and
as <tt>-stemmer.name=stemmername</tt> on the command line. This is an
optional parameter with the default of no stemming.
</dd>
<dt>stopper</dt>
<dd>a complex element containing one or more subelements named word,
specifying the stopword list to use. Specified as
&lt;stopper&gt;&lt;word&gt;stopword&lt;/word&gt;&lt;/stopper&gt; and
as <tt>-stopper.word=stopword</tt> on the command line. This is an
optional parameter with the default of no stopping.</dd>
</dl>

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
<dd> hostname of a host running an Indri server (IndriDaemon). Specified as
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
<dt><a name="rule">rule</a></dt>
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
<dt>   operator
<dd> type of item in query to apply to { term, window }</dd>
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
/*! \page IndriIndexer Indri Repository Builder
<P>
 This application builds an Indri Repository for a collection of documents.
 Parameter formats for all Indri applications are also described in
<a href="IndriParameters.html">IndriParameters.html</a>
<H3> Repository construction parameters</h3>
<dl>
<dt>memory</dt>
<dd> an integer value specifying the number of bytes to use for the indexing process. The value can include a scaling factor by adding a suffix. Valid values are (case insensitive) K = 1000, M = 1000000, G = 1000000000. So 100M would be equivalent to 100000000. The value should contain only decimal digits and the optional suffix. Specified as &lt;memory&gt;100M&lt;/memory&gt; in the parameter file and as <tt>-memory=100M</tt> on the command line. </dd>
<dt>index</dt>
<dd> path to where to place the Indri Repository. Specified as
&lt;index&gt;/path/to/repository&lt;/index&gt; in the parameter file and
as <tt>-index=/path/to/repository</tt> on the command line.
<dt>corpus</dt>
<dd>a complex element containing parameters related to a corpus. This element can be specified multiple times. The parameters are
<dl>
<dt>path</dt>
<dd>The pathname of the file or directory containing documents to index. Specified as &lt;corpus&gt;&lt;path&gt;/path/to/file_or_directory&lt;/path&gt;&lt;/corpus&gt; in the parameter file and as <tt>-corpus.path=/path/to/file_or_directory</tt> on the command line.</dd>
<dt>class</dt>
<dd>The FileClassEnviroment of the file or directory containing documents to index. Specified as &lt;corpus&gt;&lt;class&gt;trecweb&lt;/class&gt;&lt;/corpus&gt; in the parameter file and as <tt>-corpus.class=trecweb</tt> on the command line. The known classes are:
<ul>
<li>html -- web page data.
<li>trecweb -- TREC web format, eg terabyte track.
<li>trectext -- TREC format, eg TREC-3 onward.
<li>doc -- Microsoft Word format (windows platform only).
<li>ppt -- Microsoft Powerpoint format (windows platform only).
<li>pdf --  Adobe PDF format.
<li>txt --  Plain text format.
</ul>
</dd>
Combining each of these elements, the paramter file would contain:
<br>
&lt;corpus&gt;<br>
&nbsp;&nbsp;&lt;path&gt;/path/to/file_or_directory&lt;/path&gt;<br>
&nbsp;&nbsp;&lt;class&gt;trecweb&lt;/class&gt;<br>
&lt;/corpus&gt;
</dd>
</dl>
<dt>metadata</dt>
<dd>a complex element containing one or more <tt>field</tt> entry
specifying the metadata fields to index, eg DOCNO. Specified as
&lt;metadata&gt;&lt;field&gt;fieldname&lt;/field&gt;&lt;/metadata&gt; in
the parameter file and as <tt>metadata.field=fieldname</tt> on the
command line.</dd> 
<dt>field</dt>
<dd>a complex element specifying the fields to index as data, eg TITLE.
This parameter can appear multiple times in a parameter file. 
<b>If provided on the command line, only the first field specified will 
be indexed</b>. The subelements are:
<dl>
<dt>name</dt><dd>the field name, specified as
&lt;field&gt;&lt;name&gt;fieldname&lt;/name&gt;&lt;/field&gt; in the
parameter file and as <tt>-field.name=fieldname</tt> on the command line.</dd>
<dt>numeric</dt><dd>integer value of 1 if the field contains numeric
data, otherwise 0, specified as
&lt;field&gt;&lt;numeric&gt;0&lt;/numeric&gt;&lt;/field&gt; in the
parameter file and as <tt>-field.numeric=0</tt> on the command
line. This is an optional parameter, defaulting to 0.</dd>
</dl> 
</dd>
<dt>stemmer</dt>
<dd>a complex element specifying the stemming algorithm to use in the
subelement name. Valid options are Porter or Krovetz (case
insensitive). Specified as
&lt;stemmer&gt;&lt;name&gt;stemmername&lt;/name&gt;&lt;/stemmer&gt; and
as <tt>-stemmer.name=stemmername</tt> on the command line. This is an
optional parameter with the default of no stemming.
</dd>
<dt>stopper</dt>
<dd>a complex element containing one or more subelements named word,
specifying the stopword list to use. Specified as
&lt;stopper&gt;&lt;word&gt;stopword&lt;/word&gt;&lt;/stopper&gt; and
as <tt>-stopper.word=stopword</tt> on the command line. This is an
optional parameter with the default of no stopping.</dd>
</dl>

*/

#include "indri/Parameters.hpp"
#include "indri/IndexEnvironment.hpp"
#include <time.h>
#include "indri/Path.hpp"
#include "Exception.hpp"
#include "indri/FileTreeIterator.hpp"
#include <vector>
#include <map>
#include "indri/IndriTimer.hpp"

#include "indri/QueryEnvironment.hpp"
#include "indri/Thread.hpp"
#include "indri/SequentialWriteBuffer.hpp"

#include <math.h>
static indri::utility::IndriTimer g_timer;

static void buildindex_start_time() {
  g_timer.start();
}

static void buildindex_print_status( const char* status, int count ) {
  g_timer.printElapsedSeconds(std::cout);
  std::cout << ": " << status << count << "\r";
}

static void buildindex_print_status( const char* status, int count, const char* status2, INT64 count2 ) {
  g_timer.printElapsedSeconds(std::cout);
  std::cout << ": " << status << count << status2 << count2 << "\r";
}

static void buildindex_flush_status() {
  std::cout.flush();
}

static void buildindex_print_event( const char* event ) {
  g_timer.printElapsedSeconds(std::cout);
  std::cout << ": " << event << std::endl;
}

static void buildindex_print_event( std::string event ) {
  buildindex_print_event( event.c_str() );
}

class StatusMonitor : public indri::api::IndexStatus {
  void operator() ( int code, const std::string& documentFile, const std::string& error, int documentsParsed, int documentsSeen ) {
    std::stringstream event;

    switch(code) {
      case indri::api::IndexStatus::FileOpen:
        event << "Opened " << documentFile;
        buildindex_print_event( event.str() ); 
        buildindex_print_status( "Documents: ", documentsParsed );
        break;

      case indri::api::IndexStatus::FileSkip:
        event << "Skipped " << documentFile;
        buildindex_print_event( event.str() ); 
        break;

      case indri::api::IndexStatus::FileError:
        event << "Error in " << documentFile << " : " << error;
        buildindex_print_event( event.str() ); 
        break;

      default:
      case indri::api::IndexStatus::DocumentCount:
        if( !(documentsParsed % 500) )
          buildindex_print_status( "Documents: ", documentsParsed );
          buildindex_flush_status();
        break;
    }
  }
};

static bool copy_parameters_to_string_vector( std::vector<std::string>& vec, indri::api::Parameters p, const std::string& parameterName, const std::string* subName = 0 ) {
  if( !p.exists(parameterName) )
    return false;

  indri::api::Parameters slice = p[parameterName];
  
  for( int i=0; i<slice.size(); i++ ) {
    if( subName ) {
      if( slice[i].exists(*subName) ) {
        vec.push_back( slice[i][*subName] );
      }
    } else {
      vec.push_back( slice[i] );
    }
  }

  return true;
}

/*! Given a Specification and a field name, return a vector containing all
 * of the field names that conflate to that name as well as the original
 * name.
 * @param spec The indri::parse::FileClassEnvironmentFactory::Specification to inspect.
 * @param name The field name to look for.
 * @return a vector containing all of the field names that conflate to that name as well as the original name.
 */
static std::vector<std::string> findConflations(indri::parse::FileClassEnvironmentFactory::Specification *spec, std::string & name) {
  std::vector<std::string> retval;
  // have to walk the map and add an entry for each
  // conflation to a given name
  std::map<std::string, std::string>::const_iterator iter;
  for (iter = spec->conflations.begin(); 
       iter != spec->conflations.end(); iter++) {
    if( iter->second == name )
      retval.push_back(iter->first);
  }
  // put the original into the list
  retval.push_back(name);
  return retval;
}

/*! Add a string to a vector if not already present.
 * @return true if the string was added.
 */
static bool addNew(std::vector<std::string>& vec, string &name, 
                   std::string &specName, const char *msg) {
  bool retval = false;
  if( std::find( vec.begin(), vec.end(), name ) == vec.end() ) {
    std::cerr << "Adding " << name << " to " << specName << msg << std::endl;
    vec.push_back(name);
    retval = true;
  }
  return retval;
}
/*! Add field names to index or metadata for an existing file class 
 * specification.
 */
static bool augmentSpec(indri::parse::FileClassEnvironmentFactory::Specification *spec,
                        std::vector<std::string>& fields,
                        std::vector<std::string>& metadata,
                        std::vector<std::string>& metadataForward,
                        std::vector<std::string>& metadataBackward ) {
  // add to index and metadata fields in spec if necessary. 
  // return true if a field is changed.
  bool retval = false;
  // input field names are potentially conflated names:
  // eg headline for head, hl, or headline tags.
  std::vector<std::string> conflations;
  std::vector<std::string>::iterator i1;

  for (i1 = fields.begin(); i1 != fields.end(); i1++) {
    // find any conflated names
    conflations = findConflations(spec, *i1);
    for (int j = 0; j < conflations.size(); j++) {
      // only add the field for indexing if it doesn't already exist
      if (addNew(spec->index, conflations[j], 
                 spec->name, " as an indexed field")) {
        // added a field, make sure it is indexable
        // only add include tags if there are some already.
        // if it is empty, *all* tags are included.
        if( !spec->include.empty() ) {
          addNew(spec->include, conflations[j], spec->name,
                 " as an included tag");
        }
        retval = true;
      }
    }
  }
  
  // add fields that should be marked metadata for retrieval
  for (i1 = metadata.begin(); i1 != metadata.end(); i1++) {
    // find any conflated names
    conflations = findConflations(spec, *i1);
    for (int j = 0; j < conflations.size(); j++)
      retval |= addNew(spec->metadata, conflations[j], spec->name,
                       " as a metadata field");
  }
  // add fields that should have a metadata forward lookup table.
  for (i1 = metadataForward.begin(); i1 != metadataForward.end(); i1++) {
    // find any conflated names
    conflations = findConflations(spec, *i1);
    for (int j = 0; j < conflations.size(); j++) 
      retval |= addNew(spec->metadata, conflations[j], spec->name,
                       " as a forward indexed metadata field");
  }
  // add fields that should have a metadata reverse lookup table.
  for (i1 = metadataBackward.begin(); i1 != metadataBackward.end(); i1++) {
    // find any conflated names
    conflations = findConflations(spec, *i1);
    for (int j = 0; j < conflations.size(); j++) 
      retval |= addNew(spec->metadata, conflations[j], spec->name,
                       " as a forward indexed metadata field");
  }

  return retval;
}

void require_parameter( const char* name, indri::api::Parameters& p ) {
  if( !p.exists( name ) ) {
    LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a " + name + " parameter." );
  }
}

int main(int argc, char * argv[]) {
  try {
    indri::api::Parameters& parameters = indri::api::Parameters::instance();
    parameters.loadCommandLine( argc, argv );

    require_parameter( "corpus", parameters );
    require_parameter( "index", parameters );

    StatusMonitor monitor;
    indri::api::IndexEnvironment env;
    std::string repositoryPath = parameters["index"];

    buildindex_start_time();

    if( parameters.get( "version", 0 ) ) {
      std::cout << INDRI_DISTRIBUTION << std::endl;
    }

    env.setMemory( parameters.get("memory", 100*1024*1024) );
    std::string stemmerName = parameters.get("stemmer.name", "");
    if( stemmerName.length() )
      env.setStemmer(stemmerName);

    std::vector<std::string> stopwords;
    if( copy_parameters_to_string_vector( stopwords, parameters, "stopper.word" ) )
      env.setStopwords(stopwords);
    // fields to include as metadata (unindexed)
    std::vector<std::string> metadata;
    // metadata fields that should have a forward lookup table.
    std::vector<std::string> metadataForward;
    // metadata fields that should have a backward lookup table.
    std::vector<std::string> metadataBackward;
    copy_parameters_to_string_vector( metadata, parameters, "metadata.field" ); 
    copy_parameters_to_string_vector( metadataForward, parameters, "metadata.forward" ); 
    copy_parameters_to_string_vector( metadataBackward, parameters, "metadata.backward" );
    env.setMetadataIndexedFields( metadataForward, metadataBackward );
    
    std::vector<std::string> fields;
    std::string subName = "name";
    if( copy_parameters_to_string_vector( fields, parameters, "field", &subName ) )
      env.setIndexedFields(fields);

    if( indri::collection::Repository::exists( repositoryPath ) ) {
      env.open( repositoryPath, &monitor );
      buildindex_print_event( std::string() + "Opened repository " + repositoryPath );
    } else {
      env.create( repositoryPath, &monitor );
      buildindex_print_event( std::string() + "Created repository " + repositoryPath );
    }

    indri::api::Parameters corpus = parameters["corpus"];

    for( unsigned int i=0; i<corpus.size(); i++ ) {
      indri::api::Parameters thisCorpus = corpus[i];
      require_parameter( "path", thisCorpus );
      std::string corpusPath = thisCorpus["path"];
      std::string fileClass = thisCorpus.get("class", "");
      
      // augment field/metadata tags in the environment if needed.
      if( fileClass.length() ) {
        indri::parse::FileClassEnvironmentFactory::Specification *spec = env.getFileClassSpec(fileClass);
	      if( spec ) {
          // add fields if necessary, only update if changed.
          if( augmentSpec( spec, fields, metadata, metadataForward, metadataBackward ) ) 
            env.addFileClass(*spec);
          delete(spec);
        }
      }
      
      bool isDirectory = indri::file::Path::isDirectory( corpusPath );
      
      std::string anchorText = thisCorpus.get("inlink", "");
      env.setAnchorTextPath( corpusPath, anchorText );

      if( isDirectory ) {
        indri::file::FileTreeIterator files( corpusPath );

        for( ; files != indri::file::FileTreeIterator::end(); files++ ) {
          if( fileClass.length() )
            env.addFile( *files, fileClass );
          else
            env.addFile( *files );
        }
      } else {
        if( fileClass.length() )
          env.addFile( corpusPath, fileClass );
        else
          env.addFile( corpusPath );
      }
    }

    buildindex_print_event( "" );
    buildindex_print_event( "Closing index" );
    env.close();
    buildindex_print_event( "Finished" );
  } catch( lemur::api::Exception& e ) {
    LEMUR_ABORT(e);
  }

  return 0;
}
