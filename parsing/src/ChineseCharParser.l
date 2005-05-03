%option noyywrap
%option never-interactive
%option prefix="ChineseChar" 
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


#include "ChineseCharParser.hpp"

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


extern FILE * ChineseCharin;
extern char * ChineseChartext;

long CCpos;

%}
%s SKIP
gb_punct £¢|¡°|¡±|¡å|£­|¡¥|¡ª|£®|¡£|£¯|£Ü|£¨|£©|¡ª|¡º|¡»|£¡|£®|£¿|¡-
gb_space ¡¡
gb_skip_punct ¡[¡-ÿ]|£([¡-¯]|[º-À]|[ë-ð])|©[¤-ï]
gb_decpt ¡¤
gb_comma £¬
gb_skip {gb_punct}|{gb_space}|{gb_skip_punct}|{gb_decpt}
gb_ch_char  [€-ÿ][€-ÿ]
gb_digit £±|£²|£³|£´|£µ|£¶|£·|£¸|£¹|£°
gb_arabic {gb_digit}+(({gb_decpt}|{gb_comma})?{gb_digit}+)*
gb1 Ò»
gb2 ¶þ
gb3 Èý
gb4 ËÄ
gb5 Îå
gb6 Áù
gb7 Æß
gb8 °Ë
gb9 ¾Å
gb10 Ê®
gb_ch_digit {gb1}|{gb2}|{gb3}|{gb4}|{gb5}|{gb6}|{gb7}|{gb8}|{gb9}|{gb10}
gb_hundred °Ù
gb_1K   Ç§
gb_10K     Íò
gb_billion ÒÚ
gb_trillion Õ×
gb_num_mult {gb_1K}|{gb_hundred}|{gb_10K}|{gb_billion}|{gb_trillion}
gb_stop_num Ìõ
gb_pct  £¥
gb_chpct °Ù·Ö(ÂÊ|±È)|±ÈÂÊ|{gb_pct}
gb_ch_num {gb_ch_digit}+
gb_number ({gb_arabic}|{gb_ch_num}){gb_num_mult}*{gb_stop_num}?{gb_chpct}?
gb_day	ÈÕ
gb_month ÔÂ
gb_year Äê(Ëê)?
gb_ch_date ({gb_arabic}|{gb_ch_num})({gb_day}|{gb_month}|{gb_year})
gb_dateline  [ ]ÐÂ»ªÉç(..){1,5}{gb_ch_date}{2,3}[ ]|[ ]ÐÂ»ªÉç(..){1,5}{gb_ch_date}{2,3}(µç)?(£¨([^\243].)+£©)?

%%

