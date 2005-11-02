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
 * http://www.lemurproject.org/license.html
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
#define B_STYLE 19
#define E_STYLE 20
#define B_TITLE 21
#define E_TITLE 22
#define B_DATE 25
#define E_DATE 26


extern FILE * webin;
extern char * webtext;

long webloc;


%}
%%

"<DOC>"                                 {webloc += webleng; return B_DOC; }
"</DOC>"                                {webloc += webleng; return E_DOC; }
"<DOCHDR>"                              {webloc += webleng; return B_DOCHDR; }
"</DOCHDR>"                             {webloc += webleng; return E_DOCHDR; }
"<DOCNO>"[^<]*"</DOCNO>"                {webloc += webleng; return F_DOCNO; }
"<DOCOLDNO>"                            {webloc += webleng; return B_DOCOLDNO; }
"</DOCOLDNO>"                           {webloc += webleng; return E_DOCOLDNO; }
"<"[sS][tT][yY][lL][eE][^>]*">"         {webloc += webleng; return B_STYLE; }
"</"[sS][tT][yY][lL][eE]">"             {webloc += webleng; return E_STYLE; } 
"<"[sS][cC][rR][iI][pP][tT][^>]*">"     {webloc += webleng; return B_SCRIPT; }
"</"[sS][cC][rR][iI][pP][tT]">"         {webloc += webleng; return E_SCRIPT; } 
"<"[tT][iI][tT][lL][eE][^>]*">"         {webloc += webleng; return B_TITLE; }
"</"[tT][iI][tT][lL][eE]">"             {webloc += webleng; return E_TITLE; }
"<"[dD][aA][tT][eE][^>]*">"             {webloc += webleng; return B_DATE; }
"</"[dD][aA][tT][eE]">"                 {webloc += webleng; return E_DATE; }
"<!--"                                  {webloc += webleng; return B_COMMENT; }
"-->"                                   {webloc += webleng; return E_COMMENT; }
"<"[/]?[a-zA-Z][^>]*">"                 {webloc += webleng; /* zap tags */ }
"<!"[^-][^>]*">"                        {webloc += webleng; /* zap other tags*/}
[&][a-zA-Z]+[;]                         {webloc += webleng; /* zap symbols */ }
[&][#][0-9]*[;]                         {webloc += webleng; /* zap symbols */ } 
[A-Z][A-Z]+                             {webloc += webleng; return UPWORD; }
[a-zA-Z0-9]+                            {webloc += webleng; return WORD; }
"\*eos"                        { webloc += webleng; return WORD; /* *eos token for summarization module */}
[A-Z][A-Z]+((\')?[s])?                  {webloc += webleng; return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+                 {webloc += webleng; return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                       {webloc += webleng; return ACRONYM; }
[\n]                                    {webloc += webleng; /* zap newline */ }
.                                       {webloc += webleng; return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define DOCOLDNO 7
#define DOCHDR 3
#define SCRIPT 4
#define COMMENT 5
#define TAG 6
#define SYMBOL 8
#define STYLE 9

lemur::parse::WebParser::WebParser() {
  state = OUTER;
// tags for signaling the beginning and ending of an element
// for ElemDocMgr, actual tag values don't matter as long as 
// begin and end tags send same value. Retrieve later using same string
  begelem.setName("B_ELEM");
  endelem.setName("E_ELEM");
  begelem.setValue("\0");
  endelem.setValue("\0");
  iden=identifier;
}

long lemur::parse::WebParser::fileTell() const {
  return webloc;
}

void 
lemur::parse::WebParser::parseFile(const string &filename) {
  parsefile = filename;
  webloc = 0;
  webin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(webin);
}

void lemur::parse::WebParser::parseBuffer (char* buf, int len) {
  int tpos = webloc;
  webloc = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  webloc = tpos;
}

void lemur::parse::WebParser::doParse() {

  int tok;

  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the DOC state, text is parsed.
  while (tok = weblex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      if (textHandler != NULL) textHandler->foundEndDoc();
      break;
    
    case B_DOC:
      docpos = webloc - webleng;
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
         
    case B_STYLE:
      // Style fields are ignored
      // Can only transition to a STYLE state if in the DOC state,
      // handling style tags within comment tags appropriately.
      if (state == DOC)
        state = STYLE;
      break;

    case E_STYLE:
      if (state == STYLE)
        state = DOC;
      break;

    case B_TITLE:
      // title words should be indexed
      state = DOC;
      
      begelem.setValue("TITLE");
      proplist.clear();
      proplist.setProperty(&begelem);
      if (textHandler)
        textHandler->foundToken(BEGINTAG, webtext, webtext, &proplist);
      break;

    case E_TITLE:
      state = DOC;
      endelem.setValue("TITLE");
      proplist.clear();
      proplist.setProperty(&endelem);
      if (textHandler)
        textHandler->foundToken(ENDTAG, webtext, webtext, &proplist);
      break;

    case B_DATE:
      // state = DOC;
      begelem.setValue("DATE");
      proplist.clear();
      proplist.setProperty(&begelem);
      if (textHandler)
        textHandler->foundToken(BEGINTAG, webtext, webtext, &proplist);
      break;

    case E_DATE:
      // state = DOC;
      endelem.setValue("DATE");
      proplist.clear();
      proplist.setProperty(&endelem);
      if (textHandler)
        textHandler->foundToken(ENDTAG, webtext, webtext, &proplist);
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
	int len = 0, diff;
	for (c = webtext; *c != '\''; c++, len++)
	  *(c) = tolower(*c);	
        *c = '\0';
	diff = webleng - len;
	webloc -= diff; /* change endpoint for docMgr usage. */
	if (textHandler != NULL) textHandler->foundWord(webtext);
	webloc += diff;
	c++;
	if (textHandler != NULL) textHandler->foundWord(c);
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
	int len = 0, diff;
	// strip the suffix
	for (c = webtext; *c != '\'' && *c != '\0' && *c != 's'; c++, len++);
        *c = '\0';
	if (!isAcronym(webtext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (c = webtext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	diff = webleng - len;
	webloc -= diff; /* change endpoint for docMgr usage. */
	if (textHandler != NULL) textHandler->foundWord(webtext);
	webloc += diff;
      }      
      break;

    }

  }
}

