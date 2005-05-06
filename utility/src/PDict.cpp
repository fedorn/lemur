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


/* 
 * dmf 02/2004
 */

#include "PDict.hpp"
#include "WriteBuffer.hpp"

/// internal structure for dictionary records in keyfile.
struct dictRecord {
  /// length of data
  unsigned int length;
  /// file offset
  lemur::file::File::offset_type offset;
  /// number of entries.
  int numEntries;
};

#define MAX_TERM_LENGTH 512
// 4 MB per key file.
#define KEY_CACHE_SIZE 1024 * 1024 * 4

lemur::dictionary::PDict::PDict() : name(""), currentTerm(""), currentVec(NULL), 
                                    usingCounts(false) {
}

lemur::dictionary::PDict::~PDict() {
  if (! name.empty())
    close();
}

void lemur::dictionary::PDict::close() {
  // flush any open list(s)
  flush();
  // flush new stats entry
  writeTOC();
  dict.close();
  targetIDs.close();
  dictEntries.close();
  name = "";
}

bool lemur::dictionary::PDict::open(const string &dictName) {
  bool result = false;
  ifstream tocStream;
  name = dictName;
  string toc = name + ".dict";
  tocStream.open(toc.c_str(), ios::in);
  if (! tocStream.fail()) {
    // read toc items and init stats.
    stats.dictSize = 0;
    stats.sourceSize = 0;
    stats.targetSize = 0;
    string item;
    int num;
    while (tocStream >> item >> num) {
      if (item == "source")
        stats.sourceSize = num;
      else if (item == "target")
        stats.targetSize = num;
      else if (item == "entries")
        stats.dictSize = num;
      else if (item == "counts")
        usingCounts = (num == 1);
    }
    tocStream.close();
    // open the keyfiles and invlist file
    dict.open(name, KEY_CACHE_SIZE);
    string tfname = name + ".target";
    targetIDs.open(tfname, KEY_CACHE_SIZE);
    string docfname = name + ".entries"; // these need to be defines...
    dictEntries.open(docfname, ios::binary | ios::in | ios::out );
    dictEntries.seekg(0, ios::beg );
    dictEntries.seekp(0, ios::end );
    result = true;
  } else {
    name = "";
  }
  return result;
}

bool lemur::dictionary::PDict::create(const string &dictName) {
  bool result = true;
  name = dictName;
  stats.dictSize = 0;
  stats.sourceSize = 0;
  stats.targetSize = 0;
  string docfname = name + ".entries";
  string tfname = name + ".target";
  dict.create(name, KEY_CACHE_SIZE);
  targetIDs.create(tfname, KEY_CACHE_SIZE);
  dictEntries.open(docfname, ios::binary | ios::in | ios::out);
  return result;
}

void lemur::dictionary::PDict::writeTOC() const {
  string toc = name + ".dict";
  ofstream tocStream;
  tocStream.open(toc.c_str(), ios::out);
  tocStream << "source " << stats.sourceSize << endl;
  tocStream << "target " << stats.targetSize << endl;
  tocStream << "entries " << stats.dictSize << endl;
  tocStream << "counts " << (usingCounts ? 1 : 0) << endl;
  tocStream.close();
}

bool lemur::dictionary::PDict::contains(const string &term, lemur::file::Keyfile &keyfile) const {
  return (keyfile.getSize(term.c_str()) != -1);
}


lemur::dictionary::DictEntryVector *lemur::dictionary::PDict::getTranslations(const string &term, 
                                                                              DictEntryFilter *filter) const {

  dictRecord record;
  int bitsSize;
  bool found = dict.get(term.c_str(), &record, bitsSize, sizeof(dictRecord));
  DictEntryVector *result = NULL;
  if (found) {
    AllDictEntryFilter nullFilter;
    if (filter == NULL) filter = &nullFilter;
    // allocate buffer
    char *buffer = new char[record.length];
    // seek
    dictEntries.seekg( record.offset, ifstream::beg );
    // read bits
    dictEntries.read( buffer, record.length );
    // make vector from bits.
    result = new DictEntryVector(buffer, filter);
    delete[](buffer);
  }
  return result;
}

