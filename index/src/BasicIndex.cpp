/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#include <cassert>
extern "C" { 
#include "util.h"
}
#include "BasicIndex.hpp"
#include "Array.hpp"
#include "BitArray.hpp"
#include "BasicDocStream.hpp"
#include "FastList.hpp"
#include "GammaCompress.hpp"
#include "ISet.hpp"
#include "IndexCount.hpp"
#include "Int.hpp"
#include "List.hpp"
#include "MemList.hpp"
#include "Param.hpp"
#include "String.hpp"
#include "Terms.hpp"
#include "Timer.hpp"
#include "BasicTermInfoList.hpp"
#include "BasicDocInfoList.hpp"
#include "Exception.hpp"


#define MAXLINE 65536
#define MIN(a,b) (a<=b) ? a : b
#define MAX(a,b) (a<=b) ? b : a
const int maxDocSize=5000;

const char * wordIndexSuffix     = "windex";
const char * documentIndexSuffix = "dindex";
const char * wordVocSuffix       = "terms";
const char * documentVocSuffix   = "docids";
const char * wordKeySuffix       = "wkey";
const char * documentKeySuffix   = "dkey";
const char * indexFileSuffix     = "bsc";




static bool fileExist(char *name)
{
  ifstream ifs;
  ifs.open(name);
  if (!ifs.fail()) {
    ifs.close();
    return true;
  } 
  return  false;
}

BasicIndex::BasicIndex()
{
 woffset = NULL;
 doffset = NULL;
 tmpdarr = NULL;
 tmpwarr = NULL;
 countOfDoc = NULL;
 countOfTerm = NULL;
 deleteCompressor = false;
}

BasicIndex::BasicIndex (Compress * pc) 
  :  pCompressor(pc) 
{
  woffset = NULL;
  doffset = NULL;
  tmpdarr = NULL;
  tmpwarr = NULL;
  countOfDoc = NULL;
  countOfTerm = NULL;
  deleteCompressor = false;
}

BasicIndex::~BasicIndex()
{
  if (woffset) {
    delete [] woffset;
  }

  if (doffset) {
    delete [] doffset;
  }

  if (tmpdarr) {
    delete [] tmpdarr;
  }

  if (tmpwarr) {
    delete [] tmpwarr;
  }

  if (countOfDoc) {
    delete [] countOfDoc;
  }

  if (countOfTerm) {
    delete [] countOfTerm;
  }

  if (deleteCompressor) {
    delete pCompressor;
  }
}




void BasicIndex::build(DocStream *collectionStream, 
		       const char *file, const char * outputPrefix, 
		  int totalDocs, int maxMemory, 
		  int minimumCount, int maxVocSize) 
{
  cerr << "Input file: " << file << endl;
  cerr << "Output prefix: " << outputPrefix << endl;
  cerr << "Documents to process: " << totalDocs << endl;
  cerr << "Maximum memory: " << maxMemory/(1<<20) << "M" << endl;
  cerr << "Minimum term count: " << minimumCount << endl;


  pDocStream = collectionStream;

  //textFile = file;
  textFile = "[unknown]";
  prefix = outputPrefix;
  totalDocuments = totalDocs;
  numDocuments = 0;
  numBytes = 0;
  maximumMemory = maxMemory;
  pMemList = NULL;

  Timer buildTime("indexing collection");
  buildVocabulary(maxVocSize, minimumCount);
  terms.open(wordVocabulary);
  cerr << "Number of terms: " << terms.size() << endl;
  docids.open(documentVocabulary);
  cerr << "Number of documents: " << docids.size() << endl;

  // Set up memory management
  maxSegmentsPerIndex=4;
  memorySegment = maximumMemory/4;
  pMemList = new MemList(memorySegment);
  IndexCount::UseMemory(*pMemList);

  cerr << "Maximum memory: " << maximumMemory/(1<<20) << "M (" 
       << maxSegmentsPerIndex << " segments of " 
       << memorySegment/(1<<20) << "M)" << endl;

  indexCollection();
  mergeIndexFiles();
  createKeys();
  timeToIndex = buildTime.getElapsed();
  writeIndexFile();
}

