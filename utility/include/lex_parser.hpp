#ifndef _LEXPARSER_HPP
#define _LEXPARSER_HPP

#define TERM 1
#define DOCID 2

extern FILE* yyin; 
extern char* yytext; 
extern int yyleng;
int yylex();

#endif