%option noyywrap
%option never-interactive
%option prefix="trec" 
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

#include "TrecParser.hpp"

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


extern FILE * trecin;
extern char * trectext;



%}
%%

"<DOC>"                             { return B_DOC; }
"</DOC>"                            { return E_DOC; }
"<DOCNO>"[^<]*"</DOCNO>"            { return F_DOCNO; }
"<TEXT>"                            { return B_TEXT; }
"</TEXT>"                           { return E_TEXT; }
"<HL>"                              { return B_TEXT; }
"</HL>"                             { return E_TEXT; }
"<HEAD>"                            { return B_TEXT; }
"</HEAD>"                           { return E_TEXT; }
"<HEADLINE>"                        { return B_TEXT; }
"</HEADLINE>"                       { return E_TEXT; }
"<TTL>"                             { return B_TEXT; }
"</TTL>"                            { return E_TEXT; }
"<LP>"                              { return B_TEXT; }
"</LP>"                             { return E_TEXT; }
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

TrecParser::TrecParser() {
  state = OUTER;
}


long TrecParser::fileTell() {
  int offset = yy_c_buf_p-YY_CURRENT_BUFFER->yy_ch_buf;
  if (trecin) {
    long begin = ftell(trecin)-YY_CURRENT_BUFFER->yy_n_chars;
    return begin+offset;
  } 
  return offset;
}

void TrecParser::parseFile(char * filename) {  
  trecin = fopen(filename, "r");
  doParse();
  fclose(trecin);
}

void TrecParser::parseBuffer(char* buf, int len) {
  yy_scan_bytes(buf, len);
  doParse();
}

void TrecParser::doParse() {
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = treclex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      break;
    
    case B_DOC:
      state = DOC;
      docpos = fileTell() - trecleng;
      break;

    case F_DOCNO:
      {
        // Extract the document number and pass it on.
	char * dn = trectext + 7;
	while (isspace(*dn)) dn++;
	char * de = dn;
	while (!isspace(*de) && *de != '<') de++;
	*de = '\0';
	if (textHandler != NULL) textHandler->foundDoc(dn);

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
	for (char * c = trectext; *c != '\0'; c++)
	  *(c) = tolower(*c);
	if (textHandler != NULL) textHandler->foundWord(trectext);
      }
      break;

    case CONTRACTION:
      if (state == TEXT) {
	// strip suffix and convert to lowercase
	char * c;
	for (c = trectext; *c != '\''; c++)
	  *(c) = tolower(*c);	
	*c = '\0';
	if (textHandler != NULL) textHandler->foundWord(trectext);
      }
      break;

    case UPWORD:
      if (state == TEXT) {
        
        if (! isAcronym(trectext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = trectext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}
	if (textHandler != NULL) textHandler->foundWord(trectext);	

      }
      break;

    case ACRONYM:
      if (state == TEXT) {
	char * e = trectext;
	for (char * c = trectext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(trectext)) {
	  // put in lowercase if not in the acronym list
	  for (char * c = trectext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	if (textHandler != NULL) textHandler->foundWord(trectext);
      }
      break;
    
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
	if (!isAcronym(trectext)) {
	  // put in lowercase if not in the acronym list
	  for (c = trectext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	// strip suffix
	for (c = trectext; *c != '\'' && *c != '\0' && *c != 's'; c++);
        *c = '\0';
	if (textHandler != NULL) textHandler->foundWord(trectext);	
      }      
      break;

    }

  }

}

