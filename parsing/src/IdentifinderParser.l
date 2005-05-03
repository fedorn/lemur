%option noyywrap
%option never-interactive
%option prefix="iden"
%{

/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#include "IdentifinderParser.hpp"

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
#define WORD 11
#define UPWORD 18
#define CONTRACTION 14
#define ACRONYM 15
#define UNKNOWN 16
#define EOS 21
#define B_NE_NAME	30
#define E_NE_NAME	31
#define B_NE_TIME	32
#define E_NE_TIME	33
#define B_NE_NUMBER	34
#define E_NE_NUMBER	35

extern FILE * idenin;
extern char * identext;

long idenloc;


%}
%%

"<DOC>"                                 {idenloc += idenleng; return B_DOC; }
"</DOC>"                                {idenloc += idenleng; return E_DOC; }
"<DOCHDR>"                              {idenloc += idenleng; return B_DOCHDR; }
"</DOCHDR>"                             {idenloc += idenleng; return E_DOCHDR; }
"<DOCNO>"[^<]*"</DOCNO>"                {idenloc += idenleng; return F_DOCNO; }
"<DOCOLDNO>"                            {idenloc += idenleng; return B_DOCOLDNO; }
"</DOCOLDNO>"                           {idenloc += idenleng; return E_DOCOLDNO; }
"<ENAMEX "[^>]*">"		    { idenloc += idenleng; return B_NE_NAME; }
"<ENAMEX"[^>]*">"		    { idenloc += idenleng; return B_NE_NAME; }
"</ENAMEX>"			    { idenloc += idenleng; return E_NE_NAME; }
"<TIMEX "[^>]*">"		    { idenloc += idenleng; return B_NE_TIME; }
"<TIMEX"[^>]*">"		    { idenloc += idenleng; return B_NE_TIME; }
"</TIMEX>"			    { idenloc += idenleng; return E_NE_TIME; }
"<NUMEX "[^>]*">"		    { idenloc += idenleng; return B_NE_NUMBER; }
"<NUMEX"[^>]*">"		    { idenloc += idenleng; return B_NE_NUMBER; }
"</NUMEX>"			    { idenloc += idenleng; return E_NE_TIME; }
"<"[/]?[a-zA-Z][^>]*">"                 {idenloc += idenleng; /* zap tags */ }
"<!"[^-][^>]*">"                        {idenloc += idenleng; /* zap other tags*/}
[&][a-zA-Z]+[;]                         {idenloc += idenleng; /* zap symbols */ }
[&][#][0-9]*[;]                         {idenloc += idenleng; /* zap symbols */ }

[A-Z][A-Z]+                             {idenloc += idenleng; return UPWORD; }
[a-zA-Z0-9]+\'[a-zA-Z/]+                {idenloc += idenleng; return CONTRACTION;}
[a-zA-Z0-9]+                            {idenloc += idenleng; return WORD; }
[A-Z]\.([A-Z]\.)+                       {idenloc += idenleng; return ACRONYM; }
[\n]                                    {idenloc += idenleng; /* zap newline */ }
.                                       {idenloc += idenleng; return UNKNOWN; }

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

lemur::parse::IdentifinderParser::IdentifinderParser() {
  state = OUTER;
  poscount = 0;
  wordpos.setName("position");
  tag.setName("NE");
  tag.setValue("\0");
  btag.setName("B_NE");
  btag.setValue("\0");
  etag.setName("E_NE");
  etag.setValue("\0");
  iden = identifier;
}

long lemur::parse::IdentifinderParser::fileTell() const {
  return idenloc;
}

void 
lemur::parse::IdentifinderParser::parseFile(const string &filename) {
  parsefile = filename;
  idenloc = 0;
  idenin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(idenin);
}

void lemur::parse::IdentifinderParser::parseBuffer (char* buf, int len) {
  int tpos = idenloc;
  idenloc = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  idenloc = tpos;
}

void lemur::parse::IdentifinderParser::doParse() {

  int tok;

  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the DOC state, text is parsed.
  while (tok = idenlex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      break;
    
    case B_DOC:
      docpos = idenloc - idenleng;
      state = DOC;
      break;

    case F_DOCNO:
      {
        // Extract the document number and pass it on.
	char * dn = identext + 7;
	while (isspace(*dn)) dn++;
	char * de = dn;
	while (!isspace(*de) && *de != '<') de++;
	*de = '\0';
        poscount = 0;
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

    case WORD:
      if (state == DOC) {
        // put the word in lowercase and pass it on
	for (char * c = identext; *c != '\0'; c++)
	  *(c) = tolower(*c);
	wordpos.setValue(poscount);
        proplist.clear();
	proplist.setProperty(&wordpos);
	proplist.setProperty(&tag);
	proplist.setProperty(&btag);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, identext, identext, &proplist);
	poscount++;
	// reset begin tag
	btag.setValue("\0");
      }
      break;

    case CONTRACTION:
      if (state == DOC) {
      // strip out apostrophe
	char * c;
	bool after = false;
	for (c = identext; *c != '\0'; c++) {
	  if (*c == '\'') {
	    after = true;
	    *(c) = tolower(*(c+1));
          } else {
	    if (after)
            *(c) = tolower(*(c+1));
	    else
 	    *(c) = tolower(*c);	
	  }
        }
 //       *c = '\0';
	wordpos.setValue(poscount);
	poscount++;
        proplist.clear();
	proplist.setProperty(&wordpos);
	proplist.setProperty(&tag);
	proplist.setProperty(&btag);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, identext, identext, &proplist);
        // reset begin tag
        btag.setValue("\0");
      }
      break;

    case UPWORD:
      if (state == DOC) {
        if (! isAcronym(identext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = identext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}
	wordpos.setValue(poscount);
	poscount++;
        proplist.clear();
	proplist.setProperty(&wordpos);
	proplist.setProperty(&tag);
	proplist.setProperty(&btag);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, identext, identext, &proplist);
	// reset begin tag
	btag.setValue("\0");
      }
      break;

    case ACRONYM:
      if (state == DOC) {
	char * e = identext;
	// strip periods
	for (char * c = identext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(identext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = identext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	wordpos.setValue(poscount);
	poscount++;
        proplist.clear();
	proplist.setProperty(&wordpos);
	proplist.setProperty(&tag);
	proplist.setProperty(&btag);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, identext, identext, &proplist);
	// reset begin tag
        btag.setValue("\0");
      }
      break;
      
    case B_NE_NAME:
    case B_NE_TIME:
    case B_NE_NUMBER:
      if (state == DOC) {
        char szTag[32];
	char szType[64];
	strcpy(szType, BEGIN_PREFIX);
        int argsFound = sscanf(identext, "<%s TYPE=\"%s\">", szTag, szType+PREFIX_LEN);
	if (argsFound == 2) {
	  // null terminate string
	  szType[63] = '\0';
	  for (char *c =szType+PREFIX_LEN;*c;c++) {
	    if (*c == '\"') {
	      *c= '\0';
	      break;
	    }
	  }
	  tag.setValue(szType+PREFIX_LEN);
	  btag.setValue(szType);
	} 	  
      }
    break;

    case E_NE_NAME:
    case E_NE_TIME:
    case E_NE_NUMBER:
      if ((state == DOC) && (tag.getLength() > 1)) {
        char* val = new char[tag.getLength()+PREFIX_LEN];
	strncpy(val, END_PREFIX, PREFIX_LEN);
	strcpy(val+PREFIX_LEN, (char*)tag.getValue());
	etag.setValue(val);
	// send end tag, don't up the word count
	wordpos.setValue(poscount-1);
	proplist.clear();
	proplist.setProperty(&wordpos);
	proplist.setProperty(&etag);
	if (textHandler)
	  textHandler->foundToken(WORDTOK,"","",&proplist);

	etag.setValue("\0");
	tag.setValue("\0");
	delete[]val;
      }
    break;   
    }

  }
}