void BasicIndex::writeIndexFile() 
{
  char fn[1024];
  sprintf(fn, "%s.%s", (const char *) prefix, indexFileSuffix);
  FILE * fp = fopen(fn, "w");
  assert(fp);
  time_t t;
  time(&t);
  String date = ctime(&t);
  String buildTime("\"");
  char buf[1024];
  if (timeToIndex/3600) 
    { sprintf(buf, "%d hours ", (int) timeToIndex/3600); timeToIndex %= 3600; buildTime += buf; }
  if (timeToIndex/60)  
    { sprintf(buf, "%d minutes ", (int) timeToIndex/60); timeToIndex %= 60; buildTime += buf; }
  sprintf(buf, "%d seconds\"", (int) timeToIndex); buildTime += buf; 

  date[strlen(date)-1] = '\0';
  fprintf(fp, "%-20s =  \"%s\";\n", "created", (const char *) date);
  fprintf(fp, "%-20s =  %s;\n", "createdBy", getenv("USER"));
  fprintf(fp, "%-20s =  %s;\n", "createdTime", (const char *) buildTime);
  fprintf(fp, "%-20s =  %s;\n", "textFile", (const char *) textFile);
  fprintf(fp, "%-20s =  %d;\n", "numDocuments", numDocuments);
  fprintf(fp, "%-20s =  %d;\n", "numTerms", terms.size());
  fprintf(fp, "%-20s =  %d;\n", "numWords", numWords);
  fprintf(fp, "%-20s =  %d;\n", "numBytes", numBytes);
  fprintf(fp, "%-20s =  %d;\n", "maxDocLength", maxDocumentLength);
  fprintf(fp, "%-20s =  %.2f;\n", "avgDocLength", avgDocumentLength);
  fprintf(fp, "%-20s =  %s;\n", "wordVocabulary", (const char *) wordVocabulary);
  fprintf(fp, "%-20s =  %s;\n", "docVocabulary", (const char *) documentVocabulary);
  fprintf(fp, "%-20s =  %s;\n", "wordIndex", (const char *) wordIndexFile);
  fprintf(fp, "%-20s =  %s;\n", "documentIndex", (const char *) documentIndexFile);
  fprintf(fp, "%-20s =  %s;\n", "wordKey", (const char *) wordKeyFile);
  fprintf(fp, "%-20s =  %s;\n", "documentKey", (const char *) documentKeyFile);
  fprintf(fp, "%-20s =  %s;\n", "compression", pCompressor->type());
  fclose(fp);
}

static const char * extractID(const char * id) {
  String s = ParamGetString(id);
  cout << id << ":\t\t " << s << endl;
  return s;
}

static int extractIntID(const char * id) {
  Int i = ParamGetInt(id, -1);
  cout << id << ":\t\t " << i << endl;
  return i;
}

static float extractFloatID(const char * id) {
  float def=-1.0;
  float f = ParamGetFloat(id, def);
  cout << id << ":\t\t " << f << endl;
  return f;
}

