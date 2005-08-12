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
#include "minmax.hpp"
#include "indri/Path.hpp"
#include <assert.h>
#include <functional>
#include <algorithm>
#include <sstream>

// Keyfiles get 1mb each, and there are 6 of them
// dtlookup gets 10MB read buffer
// 16MB goes to the TermCache
// 1MB for the output buffer at flush time
// 1MB for general data structures
// 4MB for the invlists vector (4 bytes * 1M terms)
// 10MB for heap overhead, general breathing room
// Total base RAM usage: 50MB

#define KEYFILE_BASE_MEMORY_USAGE       (50*1024*1024)
#define KEYFILE_WRITEBUFFER_SIZE           (1024*1024)
#define KEYFILE_DOCLISTREADER_SIZE         (1024*1024)

#define KEYFILE_MINIMUM_LISTS_SIZE          (512*1024)
#define KEYFILE_EPSILON_FLUSH_POSTINGS      (512*1024)

// avoid conflicts with other index include files.
// do something about these defines. Ought to be enums.

#define NAMES_SIZE   11
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
#define PROPS_FILE   10 

#define IVLINDEX  ".ivl"
#define COLPROPS ".cps"
#define COLPROPS_PAR "COLLECTION_PROPS"

lemur::index::KeyfileIncIndex::KeyfileIncIndex(const string &prefix, int cachesize, 
                                               lemur::api::DOCID_T startdocid) {
  ignoreDoc = false;
  listlengths = 0;
  cprops = NULL;
  setName(prefix);
  _readOnly = false;
  _computeMemoryBounds( cachesize );

  if (!tryOpen()) {
    // brand new empty index
    *msgstream << "Creating new index" << endl;    
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

lemur::index::KeyfileIncIndex::KeyfileIncIndex() {
  ignoreDoc = false;
  listlengths = 0;
  curdocmgr = -1;
  _readOnly = true;
  _computeMemoryBounds( 128 * 1024 * 1024 );
  _largestFlushedTermID = 0;
  aveDocLen = 0;
  cprops = NULL;
}

lemur::index::KeyfileIncIndex::~KeyfileIncIndex() {
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
  delete cprops;
  delete[](counts);
}

void lemur::index::KeyfileIncIndex::_resetEstimatePoint() {
  // how many postings should we queue up before
  // trying to estimate a flush time?
  // pick the worst case--one unique term per document
  // that would give us one InvFPDocList plus a few bytes
  // for the list buffer:

  _estimatePoint = _listsSize / (sizeof(InvFPDocList) + 16);
}

void lemur::index::KeyfileIncIndex::_computeMemoryBounds( int memorySize ) {
  // The following is fixed allocation--in the future
  // we may want to try something parametric:
  _memorySize = memorySize;
  _listsSize = memorySize - KEYFILE_BASE_MEMORY_USAGE;

  if( _listsSize < KEYFILE_MINIMUM_LISTS_SIZE ) {
    _listsSize = KEYFILE_MINIMUM_LISTS_SIZE;
  }

  _resetEstimatePoint();
}

void lemur::index::KeyfileIncIndex::openSegments() {
  // open existing inverted list segments
  for( int i=0; i<KEYFILE_MAX_SEGMENTS; i++ ) {
    lemur::file::File* in = new lemur::file::File();
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

void lemur::index::KeyfileIncIndex::openDBs() {
  if (_readOnly) {
    dtlookup.open( names[TERM_LOOKUP], std::ios::binary | std::ios::in );
  } else {
    dtlookup.open( names[TERM_LOOKUP], 
                   std::ios::binary | std::ios::in | std::ios::out );
    dtlookup.seekp( 0, std::ios::end );
  }
  dtlookup.seekg( 0, std::ios::beg );
  dtlookupReadBuffer = new lemur::file::ReadBuffer( dtlookup, 10*1024*1024 );
  int cacheSize = 1024 * 1024;
  invlookup.open( names[DOC_LOOKUP], cacheSize, _readOnly );
  dIDs.open( names[DOC_IDS], cacheSize, _readOnly  );
  dSTRs.open( names[DOC_IDSTRS], cacheSize, _readOnly  );
  tIDs.open( names[TERM_IDS], cacheSize, _readOnly  );
  tSTRs.open( names[TERM_IDSTRS], cacheSize, _readOnly  );
}

void lemur::index::KeyfileIncIndex::createDBs() {
  dtlookup.open( names[TERM_LOOKUP], 
                 std::ios::binary | std::ios::out | std::ios::in ) ;
  dtlookup.seekg( 0, std::ios::beg );
  dtlookup.seekp( 0, std::ios::end );

  dtlookupReadBuffer = new lemur::file::ReadBuffer( dtlookup, 10*1024*1024 );
  invlookup.create( names[DOC_LOOKUP] );
  dIDs.create( names[DOC_IDS] );
  dSTRs.create( names[DOC_IDSTRS] );
  tIDs.create( names[TERM_IDS] );
  tSTRs.create( names[TERM_IDSTRS] );
}

bool lemur::index::KeyfileIncIndex::open(const string &indexName){
  // stupid
  lemur::utility::String streamSelect = lemur::api::ParamGetString("stream", "cerr");
  if (streamSelect == "cout") {
    setMesgStream(&cout);
  } else {
    setMesgStream(&cerr);
  }

  counts = new lemur::api::COUNT_T[5]; // should not be here.
  *msgstream << "Trying to open toc: " << indexName << endl;
  if (! indri::file::Path::exists(indexName) || ! lemur::api::ParamPushFile(indexName)) {
    *msgstream << "Couldn't open toc file for reading" << endl;
    return false;
  }
  fullToc();
  invertlists.resize(counts[UNIQUE_TERMS], 0 );
  _largestFlushedTermID = counts[UNIQUE_TERMS];

  std::string prefix = indexName.substr( 0, indexName.rfind('.'));
  setName(prefix);

  openDBs();
  openSegments();

  std::stringstream docfname;
  docfname << name << DTINDEX;
  if (_readOnly)
    writetlist.open( docfname.str(), ios::in | ios::binary );
  else 
    writetlist.open( docfname.str(), 
                     ios::in | ios::out | ios::binary | ios::ate );
  writetlist.seekg( 0, std::ios::beg );

  lemur::api::ParamPopFile();

  if (!docMgrIDs())
    return false;

  *msgstream << "Load index complete." << endl;
  return true;
}
lemur::api::TERMID_T lemur::index::KeyfileIncIndex::term(const lemur::api::TERM_T &inWord) const{
  const char* word = inWord.c_str();

  if (inWord.length() > (MAX_TERM_LENGTH - 1)) {
    //term is too big to be a key, so it is OOV.
    return 0;
  }

  lemur::api::TERMID_T id = _cache.find( word );

  if( id <= 0 ) {
    int actual = 0;
    bool success = tIDs.get( word, &id, actual, sizeof id );

    if( !success ) {
      id = 0;
    }
  }

  return id;
}

const lemur::api::TERM_T lemur::index::KeyfileIncIndex::term(lemur::api::TERMID_T termID)  const{
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]))
    return "";
  int actual = 0;
  tSTRs.get( termID, this->termKey, actual, sizeof this->termKey );
  termKey[actual] = 0;
  return this->termKey;
}

lemur::api::DOCID_T lemur::index::KeyfileIncIndex::document(const lemur::api::EXDOCID_T &docID) const{
  int actual = 0;
  lemur::api::DOCID_T documentID = 0;

  const char *did = docID.c_str();

  int len = docID.length();
  if (len > (MAX_DOCID_LENGTH - 1)) {
    cerr << "lemur::index::KeyfileIncIndex::document: document id " << did 
         << " is too long ("
         << len << " > " << (MAX_DOCID_LENGTH - 1) << ")" << endl;
    strncpy(docKey, did, (MAX_DOCID_LENGTH - 1));
    docKey[(MAX_DOCID_LENGTH - 1)] = '\0';
    did = docKey;
    cerr << "truncating to " << docKey << endl;
  }

  dIDs.get( did, &documentID, actual, sizeof documentID );
  //  assert( actual == sizeof documentID );

  return documentID;
}

const lemur::api::EXDOCID_T lemur::index::KeyfileIncIndex::document(lemur::api::DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS]))
    return "";
  int actual = 0;
  dSTRs.get( docID, this->docKey, actual, sizeof this->docKey );
  this->docKey[actual] = 0;
  return this->docKey;
}

