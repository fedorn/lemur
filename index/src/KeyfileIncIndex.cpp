/*==========================================================================
 *
 *  Original source copyright (c) 2002, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2003, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/
/*
 * NAME DATE - COMMENTS
 * tnt 01/2003 - created
 * dmf 06/2003 - modify for incremental
 * tds 09/2003 - modified for keyfile
 *========================================================================*/

#include "KeyfileIncIndex.hpp"
#include "KeyfileDocListSegmentReader.hpp"
#include "ReadBuffer.hpp"
#include "WriteBuffer.hpp"
#include "DocMgrManager.hpp"

#include <assert.h>
#include <functional>
#include <algorithm>
#include <sstream>

// Keyfiles get 1mb each, and there are 6 of them
// 16MB goes to the TermCache
// 1MB for the output buffer at flush time
// 1MB for general data structures
// 4MB for the invlists vector (4 bytes * 1M terms)
// 10MB for heap overhead, general breathing room
// Total base RAM usage: 40MB

#define KEYFILE_BASE_MEMORY_USAGE       (40*1024*1024)
#define KEYFILE_WRITEBUFFER_SIZE           (1024*1024)
#define KEYFILE_DOCLISTREADER_SIZE         (1024*1024)

#define KEYFILE_MINIMUM_LISTS_SIZE          (512*1024)
#define KEYFILE_EPSILON_FLUSH_POSTINGS      (512*1024)

// avoid conflicts with other index include files.
// do something about these defines. Ought to be enums.

#define NAMES_SIZE   10
// for names array
#define DOC_INDEX    0
#define DOC_LOOKUP   1
#define TERM_INDEX   2
#define TERM_LOOKUP  3
#define TERM_IDS     4
#define DOC_IDS      5
#define DOCMGR_IDS   6
#define VERSION_NUM  7
#define TERM_IDSTRS  8
#define DOC_IDSTRS   9

#define IVLINDEX  ".ivl"

KeyfileIncIndex::KeyfileIncIndex(char* prefix, int cachesize, 
				 DOCID_T startdocid) {
  listlengths = 0;
  setName(prefix);

  _computeMemoryBounds( cachesize );

  if (!tryOpen()) {
    // brand new empty index
    
    names.resize( NAMES_SIZE );

    counts[TOTAL_TERMS] = counts[UNIQUE_TERMS] = 0;
    counts[DOCS] = 0;
    counts[DT_FILES] = 1;
    curdocmgr = -1;

    // docterm index stuff
    std::stringstream docfname;
    docfname << name << DTINDEX;
    writetlist.open(docfname.str(), ios::binary | ios::out | ios::ate);

    names[TERM_LOOKUP] = name + DTLOOKUP;
    names[TERM_INDEX] = name + DTINDEX;
    names[DOC_LOOKUP] = name + INVLOOKUP;
    names[DOC_INDEX] = name + IVLINDEX;

    // create document ids trees
    names[DOC_IDS] = name + DOCIDMAP;
    names[DOC_IDSTRS] = name + DOCIDSTRMAP;

    // create term dictionary trees
    names[TERM_IDS] = name + TERMIDMAP;
    names[TERM_IDSTRS] = name + TERMIDSTRMAP;
    
    createDBs();

    // add reserved OOV term
    int zero = 0;
    int invlen = strlen(INVALID_STR)+1;
    
    tIDs.put( INVALID_STR, &zero, sizeof zero );
    tSTRs.put( 0, INVALID_STR, invlen );
    dIDs.put( INVALID_STR, &zero, sizeof zero );
    dSTRs.put( 0, INVALID_STR, invlen );

    _largestFlushedTermID = 0;
  }
}

KeyfileIncIndex::KeyfileIncIndex(const char* indexName) {
  listlengths = 0;
  curdocmgr = -1;
  _computeMemoryBounds( 128 * 1024 * 1024 );
  _largestFlushedTermID = 0;
  aveDocLen = 0;

  if( indexName ) {
    open(indexName);
  }
}

