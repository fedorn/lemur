#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "error.h"

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
   assert(0); /* Always fail and print out this file name */
}

void claim(int ex,char * s) { 
    if (!(ex)) {				
	printf("Assertion failed: %s\n", s);
	exit(1);				
    }						
}

