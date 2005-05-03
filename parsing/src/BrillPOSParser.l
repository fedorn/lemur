%option noyywrap
%option never-interactive
%option prefix="bpos"
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

#include "BrillPOSParser.hpp"

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

extern FILE * bposin;
extern char * bpostext;

long bposloc;


%}
%%

"<DOC>"                                 {bposloc += bposleng; return B_DOC; }
"</DOC>"                                {bposloc += bposleng; return E_DOC; }
"<DOCHDR>"                              {bposloc += bposleng; return B_DOCHDR; }
"</DOCHDR>"                             {bposloc += bposleng; return E_DOCHDR; }
"<DOCNO>"[^<]*"</DOCNO>"                {bposloc += bposleng; return F_DOCNO; }
"<DOCOLDNO>"                            {bposloc += bposleng; return B_DOCOLDNO; }
"</DOCOLDNO>"                           {bposloc += bposleng; return E_DOCOLDNO; }

"<"[/]?[a-zA-Z][^>]*">"                 {bposloc += bposleng; /* zap tags */ }
"<!"[^-][^>]*">"                        {bposloc += bposleng; /* zap other tags*/}
[&][a-zA-Z]+[;]                         {bposloc += bposleng; /* zap symbols */ }
[&][#][0-9]*[;]                         {bposloc += bposleng; /* zap symbols */ }

"\.\/\."                                {bposloc += bposleng; return EOS; }
"\!\/\."                                {bposloc += bposleng; return EOS; }
"\?\/\."                                {bposloc += bposleng; return EOS; }

[A-Z][A-Z]+                             {bposloc += bposleng; return UPWORD; }
[a-zA-Z0-9]+\'[a-zA-Z/]+                {bposloc += bposleng; return CONTRACTION;}
[a-zA-Z0-9]+[/][a-zA-Z0-9]+             {bposloc += bposleng; return WORD; }
[a-zA-Z0-9]+                            {bposloc += bposleng; return WORD; }
[/][a-zA-Z0-9]+                         {bposloc += bposleng; return UNKNOWN;}
[A-Z]\.([A-Z]\.)+                       {bposloc += bposleng; return ACRONYM; }
[\n]                                    {bposloc += bposleng; /* zap newline */ }
.                                       {bposloc += bposleng; return UNKNOWN; }

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

lemur::parse::BrillPOSParser::BrillPOSParser() {
  state = OUTER;
  poscount = 0;
  wordpos.setName("position");
  tag.setName("POS");
  iden = identifier;
}

long lemur::parse::BrillPOSParser::fileTell() const {
  return bposloc;
}

void 
lemur::parse::BrillPOSParser::parseFile(const string &filename) {
  parsefile = filename;
  bposloc = 0;
  bposin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(bposin);
}

void lemur::parse::BrillPOSParser::parseBuffer (char* buf, int len) {
  int tpos = bposloc;
  bposloc = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  bposloc = tpos;
}

void lemur::parse::BrillPOSParser::doParse() {

  int tok;

  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the DOC state, text is parsed.
  while (tok = bposlex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      break;
    
    case B_DOC:
      docpos = bposloc - bposleng;
      state = DOC;
      break;

    case F_DOCNO:
      {
        // Extract the document number and pass it on.
	char * dn = bpostext + 7;
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

    case EOS:
      // eos marker. don't up the word count
      wordpos.setValue(poscount-1);
      proplist.clear();
      proplist.setProperty(&wordpos);
      if (textHandler != NULL)
        textHandler->foundToken(WORDTOK, "[EOS]", "[EOS]", &proplist);
      break;

    case WORD:
      if (state == DOC) {
        // put the word in lowercase and pass it on
	for (char * c = bpostext; *c != '\0'; c++)
	  *(c) = tolower(*c);
	wordpos.setValue(poscount);
        proplist.clear();
	proplist.setProperty(&wordpos);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, bpostext, bpostext, &proplist);
	poscount++;
      }
      break;

    case CONTRACTION:
      if (state == DOC) {
      // strip out apostrophe
	char * c;
	bool after = false;
	for (c = bpostext; *c != '\0'; c++) {
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
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, bpostext, bpostext, &proplist);
      }
      break;

    case UPWORD:
      if (state == DOC) {
        if (! isAcronym(bpostext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = bpostext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}
	wordpos.setValue(poscount);
	poscount++;
        proplist.clear();
	proplist.setProperty(&wordpos);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, bpostext, bpostext, &proplist);
      }
      break;

    case ACRONYM:
      if (state == DOC) {
	char * e = bpostext;
	// strip periods
	for (char * c = bpostext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(bpostext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (char * c = bpostext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	wordpos.setValue(poscount);
	poscount++;
        proplist.clear();
	proplist.setProperty(&wordpos);
	if (textHandler != NULL) 
	  textHandler->foundToken(WORDTOK, bpostext, bpostext, &proplist);
      }
      break;
    
    }

  }
}