KeyfileIncIndex::~KeyfileIncIndex() {
  unsigned int i;
  for( i = 0; i< _segments.size(); i++ ) {
    _segments[i]->close();
    delete _segments[i];
  }

  writetlist.close();
  dtlookup.close();
  invlookup.close();
  dIDs.close();
  tIDs.close();
  dSTRs.close();
  tSTRs.close();

  for (i = 0; i < docMgrs.size(); i++)
    delete(docMgrs[i]); // objects

  delete dtlookupReadBuffer;
  delete[](counts);
}

void KeyfileIncIndex::_resetEstimatePoint() {
  // how many postings should we queue up before
  // trying to estimate a flush time?
  // pick the worst case--one unique term per document
  // that would give us one InvFPDocList plus a few bytes
  // for the list buffer:

  _estimatePoint = _listsSize / (sizeof(InvFPDocList) + 16);
}

void KeyfileIncIndex::_computeMemoryBounds( int memorySize ) {
  // The following is fixed allocation--in the future
  // we may want to try something parametric:
  _memorySize = memorySize;
  _listsSize = memorySize - KEYFILE_BASE_MEMORY_USAGE;

  if( _listsSize < KEYFILE_MINIMUM_LISTS_SIZE ) {
    _listsSize = KEYFILE_MINIMUM_LISTS_SIZE;
  }

  _resetEstimatePoint();
}

void KeyfileIncIndex::openSegments() {
  // open existing inverted list segments
  for( int i=0; i<KEYFILE_MAX_SEGMENTS; i++ ) {
    File* in = new File();
    std::stringstream name;
    name << names[DOC_INDEX] << i;

    in->open( name.str().c_str(), std::fstream::in | std::fstream::binary );

    if( in->rdstate() & std::fstream::failbit ) {
      delete in;
      break;
    } else {
      _segments.push_back( in );
    }
  }
}

void KeyfileIncIndex::openDBs() {
  dtlookup.open( names[TERM_LOOKUP], 
		 std::ios::binary | std::ios::in | std::ios::out );
		 // try doing input only (separate file pointers for i/o?
		 // seems to help
		 //std::ios::binary | std::ios::in);
  dtlookup.seekg( 0, std::ios::beg );
  dtlookup.seekp( 0, std::ios::end );
  dtlookupReadBuffer = new ReadBuffer( dtlookup, 128*1024 );
  // try bigger. But why are there writes in solaris?
  //  dtlookupReadBuffer = new ReadBuffer( dtlookup, 32*1024*1024 );

  invlookup.open( names[DOC_LOOKUP], Keyfile::sequential );
  dIDs.open( names[DOC_IDS], Keyfile::random );
  dSTRs.open( names[DOC_IDSTRS], Keyfile::random );
  tIDs.open( names[TERM_IDS], Keyfile::random );
  tSTRs.open( names[TERM_IDSTRS], Keyfile::random );
}

void KeyfileIncIndex::createDBs() {
  dtlookup.open( names[TERM_LOOKUP], 
		 std::ios::binary | std::ios::out | std::ios::in ) ;
  dtlookup.seekg( 0, std::ios::beg );
  dtlookup.seekp( 0, std::ios::end );

  dtlookupReadBuffer = new ReadBuffer( dtlookup, 128*1024 );

  invlookup.create( names[DOC_LOOKUP], Keyfile::sequential );
  dIDs.create( names[DOC_IDS], Keyfile::random );
  dSTRs.create( names[DOC_IDSTRS], Keyfile::random );
  tIDs.create( names[TERM_IDS], Keyfile::random );
  tSTRs.create( names[TERM_IDSTRS], Keyfile::random );
}

bool KeyfileIncIndex::open(const char* indexName){
  // stupid
  String streamSelect = ParamGetString("stream", "cerr");
  if (streamSelect == "cout") {
    setMesgStream(&cout);
  } else {
    setMesgStream(&cerr);
  }

  counts = new int[5]; // should not be here.
  *msgstream << "Trying to open toc: " << indexName << endl;
  if (! ParamPushFile(indexName)) {
    *msgstream << "Couldn't open toc file for reading" << endl;
    return false;
  }
  fullToc();
  invertlists.resize(counts[UNIQUE_TERMS], 0 );
  _largestFlushedTermID = counts[UNIQUE_TERMS];

  std::string indexNameString(indexName);
  std::string prefix = indexNameString.substr( 0, indexNameString.rfind('.'));
  setName(const_cast<char*>(prefix.c_str()));

  openDBs();
  openSegments();

  std::stringstream docfname;
  docfname << name << DTINDEX;
  writetlist.open( docfname.str(), 
		   ios::in | ios::out | ios::binary | ios::ate );
  writetlist.seekg( 0, std::ios::beg );

  ParamPopFile();

  if (!docMgrIDs())
    return false;

  *msgstream << "Load index complete." << endl;
  return true;
}

