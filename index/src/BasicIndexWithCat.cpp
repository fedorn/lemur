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


#include "BasicIndexWithCat.hpp"
#include "Exception.hpp"
#include <iostream>

//const char lemur::index::BasicIndexWithCat::OOVSTRING[]="[OOV]";
const string lemur::index::BasicIndexWithCat::OOVSTRING = "[OOV]"; 


lemur::index::BasicIndexWithCat::BasicIndexWithCat(Index &wordIndex, Index &categoryIndex, bool catIsTerm):
  baseIndex(&wordIndex), catIndex(&categoryIndex), catAsTerm(catIsTerm) 
{ 
  // synchronize doc id lexicons
  base2cat = new int[baseIndex->docCount()+1];
  int i;
  int baseMax = baseIndex->docCount();

  // all index translation defaults to "OOV".
  for (i=0; i<=baseMax; i++) {
    base2cat[i] = (catAsTerm? catIndex->term(OOVSTRING): catIndex->document(OOVSTRING));
  }

  int catMax;
  int id;
  if (catAsTerm) {
    catMax = catIndex->docCount();
  } else {
    catMax = catIndex->termCountUnique();
  }

  if (catMax > baseMax) 
    throw lemur::api::Exception("BasicIndexWithCat", "categor index has more doc ids than base index");
  cat2base = new int[catMax+1];

  for (i=0; i<=catMax; i++) {
    id = (catAsTerm? baseIndex->document(catIndex->document(i)):
          baseIndex->document(catIndex->term(i)));
    base2cat[id]=i;
    cat2base[i] =id; 
  }
}

void lemur::index::BasicIndexWithCat::startCatIDIteration(int docID) const
{
  if (catAsTerm) {
    if(base2cat[docID]==catIndex->term(OOVSTRING)){ tmInfoBuffer=NULL; return; }
    tmInfoBuffer = catIndex->termInfoList(base2cat[docID]);
    tmInfoBuffer->startIteration();
  } else { 
    if(base2cat[docID]==catIndex->document(OOVSTRING)){ docInfoBuffer=NULL; return; }
    docInfoBuffer = catIndex->docInfoList(base2cat[docID]);
    docInfoBuffer->startIteration();
  }
  
}

bool lemur::index::BasicIndexWithCat::hasMoreCatID() const
{ 
  if (catAsTerm) {
    if(!tmInfoBuffer) return false;
    return (tmInfoBuffer->hasMore());
  } else {
    if(!docInfoBuffer) return false;
    return (docInfoBuffer->hasMore());
  }
}


int lemur::index::BasicIndexWithCat::nextCatID() const
{
  if (catAsTerm) {
    tmInfo = tmInfoBuffer->nextEntry();
    return (tmInfo->termID());
  } else {
    docInfo = docInfoBuffer->nextEntry();
    return (docInfo->docID());
  }
}




void lemur::index::BasicIndexWithCat::startDocIDIteration(int catID) const
{
  if (!catAsTerm) { // i.e., cat is actually doc
    tmInfoBuffer = catIndex->termInfoList(catID);
    tmInfoBuffer->startIteration();
  } else {
    docInfoBuffer = catIndex->docInfoList(catID);
    docInfoBuffer->startIteration();
  }
  
}

bool lemur::index::BasicIndexWithCat::hasMoreDocID() const
{
  if (!catAsTerm) {
    return (tmInfoBuffer->hasMore());
  } else {
    return (docInfoBuffer->hasMore());
  }
}


int lemur::index::BasicIndexWithCat::nextDocID() const
{
  if (!catAsTerm) {
    tmInfo = tmInfoBuffer->nextEntry();
    return (cat2base[tmInfo->termID()]);
  } else {
    docInfo = docInfoBuffer->nextEntry();
    return (cat2base[docInfo->docID()]);
  }
}




