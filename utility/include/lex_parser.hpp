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


#ifndef _LEXPARSER_HPP
#define _LEXPARSER_HPP

#define TERM 1
#define DOCID 2

extern FILE* yyin; 
extern char* yytext; 
extern int yyleng;
int yylex();

#endif