int KeyfileIncIndex::term(const char* word){
  int id;

  id = _cache.find( word );
  
  if( id <= 0 ) {
    int actual = 0;
    bool success = tIDs.get( word, &id, actual, sizeof id );

    if( !success ) {
      id = 0;
    }
  }

  return id;
}

const char* KeyfileIncIndex::term(int termID) {
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]))
    return NULL;
  int actual = 0;
  tSTRs.get( termID, this->termKey, actual, sizeof this->termKey );
  termKey[actual] = 0;
  return this->termKey;
}

int KeyfileIncIndex::document(const char* docIDStr){
  int actual = 0;
  int documentID = 0;

  dIDs.get( docIDStr, &documentID, actual, sizeof documentID );
  //  assert( actual == sizeof documentID );

  return documentID;
}

const char* KeyfileIncIndex::document(int docID) {
  if ((docID < 0) || (docID > counts[DOCS]))
    return NULL;
  int actual = 0;
  dSTRs.get( docID, this->docKey, actual, sizeof this->docKey );
  this->docKey[actual] = 0;
  return this->docKey;
}

DocumentManager * KeyfileIncIndex::docManager(int docID) {
  if ((docID <= 0) || (docID > counts[DOCS]))
    return NULL;
  record r = fetchDocumentRecord( docID );
  int docManagerID = r.num;
  // if it never had one registered.
  if (docManagerID == -1) 
    return NULL;
  return docMgrs[docManagerID]; 
}

int KeyfileIncIndex::termCount(int termID) const {
  if ((termID <= 0) || (termID > counts[UNIQUE_TERMS]))
    return 0;

  TermData termData;
  int actual;
  bool success = invlookup.get( termID, (char*) &termData, actual, 
				   sizeof termData );

  if( success ) {
    return termData.totalCount;
  } else {
    return 0;
  }
}

float KeyfileIncIndex::docLengthAvg(){
  return aveDocLen;
}

int KeyfileIncIndex::docCount(int termID) {
  if ((termID <= 0) || (termID > counts[UNIQUE_TERMS]))
    return 0;

  TermData termData;
  int actual;
  bool success = invlookup.get( termID, (char*) &termData, actual, 
				   sizeof termData );

  if( success ) {
    return termData.documentCount;
  } else {
    return 0;
  }
}

int KeyfileIncIndex::docLength(int docID) const {
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return 0;
  }

  record r = fetchDocumentRecord( docID );
  return r.len;
}

int KeyfileIncIndex::totaldocLength(int docID) const {
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return 0;
  }
  record r = fetchDocumentRecord( docID );
  return r.totalLen;
}

int KeyfileIncIndex::docLengthCounted(int docID) {
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return 0;
  }
  InvFPTermList* tl = (InvFPTermList*) this->termInfoListSeq(docID);
  // if this fails for whatever reason
  if (!tl)
    return 0;
  int count = tl->termCount();
  delete tl;
  return count;
}

