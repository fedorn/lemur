%option noyywrap
%option never-interactive
%option prefix="reuters" 
%{

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

#include "ReutersParser.hpp"

#include <cstring>

#include <cctype>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <fstream>

#define B_DOC 1
#define E_DOC 2
#define B_DOCHDR 12
#define E_DOCHDR 13
#define F_DOCNO 3
#define WORD 11
#define UPWORD 18
#define CONTRACTION 14
#define ACRONYM 15
#define ACRONYM2 17
#define UNKNOWN 16
#define B_TEXT 19
#define E_TEXT 20


extern FILE * reutersin;
extern char * reuterstext;



%}
%%

"</newsitem>"                       { return E_DOC; }
"<newsitem "[^>]*">"                { return F_DOCNO; }
"<text>"                            { return B_TEXT; }
"</text>"                           { return E_TEXT; }
"<headline>"                        { return B_TEXT; }
"</headline>"                       { return E_TEXT; }
"<title>"                           { return B_TEXT; }
"</title>"                          { return E_TEXT; }
"<"[/]?[a-zA-Z][^>]*">"             { /* zap tags */ }
"<!"[^-][^>]*">"                    { /* zap other tags*/}
[&][a-zA-Z]+[;]                     { /* zap symbols */ }
[&][#][0-9]*[;]                     { /* zap symbols */ } 
[A-Z][A-Z]+                         { return UPWORD; }
[a-zA-Z0-9]+                        { return WORD; }
[A-Z][A-Z]+((\')?[s])?              { return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+             { return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { return ACRONYM; }
[\n]                                { /* zap newline */ }
.                                   { return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

ReutersParser::ReutersParser() {
  state = OUTER;
}



void 
ReutersParser::parse(char * filename) {

  reutersin = fopen(filename, "r");
  
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = reuterslex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      break;
    
    case F_DOCNO:
      {
        // Extract the document number and pass it on.
        char * id = strstr(reuterstext + 9, "itemid");
        id = strstr(id, "\"");
        id++;
        char * ide = strstr(id, "\"");
	*ide = '\0';
        if (textHandler != NULL) textHandler->foundDoc(id);

	state = DOC;
	break;
      }

    case B_TEXT:
      state = TEXT;
      break;

    case E_TEXT:
      state = DOC;
      break;
         
    case WORD:
      if (state == TEXT) {
        // convert word to lowercase and pass it on
        char * c; 
	for (c = reuterstext; *c != '\0'; *(c++) = tolower(*c));   	  
	if (textHandler != NULL && c != reuterstext) 
	  textHandler->foundWord(reuterstext);
	 
      }
      break;

    case CONTRACTION:
      if (state == TEXT) {
	// strip suffix and convert to lowercase
	char * c;
	for (c = reuterstext; *c != '\''; *(c++) = tolower(*c));	
	*c = '\0';
	if (textHandler != NULL && c != reuterstext) 
          textHandler->foundWord(reuterstext);
	 
      }
      break;

    case UPWORD:
      if (state == TEXT) {
        char * c;
        if (! isAcronym(reuterstext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (c = reuterstext; *c != '\0'; *(c++) = tolower(*c));
	} else {
	  for (c = reuterstext; *c != '\0'; c++);
	}	
	if (textHandler != NULL) textHandler->foundWord(reuterstext);	

	 
      }
      break;

    case ACRONYM:
      if (state == TEXT) {
	char * e = reuterstext;
	for (char * c = reuterstext; *c != '\0'; *c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(reuterstext)) {
	  // put in lowercase if not in the acronym list
	  for (char * c = reuterstext; *c != '\0'; *(c++) = tolower(*c));	 
	}
	if (textHandler != NULL && e != reuterstext) 
	  textHandler->foundWord(reuterstext);
	 
      }
      break;
    
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
	if (!isAcronym(reuterstext)) {
	  // put in lowercase if not in the acronym list
	  for (c = reuterstext; *c != '\0'; *(c++) = tolower(*c));	 
	}
	// strip suffix
	for (c = reuterstext; *c != '\'' && *c != '\0' && *c != 's'; c++);
        *c = '\0';
	if (textHandler != NULL && c != reuterstext) 
	  textHandler->foundWord(reuterstext);	
	 
      }      
      break;

    }

  }
  fclose(reutersin);

}

