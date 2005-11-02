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
 * http://www.lemurproject.org/license.html
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
#define B_HEADLN 21
#define E_HEADLN 22
#define B_TITLE 23
#define E_TITLE 24
#define B_DATE 25
#define E_DATE 26

extern FILE * trecin;
extern char * trectext;

long trecpos;

%}
%%

"<DOC>"                             { trecpos += trecleng; return B_DOC; }
"</DOC>"                            { trecpos += trecleng; return E_DOC; }
"<DOCNO>"[^<]*"</DOCNO>"            { trecpos += trecleng; return F_DOCNO; }
"<TEXT>"                            { trecpos += trecleng; return B_TEXT; }
"</TEXT>"                           { trecpos += trecleng; return E_TEXT; }
"<HL>"                              { trecpos += trecleng; return B_HEADLN; }
"</HL>"                             { trecpos += trecleng; return E_HEADLN; }
"<HEAD>"                            { trecpos += trecleng; return B_HEADLN; }
"</HEAD>"                           { trecpos += trecleng; return E_HEADLN; }
"<HEADLINE>"                        { trecpos += trecleng; return B_HEADLN; }
"</HEADLINE>"                       { trecpos += trecleng; return E_HEADLN; }
"<TITLE>"                             { trecpos += trecleng; return B_TITLE; }
"</TITLE>"                            { trecpos += trecleng; return E_TITLE; }
"<TTL>"                             { trecpos += trecleng; return B_TITLE; }
"</TTL>"                            { trecpos += trecleng; return E_TITLE; }
"<DD>"                              { trecpos += trecleng; return B_DATE; }
"</DD>"                             { trecpos += trecleng; return E_DATE; }
"<DATE>"                            { trecpos += trecleng; return B_DATE; }
"</DATE>"                           { trecpos += trecleng; return E_DATE; }
"<LP>"                              { trecpos += trecleng; return B_TEXT; }
"</LP>"                             { trecpos += trecleng; return E_TEXT; }
"<LEADPARA>"                        { trecpos += trecleng; return B_TEXT; }
"</LEADPARA>"                       { trecpos += trecleng; return E_TEXT; }
"<"[/]?[a-zA-Z][^>]*">"             { trecpos += trecleng; /* zap tags */ }
"<!"[^-][^>]*">"                   { trecpos += trecleng; /* zap other tags*/}
"<!--"[^>]*"-->"     { trecpos += trecleng; /* zap sgml/html comments*/}


[&][a-zA-Z]+[;]                     { trecpos += trecleng; /* zap symbols */ }
[&][#][0-9]*[;]                    { trecpos += trecleng; /* zap symbols */ } 
[A-Z][A-Z]+                         { trecpos += trecleng; return UPWORD; }
[a-zA-Z0-9]+                        { trecpos += trecleng; return WORD; }
"\*eos"                        { trecpos += trecleng; return WORD; /* *eos token for summarization module */}
[A-Z][A-Z]+((\')?[s])?              { trecpos += trecleng; return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+            { trecpos += trecleng; return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { trecpos += trecleng; return ACRONYM; }
[\n]                                { trecpos += trecleng; /* zap newline */ }
.                                   { trecpos += trecleng; return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

lemur::parse::TrecParser::TrecParser() {
  state = OUTER;
// tags for signaling the beginning and ending of an element
// for ElemDocMgr, actual tag values don't matter as long as 
// begin and end tags send same value. Retrieve later using same string
  begelem.setName("B_ELEM");
  endelem.setName("E_ELEM");
  begelem.setValue("\0");
  endelem.setValue("\0");
  iden = identifier;
}


long lemur::parse::TrecParser::fileTell() const {
  return trecpos;
}

void lemur::parse::TrecParser::parseFile(const string &filename) {  
  parsefile = filename;
  trecpos = 0;
  trecin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(trecin);
}

void lemur::parse::TrecParser::parseBuffer(char* buf, int len) {
  int tpos = trecpos;
  trecpos = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  trecpos = tpos;
}

void lemur::parse::TrecParser::doParse() {
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = treclex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      if (textHandler != NULL) textHandler->foundEndDoc();
      break;
    
    case B_DOC:
      state = DOC;
      docpos = trecpos - trecleng;
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

    case B_HEADLN:
      state = TEXT;
      begelem.setValue("HEADLINE");
      proplist.clear();
      proplist.setProperty(&begelem);
      if (textHandler)
        textHandler->foundToken(BEGINTAG, trectext, trectext, &proplist);
      break;

    case B_TITLE:
      state = TEXT;
      begelem.setValue("TITLE");
      proplist.clear();
      proplist.setProperty(&begelem);
      if (textHandler)
        textHandler->foundToken(BEGINTAG, trectext, trectext, &proplist);
      break;

    case E_HEADLN:
      state = DOC;
      endelem.setValue("HEADLINE");
      proplist.clear();
      proplist.setProperty(&endelem);
      if (textHandler)
        textHandler->foundToken(ENDTAG, trectext, trectext, &proplist);
      break;

    case E_TITLE:
      state = DOC;
      endelem.setValue("TITLE");
      proplist.clear();
      proplist.setProperty(&endelem);
      if (textHandler)
        textHandler->foundToken(ENDTAG, trectext, trectext, &proplist);
      break;

    case B_DATE:
      // state = DOC;
      begelem.setValue("DATE");
      proplist.clear();
      proplist.setProperty(&begelem);
      if (textHandler)
        textHandler->foundToken(BEGINTAG, trectext, trectext, &proplist);
      break;

    case E_DATE:
      // state = DOC;
      endelem.setValue("DATE");
      proplist.clear();
      proplist.setProperty(&endelem);
      if (textHandler)
        textHandler->foundToken(ENDTAG, trectext, trectext, &proplist);
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
	char * c, *tmpc;
	int len = 0, diff;
	for (c = trectext; *c != '\''; c++, len++)
	  *(c) = tolower(*c);	
	*c = '\0';
	diff = trecleng - len;
	trecpos -= diff; /* change endpoint for docMgr usage. */
	if (textHandler != NULL) textHandler->foundWord(trectext);
	trecpos += diff;
	c++;
	for (tmpc = c; *tmpc != '\0'; tmpc++)
	  *(tmpc) = tolower(*tmpc);	
	if (textHandler != NULL) textHandler->foundWord(c);
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
	int len = 0, diff;
	char * e = trectext;
	for (char * c = trectext; *c != '\0'; c++) {
	  if (*c != '.') {
	    *(e++) = *c;
	  } else {
	    len++;
	  }
	}
	// this is broken, docMgr assumes tokens never shrink.
	// Bleah.
	diff = trecleng - len;
	trecpos -= diff; /* change endpoint for docMgr usage. */
	*e = '\0';
	if (!isAcronym(trectext)) {
	  // put in lowercase if not in the acronym list
	  for (char * c = trectext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	if (textHandler != NULL) textHandler->foundWord(trectext);
	trecpos += diff;
      }
      break;
    
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
	int len = 0, diff;
	// strip suffix
	for (c = trectext; *c != '\'' && *c != '\0' && *c != 's'; c++, len++);
        *c = '\0';
	if (!isAcronym(trectext)) {
	  // put in lowercase if not in the acronym list
	  for (c = trectext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	diff = trecleng - len;
	trecpos -= diff; /* change endpoint for docMgr usage. */
	if (textHandler != NULL) textHandler->foundWord(trectext);	
	trecpos += diff;
      }      
      break;

    }

  }

}

