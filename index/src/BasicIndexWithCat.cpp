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

//const char BasicIndexWithCat::OOVSTRING[]="[OOV]";
const string BasicIndexWithCat::OOVSTRING = "[OOV]"; 

#include "Exception.hpp"
#include <iostream>

BasicIndexWithCat::BasicIndexWithCat(Index &wordIndex, Index &categoryIndex, bool catIsTerm):
    baseIndex(&wordIndex), catIndex(&categoryIndex), catAsTerm(catIsTerm) 
{ 
  // synchronize doc id lexicons
  base2cat = new int[baseIndex->docCount()];
  int i;
  int baseMax = baseIndex->docCount();

  // all index translation defaults to "OOV".
  for (i=0; i<baseMax; i++) {
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
    throw Exception("BasicIndexWithCat", "categor index has more doc ids than base index");
  cat2base = new int[catMax];

  for (i=0; i<catMax; i++) {
    id = (catAsTerm? baseIndex->document(catIndex->document(i)):
	  baseIndex->document(catIndex->term(i)));
    base2cat[id]=i;
    cat2base[i] =id; 
  }
}

void BasicIndexWithCat::startCatIDIteration(int docID) const
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

bool BasicIndexWithCat::hasMoreCatID() const
{ 
  if (catAsTerm) {
    if(!tmInfoBuffer) return false;
    return (tmInfoBuffer->hasMore());
  } else {
    if(!docInfoBuffer) return false;
    return (docInfoBuffer->hasMore());
  }
}


int BasicIndexWithCat::nextCatID() const
{
  if (catAsTerm) {
    tmInfo = tmInfoBuffer->nextEntry();
    return (tmInfo->id());
  } else {
    docInfo = docInfoBuffer->nextEntry();
    return (docInfo->docID());
  }
}




void BasicIndexWithCat::startDocIDIteration(int catID) const
{
  if (!catAsTerm) { // i.e., cat is actually doc
    tmInfoBuffer = catIndex->termInfoList(catID);
    tmInfoBuffer->startIteration();
  } else {
    docInfoBuffer = catIndex->docInfoList(catID);
    docInfoBuffer->startIteration();
  }
  
}

bool BasicIndexWithCat::hasMoreDocID() const
{
  if (!catAsTerm) {
    return (tmInfoBuffer->hasMore());
  } else {
    return (docInfoBuffer->hasMore());
  }
}


int BasicIndexWithCat::nextDocID() const
{
  if (!catAsTerm) {
    tmInfo = tmInfoBuffer->nextEntry();
    return (cat2base[tmInfo->id()]);
  } else {
    docInfo = docInfoBuffer->nextEntry();
    return (cat2base[docInfo->docID()]);
  }
}