int lemur::dictionary::PDict::numTranslations(const string &term, DictEntryFilter *filter) const {
  int result = 0;
  dictRecord record;
  int bitsSize;
  bool found = dict.get(term.c_str(), &record, bitsSize, sizeof(dictRecord));
  // no record, no xlates.
  if (!found) return result;
  // special case for null filter, count all entries.
  if (filter == NULL) {
    result = record.numEntries;
  } else {
    // allocate buffer
    char *buffer = new char[bitsSize];
    // seek
    dictEntries.seekg( record.offset, ifstream::beg );
    // read bits
    dictEntries.read( buffer, record.length );
    // make vector from bits.
    DictEntryVector *v = new DictEntryVector(buffer, filter);
    result = v->size();
    // clean up
    delete(v);
    delete[](buffer);
  }
  return result;
}

int lemur::dictionary::PDict::getSourceCount() const {
  return stats.sourceSize;
}

int lemur::dictionary::PDict::getNumPairs() const {
  return stats.dictSize;
}

int lemur::dictionary::PDict::getTargetCount() const { 
  // need a hash for target vocab
  return stats.targetSize;
}

void lemur::dictionary::PDict::flush() {
  // store currentVec for currentTerm.
  if (currentVec != NULL && currentVec->numEntries() > 0) {
    // update keyfile record.numEntries
    dictRecord record;
    record.numEntries = currentVec->numEntries();
    // seek
    dictEntries.seekp(0, ifstream::end);
    record.offset = dictEntries.tellp();
    int numBytes;
    char *bits = currentVec->toBytes(numBytes);
    record.length = numBytes;
    dictEntries.write(bits, numBytes);
    dict.put(currentTerm.c_str(), &record, sizeof record );
    delete[](bits);
  }
  delete(currentVec);
  currentVec = NULL;
  return;
}

void lemur::dictionary::PDict::add(const string &source, DictEntry &value, 
                                   double (*compose)(double, double)) {
  // presume current term is cached in mem, flushed once at source change.
  // have to check if there is an existing list at source change,
  // read in and replace. Final flush on dtor.
  // update hash for source/target vocab
  // no terms longer than MAX_TERM_LENGTH

  if (source != currentTerm) {
    // starting a new entry
    // flush old
    flush();
    // test if known
    // start list
    currentTerm = source;
    if (contains(currentTerm, dict)) {
      // known term, get its entry and append
      currentVec = getTranslations(currentTerm);
    } else {
      // new term (will be added at flush).
      currentVec = new DictEntryVector();
      stats.sourceSize++;
    }
  }
  // add to current list
  if (currentVec->addEntry(value, compose)) {
    // new Entry
    // update dict stats
    stats.dictSize++;
    // update target stats
    int count = 0;
    int bitsSize;
    bool found = targetIDs.get(value.target.c_str(), 
                               &count, bitsSize, sizeof(int));
    if (!found) stats.targetSize++;
    count++;
    targetIDs.put(value.target.c_str(), &count, sizeof(int));
  }
  return;
}

