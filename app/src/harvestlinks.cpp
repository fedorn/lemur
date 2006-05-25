
//
// harvestlinks
//
// 3 June 2004 -- tds
//
// Anchor text mining application
//

#include <time.h>
#include "indri/Parameters.hpp"

#include "indri/TokenizedDocument.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/TaggedDocumentIterator.hpp"
#include "indri/TaggedTextParser.hpp"
#include "indri/HTMLParser.hpp"
#include "indri/TokenizerFactory.hpp"
#include "indri/ConflationPattern.hpp"
#include "indri/AnchorTextWriter.hpp"
#include "indri/FileTreeIterator.hpp"
#include "indri/Path.hpp"
#include "Exception.hpp"
#include "indri/Combiner.hpp"

static void harvest_anchor_text_file( const std::string& path, const std::string& harvestPath, indri::parse::HTMLParser& parser, indri::parse::Tokenizer *tokenizer) {
  indri::parse::TaggedDocumentIterator iterator;
  iterator.open( path );
  iterator.setTags( 
    "<DOC>",              // startDocTag
    "</DOC>\n",             // endDocTag
    "</DOCHDR>"          // endMetadataTag
  );

  indri::parse::UnparsedDocument* unparsed;
  indri::parse::TokenizedDocument* tokenized;
  indri::parse::AnchorTextWriter writer( harvestPath );

  while( (unparsed = iterator.nextDocument()) != 0 ) {
    tokenized = tokenizer->tokenize( unparsed );
    indri::api::ParsedDocument* parsed = parser.parse( tokenized );
    writer.handle(parsed);
  }
  
  // close up everything
  iterator.close();
}

static void harvest_anchor_text( const std::string& corpusPath, const std::string& harvestPath ) {
  std::vector<std::string> include;
  include.push_back( "absolute-url" );
  include.push_back( "relative-url" );
  include.push_back( "a" );
  std::vector<std::string> empty;
  std::map<indri::parse::ConflationPattern*,std::string> mempty;

  indri::parse::HTMLParser parser;
  indri::parse::Tokenizer* tokenizer = indri::parse::TokenizerFactory::get( "word" );
  parser.setTags( empty, empty, include, empty, mempty );

  if( indri::file::Path::isDirectory( corpusPath ) ) {
    indri::file::FileTreeIterator files( corpusPath );

    for( ; files != indri::file::FileTreeIterator::end(); files++ ) {
      std::string filePath = *files;
      std::string relative = indri::file::Path::relative( corpusPath, filePath );
      std::string anchorText = indri::file::Path::combine( harvestPath, relative );      
      std::cout << "harvesting " << filePath << std::endl;

      try {
        harvest_anchor_text_file( *files, anchorText, parser, tokenizer );
      } catch( lemur::api::Exception& e ) {
        std::cout << e.what() << std::endl;
      }
    }
  } else {
    std::string anchorText = indri::file::Path::combine( harvestPath, "data" );
    harvest_anchor_text_file( corpusPath, anchorText, parser, tokenizer );
  }
}

int main(int argc, char * argv[]) {
  try {
    indri::api::Parameters& parameters = indri::api::Parameters::instance();
    parameters.loadCommandLine( argc, argv );

    if( parameters.get( "version", 0 ) ) {
      std::cout << INDRI_DISTRIBUTION << std::endl;
    }
    
    std::string corpusPath = parameters["corpus"];
    std::string outputPath = parameters["output"];
    std::string redirectPath = parameters["redirect"];
    int bins = parameters.get( "bins", 10 );

    std::string harvestPath = indri::file::Path::combine( outputPath, "harvest" );
    std::string bucketPath = indri::file::Path::combine( outputPath, "buckets" );
    std::string preSortPath = indri::file::Path::combine( outputPath, "presort" );
    std::string sortedPath = indri::file::Path::combine( outputPath, "sorted" );

    if( parameters.get( "delete", 1 ) ) {
      if( indri::file::Path::isDirectory( harvestPath ) )
        indri::file::Path::remove( harvestPath );
      if( indri::file::Path::isDirectory( bucketPath ) )
        indri::file::Path::remove( bucketPath );
      if( indri::file::Path::isDirectory( preSortPath ) )
        indri::file::Path::remove( preSortPath );
      if( indri::file::Path::isDirectory( sortedPath ) )
        indri::file::Path::remove( sortedPath );

      indri::file::Path::make( harvestPath ); 
      indri::file::Path::make( bucketPath ); 
      indri::file::Path::make( preSortPath ); 
      indri::file::Path::make( sortedPath ); 
    }

    // step 1: harvest text
    if( parameters.get( "harvest", 1 ) )
      harvest_anchor_text( corpusPath, harvestPath );
   
    indri::parse::Combiner combiner( bins );
    
    // step 2: hash all text into buckets
    if( parameters.get( "buckets", 1 ) )
      combiner.hashToBuckets( bucketPath, harvestPath );

    // step 3: hash all redirects into buckets
    if( parameters.get( "targets", 1 ) )
      combiner.hashRedirectTargets( bucketPath, redirectPath );

    // step 4: combine redirect buckets
    if( parameters.get( "combineredirect", 1 ) )
      combiner.combineRedirectDestinationBuckets( bucketPath );

    // step 5: combine each bin together and write back into corpus files
    if( parameters.get( "combine", 1 ) )
      combiner.combineBuckets( preSortPath, bucketPath );

    // step 6: sort resulting corpus files
    if( parameters.get( "sort", 1 ) )
      combiner.sortCorpusFiles( sortedPath, preSortPath, harvestPath );
  } catch( lemur::api::Exception& e ) {
    LEMUR_ABORT(e);
  }

  return 0;
}


