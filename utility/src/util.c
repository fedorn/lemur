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


#define ABNORMAL_RETURN_CODE 1
#define MAX_STRING_LENGTH    65535

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include "util.h"

#ifdef __alpha__

#include <sys/mount.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>

#else

//#include <malloc.h>
#include <stdlib.h>
#define F_OK 0

#endif

char   buf[1024];
static int space_in_use=0;
static int pointers_in_use=0;
int    display_allocs=FALSE;

char *dequote (char *s) {
    static char *sbuf=NULL;
    char *t;
    int i;
    if (s[0] != '\'') return s;
    else if ((i=strlen(s)) < 2) return s;
    else if (s[i-1] != '\'') 
       error("Illegal string passed to dequote: %s", s);
    if (sbuf == NULL) 
        sbuf = (char *) malloc(MAX_STRING_LENGTH);
    t = sbuf;
    s++;
    while(*s != EOS) {
       if (*s == '\'') s++;
       *t = *s;
       s++; t++;
    }
    *t = EOS;
    return sbuf;
}

void quote_no_matter_what (const char *s, char *t) {
    *t = '\'';
    t++;
    while((*s != EOS)) {
        *t = *s;
        if (*s == '\'') {
           t++; *t = '\'';
	}
        s++; t++;
    }
    *t = '\''; t++;
    *t = EOS;
}


const char *quote (const char *s) {
    static char *sbuf=NULL;
    if (sbuf == NULL) 
        sbuf = (char *) malloc(MAX_STRING_LENGTH);
    if ( strchr(s,' ')  == NULL  && 
         strstr(s,"/*") == NULL && strstr(s,"*/") == NULL ) return s;   
    else {
       quote_no_matter_what(s, sbuf);
       return sbuf;
    }
} 


/* returns TRUE iff string only contains chars in valid. */
int verify(char *string, char *valid)
{
   int i;
   for(i=0;i<strlen(string);i++) 
      if (!strchr(valid, string[i])) return TRUE;
   return FALSE;
}


/* strips leading and trailing white space */
char * strip(char *s) {
   int i,j;
   int hit_char;

   j = 0;
   hit_char = FALSE;
   for (i=0; i<=strlen(s); ++i) { 
       if (s[i] != ' ') hit_char = TRUE;
       if (hit_char) s[j++] = s[i];
   }
   for (i=strlen(s)-1; i>0; --i) 
       if (s[i] != ' ') break;
   s[i+1] = '\0';
   return s;
}


/* converts s to upper case */
char * upper(char *s) {
   int i;
   for (i=0; i<strlen(s); ++i) s[i] = toupper(s[i]);
   return s;
}

/* converts s to lower case */
char * lower(char *s) {
   int i;
   for (i=0; i<strlen(s); ++i) s[i] = tolower(s[i]);
   return s;
}


/* queries existence of file */
int qfilef(const char *fname) {
   FILE * fp;
   if (fname == FALSE) return FALSE;
   fp = fopen(fname, "r");   
   if (fp != NULL) { 
     fclose(fp); 
     return TRUE;
   } else {
     return FALSE;
   }
}


/* returns free storage in file system */
int free_storage (char *fn) 
{
  /* uses a defunct function call. Also, not ever called */
  abort();
  /*
  struct statfs sfs;
  if (statfs(fn, &sfs) == -1)
    return -1;
  return sfs.f_bsize * sfs.f_bfree;
*/
}

/* Return an allocated duplicate of string */
char *util_strdup(char *string)
{
  int len = strlen(string);
  char *dup = (char *)malloc(len+1);

  if (dup == NULL)
    {
      perror("malloc");
      return NULL;
    }
  strcpy(dup, string);
  return dup;
}


void * util_malloc (int size) 
{
    char * p = (char *) malloc(size+sizeof(int));
    if (p == NULL) error("UTIL_MALLOC: Ran out of space. Space in use: %d (%d pointers)\n", 
                                           space_in_use, pointers_in_use);
    space_in_use += size;
    ++pointers_in_use;
    *((int *) p) = size; 
    if (display_allocs) 
       fprintf(stderr, "UTIL_MALLOC: Allocated %d bytes, %d bytes total, %d pointers\n", 
                                    size, space_in_use, pointers_in_use);
    return (void *) (p+sizeof(int));
}

void * util_calloc (int num, int size) 
{
    char * p = (char *) calloc(num*size+sizeof(int), 1);
    if (p == NULL) error("UTIL_CALLOC: Ran out of space. Space in use: %d (%d pointers)\n", 
                                           space_in_use, pointers_in_use);
    space_in_use += num*size;
    ++pointers_in_use;
    *((int *) p) = num*size;    
    if (display_allocs) 
       fprintf(stderr, "UTIL_CALLOC: Allocated %d bytes, %d bytes total, %d pointers\n", 
                                           num*size, space_in_use, pointers_in_use);
    return (void *) (p+sizeof(int));
}

void * util_realloc (void * p, int size)
{
    int oldsize;
    char *realp;
    realp = ((char *)p)-sizeof(int);
    oldsize = *((int *)(realp));
    realp = (char *) realloc(realp, size+sizeof(int));
    if (realp == NULL) error("UTIL_REALLOC: Ran out of space. Space in use: %d (%d pointers)\n", 
                                           space_in_use, pointers_in_use);
    *((int *)(realp)) = size;
    space_in_use += (size-oldsize);
    if (display_allocs) 
        fprintf(stderr, "UTIL_REALLOC: Allocated %d bytes, %d bytes total, %d pointers\n", 
                                        size, space_in_use, pointers_in_use);
    return (realp+sizeof(int));
}

void util_free (void * p)
{
    int size;
    size = *((int *) (((char *) p)-sizeof(int)));
    space_in_use -= size;
    --pointers_in_use;
    free(((char *)p)-sizeof(int));
    if (display_allocs) 
       fprintf(stderr, "UTIL_FREE: Freed up %d bytes, %d bytes remaining, %d pointers\n", 
                                        size, space_in_use, pointers_in_use);
}

int util_space_in_use (void)
{
   return space_in_use;
}

int util_pointers_in_use (void)
{
   return pointers_in_use;
}