void lemur::dictionary::PDict::remove(const string &source, DictEntry &value) {
  // flush any open lists
  flush();
  currentTerm = "";

  // get list
  DictEntryVector *entries = getTranslations(source);
  if (entries == NULL) {
    // warn?
    return;
  }
  
  // remove entry, return if not found.
  if (! entries->removeEntry(value)) return;
  int numEntries = entries->numEntries(); 
  int bitsSize;
  bool found;
  if (numEntries == 0) {
    // special case on emptying a list
    // delete record for source.
    dict.remove(source.c_str());
    stats.sourceSize--;
  } else {
    // store list in original location (no growth, lose space of entry).
    dictRecord record;
    found = dict.get(source.c_str(), &record, bitsSize, sizeof(dictRecord));
    // update keyfile record.numEntries
    record.numEntries--;
    dictEntries.seekp(record.offset, ifstream::beg);
    int numBytes;
    char *bits = entries->toBytes(numBytes);
    record.length = numBytes;
    dictEntries.write(bits, numBytes);
    dict.put(source.c_str(), &record, sizeof record );
    delete[](bits);
  }
  // update hash for target vocab
  stats.dictSize--;
  int count;
  found = targetIDs.get(value.target.c_str(), &count, bitsSize, sizeof(int));
  if (found) count--; 
  else cerr << "lemur::dictionary::PDict::remove: no target. bad things happening" << endl;
  if (count == 0) {
    // remove it
    targetIDs.remove(value.target.c_str());
    stats.targetSize--;    
  } else {
    // update
    targetIDs.put(value.target.c_str(), &count, sizeof(int));
  }

  delete(entries);
  return;
}

void lemur::dictionary::PDict::remove(const string &source) {
  // flush any open lists
  flush();
  currentTerm = "";
  // get list
  DictEntryVector *entries = getTranslations(source);
  if (entries == NULL) {
    // warn?
    return;
    
  }

  int numEntries = entries->numEntries(); 
  
  // for each entry
  for (DictEntryVector::iterator it = entries->begin();
       it != entries->end(); it++) {
    int count, bitsSize;
    DictEntry value = *it;
    bool found = targetIDs.get(value.target.c_str(), 
                               &count, bitsSize, sizeof(int));
    if (found) 
      count--; 
    else 
      cerr << "lemur::dictionary::PDict::remove: no target. bad things happening" << endl;
    if (count == 0) {      
      // remove it
      targetIDs.remove(value.target.c_str());
      stats.targetSize--;    
    } else {
      // update
      targetIDs.put(value.target.c_str(), &count, sizeof(int));
    }
  }
  
  // delete record for source.
  dict.remove(source.c_str());
  
  // update hash for source/target vocab
  stats.dictSize -= numEntries;
  stats.sourceSize--;
  delete(entries);
  return;
}

void lemur::dictionary::PDict::write(const string &outputName, const string &delim) {
  // need to iterate over keys here.
  // need to cope with multi-char delimiter.
  // need to cope with escaped delimiters in fields.
  ofstream output(outputName.c_str());
  string source;
  char key[MAX_TERM_LENGTH]; // use max key size here and pass value into getNext.
  // defined in keyref.h, might rather include it here.
  key[0] = ' '; // first key is > than this.
  key[1] = '\0';

  dictRecord record;
  int bitsSize;
  // flush any open lists
  flush();
  currentTerm = "";

  // make sure we're starting at the beginning.
  dict.setFirst();
  while (dict.getNext(key, sizeof(key), &record, bitsSize, 
                      sizeof(dictRecord))) {
    source = key; 
    // expand getXlate here?
    //    DictEntryVector *v = getTranslations(source);
    DictEntryVector *v;
    AllDictEntryFilter nullFilter;
    // allocate buffer
    char *buffer = new char[record.length];
    // seek
    dictEntries.seekg( record.offset, ifstream::beg );
    // read bits
    dictEntries.read( buffer, record.length );
    // make vector from bits.
    v = new DictEntryVector(buffer, &nullFilter);
    delete[](buffer);
    for (DictEntryVector::iterator it = v->begin(); it != v->end(); it++) {
      output << source << delim << (*it).toString(delim) << endl;
    }
    delete(v);
  }
  output.close();
  return;
}