bool BasicIndex::open(const char * fn) 
{
  cout << "Opening index " << fn << endl;
  ParamPushFile(fn);
  extractID("created");
  extractID("createdBy");
  //  textFile = extractID("textFile");
  numDocuments = extractIntID("numDocuments");
  numWords = extractIntID("numWords");
  numBytes = extractIntID("numBytes");
  maxDocumentLength = extractIntID("maxDocLength");
  avgDocumentLength = extractFloatID("avgDocLength");
  wordVocabulary = extractID("wordVocabulary");
  documentVocabulary = extractID("docVocabulary");
  wordIndexFile = extractID("wordIndex");
  documentIndexFile = extractID("documentIndex");
  wordKeyFile = extractID("wordKey");
  documentKeyFile = extractID("documentKey");
  String compressionType = extractID("compression");
  ParamPopFile();

  // Open up vocabularies
  terms.open(wordVocabulary);
  docids.open(documentVocabulary);

  // Open up indices
  // textStream.open(textFile); assert(textStream);
  wordIndexStream.open(wordIndexFile); assert(wordIndexStream);
  documentIndexStream.open(documentIndexFile); assert(documentIndexStream);

  ifstream wordKeyStream(wordKeyFile); assert(wordKeyStream);
  ifstream documentKeyStream(documentKeyFile); assert(documentKeyStream);
  cout << "Input streams successfully opened." << endl;

  // Set up compression scheme 
  if (compressionType == "gamma") {
    pCompressor = new GammaCompress();
    deleteCompressor = true;
  }
  else {
    // no other compression currently supported!
    assert(0);
  }

  // Read in key information, etc.
  char rec[65536];
  char * p;

  woffset = new int[terms.size()];
  countOfTerm = new int[terms.size()];
  int totalWordCount = 0;
  cout << "Loading the word key..." << endl;
  while (wordKeyStream.peek() != EOF) {
    wordKeyStream.getline(rec, MAXLINE, '\n');
    p = strtok(rec, " ");
    int k = terms[p];
    assert (k != -1);
    p = strtok(NULL, " ");
    int c = atoi(p);
    countOfTerm[k] = c;
    p = strtok(NULL, " ");
    woffset[k] = atoi(p);
    totalWordCount += c;
  }
  wordKeyStream.close();

  doffset = new int[docids.size()];
  // toffset = new int[docids.size()];
  countOfDoc = new int[docids.size()];

  int totalDocCount = 0;
  cerr << "Loading the document key..." << endl;
  while (documentKeyStream.peek() != EOF) {
    documentKeyStream.getline(rec, MAXLINE, '\n');
    p = strtok(rec, " ");
    int k = docids[p];
    assert (k != -1);
    p = strtok(NULL, " ");
    int c = atoi(p);
    countOfDoc[k] = c;
    p = strtok(NULL, " ");
    doffset[k] = atoi(p);
    // p = strtok(NULL, " ");
    // toffset[k] = atoi(p);
    totalDocCount += c;
  }
  documentKeyStream.close();
  assert(totalDocCount == totalWordCount);

  tmpdarr = new int[2*docids.size()];
  tmpwarr = new int[2*terms.size()];

  cout << "Index file " << fn << " opened" << endl;
  return true;
}

void BasicIndex::buildVocabulary(int maxVocSize, int minCount) 
{
  Array<IndexCount> ti(maxVocSize);
  ISet<String> voc(maxVocSize);
  ISet<String> doc(maxVocSize);
  string word;

  int numWordsRead=0;
  int numDocs=0;
  int i;
  for (i=0; i<maxVocSize; ++i) {
    ti[i].index = i;
    ti[i].count = 0;
  }
  
  //  pDocStream->open(textFile);
  pDocStream->startDocIteration();

  //   DocStream &dfs = *pDocStream;

  Timer timer("building vocabulary");
  Timer ctimer("collecting counts");

  int k;

  while (pDocStream->hasMore() && (numDocs < totalDocuments)) {

    Document *thisDoc = pDocStream->nextDoc();
    // make sure docid is not duplicated
    assert (doc[thisDoc->getID()]==-1);  
    doc += thisDoc->getID();
    //    cerr << "id: "<< thisDoc->getID() << endl;
    thisDoc->startTermIteration();
    while (thisDoc->hasMore()) {
      TokenTerm *term = thisDoc->nextTerm();
      
      if (term->spelling()[0]  == '\0') {
	continue;
      }
       voc += term->spelling();
       k = voc[term->spelling()];
      if (k > maxVocSize) {
	cerr << "Vocabulary size limit of " << maxVocSize << " exceeded!" << endl;
	throw Exception("BasicIndex", "vocabulary size overflow");
      }
      ti[k].count += 1;
      if (++numWordsRead % 50000 == 0) { 
	fprintf(stderr, "\r%d words, %d terms, %d documents processed...", 
		numWordsRead, voc.size(), numDocs);
      }
    }
    numDocs++;
   }

  numWords = numWordsRead;
  fprintf(stderr, "\n%d words, %d documents processed", 
	  numWordsRead, numDocs);
  cerr << endl;
  ctimer.Stop();

  int n = voc.size();
  cerr << "Total number of terms: " << n << endl;
  Timer sorttime("Sort of vocabulary");
  IndexCount::rqSort(n, ti.Get());
  sorttime.Stop();

  char name[1024];
  sprintf(name, "%s.%s", (const char *) prefix, wordVocSuffix);
  wordVocabulary = name;
  ofstream ofs(wordVocabulary);
  assert(ofs);
  cerr << "Pruning vocabulary with minimum count " << minCount << endl;
  ofs << Terms::getOOVSpelling() << endl;
  for (i=n-1; i>=0; --i) {
    String &s = voc[ti[i].index];
    if (s.length() == 0) continue;
    if (ti[i].count < minCount) break;
    ofs << s << endl;
  }
  
  sprintf(name, "%s.%s", (const char *) prefix, documentVocSuffix);
  documentVocabulary = name;
  ofstream ofs2(documentVocabulary);
  assert(ofs2);
  cerr << "Number of document ids: " << doc.size() << endl;
  ofs2 << Terms::getOOVSpelling() << endl;
  for (i=0; i<doc.size(); ++i) {
    ofs2 << doc[i] << endl;
  }

  numDocuments = numDocs;
}