const lemur::api::DocumentManager * lemur::index::KeyfileIncIndex::docManager(lemur::api::DOCID_T docID) const{
  if ((docID <= 0) || (docID > counts[DOCS]))
    return NULL;
  record r = fetchDocumentRecord( docID );
  int docManagerID = r.num;
  //  cerr << "DOCManagerID " << docManagerID << endl;
  // if it never had one registered.
  if (docManagerID == -1) 
    return NULL;
  return docMgrs[docManagerID]; 
}

const lemur::parse::CollectionProps *lemur::index::KeyfileIncIndex::collectionProps() const {
  // if already loaded, return list
  if (cprops)
    return cprops;

  // if not loaded, try to load
  lemur::parse::Property p;
  string key, val;
  ifstream rf(names[PROPS_FILE].c_str());
  if (!rf.is_open()) 
    return NULL;

  cprops = new lemur::parse::BasicCollectionProps();
  std::string line;
  while (getline(rf, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      key = line.substr(0, space);
      val = line.substr(space + 1);
      p.setName(key);
      p.setValue(val.c_str());
      cprops->setProperty(&p);
    }
  }
  rf.close();
  return cprops;
}

lemur::api::COUNT_T lemur::index::KeyfileIncIndex::termCount(lemur::api::TERMID_T termID) const {
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

float lemur::index::KeyfileIncIndex::docLengthAvg() const{
  return aveDocLen;
}

lemur::api::COUNT_T lemur::index::KeyfileIncIndex::docCount(lemur::api::TERMID_T termID) const {
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

lemur::api::COUNT_T lemur::index::KeyfileIncIndex::docLength(lemur::api::DOCID_T docID) const {
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return 0;
  }

  record r = fetchDocumentRecord( docID );
  return r.len;
}

lemur::api::COUNT_T lemur::index::KeyfileIncIndex::totaldocLength(lemur::api::DOCID_T docID) const {
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return 0;
  }
  record r = fetchDocumentRecord( docID );
  return r.totalLen;
}

