/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

// David Fisher
// init: 05/22/2002
// Abstract interface for clustering algorithms.
#include "Cluster.hpp"
#include "OfflineCluster.hpp"
#include "IndexedReal.hpp"
// The Cluster object.
using namespace lemur::api;

lemur::cluster::Cluster::Cluster(int cid, const lemur::api::Index &db, 
                                 const SimilarityMethod &sim) :
  id(cid), ind(db), similarity(sim) {
  char myName[NAMESIZE];
  sprintf(myName, "%d", id);
  name = myName;
  size = 0;
}

lemur::cluster::Cluster::~Cluster() {
}

void lemur::cluster::Cluster::setId(int cid) {
  id = cid;
}
void lemur::cluster::Cluster::setName(const string &newName){
  name = newName;
}

int lemur::cluster::Cluster::getId() const { return id; }

const string &lemur::cluster::Cluster::getName() const { 
  return (name); 
}
   
const vector <lemur::cluster::ClusterElt> *lemur::cluster::Cluster::getIds() 
  const { return &ids; }

vector<DOCID_T> lemur::cluster::Cluster::getDocIds() const {
  vector<DOCID_T> res;
  for (vector<ClusterElt>::const_iterator it = ids.begin();
       it != ids.end(); it++) 
    if ((*it).myType == DOC_ELT)
      res.push_back((*it).id);
  return res;
}

void lemur::cluster::Cluster::add(const ClusterElt &elt) {
  ids.push_back(elt);
  size++;
}

void lemur::cluster::Cluster::add(vector<lemur::api::DOCID_T> docids) {
  ClusterElt fred;
  for (int i = 0; i < docids.size(); i++) {
    fred.id = docids[i];
    fred.myType = DOC_ELT;
    add(fred);
  }
}

void lemur::cluster::Cluster::merge(const Cluster *c) {
  const vector <ClusterElt> *newIds = c->getIds();
  for (vector<ClusterElt>::const_iterator it = newIds->begin();
       it != newIds->end(); it++)
    add(*it);
}

vector<lemur::cluster::Cluster *> lemur::cluster::Cluster::split(int numParts){
  vector<Cluster *> *clusters;
  // Need to pass in other parameters here.
  OfflineCluster *cdb = new OfflineCluster(ind);
  clusters = cdb->kMeans(getDocIds(), numParts);
  vector<Cluster *>results(*clusters);
  delete(clusters);
  delete(cdb);
  return results;
}

void lemur::cluster::Cluster::remove(const ClusterElt &elt) {
  vector<ClusterElt>::iterator it = ids.begin();
  while (it != ids.end() && 
         ((*it).myType != elt.myType || (*it).id != elt.id)) it++;
  if (it != ids.end())
    ids.erase(it);
  size--;
}

bool lemur::cluster::Cluster::read(ifstream &in) {
  if (in.eof())
    return false;
  int len, nameLen;
  ClusterElt elt;
  in.read((char*) &id, sizeof(int));
  if (!(in.gcount() == sizeof(int)))
    return false;
  in.read((char*) &nameLen, sizeof(int));
  if (!(in.gcount() == sizeof(int)))
    return false;
  char * newName = new char[nameLen + 1];
  in.read((char*) newName, nameLen);
  if (!(in.gcount() == nameLen))
    return false;
  newName[nameLen] = '\0';
  name = newName;
  delete[](newName);
  in.read((char*) &len, sizeof(int));
  if (!(in.gcount() == sizeof(int)))
    return false;
  for (int i = 0; i < len; i++) {
    in.read((char*) &elt, sizeof(ClusterElt));
    if (!(in.gcount() == sizeof(ClusterElt)))
      return false;
    ids.push_back(elt);
  }
  size = ids.size();
  return true;
}

void lemur::cluster::Cluster::write(ofstream &out){
  int len = ids.size();
  int nameLen = name.size();
  out.write((char *) &id, sizeof(int));
  out.write((char *) &nameLen, sizeof(int));
  out.write(name.c_str(), nameLen);
  out.write((char *) &len, sizeof(int));
  for (vector<ClusterElt>::const_iterator it = ids.begin();
       it != ids.end(); it++)
    out.write((char *) &(*it), sizeof(ClusterElt));
}

void lemur::cluster::Cluster::print() const {
  cout << getName() << "(" << getId() << "): ";
  for (vector<ClusterElt>::const_iterator it = ids.begin();
       it != ids.end(); it++) {
    if ((*it).myType == DOC_ELT)
      cout << ind.document((*it).id) << " ";
    else 
      cout << "C(" << ((*it).id) << ") ";
  }
  cout << endl;
}

string lemur::cluster::Cluster::getKeyWords(int maxTerms) const {
  double maxtf = 0, df = 0;
  COUNT_T numTerms = ind.termCountUnique(); 
  IndexedRealVector results(numTerms); 
  int *tf = new int[ numTerms + 1 ];
  int *posts = new int[ numTerms + 1 ];
  int i;
  for (i = 0; i <= numTerms; i++){
    tf[i] = 0;
    posts[i] = 0;
  }
  maxtf = 0, df = 0;
  for (vector<ClusterElt>::const_iterator it = ids.begin(); 
       it != ids.end(); it++) {
    if ((*it).myType == DOC_ELT) {
      DOCID_T docID = (*it).id;
      TermInfoList *tlist = ind.termInfoList(docID);
      TermInfo *info;
      tlist->startIteration();
      while (tlist->hasMore()) {
        info = tlist->nextEntry();
        TERMID_T id = info->termID();
        COUNT_T freq = info->count();
        tf[id] += freq;
        if (tf[id] > maxtf)
          maxtf = tf[id];
        posts[id]++;
      }
      delete tlist;
      df++;
    }
  }
  results.clear();
  double score;
  for (i = 1; i <= numTerms; i++){
    score = (tf[i]/maxtf) * (posts[i]/df);
    if (score > 0)
      results.PushValue(i, score);
  }
  results.Sort();
  IndexedRealVector::iterator j;
  int count = 1; // off by one on max terms for keywords.
  string retval;
  for (j = results.begin(); j != results.end(); j++) {
    if (count > maxTerms) {
      break;
    } else {
      if (count != 1) retval += "-";
      retval += ind.term((*j).ind);
    }  
    count++;
  }
  delete[](tf);
  delete[](posts);
  return retval;
}
