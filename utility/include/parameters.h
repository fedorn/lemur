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

#ifndef _PARAM_H_
#define _PARAM_H_

#include <stdio.h>
/* #include "util.h" */

#ifdef WIN32
typedef __int64 INT64;
#else
typedef long long INT64;
#endif

INT64  param_getll(const char *parameter_name, INT64 default_value);

int    param_geti(const char *parameter_name, int default_value);

double param_getf(const char *parameter_name, double default_value);

char  *param_getc(const char *parameter_name, char *default_value);

char  *param_gets(const char *parameter_name);

int param_getb(const char *parameter_name, int default_value);
  /* Returns true if the value of <parameter_name> is 1, true, or yes,
     (case insensitive), false for any other value, and default_value
     for no value. */

int param_symvarie(const char *parameter_name, int *returned_value);
  /* Returns true if a value was found, false otherwise */

int param_symvarfe(const char *parameter_name, double *returned_value);
  /* Ditto */

int param_symvarce(const char *parameter_name, char *returned_value);
  /* Ditto. Note that the second argument is a "pointer to a char *",
     i.e., approximately a pointer to a string. */

void param_set(const char *parameter_name, char *new_value);
  /* Changes the value of ddinf parameter <parameter_name>. This can be
     used to communicate with subroutines which expect ddinf
     parameters without having to make sure they exist in the ddinf file.
     Note, however, that values assigned in the ddinf file are 
     OVERRIDDEN by a call to param_set. */
  /* One might want to implement a param_add which would allow adding
     new ddinf parameters within a program, but which could not
     override values from the ddinf file. */

void  param_dump (FILE *stream);
  /* Writes the current ddinf bindings to a stream */

void  param_push_prefix (const char *hot_prefix);
  /* Push the current prefix to be applied to all ddnames */

void  param_pop_prefix (void);
  /* Pop the current prefix */

int param_push_file (const char *fn);
  /* Use the file for all bindings */

char *param_pop_file (void);
  /* Pop current bindings */

void param_display();
/* display parameter bindings */ 
   
#endif

