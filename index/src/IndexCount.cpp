 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <assert.h>
#include "FastList.hpp"
#include "FLL.hpp"
#include "IndexCount.hpp"

MemList *FastList<IndexCount>::pMemList = NULL;
MemList *FLL<IndexCount>::pMemList = NULL;
MemList *IndexCount::pMemList = NULL;

template class FastList<IndexCount >;
template class FLL<IndexCount >;

void IndexCount::UseMemory(MemList &memList)
{
  FastList<IndexCount>::UseMemory(memList);
  FLL<IndexCount>::UseMemory(memList);
  pMemList = &memList;
}

void IndexCount::insertionsort(IndexCount * a, int n) {
    int i,j;
    IndexCount v;
    for (i=1; i<n; i++) {
	v = a[i];
	for (j=i-1; j>=0; j--) {
	    if (v.count >= a[j].count) break;
	    a[j+1] = a[j];
	}
	a[j+1] = v;
    }
}

void IndexCount::rquicksort(IndexCount * a, int n) {
    int i, j, pivot;
    IndexCount v, t;

    if (n <= 1) {return;}
    if (n < 20) {
	insertionsort(a,n);
	return;
    }

    pivot = rand() % n;
    t = a[pivot]; a[pivot] = a[n-1]; a[n-1] = t;

    v = a[n-1];
    i = -1;
    j = n-1;
    for (;;) {
	while (a[++i].count < v.count);
	while (a[--j].count > v.count);
	if (i >= j) break;
	t = a[i]; a[i] = a[j]; a[j] = t;
    }
    t = a[i]; a[i] = a[n-1]; a[n-1] = t;
    rquicksort(a, i);
    rquicksort(a+i+1, n-i-1);
}

// Sort in increasing order 
int IndexCount::rqSort(int n, IndexCount * a) {
    int s, i;
    IndexCount t;

    if (n <= 1) return 1;

    /* Put the smallest element in the leftmost position, then
       quicksort the rest of the array */

    s = n-1;
    for (i=n-2; i >= 0; i--) {
	if (a[i].count < a[s].count) s = i;
    }

    t = a[0];
    a[0] = a[s];
    a[s] = t;

    rquicksort(a+1, n-1);
    return 0;
}
