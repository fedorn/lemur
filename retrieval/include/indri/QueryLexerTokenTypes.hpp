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
		O_BRACE = 37,
		C_BRACE = 38,
		DBL_QUOTE = 39,
		QUOTE = 40,
		DOT = 41,
		COMMA = 42,
		DASH = 43,
		COLON = 44,
		TAB = 45,
		CR = 46,
		LF = 47,
		SPACE = 48,
		HIGH_CHAR = 49,
		DIGIT = 50,
		ASCII_LETTER = 51,
		SAFE_LETTER = 52,
		SAFE_CHAR = 53,
		BASESIXFOUR_CHAR = 54,
		TEXT_TERM = 55,
		TERM = 56,
		ENCODED_QUOTED_TERM = 57,
		ENCODED_TERM = 58,
		OPERATOR = 59,
		JUNK = 60,
		SLASH = 61,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
