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
		PRIOR = 18,
		DATEAFTER = 19,
		DATEBEFORE = 20,
		DATEBETWEEN = 21,
		LESS = 22,
		GREATER = 23,
		BETWEEN = 24,
		EQUALS = 25,
		NUMBER = 26,
		NEGATIVE_NUMBER = 27,
		FLOAT = 28,
		STAR = 29,
		O_PAREN = 30,
		C_PAREN = 31,
		O_ANGLE = 32,
		C_ANGLE = 33,
		O_SQUARE = 34,
		C_SQUARE = 35,
		DBL_QUOTE = 36,
		QUOTE = 37,
		DOT = 38,
		COMMA = 39,
		DASH = 40,
		COLON = 41,
		TAB = 42,
		CR = 43,
		LF = 44,
		SPACE = 45,
		HIGH_CHAR = 46,
		DIGIT = 47,
		ASCII_LETTER = 48,
		SAFE_LETTER = 49,
		SAFE_CHAR = 50,
		BASESIXFOUR_CHAR = 51,
		TEXT_TERM = 52,
		TERM = 53,
		ENCODED_QUOTED_TERM = 54,
		ENCODED_TERM = 55,
		OPERATOR = 56,
		JUNK = 57,
		SLASH = 58,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
