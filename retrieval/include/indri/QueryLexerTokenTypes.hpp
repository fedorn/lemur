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
		PRIOR = 17,
		DATEAFTER = 18,
		DATEBEFORE = 19,
		DATEBETWEEN = 20,
		LESS = 21,
		GREATER = 22,
		BETWEEN = 23,
		EQUALS = 24,
		NUMBER = 25,
		FLOAT = 26,
		STAR = 27,
		O_PAREN = 28,
		C_PAREN = 29,
		O_ANGLE = 30,
		C_ANGLE = 31,
		O_SQUARE = 32,
		C_SQUARE = 33,
		DBL_QUOTE = 34,
		QUOTE = 35,
		DOT = 36,
		COMMA = 37,
		DASH = 38,
		COLON = 39,
		TAB = 40,
		CR = 41,
		LF = 42,
		SPACE = 43,
		HIGH_CHAR = 44,
		ASCII_LETTER = 45,
		ASCII_LETTER_NO_B = 46,
		SAFE_CHAR = 47,
		BASESIXFOUR_CHAR = 48,
		TEXT_TERM = 49,
		TERM = 50,
		ENCODED_QUOTED_TERM = 51,
		ENCODED_TERM = 52,
		OPERATOR = 53,
		JUNK = 54,
		SLASH = 55,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