InvFPDocList* KeyfileIncIndex::internalDocInfoList(int termID) {
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]) ) {
    *msgstream << "Error:  Trying to get docInfoList for invalid termID" 
	       << endl;
    *msgstream << "TermID: " << termID << endl;
    return NULL;
  }
  if (termID == 0)
    return NULL;

  // we have to build the doc info list from the segments on disk
  int actual = 0;   
  TermData termData;

  bool success = invlookup.get( termID, (char**) &termData, actual, 
				   sizeof termData );
  int i;
  
  if( success ) {
    int missingSegments = (sizeof(TermData) - actual) / sizeof(SegmentOffset);
    int vectorSegments = KEYFILE_MAX_SEGMENTS - missingSegments;
    std::vector<InvFPDocList*> lists;  
    InvFPDocList* total = 0;
    
    for( i = 0; i < vectorSegments; i++ ) {
      char* buffer = new char[termData.segments[i].length];

      File* segment = _segments[ termData.segments[i].segment ];
      segment->seekg( termData.segments[i].offset, std::ifstream::beg );
      segment->read( buffer, termData.segments[i].length );

      if( !total ) {
        total = new InvFPDocList( (int*) buffer );
      } else {
        InvFPDocList* segmentList = new InvFPDocList( (int*) buffer );
        total->append( segmentList );
        delete segmentList;
      }

      delete[](buffer);
    }

    if( invertlists[termID-1] ) {
      // fold in any data that hasn't made it to disk yet
      total->append( invertlists[i] );
    }

    return total;  
  } else {
    return NULL;
  }
}

DocInfoList* KeyfileIncIndex::docInfoList(int termID){
  if ((termID <= 0) || (termID > counts[UNIQUE_TERMS]) ) {
    return NULL;
  }

  DocInfoList* result = internalDocInfoList(termID);
  
  if( !result ) {
    throw Exception( "KeyfileIncIndex::docInfoList", 
		     "Failed to retrieve docInfoList" );
  }

  return result;
}

TermInfoList* KeyfileIncIndex::termInfoList(int docID){
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return NULL;
  }
  //we have to count the terms to make it bag of words from sequence
  InvFPTermList* tlist = (InvFPTermList *)termInfoListSeq(docID);
  tlist->countTerms();
  return tlist;
}

TermInfoList* KeyfileIncIndex::termInfoListSeq(int docID){
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return NULL;
  }
  record r = fetchDocumentRecord( docID );
  writetlist.seekg( r.offset, std::fstream::beg );
  InvFPTermList* tlist = new InvFPTermList();
  if (!tlist->binReadC(writetlist)) {
    return NULL;
  } 
  return tlist;
}

/*=======================================================================
 * PRIVATE METHODS 
 =======================================================================*/
void KeyfileIncIndex::fullToc() {
  counts[DOCS] = ParamGetInt(NUMDOCS_PAR, 0);
  counts[TOTAL_TERMS] = ParamGetInt(NUMTERMS_PAR, 0);
  counts[UNIQUE_TERMS] = ParamGetInt(NUMUTERMS_PAR, 0);
  counts[DT_FILES] = ParamGetInt(NUMDT_PAR, 0);

  names.resize( NAMES_SIZE, std::string() );
  names[DOC_INDEX] = ParamGetString(INVINDEX_PAR);
  names[DOC_LOOKUP] = ParamGetString(INVLOOKUP_PAR);
  names[TERM_INDEX] = ParamGetString(DTINDEX_PAR);
  names[TERM_LOOKUP] = ParamGetString(DTLOOKUP_PAR);
  names[TERM_IDS] = ParamGetString(TERMIDMAP_PAR);
  names[DOC_IDS] = ParamGetString(DOCIDMAP_PAR);
  names[DOCMGR_IDS] = ParamGetString(DOCMGR_PAR);
  names[VERSION_NUM] = ParamGetString(VERSION_PAR);
  names[TERM_IDSTRS] = ParamGetString(TERMIDSTRMAP_PAR);
  names[DOC_IDSTRS] = ParamGetString(DOCIDSTRMAP_PAR);
  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];
}


bool KeyfileIncIndex::docMgrIDs() {
  FILE* in = fopen(names[DOCMGR_IDS].c_str(), "r");
  *msgstream << "Trying to open doc manager ids file: " 
	     << names[DOCMGR_IDS] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening doc manager ids \n");
    return false;
  }

  int ind, len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &ind, &len) != 2)
      continue;

    str = new char[len + 1]; 
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    str[len] = '\0';
    DocumentManager* dm = DocMgrManager::openDocMgr(str);
    docMgrs.push_back(dm);
    docmgrs.push_back(str);
  }
  fclose(in);
  return true;
}

void KeyfileIncIndex::setMesgStream(ostream * lemStream) {
  msgstream = lemStream;
}

void KeyfileIncIndex::setName(char* prefix) {
  name = prefix;
}