void BasicIndex::writeWordIndex(int indexNum, FastList<IndexCount> * dlw) 
{
  char outputName[1024];
  int * a = new int[2*numDocuments];
  sprintf(outputName, "%s.%s.%d", 
	  (const char *) prefix, wordIndexSuffix, indexNum);
  wordIndexFile = outputName;
  ofstream ofs(outputName);
  assert(ofs);
  int termsWritten=0;

  cerr << "\nWriting index to " << outputName << endl;
  for (int i=0; i<terms.size(); ++i) {
    FastList<IndexCount> &docList = dlw[i];
    if (docList.HeadLL() == NULL) continue;
    int numEntries = docList.ComputeLength();
    int totalCount = 0;
    assert(numEntries > 0);
    int prevDoc=0;
    int index=0;
    const FLL<IndexCount> *dll;
    for (dll=docList.HeadLL(); dll; dll = dll->Next()) {
      IndexCount &dc = dll->Data();
      a[index] = dc.index-prevDoc;
      prevDoc = dc.index;
      int c = dc.count;
      a[numEntries+index] = c;
      totalCount += c;
      index++;
      assert(c > 0);
    }
    assert(index == numEntries);
    ofs.write((char *)&i, sizeof(int));
    ofs.write((char *)&totalCount, sizeof(int));
    pCompressor->compress(ofs, 2*numEntries, a);
    if (++termsWritten % 100 == 0) {
      fprintf(stderr, "\r%d term entries written", termsWritten);
    }
  }
  fprintf(stderr, "\n");
  delete [] a;
}


int BasicIndex::indexCollection() 
{
  FastList<IndexCount> * docListOfWord = 
    new FastList<IndexCount>[terms.size()];
  int * wordCount = new int[terms.size()];
  int * docEntry = new int[2*terms.size()];
  ISet<Int> docSet(maxDocSize);
  string word;
  char rec[MAXLINE];
  int numWordsRead=0;
  int numDocs=0;
  int numIndices=0;
  char cmd[1024];
  int did;
  int i;
  char name[1024];
  sprintf(name, "%s.%s", (const char *) prefix, documentIndexSuffix);
  documentIndexFile = name;
  ofstream ofs(name);
  assert(ofs);

  pDocStream->startDocIteration();
  //  pDocStream->open(textFile);
  // DocStream &dfs = *pDocStream;

  for (i=0; i<terms.size(); ++i) {
    wordCount[i] = 0;
  }

  Timer timer("indexing collection");

  cerr << "Erasing existing index files" << endl;

  sprintf(cmd, "echo \"/bin/rm %s.%s.*\"", 
	  (const char *) prefix, wordIndexSuffix);
  system(cmd);
  sprintf(cmd, "/bin/rm %s.%s.*", (const char *) prefix, wordIndexSuffix);
  system(cmd);

  avgDocumentLength = 0;
  maxDocumentLength = 0;
  int n, totalCount;

  while (pDocStream->hasMore() && (numDocs < totalDocuments)) {
    Document *thisDoc = pDocStream->nextDoc();

    did = docids[thisDoc->getID()];
    assert(did > 0);
    thisDoc->startTermIteration();
    while (thisDoc->hasMore()) {

      TokenTerm *term = thisDoc->nextTerm();
      //      cerr << "term: "<< term->spelling() << endl;
      if (term->spelling()[0]  == '\0') {
	continue;
      }
      numBytes += strlen(term->spelling());

      int k = terms[term->spelling()];
      if (k == -1) {
	cerr << "Unknown spelling: \"" << term->spelling() << "\"" << endl;
      }
      docSet += Int(k);
      wordCount[k] += 1;
      if (++numWordsRead % 50000 == 0) { 
	fprintf(stderr, "\r%d words, %d documents processed, memory %dM", 
		numWordsRead, numDocs, pMemList->Used()/(1<<20));
      }
    }

    // Write an entry in the document index
    n = 2*docSet.size();
    totalCount = 0;
    for (i=0; i<docSet.size(); ++i) {
      int k = docSet[i];
      docEntry[i] = k;
      docEntry[docSet.size()+i] = wordCount[k];
      totalCount += wordCount[k];
    }
    avgDocumentLength += totalCount;
    maxDocumentLength = MAX(maxDocumentLength, totalCount);
    ofs.write((char *)&did, sizeof(int));
    ofs.write((char *) &totalCount, sizeof(int));
    pCompressor->compress(ofs, n, docEntry);
    
    // Now add to the lists for the word index
    for (i=0; i<docSet.size(); ++i) {
      int k = docSet[i];
      FastList<IndexCount> &docList = docListOfWord[k];
      IndexCount * dc = new IndexCount(numDocs+1, wordCount[k]);
      docList.AddToTail(*dc);
      wordCount[k] = 0;
    }
    if (pMemList->Used() > memorySegment * maxSegmentsPerIndex) {
      cerr << endl;
      timer.Elapsed();
      writeWordIndex(numIndices++, docListOfWord);
      for (i=0; i<terms.size(); ++i) {
	FastList<IndexCount> &docList = docListOfWord[i];
	docList.Clear();
      }
      pMemList->Reset();
    }
    numDocs++;
    docSet.clear();
  }

  avgDocumentLength /= numDocs;
  writeWordIndex(numIndices++, docListOfWord);
  fprintf(stderr, "\n%d documents, %d words indexed", 
	  numDocs, numWordsRead);
  cerr << endl;
  
  ofs.close();

  delete [] docListOfWord;
  delete [] wordCount;
  delete [] docEntry;

  headDocIndex();
  
  timer.Stop();
  return 0;
}


