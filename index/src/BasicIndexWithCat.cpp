#include "BasicIndexWithCat.hpp"

const char BasicIndexWithCat::OOVSTRING[]="[OOV]";
 
#include "Exception.hpp"

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

void BasicIndexWithCat::startCatIDIteration(int docID)
{
  if (catAsTerm) {
    tmInfoBuffer = catIndex->termInfoList(base2cat[docID]);
    tmInfoBuffer->startIteration();
  } else {
    docInfoBuffer = catIndex->docInfoList(base2cat[docID]);
    docInfoBuffer->startIteration();
  }
  
}

bool BasicIndexWithCat::hasMoreCatID()
{
  if (catAsTerm) {
    return (tmInfoBuffer->hasMore());
  } else {
    return (docInfoBuffer->hasMore());
  }
}


int BasicIndexWithCat::nextCatID()
{
  if (catAsTerm) {
    tmInfo = tmInfoBuffer->nextEntry();
    return (tmInfo->id());
  } else {
    docInfo = docInfoBuffer->nextEntry();
    return (docInfo->docID());
  }
}




void BasicIndexWithCat::startDocIDIteration(int catID)
{
  if (!catAsTerm) { // i.e., cat is actually doc
    tmInfoBuffer = catIndex->termInfoList(catID);
    tmInfoBuffer->startIteration();
  } else {
    docInfoBuffer = catIndex->docInfoList(catID);
    docInfoBuffer->startIteration();
  }
  
}

bool BasicIndexWithCat::hasMoreDocID()
{
  if (!catAsTerm) {
    return (tmInfoBuffer->hasMore());
  } else {
    return (docInfoBuffer->hasMore());
  }
}


int BasicIndexWithCat::nextDocID()
{
  if (!catAsTerm) {
    tmInfo = tmInfoBuffer->nextEntry();
    return (cat2base[tmInfo->id()]);
  } else {
    docInfo = docInfoBuffer->nextEntry();
    return (cat2base[docInfo->docID()]);
  }
}




