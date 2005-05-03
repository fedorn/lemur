%option noyywrap
%option never-interactive
%option prefix="Chinese" 
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


#include "ChineseParser.hpp"

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


extern FILE * Chinesein;
extern char * Chinesetext;


long chinesepos;

%}
%s SKIP
gb_punct £¢|¡°|¡±|¡å|£­|¡¥|¡ª|£®|¡£|£¯|£Ü|£¨|£©|¡ª|¡º|¡»|£¡|£®|£¿|¡-
gb_space ¡¡
gb_skip_punct ¡[¡-ÿ]|£([¡-¯]|[º-À]|[ë-ð])|©[¤-ï]
gb_decpt ¡¤
gb_skip {gb_punct}|{gb_space}|{gb_skip_punct}|{gb_decpt}

%%

"<DOC>"                             { chinesepos += Chineseleng; return B_DOC; /*1*/}
"</DOC>"                            { chinesepos += Chineseleng;return E_DOC; /*2*/}
"<DOCNO>"[^<]*"</DOCNO>"            { chinesepos += Chineseleng;return F_DOCNO; /*3*/}
"<TEXT>"                            { chinesepos += Chineseleng;return B_TEXT; /*4*/}
"</TEXT>"                           { chinesepos += Chineseleng;return E_TEXT; /*5*/}
"<HL>"                              { chinesepos += Chineseleng;return B_TEXT;/*6*/ }
"</HL>"                             { chinesepos += Chineseleng;return E_TEXT;/*7*/ }
"<HEAD>"                            { chinesepos += Chineseleng;return B_TEXT;/*8*/ }
"</HEAD>"                           { chinesepos += Chineseleng;return E_TEXT;/*9*/ }
"<HEADLINE>"                        { chinesepos += Chineseleng;return B_TEXT;/*10*/ }
"</HEADLINE>"                       { chinesepos += Chineseleng;return E_TEXT; /*11*/}
"<TTL>"                             { chinesepos += Chineseleng;return B_TEXT;/*12*/ }
"</TTL>"                            { chinesepos += Chineseleng;return E_TEXT;/*13*/ }
"<LP>"                              { chinesepos += Chineseleng;return B_TEXT; /*14*/}
"</LP>"                             { chinesepos += Chineseleng;return E_TEXT; /*15*/}
"<"[/]?[a-zA-Z][^>]*">"             { chinesepos += Chineseleng;/* zap tags */ }
"<!"[^-][^>]*">"                    { chinesepos += Chineseleng;/* zap other tags*/}
[&][a-zA-Z]+[;]                     { chinesepos += Chineseleng;/* zap symbols */ }
[&][#][0-9]*[;]                     { chinesepos += Chineseleng;/* zap symbols */ }
{gb_skip}[^\r\t\n ]*                { yyless(2); chinesepos += Chineseleng;  /* zap one chinese punctutions */ } 
[A-Z][A-Z]+                         { chinesepos += Chineseleng;return UPWORD; }
[a-zA-Z0-9]+                        { chinesepos += Chineseleng;return WORD; }
[A-Z][A-Z]+((\')?[s])?              { chinesepos += Chineseleng;return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+             { chinesepos += Chineseleng;return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { chinesepos += Chineseleng;return ACRONYM; }
[^\r\t\n ]+                         { chinesepos += Chineseleng;return WORD;/*16*/ }
[\n]                                { chinesepos += Chineseleng;/* zap newline */ }
.                                   { chinesepos += Chineseleng;return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

lemur::parse::ChineseParser::ChineseParser() {
  state = OUTER;
  iden = identifier;
}

long lemur::parse::ChineseParser::fileTell() const {
  return chinesepos;
}

void lemur::parse::ChineseParser::parseFile(const string &filename) {  
  parsefile = filename;
  chinesepos = 0;
  Chinesein = fopen(filename.c_str(), "rb");
  doParse();
  fclose(Chinesein);
}

void lemur::parse::ChineseParser::parseBuffer(char* buf, int len) {
  int tpos = chinesepos;
  chinesepos = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  chinesepos = tpos;
}



void lemur::parse::ChineseParser::doParse() {
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = Chineselex()) {
    switch (tok) {	
    case E_DOC:
      state = OUTER;
      if (textHandler != NULL) textHandler->foundEndDoc();
      break;
    
    case B_DOC:
      state = DOC;
      docpos = chinesepos - Chineseleng;
      break;

    case F_DOCNO:
      {
        // Extract the document number and pass it on.
	char * dn = Chinesetext + 7;
	while (isspace(*dn)) dn++;
	char * de = dn;
	while (!isspace(*de) && *de != '<') de++;
	*de = '\0';
	if (textHandler != NULL) textHandler->foundDoc(dn);

	state = TEXT;
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
	if (textHandler != NULL) textHandler->foundWord(Chinesetext);
      }
      break;

    case CONTRACTION:
      if (state == TEXT) {
	if (textHandler != NULL) textHandler->foundWord(Chinesetext);
      }
      break;

    case UPWORD:
      if (state == TEXT) {
	if (textHandler != NULL) textHandler->foundWord(Chinesetext);	
      }
      break;

    case ACRONYM:
      if (state == TEXT) {
	char * e = Chinesetext;
	for (char * c = Chinesetext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (textHandler != NULL) textHandler->foundWord(Chinesetext);
      }
      break;
    
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
        int len = 0, diff;
	// strip suffix
	for (c = Chinesetext; *c != '\'' && *c != '\0' && *c != 's'; c++, len++);
        *c = '\0';
diff = Chineseleng - len;
chinesepos -= diff;

	if (textHandler != NULL) textHandler->foundWord(Chinesetext);	
chinesepos += diff;

      }      
      break;

    }

  }
}