int BasicIndex::headWordIndex() 
{
  int w, d, totalCount, nWords=0;
  int * a = new int[2*numDocuments];
  char fn[1024];
  sprintf(fn, "%s.%s", (const char *) prefix, wordIndexSuffix);
  ifstream ifs(fn);
  assert(ifs);

  while (ifs.peek() != EOF) {
    ifs.read((char *)&w, sizeof(int));
    ifs.read((char *)&totalCount, sizeof(int));
    int size = pCompressor->decompress(ifs, a)/2;
    nWords++;
    if (nWords > 20) break;
    cerr << w << ") " << terms[w] << ": ";
    d = MIN(size, 5);
    int prevDoc=0;
    for (int i=0; i < d; ++i) {
      cerr << "(" << (a[i]+prevDoc) << "," << a[size+i] << ")->";
      prevDoc = a[i]+prevDoc;
    }
    if (size > 5) 
      cerr << "...[" << size << " total]" << endl;
    else 
      cerr << "nil" << endl;
  }
  delete [] a;
  return 0;
}

int BasicIndex::headDocIndex() 
{
  int w, d, m, nDocs=0;
  int * a = new int[2*terms.size()];

  char fn[1024];
  sprintf(fn, "%s.%s", (const char *) prefix, documentIndexSuffix);
  ifstream ifs(fn);
  assert(ifs);

  while (ifs.peek() != EOF) {
    ifs.read((char *)&d, sizeof(int));
    ifs.read((char *)&m, sizeof(int));
    int size = pCompressor->decompress(ifs, a)/2;
    nDocs++;
    if (nDocs > 10) break;
    cerr << d << ") " << docids[d] << ": ";
    w = MIN(size, 5);
    for (int i=0; i < w; ++i) {
      cerr << "(" << terms[a[i]] << "," << a[size+i] << ")->";
    }
    if (size > 5) 
      cerr << "...[" << size << " total]" << endl;
    else 
      cerr << "nil" << endl;
  }
  delete [] a;
  return 0;
}

