
/**************************************************************************/
/**************************************************************************/
/**************            ARABIC STEMMER HEADER FILE         *************/
/**************************************************************************/
/**************************************************************************/

/*

	Copyright (c) 2001 UMASS CIIR All rights reserved.
	Written by Nick Dufresne (nickd@cs.brandeis.edu)
	
	
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//#include <malloc.h>

void arabic_remove_diacritics (char *, char *);
//removes diacritics from word

void arabic_stop(char *, char *);  // only removes stops
void no_stem(char *, char *) ;	   // doesn't do anything

//normalize arabic word
void arabic_norm2(char *, char *);
void arabic_norm2_stop(char *, char *);
void arabic_light10(char *, char *);   
void arabic_light10_stop(char *, char *);   

//use this to clean up hash files created to hold the words list
void arabic_clean_up (void);


/******* set_stemmer support **********/

typedef struct {
     char *option;
     char *description;
     void (*stem_fct)(char *, char *) ;
} stem_info_t;

void show_stemmer_options() ;
void *set_stemmer(char *) ;
char *stem_phrase(char *phrase, int *numtoks, void (*stemmer)(char*,char*)) ;

#define NUMSTEMMERS 6
#define ARABIC_BREAKS  " \t\r\n~`!@#$%^&*()_-+=[]{}|\\:;\"'<>,.?/\xA1\xF7\xD7\xBA\xBF"
// Edit arabic_stem.c to add stemmers and make available to other programs

