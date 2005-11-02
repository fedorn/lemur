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
 * http://www.lemurproject.org/license.html
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

long reuterspos; 

%}
%%

"</newsitem>"                       { reuterspos += reutersleng; return E_DOC; }
"<newsitem "[^>]*">"                { reuterspos += reutersleng; return F_DOCNO; }
"<text>"                            { reuterspos += reutersleng; return B_TEXT; }
"</text>"                           { reuterspos += reutersleng; return E_TEXT; }
"<headline>"                        { reuterspos += reutersleng; return B_TEXT; }
"</headline>"                       { reuterspos += reutersleng; return E_TEXT; }
"<title>"                           { reuterspos += reutersleng; return B_TEXT; }
"</title>"                          { reuterspos += reutersleng; return E_TEXT; }
"<"[/]?[a-zA-Z][^>]*">"             { reuterspos += reutersleng; /* zap tags */ }
"<!"[^-][^>]*">"                    { reuterspos += reutersleng; /* zap other tags*/}
[&][a-zA-Z]+[;]                     { reuterspos += reutersleng; /* zap symbols */ }
[&][#][0-9]*[;]                     { reuterspos += reutersleng; /* zap symbols */ } 
[A-Z][A-Z]+                         { reuterspos += reutersleng; return UPWORD; }
[a-zA-Z0-9]+                        { reuterspos += reutersleng; return WORD; }
"\*eos"                        { reuterspos += reutersleng; return WORD; /* *eos token for summarization module */}
[A-Z][A-Z]+((\')?[s])?              { reuterspos += reutersleng; return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+             { reuterspos += reutersleng; return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { reuterspos += reutersleng; return ACRONYM; }
[\n]                                { reuterspos += reutersleng; /* zap newline */ }
.                                   { reuterspos += reutersleng; return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

lemur::parse::ReutersParser::ReutersParser() {
  state = OUTER;
  iden = identifier;
}



long lemur::parse::ReutersParser::fileTell() const {
  return reuterspos;
}

void 
lemur::parse::ReutersParser::parseFile(const string &filename) {
  parsefile = filename;
  reuterspos = 0;
  reutersin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(reutersin);

}

void lemur::parse::ReutersParser::parseBuffer(char* buf, int len) {
  int tpos = reuterspos;
  reuterspos = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  reuterspos = tpos;
}

void lemur::parse::ReutersParser::doParse() {
  
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = reuterslex()) {
    switch (tok) {

    case E_DOC:
      state = OUTER;
      if (textHandler != NULL) textHandler->foundEndDoc();
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
	docpos = reuterspos;
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
	for (c = reuterstext; *c != '\0'; c++)
	  *(c) = tolower(*c);   	  
	if (textHandler != NULL && c != reuterstext) 
	  textHandler->foundWord(reuterstext);
	 
      }
      break;

    case CONTRACTION:
      if (state == TEXT) {
	// strip suffix and convert to lowercase
	char * c;
	int len = 0, diff;
	for (c = reuterstext; *c != '\''; c++, len++)
	  *(c) = tolower(*c);	
	*c = '\0';
	diff = reutersleng - len;
	reuterspos -= diff; /* change endpoint for docMgr usage. */
	if (textHandler != NULL) textHandler->foundWord(reuterstext);
	reuterspos += diff;
	c++;
	if (textHandler != NULL) textHandler->foundWord(c);
      }
      break;

    case UPWORD:
      if (state == TEXT) {
        char * c;
        if (! isAcronym(reuterstext)) {
	  // put in lowercase if the word is not in the acronym list
	  for (c = reuterstext; *c != '\0'; c++)
	    *(c) = tolower(*c);
	}	
	if (textHandler != NULL) textHandler->foundWord(reuterstext);	

	 
      }
      break;

    case ACRONYM:
      if (state == TEXT) {
	char * e = reuterstext;
	//	for (char * c = reuterstext; *c != '\0'; *c++) { //HUH?
	for (char * c = reuterstext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (!isAcronym(reuterstext)) {
	  // put in lowercase if not in the acronym list
	  for (char * c = reuterstext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	if (textHandler != NULL && e != reuterstext) 
	  textHandler->foundWord(reuterstext);
	 
      }
      break;
    
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
	int len = 0, diff;
	// strip suffix
	for (c = reuterstext; *c != '\'' && *c != '\0' && *c != 's'; c++, len++);
        *c = '\0';
	if (!isAcronym(reuterstext)) {
	  // put in lowercase if not in the acronym list
	  for (c = reuterstext; *c != '\0'; c++)
	    *(c) = tolower(*c);	 
	}
	diff = reutersleng - len;
	reuterspos -= diff; /* change endpoint for docMgr usage. */
	if (textHandler != NULL && c != reuterstext) 
	  textHandler->foundWord(reuterstext);	
	reuterspos += diff;
      }      
      break;

    }

  }

}