int BasicIndex::mergePair(const char * fn1, const char * fn2, const char * fn3) 
{
  int n1, n2, n3, w1, w2, c1, c2, c3, i;
  static int inited=0;
  static int * dc1, * dc2, * dc3;

  if (!inited) {
    dc1 = new int[2*numDocuments];
    dc2 = new int[2*numDocuments];
    dc3 = new int[2*numDocuments];
    inited = 1;
  }

  cerr << "Merging: \n " << fn1 << "\n " << fn2 << endl;

  ifstream ifs1(fn1);
  assert(ifs1);
  ifstream ifs2(fn2);
  assert(ifs2);
  ofstream ofs(fn3);
  assert(ofs);

  ifs1.read((char *)&w1, sizeof(int));
  ifs1.read((char *)&c1, sizeof(int));
  n1 = pCompressor->decompress(ifs1, dc1);
  
  ifs2.read((char *)&w2, sizeof(int));
  ifs2.read((char *)&c2, sizeof(int));
  n2 = pCompressor->decompress(ifs2, dc2);

  while ((!ifs1.eof()) && (!ifs2.eof())) {
    if (w1 < w2) {
      ofs.write((char *)&w1, sizeof(int));
      ofs.write((char *)&c1, sizeof(int));
      pCompressor->compress(ofs, n1, dc1);
      ifs1.read((char *)&w1, sizeof(int));
      ifs1.read((char *)&c1, sizeof(int));
      n1 = pCompressor->decompress(ifs1, dc1);
    }
    else if (w1 > w2) {
      ofs.write((char *)&w2, sizeof(int));
      ofs.write((char *)&c2, sizeof(int));
      pCompressor->compress(ofs, n2, dc2);
      ifs2.read((char *)&w2, sizeof(int));
      ifs2.read((char *)&c2, sizeof(int));
      n2 = pCompressor->decompress(ifs2, dc2);
    }
    else if (w1 == w2) {
      // write both records
      n3 = n1 + n2;
      c3 = c1 + c2;
      int m1 = n1/2;
      int m2 = n2/2;
      int currDoc = 0;
      for (i=0; i<m1; ++i) {
	dc3[i] = dc1[i];
	currDoc += dc1[i];
      }
      assert(dc2[0] > currDoc);

      // Handle the "boundary" between records, 
      // which are delta-encoded
      dc3[m1+0] = dc2[0]-currDoc;
	  
      for (i=1; i<m2; ++i) {
	dc3[m1+i] = dc2[i];
      }
      for (i=0; i<m1; ++i) {
	dc3[m1+m2+i] = dc1[m1+i];
      }
      for (i=0; i<m2; ++i) {
	dc3[m1+m2+m1+i] = dc2[m2+i];
      }
      ofs.write((char *)&w1, sizeof(int));
      ofs.write((char *)&c3, sizeof(int));
      pCompressor->compress(ofs, n3, dc3);
      // read in new records
      ifs1.read((char *)&w1, sizeof(int));
      ifs1.read((char *)&c1, sizeof(int));
      n1 = pCompressor->decompress(ifs1, dc1);
      ifs2.read((char *)&w2, sizeof(int));
      ifs2.read((char *)&c2, sizeof(int));
      n2 = pCompressor->decompress(ifs2, dc2);
    }
  }
  
  while (!ifs1.eof()) {
    ofs.write((char *)&w1, sizeof(int));
    ofs.write((char *)&c1, sizeof(int));
    pCompressor->compress(ofs, n1, dc1);
    ifs1.read((char *)&w1, sizeof(int));
    ifs1.read((char *)&c1, sizeof(int));
    n1 = pCompressor->decompress(ifs1, dc1);
  }

  while (!ifs2.eof()) {
    ofs.write((char *)&w2, sizeof(int));
    ofs.write((char *)&c2, sizeof(int));
    pCompressor->compress(ofs, n2, dc2);
    ifs2.read((char *)&w2, sizeof(int));
    ifs2.read((char *)&c2, sizeof(int));
    n2 = pCompressor->decompress(ifs2, dc2);
  }

  return 0;
}