bool KeyfileIncIndex::beginDoc(DocumentProps* dp){
  if (dp == NULL)
    return false;

  counts[DOCS]++;
  int documentID = counts[DOCS];

  addDocumentLookup( documentID, dp->stringID() );
  return true;
}

void KeyfileIncIndex::_updateTermlist( InvFPDocList* curlist, int position ) {
  LocatedTerm lt;
  lt.loc = position;
  lt.term = curlist->termID();
  termlist.push_back(lt);
  listlengths++;
}

void KeyfileIncIndex::addKnownTerm( int termID, int position ) {
  InvFPDocList* curlist;

  curlist = invertlists[termID - 1];
  if (curlist == NULL) {
    // append it to old list when flushed
    // tds - removing term length; shouldn't really be there anyway
    curlist = new InvFPDocList(termID, 0);
    invertlists[termID-1] = curlist;
  }
  curlist->addLocation(counts[DOCS], position );
  _updateTermlist( curlist, position );
}

int KeyfileIncIndex::addUnknownTerm( InvFPTerm* term ) {
  // update unique word counter
  counts[UNIQUE_TERMS]++;
  int termID = counts[UNIQUE_TERMS];

  InvFPDocList* curlist = new InvFPDocList(termID, term->strLength());
  invertlists.push_back( curlist );  // term is added at index termID-1

  // store the term in the keyfiles (tIDs, tSTRs)
  addTermLookup( termID, term->spelling() );

  addKnownTerm( termID, term->position() );
  return termID;
}

int KeyfileIncIndex::addUncachedTerm( InvFPTerm* term ) {
  int tid;
  int actual = 0;
    
  if( tIDs.get( term->spelling(), &tid, actual, sizeof tid ) ) {
    addKnownTerm( tid, term->position() );
  } else {
    tid = addUnknownTerm( term );
  }

  return tid;
}

bool KeyfileIncIndex::addTerm(Term& t){
  InvFPTerm* term = static_cast< InvFPTerm* >(&t);
  assert( term->strLength() > 0 );
  assert( term->spelling() != NULL );

  int id = _cache.find( term->spelling() );

  if( id > 0 ) {
    addKnownTerm( id, term->position() );
  } else {
    id = addUncachedTerm( term );
    _cache.add( term->spelling(), id );
  }

  return true;
}

void KeyfileIncIndex::endDoc(DocumentProps* dp) {
  doendDoc(dp, curdocmgr);
}

void KeyfileIncIndex::endDoc(DocumentProps* dp, const char* mgr){
  doendDoc(dp, docMgrID(mgr));
}

void KeyfileIncIndex::endCollection(CollectionProps* cp){
  // flush everything in the cache
  lastWriteCache();
  // write list of document managers
  writeDocMgrIDs();
  //write out the main toc file
  writeTOC();
}

void KeyfileIncIndex::setDocManager (const char* mgrID) {
  curdocmgr = docMgrID(mgrID);
}

/*==========================================================================
 *  PRIVATE METHODS
 *==========================================================================*/
bool KeyfileIncIndex::tryOpen() {
  // open an existing index and load up all of the data structures.
  bool retval = false;
  std::string fname;
  fname = name + ".key"; // fix this.
  return open(fname.c_str());
}

void KeyfileIncIndex::writeTOC() {
  std::string fname = name + ".key";
  *msgstream << "Writing out main stats table" << std::endl;

  ofstream toc(fname.c_str());

  if (! toc.is_open()) {
    fprintf(stderr, "Could not open .toc file for writing.\n");
    return;
  }
  toc << VERSION_PAR << " = " << IND_VERSION << ";\n";
  toc << NUMDOCS_PAR << " = " << counts[DOCS] << ";\n";
  toc << NUMTERMS_PAR << " = " << counts[TOTAL_TERMS] << ";\n";
  toc << NUMUTERMS_PAR << " = " << counts[UNIQUE_TERMS] << ";\n";
  toc << AVEDOCLEN_PAR << " = " << counts[TOTAL_TERMS]/counts[DOCS] << ";\n";
  toc << INVINDEX_PAR << " = " << name << IVLINDEX << ";\n";
  toc << INVLOOKUP_PAR << " = " << name << INVLOOKUP << ";\n";
  toc << DTINDEX_PAR << " = " << name << DTINDEX << ";\n"; 
  toc << DTLOOKUP_PAR << " = " << name << DTLOOKUP << ";\n";
  toc << DOCIDMAP_PAR << " = " << name << DOCIDMAP << ";\n";
  toc << DOCIDSTRMAP_PAR << " = " << name << DOCIDSTRMAP << ";\n";
  toc << TERMIDMAP_PAR << " = " << name << TERMIDMAP << ";\n";
  toc << TERMIDSTRMAP_PAR << " = " << name << TERMIDSTRMAP << ";\n";
  toc << DOCMGR_PAR << " = " << name << DOCMGRMAP << ";\n";
  toc.close();
}