"<DOC>"                             { CCpos += ChineseCharleng; return B_DOC; /*1*/}
"</DOC>"                            { CCpos += ChineseCharleng; return E_DOC; /*2*/}
"<DOCNO>"[^<]*"</DOCNO>"            { CCpos += ChineseCharleng; return F_DOCNO; /*3*/}
"<TEXT>"                            { CCpos += ChineseCharleng; return B_TEXT; /*4*/}
"</TEXT>"                           { CCpos += ChineseCharleng; return E_TEXT; /*5*/}
"<HL>"                              { CCpos += ChineseCharleng; return B_TEXT;/*6*/}
"</HL>"                             { CCpos += ChineseCharleng; return E_TEXT;/*7*/}
"<HEAD>"                            { CCpos += ChineseCharleng; return B_TEXT;/*8*/}
"</HEAD>"                           { CCpos += ChineseCharleng; return E_TEXT;/*9*/}
"<HEADLINE>"                        { CCpos += ChineseCharleng; return B_TEXT;/*10*/}
"</HEADLINE>"                       { CCpos += ChineseCharleng; return E_TEXT; /*11*/}
"<TTL>"                             { CCpos += ChineseCharleng; return B_TEXT;/*12*/ }
"</TTL>"                            { CCpos += ChineseCharleng; return E_TEXT;/*13*/ }
"<LP>"                              { CCpos += ChineseCharleng; return B_TEXT; /*14*/}
"</LP>"                             { CCpos += ChineseCharleng; return E_TEXT; /*15*/}
"<"[/]?[a-zA-Z][^>]*">"             { CCpos += ChineseCharleng; /* zap tags */ }
"<!"[^-][^>]*">"                    { CCpos += ChineseCharleng; /* zap other tags*/}
[&][a-zA-Z]+[;]                     { CCpos += ChineseCharleng; /* zap symbols */ }
[&][#][0-9]*[;]                     { CCpos += ChineseCharleng; /* zap symbols */ }
{gb_skip}                           { CCpos += ChineseCharleng; /* zap one chinese punctutions */ } 
[A-Z][A-Z]+                         { CCpos += ChineseCharleng; return UPWORD; }
[a-zA-Z0-9]+                        { CCpos += ChineseCharleng; return WORD; }
[A-Z][A-Z]+((\')?[s])?              { CCpos += ChineseCharleng; return ACRONYM2; }
[a-zA-Z0-9]+\'[a-zA-Z]+             { CCpos += ChineseCharleng; return CONTRACTION;}
[A-Z]\.([A-Z]\.)+                   { CCpos += ChineseCharleng; return ACRONYM; }
{gb_dateline}|{gb_ch_date}|{gb_number} { CCpos += ChineseCharleng; return WORD; /* chinese number or date */ }
{gb_ch_char}                        { CCpos += ChineseCharleng; return WORD; /* single chinese char */ }
[\n]                                { CCpos += ChineseCharleng; /* zap newline */ }
.                                   { CCpos += ChineseCharleng; return UNKNOWN; }

%%

#define OUTER 0
#define DOC 1
#define TEXT 9

lemur::parse::ChineseCharParser::ChineseCharParser() {
  state = OUTER;
  iden = identifier;
}

long lemur::parse::ChineseCharParser::fileTell() const {
  return CCpos;
}

void lemur::parse::ChineseCharParser::parseFile(const string &filename) {  
  parsefile = filename;
  CCpos = 0;
  ChineseCharin = fopen(filename.c_str(), "rb");
  doParse();
  fclose(ChineseCharin);
}

void lemur::parse::ChineseCharParser::parseBuffer(char* buf, int len) {
  int tpos = CCpos;
  CCpos = 0;
  YY_BUFFER_STATE oldBuf = YY_CURRENT_BUFFER;
  YY_BUFFER_STATE myBuf = yy_scan_bytes(buf, len);
  doParse();
  if (oldBuf) yy_switch_to_buffer(oldBuf);
  yy_delete_buffer(myBuf);
  CCpos = tpos;
}


void lemur::parse::ChineseCharParser::doParse() {
  int tok;
  // The core loop of the parser.
  // The parser is state based.  Encountering a tag
  // can transition the machine into another state.
  // When in the TEXT state, text is parsed.
  while (tok = ChineseCharlex()) {
      //      fprintf(stderr, "parsing tok %d %s\n", tok, ChineseChartext);
    switch (tok) {	
    case E_DOC:
      state = OUTER;
      if (textHandler != NULL) textHandler->foundEndDoc();
      break;
    
    case B_DOC:
      state = DOC;
      docpos = CCpos - ChineseCharleng;
      break;

    case F_DOCNO:
      {
        // Extract the document number and pass it on.
	char * dn = ChineseChartext + 7;
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
	if (textHandler != NULL) textHandler->foundWord(ChineseChartext);
      }
      break;

    case CONTRACTION:
      if (state == TEXT) {
	if (textHandler != NULL) textHandler->foundWord(ChineseChartext);
      }
      break;

    case UPWORD:
      if (state == TEXT) {
	if (textHandler != NULL) textHandler->foundWord(ChineseChartext);
      }
      break;

    case ACRONYM:
      if (state == TEXT) {
	char * e = ChineseChartext;
	for (char * c = ChineseChartext; *c != '\0'; c++) {
	  if (*c != '.') *(e++) = *c;
	}
	*e = '\0';
	if (textHandler != NULL) textHandler->foundWord(ChineseChartext);
      }
      break;
    
    case ACRONYM2:
      if (state == TEXT) {
        char * c;
        int len = 0, diff;

	// strip suffix
	for (c = ChineseChartext; *c != '\'' && *c != '\0' && *c != 's'; c++, len++);
        *c = '\0';
	diff = ChineseCharleng - len;
	CCpos -= diff;
	if (textHandler != NULL) textHandler->foundWord(ChineseChartext);
	CCpos += diff;
      }      
      break;
    }
  }
}