void BasicIndex::mergeIndexFiles() 
{
  char fname1[1024], fname2[1024], cmd[1024], tmpnam1[1024], tmpnam2[1024];
  cerr << "Merging index files " << endl;

  while (1) {




    // We're finished when only indexPrefix.0 is present;
    sprintf(tmpnam1, "%s.%s.%d", (const char *) prefix, wordIndexSuffix, 0);

    //  assert(qfilef(tmpnam1));
    
    assert(fileExist(tmpnam1));

    sprintf(tmpnam1, "%s.%s.%d", (const char *) prefix, wordIndexSuffix, 1);
    //if (!qfilef(tmpnam1)) break;
    if (!fileExist(tmpnam1)) break;

    for (int i=0; ; i+=2) {
      sprintf(fname1, "%s.%s.%d", (const char *) prefix, wordIndexSuffix, i);
      sprintf(fname2, "%s.%s.%d", (const char *) prefix, wordIndexSuffix, i+1);
      //      if (!(qfilef(fname1))) break;
      if (!(fileExist(fname1))) break;
      //      else if (!qfilef(fname2)) {
      else if (!fileExist(fname2)) {
      
	sprintf(tmpnam1, "%s.%s.%d", 
		(const char *) prefix, wordIndexSuffix, i/2);
	sprintf(cmd, "/bin/mv %s %s", fname1, tmpnam1);
	system(cmd);
      }
      else {
	sprintf(tmpnam1, "%s.%s.tmp.%d.%d", 
		(const char *) prefix, wordIndexSuffix, i, i+1);
	sprintf(tmpnam2, "%s.%s.%d", 
		(const char *) prefix, wordIndexSuffix, i/2);
	mergePair(fname1, fname2, tmpnam1);
	sprintf(cmd, "/bin/rm %s %s", fname1, fname2);
	system(cmd);
	sprintf(cmd, "/bin/mv %s %s", tmpnam1, tmpnam2);
	system(cmd);
      }
    }
  }

  sprintf(fname1, "%s.%s.%d", (const char *) prefix, wordIndexSuffix, 0);
  sprintf(tmpnam1, "%s.%s", (const char *) prefix, wordIndexSuffix);
  sprintf(cmd, "/bin/mv %s %s", fname1, tmpnam1);
  wordIndexFile = tmpnam1;
  system(cmd);

  headWordIndex();
}

void BasicIndex::createKey(const char * inName, const char * outName, 
		      Terms & voc, int * byteOffset) {
  ofstream ofs(outName);
  assert(ofs);
  ifstream ifs(inName);
  assert(ifs);

  int d, c;
  cerr << "Creating key file " << outName << endl;
  BitArray * ba = new BitArray(1024);

  int bytesRead=0;
  while (ifs.peek() != EOF) {
    ifs.read((char *)&d, sizeof(int));
    ifs.read((char *)&c, sizeof(int));
    pCompressor->read(ifs, ba);
    ofs << voc[d] << " " << c << " " << bytesRead;
    if (byteOffset != (int *) NULL) {
      ofs << " " << byteOffset[d];
    }
    ofs << endl;
    bytesRead += 4*sizeof(int) + ba->Bytes();
  }
}

void BasicIndex::createKeys() 
{
  char outputName[1024];
  char inputName[1024];

  sprintf(outputName, "%s.%s", (const char *) prefix, documentKeySuffix);
  sprintf(inputName, "%s.%s", (const char *) prefix, documentIndexSuffix);
  // createKey(inputName, outputName, docids, byteOffsetOfDoc);
  createKey(inputName, outputName, docids, NULL); 
  documentKeyFile = outputName;

  sprintf(outputName, "%s.%s", (const char *) prefix, wordKeySuffix);
  sprintf(inputName, "%s.%s", (const char *) prefix, wordIndexSuffix);
  createKey(inputName, outputName, terms, NULL);
  wordKeyFile = outputName;
}

DocInfoList * BasicIndex::docInfoList(int termID)
{
  wordIndexStream.seekg(woffset[termID]);
  int w;
  int count;
  wordIndexStream.read((char *)&w, sizeof(int));
  wordIndexStream.read((char *)&count, sizeof(int));

  int size = pCompressor->decompress(wordIndexStream, tmpdarr)/2;
  return (new BasicDocInfoList(tmpdarr,size));
}


TermInfoList *BasicIndex::termInfoList(int docID) 
{
  documentIndexStream.seekg(doffset[docID]);
  int d, count;
  documentIndexStream.read((char *)&d, sizeof(int));
  documentIndexStream.read((char *)&count, sizeof(int));
  int size = pCompressor->decompress(documentIndexStream, tmpwarr)/2;
  return (new BasicTermInfoList(tmpwarr,size));
  
}

int BasicIndex::docCount(int t)
{
  int w;
  int count;
  wordIndexStream.seekg(woffset[t]);
  wordIndexStream.read((char *)&w, sizeof(int));
  wordIndexStream.read((char *)&count, sizeof(int));
  //  return (pCompressor->decompress(wordIndexStream, tmpdarr)/2);
  wordIndexStream.read((char *)&count, sizeof(int));
  return count/2;
}