bool lemur::dictionary::PDict::read(const string &dictName, const string &delim, bool counts) {
  // need to cope with multi-char delimiter.
  // need to cope with escaped delimiters in fields.
  usingCounts = counts;
  bool retval = true;
  char d = delim.c_str()[0];
  ifstream dict(dictName.c_str());
  if (dict.fail()) {
    cerr << "read: unable to open " << dictName << endl;
    return false;
  }
  
  string source, type, target, prob;  
  while (!getline(dict, source, d).eof()) {
    getline(dict, type, d);
    getline(dict, target, d);
    getline(dict, prob);
    double pr = atof(prob.c_str());
    DictEntry ent(target, type, pr);
    add(source, ent);
  }
  dict.close();
  return retval;
}

void lemur::dictionary::PDict::normalize() {
  // normalize each entry vector
  // update data file.
  // need to iterate over keys here.
  char key[MAX_TERM_LENGTH]; // maxkey_lc
  key[0] = '\0';
  dictRecord record;
  int bitsSize;
  // flush any open lists
  flush();
  currentTerm = "";

  // make sure we're starting at the beginning.
  dict.setFirst();  
  while (dict.getNext(key, sizeof(key),
                      &record, bitsSize, sizeof(dictRecord))) {
    DictEntryVector *v;
    AllDictEntryFilter nullFilter;
    // allocate buffer
    char *buffer = new char[record.length];
    // seek
    dictEntries.seekg( record.offset, ifstream::beg );
    // read bits
    dictEntries.read( buffer, record.length );
    // make vector from bits.
    v = new DictEntryVector(buffer, &nullFilter);
    v->normalize();
    // store list in original location (no change in size).
    // reuse the buffer
    v->toBytes(buffer);
    dictEntries.seekp(record.offset, ifstream::beg);
    dictEntries.write(buffer, record.length);
    delete[](buffer);
    delete(v);
  }
  // we have left the counts domain.
  usingCounts = false;
  return;
}


lemur::dictionary::DictEntryVector *lemur::dictionary::PDict::nextTranslations(string &term, 
                                                                               DictEntryFilter *filter) const {

  dictRecord record;
  int bitsSize;
  char key[MAX_TERM_LENGTH]; // maxkey_lc
  bool found = dict.getNext(key, sizeof(key), &record, bitsSize, 
                            sizeof(dictRecord));
  DictEntryVector *result = NULL;
  if (found) {
    term = key;
    AllDictEntryFilter nullFilter;
    if (filter == NULL) filter = &nullFilter;
    // allocate buffer
    char *buffer = new char[record.length];
    // seek
    dictEntries.seekg( record.offset, ifstream::beg );
    // read bits
    dictEntries.read( buffer, record.length );
    // make vector from bits.
    result = new DictEntryVector(buffer, filter);
    delete[](buffer);
  }
  return result;
}

// DictEntryVector
lemur::dictionary::DictEntryVector::DictEntryVector(char *buffer, DictEntryFilter *filter) : 
  vector<DictEntry>() {
  fromBytes(buffer, filter);  
}

static double sumFn(double d1, double d2) {return (d1 + d2);}

bool lemur::dictionary::DictEntryVector::addEntry(DictEntry &entry, double (*compose)(double, double)) {
  bool retval = false;
  if (compose == NULL) compose = sumFn;

  lemur::dictionary::DictEntryVector::iterator it = find(begin(), end(), entry);
  if (it != end()) {
    // compose the prob values into the new entry then overwrite.
    entry.prob = compose((*it).prob, entry.prob);   
    *it = entry; // overwrite it. // stats don't change
  } else {
    // new entry
    retval = true;
    push_back(entry);
  }
  return retval;
}

bool lemur::dictionary::DictEntryVector::removeEntry(DictEntry &entry) {
  bool retval = false;
  lemur::dictionary::DictEntryVector::iterator it = find(begin(), end(), entry);
  if (it != end()) {
    erase(it);
    retval = true;
  }
  return retval;
}

