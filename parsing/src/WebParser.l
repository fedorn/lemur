%option noyywrap
%option never-interactive
%option prefix="web"
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

#include "WebParser.hpp"

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
#define B_DOCOLDNO 4
#define E_DOCOLDNO 5
#define B_SCRIPT 6
#define E_SCRIPT 7
#define B_COMMENT 8
#define E_COMMENT 9
#define WORD 11
#define UPWORD 18
#define CONTRACTION 14
#define ACRONYM 15
#define ACRONYM2 17
#define UNKNOWN 16


extern FILE * webin;
extern char * webtext;



%}
%%

"<DOC>"                                 { return B_DOC; }
"</DOC>"                                { return E_DOC; }
"<DOCHDR>"                              { return B_DOCHDR; }
"</DOCHDR>"                             { return E_DOCHDR; }
"<DOCNO>"[[:space:]]*[A-Za-z0-9\-]+[[:space:]]*"</DOCNO>" { return F_DOCNO; }
"<DOCOLDNO>"                            { return B_DOCOLDNO; }
"</DOCOLDNO>"                           { return E_DOCOLDNO; }
"<"[sS][cC][rR][iI][pP][tT][^>]*">"     { return B_SCRIPT; }
"</"[sS][cC][rR][iI][pP][tT]">"         { return E_SCRIPT; } 
"<!--"                                  { return B_COMMENT; }
"-->"                                   { return E_COMMENT; }
"<"[/]?[a-zA-Z][^>]*">"                 { /* zap tags */ }
"<!"[^-][^>]*">"                        { /* zap other tags*/}
[&][a-zA-Z]+[;]                         { /* zap symbols */ }
[&][#][0-9]*[;]                         { /* zap symbols */ } 
[A-Z][A-Z]+                             { return UPWORD; }
[a-zA-Z0-9]+                            { return WORD; }
[A-Z][A-Z]+((\')?[s])?                  { return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+                 { return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                       { return ACRONYM; }
[\n]                                    { /* zap newline */ }
.                                       { return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define DOCOLDNO 7
#define DOCHDR 3
#define SCRIPT 4
#define COMMENT 5
#define TAG 6
#define SYMBOL 8

WebParser::WebParser() {
  state = OUTER;
}

long WebParser::fileTell() {
  int offset = yy_c_buf_p-YY_CURRENT_BUFFER->yy_ch_buf;
  if (webin) {
    long begin = ftell(webin)-YY_CURRENT_BUFFER->yy_n_chars;
    return begin+offset;
  }
  return offset;
}

void 
WebParser::parseFile(char * filename) {
  
  webin = fopen(filename, "r");
  doParse();
  fclose(webin);
}

void WebParser::parseBuffer (char* buf, int len) {
  yy_scan_bytes(buf, len);
  doParse();
}

void WebParser::doParse() {

  int tok;

  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the DOC state, text is parsed.
  while (tok = weblex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      break;
    
    case B_DOC:
      docpos = fileTell() - webleng;
      state = DOC;
      break;

    case F_DOCNO:
      {
        // Extract the document number and pass it on.
	char * dn = webtext + 7;
	while (isspace(*dn)) dn++;
	char * de = dn;
	while (!isspace(*de) && *de != '<') de++;
	*de = '\0';
	if (textHandler != NULL) textHandler->foundDoc(dn);

	state = DOC;
	break;
      }

    case B_DOCHDR:
      // DOCHDRs are ignored
      state = DOCHDR;
      break;

    case E_DOCHDR:
      state = DOC;
      break;

    case B_DOCOLDNO:
      // DOCOLDNOs are ignored
      state = DOCOLDNO;
      break;

    case E_DOCOLDNO:
      state = DOC;
      break;

    case B_COMMENT:
      // Comments are ignored
      // Can only transition to the COMMENT state if in the DOC state,
      // handling comments within script tags appropriately.
      if (state == DOC)
        state = COMMENT;
      break;

    case E_COMMENT:
      if (state == COMMENT)
        state = DOC;
      break;

    case B_SCRIPT:
      // Script fields are ignored
      // Can only transition to a SCRIPT state if in the DOC state,
      // handling script tags within comment tags appropriately.
      if (state == DOC)
        state = SCRIPT;
      break;

    case E_SCRIPT:
      if (state == SCRIPT)
        state = DOC;
      break;
         
    case WORD:
      if (state == DOC) {
        // put the word in lowercase and pass it on
	for (char * c = webtext; *c != '\0'; c++)
	  *(c) = tolower(*c);
	if (textHandler != NULL) textHandler->foundWord(webtext);
      }
      break;

    case CONTRACTION:
      if (state == DOC) {
        // strip the suffx, put the word in lowercase, pass it on
	char * c;
	for (c = webtext; *c != '\''; c++)
	  *(c) = tolower(*c);	
        *c = '\0';
	if (textHandler != NULL) textHandler->foundWord(webtext);
      }
      break;

    case UPWORD:
      if (state == DOC) {
        if (! isAcronym(webtext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = webtext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}
	if (textHandler != NULL) textHandler->foundWord(webtext);	
      }
      break;

    case ACRONYM:
      if (state == DOC) {
	char * e = webtext;
	// strip periods
	for (char * c = webtext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(webtext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = webtext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	if (textHandler != NULL) textHandler->foundWord(webtext);
      }
      break;
    
    case ACRONYM2:
      if (state == DOC) {
        char * c;
	if (!isAcronym(webtext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (c = webtext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	// strip the suffix
	for (c = webtext; *c != '\'' && *c != '\0' && *c != 's'; c++);
        *c = '\0';
	if (textHandler != NULL) textHandler->foundWord(webtext);	
      }      
      break;

    }

  }
}

