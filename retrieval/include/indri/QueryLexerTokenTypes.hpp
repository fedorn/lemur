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
		NEGATIVE_NUMBER = 26,
		FLOAT = 27,
		STAR = 28,
		O_PAREN = 29,
		C_PAREN = 30,
		O_ANGLE = 31,
		C_ANGLE = 32,
		O_SQUARE = 33,
		C_SQUARE = 34,
		DBL_QUOTE = 35,
		QUOTE = 36,
		DOT = 37,
		COMMA = 38,
		DASH = 39,
		COLON = 40,
		TAB = 41,
		CR = 42,
		LF = 43,
		SPACE = 44,
		HIGH_CHAR = 45,
		DIGIT = 46,
		ASCII_LETTER = 47,
		ASCII_LETTER_NO_B = 48,
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