lemur::api::COUNT_T lemur::index::KeyfileIncIndex::docLengthCounted(lemur::api::DOCID_T docID) const {
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return 0;
  }
  InvFPTermList* tl = (InvFPTermList*) this->termInfoListSeq(docID);
  // if this fails for whatever reason
  if (!tl)
    return 0;
  lemur::api::COUNT_T count = tl->termCount();
  delete tl;
  return count;
}

lemur::index::InvFPDocList* lemur::index::KeyfileIncIndex::internalDocInfoList(lemur::api::TERMID_T termID) const{
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

      lemur::file::File* segment = _segments[ termData.segments[i].segment ];
      segment->seekg( termData.segments[i].offset, std::ifstream::beg );
      segment->read( buffer, termData.segments[i].length );

      if( !total ) {
        total = new InvFPDocList( (lemur::api::LOC_T*) buffer );
      } else {
        InvFPDocList* segmentList = new InvFPDocList( (lemur::api::LOC_T*) buffer );
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

lemur::api::DocInfoList* lemur::index::KeyfileIncIndex::docInfoList(lemur::api::TERMID_T termID) const{
  if ((termID <= 0) || (termID > counts[UNIQUE_TERMS]) ) {
    return NULL;
  }

  lemur::api::DocInfoList* result = internalDocInfoList(termID);
  
  if( !result ) {
    throw lemur::api::Exception( "lemur::index::KeyfileIncIndex::docInfoList", 
                                 "Failed to retrieve docInfoList" );
  }

  return result;
}

lemur::api::TermInfoList* lemur::index::KeyfileIncIndex::termInfoList(lemur::api::DOCID_T docID) const{
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return NULL;
  }
  //we have to count the terms to make it bag of words from sequence
  InvFPTermList* tlist = (InvFPTermList *)termInfoListSeq(docID);
  tlist->countTerms();
  return tlist;
}

lemur::api::TermInfoList* lemur::index::KeyfileIncIndex::termInfoListSeq(lemur::api::DOCID_T docID) const{
  if ((docID <= 0) || (docID > counts[DOCS])) {
    return NULL;
  }
  record r = fetchDocumentRecord( docID );
  // shouldn't really be re-using this varname like this
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
void lemur::index::KeyfileIncIndex::fullToc() {
  counts[DOCS] = lemur::api::ParamGetInt(NUMDOCS_PAR, 0);
  counts[TOTAL_TERMS] = lemur::api::ParamGetInt(NUMTERMS_PAR, 0);
  counts[UNIQUE_TERMS] = lemur::api::ParamGetInt(NUMUTERMS_PAR, 0);
  counts[DT_FILES] = lemur::api::ParamGetInt(NUMDT_PAR, 0);

  names.resize( NAMES_SIZE, std::string() );
  names[DOC_INDEX] = lemur::api::ParamGetString(INVINDEX_PAR);
  names[DOC_LOOKUP] = lemur::api::ParamGetString(INVLOOKUP_PAR);
  names[TERM_INDEX] = lemur::api::ParamGetString(DTINDEX_PAR);
  names[TERM_LOOKUP] = lemur::api::ParamGetString(DTLOOKUP_PAR);
  names[TERM_IDS] = lemur::api::ParamGetString(TERMIDMAP_PAR);
  names[DOC_IDS] = lemur::api::ParamGetString(DOCIDMAP_PAR);
  names[DOCMGR_IDS] = lemur::api::ParamGetString(DOCMGR_PAR);
  names[VERSION_NUM] = lemur::api::ParamGetString(VERSION_PAR);
  names[TERM_IDSTRS] = lemur::api::ParamGetString(TERMIDSTRMAP_PAR);
  names[DOC_IDSTRS] = lemur::api::ParamGetString(DOCIDSTRMAP_PAR);
  names[PROPS_FILE] = lemur::api::ParamGetString(COLPROPS_PAR);
  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];
}


bool lemur::index::KeyfileIncIndex::docMgrIDs() {
  ifstream in (names[DOCMGR_IDS].c_str());
  *msgstream << "Trying to open doc manager ids file: " 
             << names[DOCMGR_IDS] << endl;
  if (!in.is_open()) {
    *msgstream << "Error opening doc manager ids" << std::endl;
    return false;
  }
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      //  numbers are ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      lemur::api::DocumentManager* dm = lemur::api::DocMgrManager::openDocMgr(value, _readOnly);
      docMgrs.push_back(dm);
      docmgrs.push_back(value);
    }
  }
  in.close();
  return true;
}