void KeyfileIncIndex::writeDocMgrIDs() {
  std::string dmname = name + DOCMGRMAP;
  std::ofstream dmID;

  dmID.open( dmname.c_str(), std::ofstream::out );
  for ( unsigned int i=0;i<docmgrs.size();i++ ) {
    dmID << i << " "
         << docmgrs[i].length() << " "
         << docmgrs[i] << " ";
  }
  dmID.close();
}

// -------------------------------------------------------------------------------------
//
// Inverted lists cache management
//
// -------------------------------------------------------------------------------------

void KeyfileIncIndex::writeCache( bool finalRun ) {
  writeCacheSegment();

  if( _segments.size() >= KEYFILE_MAX_SEGMENTS || 
      (finalRun && _segments.size() > 1) ) {
    mergeCacheSegments();
  }
}

void KeyfileIncIndex::lastWriteCache() {
  listlengths = -1;
  writeCache( true );
}

int KeyfileIncIndex::_cacheSize() {
  int totalSize = 0;

  for( unsigned int i=0; i<invertlists.size(); i++ ) {
    InvFPDocList* list = invertlists[i];

    if( list ) {
      totalSize += (sizeof(InvFPDocList) + list->memorySize());
    }
  }

  return totalSize;
}

void KeyfileIncIndex::writeCacheSegment() {
  std::stringstream name;
  File outFile;

  // name of the file is <indexName>.ivl<segmentNumber>
  name << names[DOC_INDEX] << _segments.size();

  // open an output stream, and wrap it in a file buffer that
  // gives us a megabyte of output cache -- seems to make writing
  // faster with these very small writes (system call overhead?)
  outFile.open( name.str().c_str(), 
		std::ofstream::out | std::ofstream::binary );
  WriteBuffer out(outFile, KEYFILE_WRITEBUFFER_SIZE);

  int writes = 0;
  int totalSegments = _segments.size();

  for( int i = 0; i < counts[UNIQUE_TERMS]; i++ ) {
    InvFPDocList* list = invertlists[i];
  
    if( list != NULL ) {
      int vecLen;
      std::auto_ptr<int> byteVec( list->byteVec(vecLen) );

      TermData termData;

      termData.documentCount = 0;
      termData.totalCount = 0;

      // update term statistics
      int actual = 0;
      if( list->termID() <= _largestFlushedTermID ) {         
        invlookup.get( list->termID(), &termData, actual, sizeof termData);
      }

      termData.documentCount += list->docFreq();
      termData.totalCount += list->termCTF();

      // we need to add the location of the new inverted list
      int missingSegments = std::min<unsigned>( (sizeof(TermData) - actual) / 
						sizeof(SegmentOffset), 
						KEYFILE_MAX_SEGMENTS);
      int termSegments = KEYFILE_MAX_SEGMENTS - missingSegments;

      termData.segments[ termSegments ].segment = totalSegments;
      termData.segments[ termSegments ].offset = out.tellp();
      termData.segments[ termSegments ].length = vecLen;

      invlookup.put( list->termID(), &termData, 
		     sizeof(TermData) - 
		     sizeof(SegmentOffset)*(missingSegments-1) );
      out.write( (char*) byteVec.get(), vecLen );
      
      delete list;
      invertlists[i] = NULL;

      writes++;
    }
  }

  // flush the file buffer and close the segment stream
  out.flush();
  outFile.close();

  // reopen the output file as an input stream for queries:
  File* in = new File();
  in->open( name.str().c_str(), std::ifstream::in | std::ifstream::binary );
  _segments.push_back( in );

  // record the largest flushed termID
  _largestFlushedTermID = counts[UNIQUE_TERMS];
}


