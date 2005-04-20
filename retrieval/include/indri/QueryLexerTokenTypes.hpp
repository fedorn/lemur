#ifndef INC_QueryLexerTokenTypes_hpp_
#define INC_QueryLexerTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(indri)
ANTLR_BEGIN_NAMESPACE(lang)
/* $ANTLR 2.7.4: "indrilang.g" -> "QueryLexerTokenTypes.hpp"$ */

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
		LESS = 24,
		GREATER = 25,
		BETWEEN = 26,
		EQUALS = 27,
		NUMBER = 28,
		NEGATIVE_NUMBER = 29,
		FLOAT = 30,
		STAR = 31,
		O_PAREN = 32,
		C_PAREN = 33,
		O_ANGLE = 34,
		C_ANGLE = 35,
		O_SQUARE = 36,
		C_SQUARE = 37,
		O_BRACE = 38,
		C_BRACE = 39,
		DBL_QUOTE = 40,
		QUOTE = 41,
		DOT = 42,
		COMMA = 43,
		DASH = 44,
		COLON = 45,
		TAB = 46,
		CR = 47,
		LF = 48,
		SPACE = 49,
		HIGH_CHAR = 50,
		DIGIT = 51,
		ASCII_LETTER = 52,
		SAFE_LETTER = 53,
		SAFE_CHAR = 54,
		BASESIXFOUR_CHAR = 55,
		TEXT_TERM = 56,
		TERM = 57,
		ENCODED_QUOTED_TERM = 58,
		ENCODED_TERM = 59,
		OPERATOR = 60,
		JUNK = 61,
		SLASH = 62,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