char *lemur::dictionary::DictEntryVector::toBytes(int &numBytes) const{
  numBytes = sizeof(unsigned int);
  lemur::dictionary::DictEntryVector::const_iterator it;
  for (it = begin(); it != end(); it++)
    numBytes += (*it).numBytes();
  char *buffer = new char[numBytes];
  toBytes(buffer);
  return buffer;
}

void lemur::dictionary::DictEntryVector::toBytes(char *buffer) const{
  lemur::dictionary::DictEntryVector::const_iterator it;
  int consumed;
  char *tmpBuffer = buffer + sizeof(int);
  unsigned int numEntries = 0;
  for (it = begin(); it != end(); it++){
    consumed = (*it).toBytes(tmpBuffer);
    tmpBuffer += consumed;
    numEntries++;
  }
  std::memcpy(buffer, &numEntries, sizeof(unsigned int));
}

void lemur::dictionary::DictEntryVector::fromBytes(char *buffer, DictEntryFilter *filter){
  DictEntry entry;
  unsigned int numEntries;
  std::memcpy(&numEntries, buffer, sizeof(unsigned int));
  buffer += sizeof(unsigned int);
  for (int i = 0; i < numEntries; i++) {
    int consumed = entry.fromBytes(buffer);
    buffer += consumed;
    if (filter->accept(entry)){
      push_back(entry);
    }
  }
}

int lemur::dictionary::DictEntryVector::numEntries() const {
  return size();
}

void lemur::dictionary::DictEntryVector::normalize(){
  lemur::dictionary::DictEntryVector::iterator it;
  double sum = 0.0;
  for (it = begin(); it != end(); it++)
    sum += (*it).prob;
  for (it = begin(); it != end(); it++){
    (*it).prob /= sum;
  }
}

// DictEntry
lemur::dictionary::DictEntry::DictEntry() {
  prob = 0;
}

int lemur::dictionary::DictEntry::toBytes(char *buffer) const {
  char *start = buffer;
  // layout bit record as follows
  // target string size [sizeof(int)]
  // target string
  // type size          [sizeof(int)]
  // type
  // prob               sizeof(double)
  int len = target.length();  
  std::memcpy(buffer, &len, sizeof(int));
  buffer += sizeof(int);
  std::memcpy(buffer, target.c_str(), len);
  buffer += len;
  len = type.length();  
  std::memcpy(buffer, &len, sizeof(int));
  buffer += sizeof(int);
  std::memcpy(buffer, type.c_str(), len);
  buffer += len;
  std::memcpy(buffer, &prob, sizeof(double));
  buffer += sizeof(double);
  return (buffer - start);
}

int lemur::dictionary::DictEntry::numBytes() const {
  // layout bit record as follows
  // target string size [sizeof(int)]
  // target string
  // type size          [sizeof(int)]
  // type
  // prob               sizeof(double)
  int total = (2 * sizeof(int)) + sizeof(double) + target.length() + 
    type.length();
  return total;
}

int lemur::dictionary::DictEntry::fromBytes(char *buffer){
  char *start = buffer;
  // layout bit record as follows
  // target string size [sizeof(int)]
  // target string
  // type size          [sizeof(int)]
  // type
  // prob               sizeof(double)
  char tmpString[MAX_TERM_LENGTH];
  int len;
  std::memcpy(&len, buffer, sizeof(int));
  buffer += sizeof(int);
  std::memcpy(tmpString, buffer, len);
  tmpString[len] = '\0';
  target = tmpString;
  buffer += len;
  std::memcpy(&len, buffer, sizeof(int));
  buffer += sizeof(int);
  std::memcpy(tmpString, buffer, len);
  tmpString[len] = '\0';
  type = tmpString;
  buffer += len;
  std::memcpy(&prob, buffer, sizeof(double));
  buffer += sizeof(double);
  return (buffer - start);
}

string lemur::dictionary::DictEntry::toString(string delim) const {
  std::ostringstream retval;
  retval << type << delim << target << delim << prob << delim;
  return retval.str();
}