class reader_less {
public:
  bool operator () ( const KeyfileDocListSegmentReader* one, 
		     const KeyfileDocListSegmentReader* two ) {
    return (*one) < (*two);
  }
};

void KeyfileIncIndex::mergeCacheSegments() {
  unsigned int firstMergeSegment = 0;

  if( firstMergeSegment == 0 ) {
    invlookup.close();
    // g++ doesn't like this
    //    ::unlink( names[DOC_LOOKUP].c_str() );
    std::remove( names[DOC_LOOKUP].c_str() );
    invlookup.create( names[DOC_LOOKUP].c_str(), Keyfile::sequential );
  }

  std::priority_queue<KeyfileDocListSegmentReader*,
    std::vector<KeyfileDocListSegmentReader*>,reader_less> readers;
  File outFile;
  std::vector<std::string> listNames;

  std::stringstream oldName;
  oldName << names[DOC_INDEX] << _segments.size();
  outFile.open( oldName.str(), std::ofstream::out | std::ofstream::binary );
  WriteBuffer out(outFile, KEYFILE_WRITEBUFFER_SIZE);  

  string nameToOpen(names[DOC_INDEX]);
  File * fileToOpen;
  // open segments
  for( unsigned int i = firstMergeSegment; i < _segments.size(); i++ ) {
    //    g++ doesn't like this.
    //    KeyfileDocListSegmentReader* reader =  new KeyfileDocListSegmentReader( _segments[i], std::string(names[DOC_INDEX]), i, KEYFILE_DOCLISTREADER_SIZE );
    fileToOpen =  _segments[i];
    KeyfileDocListSegmentReader* reader =  
      new KeyfileDocListSegmentReader(fileToOpen, nameToOpen, i, 
				      KEYFILE_DOCLISTREADER_SIZE );
    reader->pop(); // read the first list into memory
    readers.push( reader );
  }

  while( readers.size() ) {
    // find the minimum term id
    KeyfileDocListSegmentReader* reader = readers.top();
    InvFPDocList* list = reader->top();
    int termID = list->termID();

    reader->pop();
    // reindex the reader
    readers.pop();
    if( reader->top() ) {
      readers.push(reader);
    } else {
      reader->file()->unlink();
      delete reader->file();
      delete reader;
    }

    // now, find all other readers with similar term lists
    while( readers.size() && readers.top()->top()->termID() == termID ) {
      reader = readers.top();

      // fetch the list, append it to the current list
      list->append( reader->top() );
      delete reader->top();
      reader->pop();

      // re-index the reader
      readers.pop();
      if( reader->top() ) {
        readers.push(reader);
      } else {
        reader->file()->unlink();
        delete reader->file();
        delete reader;
      }
    }

    TermData termData;

    termData.totalCount = list->termCTF();
    termData.documentCount = list->docFreq();

    // now the list is complete, so write it out
    int listLength;
    int* listBuffer = list->byteVec( listLength );
    File::offset_type offset = out.tellp();
    out.write( (char*) listBuffer, listLength );
    delete[](listBuffer);
    delete list;

    // fetch the offset data from the lookup structure
    int actual = 0;

    if( firstMergeSegment > 0 ) {
      invlookup.get( termID, &termData, actual, sizeof(TermData) );
    }

    // we need to add the location of the new inverted list
    int missingSegments = std::min<unsigned>( (sizeof(TermData) - actual) / sizeof(SegmentOffset), KEYFILE_MAX_SEGMENTS);
    int termSegments = KEYFILE_MAX_SEGMENTS - missingSegments;

    // find the first segment offset that was merged in this pass
    int segIndex;
    for( segIndex = 0; segIndex < termSegments; segIndex++ ) {
      if( termData.segments[segIndex].segment >= firstMergeSegment )
        break;
    }

    termData.segments[segIndex].segment = firstMergeSegment;
    termData.segments[segIndex].offset = offset;
    termData.segments[segIndex].length = listLength;

    invlookup.put( termID, &termData, sizeof(TermData) - sizeof(SegmentOffset)*(KEYFILE_MAX_SEGMENTS-(segIndex+1)) );
  }

  out.flush();
  File::offset_type length = outFile.size();
  outFile.close();
  
  // all the old files are deleted now, so we can rename the 
  // merged segment to be segment <firstMergeSegment>.
  std::stringstream newName;
  newName << names[DOC_INDEX] << firstMergeSegment;
  File::rename( oldName.str(), newName.str() );

  // clear out the _segment vector (all the ifstreams have
  // already been deleted)
  _segments.erase( _segments.begin() + firstMergeSegment, _segments.end() );
  
  // open the final segment again for reading
  File* in = new File();
  in->open( newName.str(), std::ifstream::in | std::ifstream::binary );
  _segments.push_back( in );
}

