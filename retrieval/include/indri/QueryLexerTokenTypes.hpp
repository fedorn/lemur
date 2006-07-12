#ifndef INC_QueryLexerTokenTypes_hpp_
#define INC_QueryLexerTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(indri)
ANTLR_BEGIN_NAMESPACE(lang)
/* $ANTLR 2.7.3 (20060307-1): "indrilang.g" -> "QueryLexerTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API QueryLexerTokenTypes {
#endif
	enum {
		EOF_ = 1,
		SUM = 4,
		WSUM = 5,
		WAND = 6,
		OD = 7,
		OR = 8,
		NOT = 9,
		UW = 10,
		COMBINE = 11,
		WEIGHT = 12,
		MAX = 13,
		FILREQ = 14,
		FILREJ = 15,
		ANY = 16,
		BAND = 17,
		WSYN = 18,
		SYN = 19,
		PRIOR = 20,
		DATEAFTER = 21,
		DATEBEFORE = 22,
		DATEBETWEEN = 23,
		DATEEQUALS = 24,
		LESS = 25,
		GREATER = 26,
		BETWEEN = 27,
		EQUALS = 28,
		NUMBER = 29,
		NEGATIVE_NUMBER = 30,
		FLOAT = 31,
		STAR = 32,
		O_PAREN = 33,
		C_PAREN = 34,
		O_ANGLE = 35,
		C_ANGLE = 36,
		O_SQUARE = 37,
		C_SQUARE = 38,
		O_BRACE = 39,
		C_BRACE = 40,
		DBL_QUOTE = 41,
		QUOTE = 42,
		DOT = 43,
		COMMA = 44,
		SLASH = 45,
		B_SLASH = 46,
		DASH = 47,
		COLON = 48,
		TAB = 49,
		CR = 50,
		LF = 51,
		SPACE = 52,
		HIGH_CHAR = 53,
		DIGIT = 54,
		ASCII_LETTER = 55,
		SAFE_LETTER = 56,
		SAFE_CHAR = 57,
		BASESIXFOUR_CHAR = 58,
		TEXT_TERM = 59,
		TERM = 60,
		ENCODED_QUOTED_TERM = 61,
		ENCODED_TERM = 62,
		OPERATOR = 63,
		JUNK = 64,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
