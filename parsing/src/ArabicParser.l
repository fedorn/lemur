%option noyywrap
%option never-interactive
%option prefix="Arabic" 
%{

/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "ArabicParser.hpp"

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

extern FILE * Arabicin;
extern char * Arabictext;
%}

arabic_skip [~`!@#$%^&*()_\-+=[\]{}|\\:;"'<>,.?/¡÷×º¿]
not_arabic_skip [^\r\t\n ~`!@#$%^&*()_\-+=[\]{}|\\:;"'<>,.?/¡÷×º¿]
%%

"<DOC>"                             { return B_DOC; /*1*/}
"</DOC>"                            { return E_DOC; /*2*/}
"<DOCNO>"[^<]*"</DOCNO>"            { return F_DOCNO; /*3*/}
"<TEXT>"                            { return B_TEXT; /*4*/}
"</TEXT>"                           { return E_TEXT; /*5*/}
"<HL>"                              { return B_TEXT;/*6*/ }
"</HL>"                             { return E_TEXT;/*7*/ }
"<HEAD"(ER)?">"                     { return B_TEXT;/*8*/ }
"</HEAD"(ER)?">"                    { return E_TEXT;/*9*/ }
"<HEADLINE>"                        { return B_TEXT;/*10*/ }
"</HEADLINE>"                       { return E_TEXT; /*11*/}
"<TTL>"                             { return B_TEXT;/*12*/ }
"</TTL>"                            { return E_TEXT;/*13*/ }
"<LP>"                              { return B_TEXT; /*14*/}
"</LP>"                             { return E_TEXT; /*15*/}
"<FOOTER>"                          { return B_TEXT; /*???? vl indexes*/}
"</FOOTER>"                         { return E_TEXT; /*???? vl indexes*/}
"<TRAILER>"                         { return B_TEXT; /*???? vl indexes*/}
"</TRAILER>"                        { return E_TEXT; /*???? vl indexes*/}
"<"[/]?[a-zA-Z][^>]*">"             { /* zap tags */ }
"<!"[^-][^>]*">"                    { /* zap other tags*/}
[&][a-zA-Z]+[;]                     { /* zap symbols */ }
[&][#][0-9]*[;]                     { /* zap symbols */ }
{arabic_skip}                       { /* zap Arabic punctution */ } 
[A-Z][A-Z]+                         { return UPWORD; }
[a-zA-Z0-9]+                        { return WORD; }
[A-Z][A-Z]+((\')?[s])?              { return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+             { return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { return ACRONYM; }
{not_arabic_skip}+                  { return WORD;/*16*/ }
[\n]                                { /* zap newline */ }
.                                   { return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

ArabicParser::ArabicParser() {
  state = OUTER;
}

long ArabicParser::fileTell() {
  int offset = yy_c_buf_p-YY_CURRENT_BUFFER->yy_ch_buf;
  if (Arabicin) {
    long begin = ftell(Arabicin)-YY_CURRENT_BUFFER->yy_n_chars;
    return begin+offset;
  } 
  return offset;
}

void ArabicParser::parseFile(char * filename) {  
  Arabicin = fopen(filename, "r");
  doParse();
  fclose(Arabicin);
}

void ArabicParser::parseBuffer(char* buf, int len) {
  yy_scan_bytes(buf, len);
  doParse();
}

void ArabicParser::doParse() {
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = Arabiclex()) {
    switch (tok) {	
    case E_DOC:
      state = OUTER;
      break;
    case B_DOC:
      state = DOC;
      docpos = fileTell() - Arabicleng;
      break;
    case F_DOCNO:
      {
	// Extract the document number and pass it on.
	char * dn = Arabictext + 7;
	while (isspace(*dn)) dn++;
	char * de = dn;
	while (!isspace(*de) && *de != '<') de++;
	*de = '\0';
	if (textHandler != NULL) textHandler->foundDoc(dn);
	state = TEXT;
      }
      break;
    case B_TEXT:
      state = TEXT;
      break;
    case E_TEXT:
      state = DOC;
      break;
    case WORD:
      if (state == TEXT) {
        // convert word to lowercase and pass it on
        for (char * c = Arabictext; *c != '\0'; c++)
	  *(c) = tolower(*c);
	if (textHandler != NULL) textHandler->foundWord(Arabictext);
      }
      break;
    case CONTRACTION:
      if (state == TEXT) {
	// strip suffix and convert to lowercase
	char * c;
	for (c = Arabictext; *c != '\''; c++)
	  *(c) = tolower(*c);	
	*c = '\0';
	if (textHandler != NULL) textHandler->foundWord(Arabictext);
      }
      break;
    case UPWORD:
      if (state == TEXT) {
        if (! isAcronym(Arabictext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = Arabictext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}
	if (textHandler != NULL) textHandler->foundWord(Arabictext);	
      }
      break;
    case ACRONYM:
      if (state == TEXT) {
	char * e = Arabictext;
	for (char * c = Arabictext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(Arabictext)) {
	  // put in lowercase if not in the acronym list
	  for (char * c = Arabictext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}
	if (textHandler != NULL) textHandler->foundWord(Arabictext);
      }
      break;
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
	if (!isAcronym(Arabictext)) {
	  // put in lowercase if not in the acronym list
	   for (c = Arabictext; *c != '\0'; c++)
	     *(c) = tolower(*c);	 
	}
	// strip suffix
	for (c = Arabictext; *c != '\'' && *c != '\0' && *c != 's'; c++);
        *c = '\0';
	if (textHandler != NULL) textHandler->foundWord(Arabictext);	
      }      
      break;
    }
  }
}

