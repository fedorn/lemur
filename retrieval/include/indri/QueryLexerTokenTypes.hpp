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
		SYN = 18,
		PRIOR = 19,
		DATEAFTER = 20,
		DATEBEFORE = 21,
		DATEBETWEEN = 22,
		LESS = 23,
		GREATER = 24,
		BETWEEN = 25,
		EQUALS = 26,
		NUMBER = 27,
		NEGATIVE_NUMBER = 28,
		FLOAT = 29,
		STAR = 30,
		O_PAREN = 31,
		C_PAREN = 32,
		O_ANGLE = 33,
		C_ANGLE = 34,
		O_SQUARE = 35,
		C_SQUARE = 36,
		DBL_QUOTE = 37,
		QUOTE = 38,
		DOT = 39,
		COMMA = 40,
		DASH = 41,
		COLON = 42,
		TAB = 43,
		CR = 44,
		LF = 45,
		SPACE = 46,
		HIGH_CHAR = 47,
		DIGIT = 48,
		ASCII_LETTER = 49,
		SAFE_LETTER = 50,
		SAFE_CHAR = 51,
		BASESIXFOUR_CHAR = 52,
		TEXT_TERM = 53,
		TERM = 54,
		ENCODED_QUOTED_TERM = 55,
		ENCODED_TERM = 56,
		OPERATOR = 57,
		JUNK = 58,
		SLASH = 59,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