void lemur::index::KeyfileIncIndex::setMesgStream(ostream * lemStream) {
  msgstream = lemStream;
}

void lemur::index::KeyfileIncIndex::setName(const string &prefix) {
  name = prefix;
}

bool lemur::index::KeyfileIncIndex::beginDoc(const lemur::parse::DocumentProps* dp){
  if (dp == NULL)
    return false;
  const char *did = dp->stringID();
  lemur::api::DOCID_T docID = document(did);
  if (docID > 0) {
    //already have seen this document's id.
    cerr << "lemur::index::KeyfileIncIndex::beginDoc: duplicate document id " << did 
         << ". Document will be ignored." << endl;
    ignoreDoc = true;
    return false;
  }
  
  int len = strlen(did);
  if (len > (MAX_DOCID_LENGTH - 1)) {
    cerr << "lemur::index::KeyfileIncIndex::beginDoc: document id " << did 
         << " is too long ("
         << len << " > " << (MAX_DOCID_LENGTH - 1) << ")" << endl;
    strncpy(docKey, did, (MAX_DOCID_LENGTH - 1));
    docKey[(MAX_DOCID_LENGTH - 1)] = '\0';
    did = docKey;
    cerr << "truncating to " << docKey << endl;
  }
  counts[DOCS]++;
  lemur::api::DOCID_T documentID = counts[DOCS];
  addDocumentLookup( documentID, did);
  return true;
}

