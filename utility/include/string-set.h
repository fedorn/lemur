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
