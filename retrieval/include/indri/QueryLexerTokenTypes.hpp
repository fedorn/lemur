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
		WCARD = 29,
		NUMBER = 30,
		NEGATIVE_NUMBER = 31,
		FLOAT = 32,
		STAR = 33,
		O_PAREN = 34,
		C_PAREN = 35,
		O_ANGLE = 36,
		C_ANGLE = 37,
		O_SQUARE = 38,
		C_SQUARE = 39,
		O_BRACE = 40,
		C_BRACE = 41,
		DBL_QUOTE = 42,
		QUOTE = 43,
		DOT = 44,
		COMMA = 45,
		SLASH = 46,
		B_SLASH = 47,
		DASH = 48,
		COLON = 49,
		TAB = 50,
		CR = 51,
		LF = 52,
		SPACE = 53,
		HIGH_CHAR = 54,
		DIGIT = 55,
		ASCII_LETTER = 56,
		SAFE_LETTER = 57,
		SAFE_CHAR = 58,
		BASESIXFOUR_CHAR = 59,
		TEXT_TERM = 60,
		TERM = 61,
		ENCODED_QUOTED_TERM = 62,
		ENCODED_TERM = 63,
		OPERATOR = 64,
		JUNK = 65,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
