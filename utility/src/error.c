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


#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "error.h"
#include <stdlib.h>

char   buf[1024];
#define CRLF  printf("\n")

void error(char *fmt, ...) {
   va_list args;
   va_start(args, fmt);
   vfprintf(stderr, fmt, args); CRLF;
   va_end(args);
   fprintf(stderr, "\n");
   if (errno > 0) {
      perror(buf);
      fprintf(stderr, "errno=%d\n", errno);
      fprintf(stderr, buf);
      fprintf(stderr, "\n");
      }
   fflush(stderr);
   fflush(stdout);
   fprintf(stderr,"Program aborted due to a fatal error\n");
   exit(1);
   //   assert(0); /* Always fail and print out this file name */
}

void claim(int ex,char * s) { 
    if (!(ex)) {				
	printf("Assertion failed: %s\n", s);
	exit(1);				
    }						
}