// -----------------------------------------------------------------------------
//
// Document management
//
// -----------------------------------------------------------------------------

int KeyfileIncIndex::docMgrID(const char* mgr) {
  std::string mgrString( mgr );
  std::vector<std::string>::iterator iter;

  iter = std::find( docmgrs.begin(), docmgrs.end(), mgrString );

  if( iter != docmgrs.end() ) {
    return iter - docmgrs.begin();
  } else {
    docmgrs.push_back( mgrString );
    // There's an issue here if we pass in the name
    // but haven't specified a type...
    // type should come in as a parameter (or be parsed out of name...
    // or try open.
    // FIXME!!! dmf
    DocumentManager *dm = DocMgrManager::createDocMgr("fixme", mgrString);
    docMgrs.push_back(dm);
    return docmgrs.size()-1;
  }
}

void KeyfileIncIndex::doendDoc(DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    int len = dp->length();
    int tls = termlist.size();

    // make sure the ftell is correct
    long offset = (long)writetlist.tellp();
    
    record rec;
    rec.offset = writetlist.tellp();
    rec.len = len;
    rec.num = mgrid;

    int lastPos = 0;
    
    if( termlist.size() > 0 ) {
      lastPos = termlist[termlist.size() - 1].loc;
      rec.totalLen = lastPos + 1;
    } else {
      rec.totalLen = 0;
    }
    
    int docID = counts[DOCS];

    dtlookup.write( (char*) &rec, sizeof rec );

    InvFPTermList *tlist = new InvFPTermList(docID, len, termlist);
    tlist->binWriteC(writetlist);
    delete tlist;
    counts[TOTAL_TERMS] += len;
  }
  termlist.clear();

  bool shouldWriteCache = (listlengths == -1);

  if( listlengths > _estimatePoint ) {
      int cacheSize = _cacheSize();
      float bytesPerPosting = (float)cacheSize / (float)listlengths;
      _estimatePoint = (int) (_listsSize / bytesPerPosting);

      // if we're within a few postings of an optimal flush point, just do it now
      shouldWriteCache = _estimatePoint < (listlengths + KEYFILE_EPSILON_FLUSH_POSTINGS);
  }

  if( shouldWriteCache ) {
    writeCache();
    listlengths = 0;
    _resetEstimatePoint();
  }
}

// ----------------------------------------------------------------------------------
//
// Index helper methods
//
// ----------------------------------------------------------------------------------

KeyfileIncIndex::record KeyfileIncIndex::fetchDocumentRecord(int key) const {
  if (key == 0)
    return record();

  record rec;

  dtlookupReadBuffer->seekg( (key-1)*sizeof(rec), std::fstream::beg );
  dtlookupReadBuffer->read( (char*) &rec, sizeof rec );

  return rec;
}

void KeyfileIncIndex::addDocumentLookup( int documentKey, 
					 const char* documentName ) {
  addGeneralLookup( dSTRs, dIDs, documentKey, documentName );
}

void KeyfileIncIndex::addTermLookup( int termKey, const char* termSpelling ) {
  addGeneralLookup( tSTRs, tIDs, termKey, termSpelling );
}

void KeyfileIncIndex::addGeneralLookup( Keyfile& numberNameIndex, 
					Keyfile& nameNumberIndex, 
					int number, const char* name ) {
  numberNameIndex.put( number, (void*) name, strlen(name) );
  nameNumberIndex.put( name, &number, sizeof number );
}