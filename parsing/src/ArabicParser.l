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
 * http://www.lemurproject.org/license.html
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

long arabicpos;
%}

arabic_skip [~`!@#$%^&*()_\-+=[\]{}|\\:;"'<>,.?/¡÷×º¿]
not_arabic_skip [^\r\t\n ~`!@#$%^&*()_\-+=[\]{}|\\:;"'<>,.?/¡÷×º¿]
%%

"<DOC>"                             { arabicpos += Arabicleng; return B_DOC; /*1*/}
"</DOC>"                            { arabicpos += Arabicleng; return E_DOC; /*2*/}
"<DOCNO>"[^<]*"</DOCNO>"            { arabicpos += Arabicleng; return F_DOCNO; /*3*/}
"<TEXT>"                            { arabicpos += Arabicleng; return B_TEXT; /*4*/}
"</TEXT>"                           { arabicpos += Arabicleng; return E_TEXT; /*5*/}
"<HL>"                              { arabicpos += Arabicleng; return B_TEXT;/*6*/ }
"</HL>"                             { arabicpos += Arabicleng; return E_TEXT;/*7*/ }
"<HEAD"(ER)?">"                     { arabicpos += Arabicleng; return B_TEXT;/*8*/ }
"</HEAD"(ER)?">"                    { arabicpos += Arabicleng; return E_TEXT;/*9*/ }
"<HEADLINE>"                        { arabicpos += Arabicleng; return B_TEXT;/*10*/ }
"</HEADLINE>"                       { arabicpos += Arabicleng; return E_TEXT; /*11*/}
"<TTL>"                             { arabicpos += Arabicleng; return B_TEXT;/*12*/ }
"</TTL>"                            { arabicpos += Arabicleng; return E_TEXT;/*13*/ }
"<LP>"                              { arabicpos += Arabicleng; return B_TEXT; /*14*/}
"</LP>"                             { arabicpos += Arabicleng; return E_TEXT; /*15*/}
"<FOOTER>"                          { arabicpos += Arabicleng; return B_TEXT; /*???? vl indexes*/}
"</FOOTER>"                         { arabicpos += Arabicleng; return E_TEXT; /*???? vl indexes*/}
"<TRAILER>"                         { arabicpos += Arabicleng; return B_TEXT; /*???? vl indexes*/}
"</TRAILER>"                        { arabicpos += Arabicleng; return E_TEXT; /*???? vl indexes*/}
"<"[/]?[a-zA-Z][^>]*">"             { arabicpos += Arabicleng; /* zap tags */ }
"<!"[^-][^>]*">"                    { arabicpos += Arabicleng; /* zap other tags*/}
[&][a-zA-Z]+[;]                     { arabicpos += Arabicleng; /* zap symbols */ }
[&][#][0-9]*[;]                     { arabicpos += Arabicleng; /* zap symbols */ }
{arabic_skip}                       { arabicpos += Arabicleng; /* zap Arabic punctution */ } 
[A-Z][A-Z]+                         { arabicpos += Arabicleng; return UPWORD; }
[a-zA-Z0-9]+                        { arabicpos += Arabicleng; return WORD; }
"\*eos"                        { arabicpos += Arabicleng; return WORD; /* *eos token for summarization module */}
[A-Z][A-Z]+((\')?[s])?              { arabicpos += Arabicleng; return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+             { arabicpos += Arabicleng; return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { arabicpos += Arabicleng; return ACRONYM; }
{not_arabic_skip}+                  { arabicpos += Arabicleng;return WORD;/*16*/ }
[\n]                                { arabicpos += Arabicleng;/* zap newline */ }
.                                   { arabicpos += Arabicleng;return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

lemur::parse::ArabicParser::ArabicParser() {
  state = OUTER;
  iden = identifier;
}

long lemur::parse::ArabicParser::fileTell() const{
  return arabicpos;
}

void lemur::parse::ArabicParser::parseFile(const string &filename) {  
  parsefile = filename;
  arabicpos = 0;
  Arabicin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(Arabicin);
}

void lemur::parse::ArabicParser::parseBuffer(char* buf, int len) {
  int tpos = arabicpos;
  arabicpos = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  arabicpos = tpos;
}

void lemur::parse::ArabicParser::doParse() {
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = Arabiclex()) {
    switch (tok) {	
    case E_DOC:
      state = OUTER;
      if (textHandler != NULL) textHandler->foundEndDoc();
      break;
    case B_DOC:
      state = DOC;
      docpos = arabicpos - Arabicleng;
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
	char * c, *tmpc;
	int len = 0, diff;      
	for (c = Arabictext; *c != '\''; c++, len++)
	  *(c) = tolower(*c);	
	*c = '\0';
	diff = Arabicleng - len;
	arabicpos -= diff;
	if (textHandler != NULL) textHandler->foundWord(Arabictext);
	arabicpos += diff;
	c++;
	for (tmpc = c; *tmpc != '\0'; tmpc++)
	  *(tmpc) = tolower(*tmpc);	
	if (textHandler != NULL) textHandler->foundWord(c);
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
	int len = 0, diff;      
	if (!isAcronym(Arabictext)) {
	  // put in lowercase if not in the acronym list
	   for (c = Arabictext; *c != '\0'; c++)
	     *(c) = tolower(*c);	 
	}
	// strip suffix
	for (c = Arabictext; *c != '\'' && *c != '\0' && *c != 's'; c++, len++);
        *c = '\0';
	diff = Arabicleng - len;
	arabicpos -= diff;
	if (textHandler != NULL) textHandler->foundWord(Arabictext);
	arabicpos += diff;
      }      
      break;
    }
  }
}