void lemur::index::KeyfileIncIndex::_updateTermlist( InvFPDocList* curlist, lemur::api::LOC_T position ) {
  LocatedTerm lt;
  lt.loc = position;
  lt.term = curlist->termID();
  termlist.push_back(lt);
  listlengths++;
}

void lemur::index::KeyfileIncIndex::addKnownTerm( lemur::api::TERMID_T termID, lemur::api::LOC_T position ) {
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

lemur::api::TERMID_T lemur::index::KeyfileIncIndex::addUnknownTerm( const InvFPTerm* term ) {
  // update unique word counter
  counts[UNIQUE_TERMS]++;
  lemur::api::TERMID_T termID = counts[UNIQUE_TERMS];

  InvFPDocList* curlist = new InvFPDocList(termID, term->strLength());
  invertlists.push_back( curlist );  // term is added at index termID-1

  // store the term in the keyfiles (tIDs, tSTRs)
  addTermLookup( termID, term->spelling() );

  addKnownTerm( termID, term->position() );
  return termID;
}

lemur::api::TERMID_T lemur::index::KeyfileIncIndex::addUncachedTerm( const InvFPTerm* term ) {
  lemur::api::TERMID_T tid;
  int actual = 0;
    
  if( tIDs.get( term->spelling(), &tid, actual, sizeof tid ) ) {
    addKnownTerm( tid, term->position() );
  } else {
    tid = addUnknownTerm( term );
  }

  return tid;
}

bool lemur::index::KeyfileIncIndex::addTerm(const lemur::api::Term& t){
  if (ignoreDoc) return false; // skipping this document.
  const InvFPTerm* term = dynamic_cast< const InvFPTerm* >(&t);
  assert( term->strLength() > 0 );
  assert( term->spelling() != NULL );
  int len = term->strLength();
  if (len > (MAX_TERM_LENGTH - 1)) {
    //term is too big to be a key.
    cerr << "lemur::index::KeyfileIncIndex::addTerm: ignoring " << term->spelling()
         << " which is too long ("
         << len << " > " << (MAX_TERM_LENGTH - 1) << ")" << endl;
    return false;
  }

  lemur::api::TERMID_T id = _cache.find( term->spelling() );

  if( id > 0 ) {
    addKnownTerm( id, term->position() );
  } else {
    id = addUncachedTerm( term );
    _cache.add( term->spelling(), id );
  }

  return true;
}

void lemur::index::KeyfileIncIndex::endDoc(const lemur::parse::DocumentProps* dp) {
  if (! ignoreDoc) doendDoc(dp, curdocmgr);
  ignoreDoc = false;
}

void lemur::index::KeyfileIncIndex::endDoc(const lemur::parse::DocumentProps* dp, const string &mgr){
  if (! ignoreDoc) doendDoc(dp, docMgrID(mgr));
  ignoreDoc = false;
}

void lemur::index::KeyfileIncIndex::endCollection(const lemur::parse::CollectionProps* cp){
  // flush everything in the cache
  if (_readOnly) return;
  lastWriteCache();
  // write list of document managers
  writeDocMgrIDs();
  //write out the main toc file
  writeTOC(cp);
}

void lemur::index::KeyfileIncIndex::setDocManager (const string &mgrID) {
  curdocmgr = docMgrID(mgrID);
}

/*==========================================================================
 *  PRIVATE METHODS
 *==========================================================================*/
bool lemur::index::KeyfileIncIndex::tryOpen() {
  // open an existing index and load up all of the data structures.
  std::string fname;
  fname = name + ".key"; // fix this.
  return open(fname);
}

void lemur::index::KeyfileIncIndex::writeTOC(const lemur::parse::CollectionProps* cp) {
  // change format to match indri parameters xml format
  std::string fname = name + ".key";
  *msgstream << "Writing out main stats table" << std::endl;

  ofstream toc(fname.c_str());

  if (! toc.is_open()) {
    fprintf(stderr, "Could not open .toc file for writing.\n");
    return;
  }
  toc << "<parameters>\n";
  toc << "<" << VERSION_PAR << ">" << IND_VERSION << "</" << VERSION_PAR << ">\n";
  toc << "<" << NUMDOCS_PAR << ">" << counts[DOCS] << "</" << NUMDOCS_PAR <<">\n";
  toc << "<" << NUMTERMS_PAR << ">" << counts[TOTAL_TERMS] << "</" << NUMTERMS_PAR<< ">\n";
  toc << "<" << NUMUTERMS_PAR << ">" << counts[UNIQUE_TERMS] << "</" << NUMUTERMS_PAR<< ">\n";
  toc << "<" << AVEDOCLEN_PAR << ">" << counts[TOTAL_TERMS]/counts[DOCS] << "</" << AVEDOCLEN_PAR << ">\n";
  toc << "<" << INVINDEX_PAR << ">" << name << IVLINDEX << "</" << INVINDEX_PAR<< ">\n";
  toc << "<" << INVLOOKUP_PAR << ">" << name << INVLOOKUP << "</" << INVLOOKUP_PAR << ">\n";
  toc << "<" << DTINDEX_PAR << ">" << name << DTINDEX << "</" << DTINDEX_PAR << ">\n"; 
  toc << "<" << DTLOOKUP_PAR << ">" << name << DTLOOKUP << "</" << DTLOOKUP_PAR << ">\n";
  toc << "<" << DOCIDMAP_PAR << ">" << name << DOCIDMAP << "</" << DOCIDMAP_PAR << ">\n";
  toc << "<" << DOCIDSTRMAP_PAR << ">" << name << DOCIDSTRMAP << "</" << DOCIDSTRMAP_PAR << ">\n";
  toc << "<" << TERMIDMAP_PAR << ">" << name << TERMIDMAP << "</" << TERMIDMAP_PAR << ">\n";
  toc << "<" << TERMIDSTRMAP_PAR << ">" << name << TERMIDSTRMAP << "</" << TERMIDSTRMAP_PAR << ">\n";
  toc << "<" << DOCMGR_PAR << ">" << name << DOCMGRMAP << "</" << DOCMGR_PAR << ">\n";
  toc << "<" << COLPROPS_PAR << ">" << name << COLPROPS << "</" << COLPROPS_PAR << ">" <<"\n";
  toc << "</parameters>\n";
  toc.close();

  const lemur::parse::BasicCollectionProps* props = dynamic_cast<const lemur::parse::BasicCollectionProps*>(cp);

  if (props) {
    fname = name + COLPROPS;
    ofstream pf(fname.c_str());
    
    if (! pf.is_open()) {
      fprintf(stderr, "Could not save collection properties information\n");
      return;
    }
    
    const lemur::parse::Property* p = NULL;
    string value;
    props->startIteration();
    while (props->hasMore()) {
      p = props->nextEntry();
      if (p->getType() == lemur::parse::Property::STDSTRING)
        pf << p->getName() << " " << *(string*)p->getValue() << endl;
      else if (p->getType() == lemur::parse::Property::STRING)
        pf << p->getName() << " " << (char*)p->getValue() << endl;
    }
    
    pf.close();
  }
}

void lemur::index::KeyfileIncIndex::writeDocMgrIDs() {
  std::string dmname = name + DOCMGRMAP;
  std::ofstream dmID;

  dmID.open( dmname.c_str(), std::ofstream::out );
  for ( unsigned int i=0;i<docmgrs.size();i++ ) {
    dmID << i << " "
         << docmgrs[i].length() << " "
         << docmgrs[i] << std::endl;
  }
  dmID.close();
}

// -------------------------------------------------------------------------------------
//
// Inverted lists cache management
//
// -------------------------------------------------------------------------------------

void lemur::index::KeyfileIncIndex::writeCache( bool finalRun ) {
  writeCacheSegment();

  if( _segments.size() >= KEYFILE_MAX_SEGMENTS || 
      (finalRun && _segments.size() > 1) ) {
    mergeCacheSegments();
  }
}

void lemur::index::KeyfileIncIndex::lastWriteCache() {
  listlengths = -1;
  writeCache( true );
}

int lemur::index::KeyfileIncIndex::_cacheSize() {
  int totalSize = 0;

  for( unsigned int i=0; i<invertlists.size(); i++ ) {
    InvFPDocList* list = invertlists[i];

    if( list ) {
      totalSize += (sizeof(InvFPDocList) + list->memorySize());
    }
  }

  return totalSize;
}

void lemur::index::KeyfileIncIndex::writeCacheSegment() {
  std::stringstream name;
  lemur::file::File outFile;

  // name of the file is <indexName>.ivl<segmentNumber>
  name << names[DOC_INDEX] << _segments.size();

  // open an output stream, and wrap it in a file buffer that
  // gives us a megabyte of output cache -- seems to make writing
  // faster with these very small writes (system call overhead?)
  outFile.open( name.str().c_str(), 
                std::ofstream::out | std::ofstream::binary );
  lemur::file::WriteBuffer out(outFile, KEYFILE_WRITEBUFFER_SIZE);

  int writes = 0;
  int totalSegments = _segments.size();

  for( int i = 0; i < counts[UNIQUE_TERMS]; i++ ) {
    InvFPDocList* list = invertlists[i];
  
    if( list != NULL ) {
      lemur::api::COUNT_T vecLen;
      lemur::api::LOC_T *byteVec = list->byteVec(vecLen) ;

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
      int missingSegments = _MINIM<unsigned>( (sizeof(TermData) - actual) / 
                                              sizeof(SegmentOffset), 
                                              KEYFILE_MAX_SEGMENTS);
      int termSegments = KEYFILE_MAX_SEGMENTS - missingSegments;

      termData.segments[ termSegments ].segment = totalSegments;
      termData.segments[ termSegments ].offset = out.tellp();
      termData.segments[ termSegments ].length = vecLen;
      invlookup.put( list->termID(), &termData, 
                     sizeof(TermData) - 
                     sizeof(SegmentOffset)*(missingSegments-1) );
      out.write( (char*) byteVec, vecLen );
      delete[](byteVec);
      delete list;
      invertlists[i] = NULL;

      writes++;
    }
  }

  // flush the file buffer and close the segment stream
  out.flush();
  outFile.close();

  // reopen the output file as an input stream for queries:
  lemur::file::File* in = new lemur::file::File();
  in->open( name.str().c_str(), std::ifstream::in | std::ifstream::binary );
  _segments.push_back( in );

  // record the largest flushed termID
  _largestFlushedTermID = counts[UNIQUE_TERMS];
}


class reader_less {
public:
  bool operator () ( const lemur::file::KeyfileDocListSegmentReader* one, 
                     const lemur::file::KeyfileDocListSegmentReader* two ) {
    return (*one) < (*two);
  }
};

void lemur::index::KeyfileIncIndex::mergeCacheSegments() {
  unsigned int firstMergeSegment = 0;

  if( firstMergeSegment == 0 ) {
    invlookup.close();
    // g++ doesn't like this
    // need to move this ifdef out of here
#ifdef WIN32
    ::unlink( names[DOC_LOOKUP].c_str() );
#else
    std::remove( names[DOC_LOOKUP].c_str() );
#endif
    invlookup.create( names[DOC_LOOKUP].c_str() );
  }

  std::priority_queue<lemur::file::KeyfileDocListSegmentReader*,
    std::vector<lemur::file::KeyfileDocListSegmentReader*>,reader_less> readers;
  lemur::file::File outFile;
  std::vector<std::string> listNames;

  std::stringstream oldName;
  oldName << names[DOC_INDEX] << _segments.size();
  outFile.open( oldName.str(), std::ofstream::out | std::ofstream::binary );
  lemur::file::WriteBuffer out(outFile, KEYFILE_WRITEBUFFER_SIZE);  

  string nameToOpen(names[DOC_INDEX]);
  lemur::file::File * fileToOpen;
  // open segments
  for( unsigned int i = firstMergeSegment; i < _segments.size(); i++ ) {
    //    g++ doesn't like this.
    //    KeyfileDocListSegmentReader* reader =  new KeyfileDocListSegmentReader( _segments[i], std::string(names[DOC_INDEX]), i, KEYFILE_DOCLISTREADER_SIZE );
    fileToOpen =  _segments[i];
    lemur::file::KeyfileDocListSegmentReader* reader =  
      new lemur::file::KeyfileDocListSegmentReader(fileToOpen, nameToOpen, i, 
                                                   KEYFILE_DOCLISTREADER_SIZE );
    reader->pop(); // read the first list into memory
    // if it is empty, don't push it.
    if( reader->top() ) {
      readers.push(reader);
    } else {
      reader->file()->unlink();
      delete reader->file();
      delete reader;
    }
  }

  while( readers.size() ) {
    // find the minimum term id
    lemur::file::KeyfileDocListSegmentReader* reader = readers.top();
    InvFPDocList* list = reader->top();
    lemur::api::TERMID_T termID = list->termID();

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
    lemur::api::COUNT_T listLength;
    lemur::api::LOC_T* listBuffer = list->byteVec( listLength );
    lemur::file::File::offset_type offset = out.tellp();
    out.write( (char*) listBuffer, listLength );
    delete[](listBuffer);
    delete list;

    // fetch the offset data from the lookup structure
    int actual = 0;

    if( firstMergeSegment > 0 ) {
      invlookup.get( termID, &termData, actual, sizeof(TermData) );
    }

    // we need to add the location of the new inverted list
    int missingSegments = _MINIM<unsigned>( (sizeof(TermData) - actual) / sizeof(SegmentOffset), KEYFILE_MAX_SEGMENTS);
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
  outFile.close();
  
  // all the old files are deleted now, so we can rename the 
  // merged segment to be segment <firstMergeSegment>.
  std::stringstream newName;
  newName << names[DOC_INDEX] << firstMergeSegment;
  lemur::file::File::rename( oldName.str(), newName.str() );

  // clear out the _segment vector (all the ifstreams have
  // already been deleted)
  _segments.erase( _segments.begin() + firstMergeSegment, _segments.end() );
  
  // open the final segment again for reading
  lemur::file::File* in = new lemur::file::File();
  in->open( newName.str(), std::ifstream::in | std::ifstream::binary );
  _segments.push_back( in );
}

// -----------------------------------------------------------------------------
//
// Document management
//
// -----------------------------------------------------------------------------

int lemur::index::KeyfileIncIndex::docMgrID(const string &mgrString) {
  //  std::string mgrString( mgr );
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
    lemur::api::DocumentManager *dm = lemur::api::DocMgrManager::createDocMgr("fixme", mgrString);
    docMgrs.push_back(dm);
    return docmgrs.size()-1;
  }
}

void lemur::index::KeyfileIncIndex::doendDoc(const lemur::parse::DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    int len = dp->length();
    
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
    
    lemur::api::DOCID_T docID = counts[DOCS];

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

lemur::index::KeyfileIncIndex::record lemur::index::KeyfileIncIndex::fetchDocumentRecord(lemur::api::DOCID_T key) const {
  if (key == 0)
    return record();

  record rec;

  dtlookupReadBuffer->seekg( (key-1)*sizeof(rec), std::fstream::beg );
  dtlookupReadBuffer->read( (char*) &rec, sizeof rec );

  return rec;
}

void lemur::index::KeyfileIncIndex::addDocumentLookup( lemur::api::DOCID_T documentKey, 
                                                       const char* documentName ) {
  addGeneralLookup( dSTRs, dIDs, documentKey, documentName );
}

void lemur::index::KeyfileIncIndex::addTermLookup( lemur::api::TERMID_T termKey, const char* termSpelling ) {
  addGeneralLookup( tSTRs, tIDs, termKey, termSpelling );
}

void lemur::index::KeyfileIncIndex::addGeneralLookup( lemur::file::Keyfile& numberNameIndex, 
                                                      lemur::file::Keyfile& nameNumberIndex, 
                                                      lemur::api::TERMID_T number, const char* name ) {
  numberNameIndex.put( number, (void*) name, strlen(name) );
  nameNumberIndex.put( name, &number, sizeof number );
}
