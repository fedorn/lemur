
//
// TopdocsIndex
//
// 29 October 2004 -- tds
//

#include "indri/TopdocsIndex.hpp"
#include "indri/Parameters.hpp"
#include "indri/Path.hpp"
#include <algorithm>
#include <queue>

#define TOPDOCS_MINIMUM_DOCUMENTS (1000)
#define TOPDOCS_FRACTION (0.01)

void TopdocsIndex::create( const std::string& pathname ) {
  _documents = 0;
  _pathname = pathname;

  // makedir filename
  if( Path::exists( _pathname ) )
    Path::remove( _pathname );
  
  Path::create( _pathname );

  // make an index in there
  std::string listsPath = Path::combine( _pathname, "lists" );
  _lists.create( listsPath );
}

void TopdocsIndex::openRead( const std::string& pathname ) {
  _pathname = pathname;
  Parameters param;

  std::string manifestPath = Path::combine( _pathname, "manifest" );
  std::string listsPath = Path::combine( _pathname, "lists" );

  param.loadFile( Path::combine( _pathname, "manifest" ) );
  _documents = param.get( "documents", 0 );

  _lists.openRead( listsPath );
}

void TopdocsIndex::open( const std::string& pathname ) {
  _pathname = pathname;
  Parameters param;
  
  std::string manifestPath = Path::combine( _pathname, "manifest" );
  std::string listsPath = Path::combine( _pathname, "lists" );

  param.loadFile( manifestPath );
  _documents = param.get( "documents", 0 );
  _lists.open( listsPath );
}

void TopdocsIndex::close() {
  Parameters param;
  param.set( "documents", _documents );
  std::string manifestPath = Path::combine( _pathname, "manifest" );

  param.writeFile( manifestPath );
  _lists.close();
}

void TopdocsIndex::_getListEntries( std::vector<Entry>& output, Entry& smallest, IndriIndex& index, int term, UINT64 total, UINT64 keep ) {
  assert( keep > 0 );
  assert( total >= keep );
  assert( total > 0 );
  assert( term > 0 );

  // keep the best scores, but with the worst one on top
  std::priority_queue<Entry, std::vector<Entry>, Entry::score_greater> tops;
  DocPositionInfoList* iterator = index.docPositionInfoList( term );

  iterator->startIteration();
  
  do {
    // move to the next document in the list
    iterator->nextEntry();

    // make an entry for this document
    DocInfo* info = iterator->currentEntry();
    Entry entry( info->docID(),
                 info->termCount(),
                 index.docLength( info->docID() ) );

    // add this entry to the list of ones we've seen
    tops.push( entry );

    // get rid of any unwanted junk
    while( tops.size() > keep )
      tops.pop();
  } while( iterator->hasMore() );

  // now we're done harvesting data from the inverted list, so we
  // write this data out to a buffer
  assert( tops.size() == keep );

  // grab the smallest element
  smallest = tops.top();

  // shuffle the data out into another container:
  output.clear();
  while( tops.size() ) {
    output.push_back( tops.top() );
    tops.pop();
  }

  // sort by document ID
  std::sort( output.begin(), output.end(), Entry::document_less() );
  delete iterator;
}

void TopdocsIndex::_writeList( const std::vector<Entry>& data, const Entry& smallest, int term ) {
  assert( term > 0 );

  // 5 bytes max to compress each datum, 3 ints, <keep> entries
  Buffer buffer;
  size_t length = 5 * 3 * (data.size() + 1);
  char* storage = buffer.write( length );
  RVLCompressStream stream( storage, length );

  // smallest one
  stream << smallest.documentID
         << smallest.count
         << smallest.length;

  int lastDocument = 0;

  // all entries
  for( int i=0; i<data.size(); i++ ) {
    stream << (data[i].documentID - lastDocument)
           << data[i].count
           << data[i].length;

    lastDocument = data[i].documentID;
  }

  // write the stream out to the keyfile
  _lists.put( term, stream.data(), stream.dataSize() );
}

void TopdocsIndex::update( IndriIndex& index ) {
  // all up to date, no updating necessary
  if( _documents == index.docCount() )
    return;

  int terms = index.termCountUnique();

  // record the number of documents at this build time, for incremental uses
  _documents = index.docCount();
  int writes = 0;

  for( int i=0; i<terms; i++ ) {
    int count = index.docCount(i);
    if( count > TOPDOCS_MINIMUM_DOCUMENTS ) {
      // candidate for building a topdocs list
      std::vector<Entry> entries;
      Entry smallest;

      _getListEntries( entries, smallest, index, i, count, UINT64(count * TOPDOCS_FRACTION) );
      _writeList( entries, smallest, i );
    }
  }
}

TopdocsIndex::TopdocsList* TopdocsIndex::fetch( int term ) {
  assert( term > 0 );
  int length;
  char* data = 0;

  if( _lists.get( term, &data, length ) ) {
    TopdocsList* output = new TopdocsList();
    RVLDecompressStream stream( data, length );

    stream >> output->smallest.documentID
           >> output->smallest.count
           >> output->smallest.length;

    while( !stream.done() ) {
      Entry entry;

      stream >> entry.documentID
             >> entry.count
             >> entry.length;

      output->entries.push_back( entry );
    }

    return output;
  } else {
    return 0;
  }
}


