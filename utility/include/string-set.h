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

#ifndef _STRINGSETH_
#define _STRINGSETH_

#include <stdio.h>

typedef struct {
    int size;       /* the current size of the table */
    int count;      /* number of things currently in the table */
    char ** table;  /* the table itself */
} String_set;

String_set * string_set_create(void);
char *       string_set_add(const char * source_string, String_set * ss);
char *       string_set_lookup(const char * source_string, String_set * ss);
void         string_set_delete(String_set *ss);
void         string_set_display(FILE * fp, String_set *ss);

#endif
