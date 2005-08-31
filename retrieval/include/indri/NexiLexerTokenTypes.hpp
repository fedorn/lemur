#ifndef INC_NexiLexerTokenTypes_hpp_
#define INC_NexiLexerTokenTypes_hpp_

ANTLR_BEGIN_NAMESPACE(indri)
ANTLR_BEGIN_NAMESPACE(lang)
/* $ANTLR 2.7.4: "nexilang.g" -> "NexiLexerTokenTypes.hpp"$ */

#ifndef CUSTOM_API
# define CUSTOM_API
#endif

#ifdef __cplusplus
struct CUSTOM_API NexiLexerTokenTypes {
#endif
	enum {
		EOF_ = 1,
		LESS = 4,
		GREATER = 5,
		LESSEQ = 6,
		GREATEREQ = 7,
		EQUALS = 8,
		ABOUT = 9,
		AND = 10,
		OR = 11,
		WILD = 12,
		NUMBER = 13,
		NEGATIVE_NUMBER = 14,
		FLOAT = 15,
		STAR = 16,
		O_PAREN = 17,
		C_PAREN = 18,
		O_SQUARE = 19,
		C_SQUARE = 20,
		DBL_QUOTE = 21,
		QUOTE = 22,
		DOT = 23,
		COMMA = 24,
		SLASH = 25,
		MINUS = 26,
		PLUS = 27,
		TAB = 28,
		CR = 29,
		LF = 30,
		SPACE = 31,
		HIGH_CHAR = 32,
		DIGIT = 33,
		ASCII_LETTER = 34,
		SAFE_LETTER = 35,
		SAFE_CHAR = 36,
		TEXT_TERM = 37,
		TERM = 38,
		OPERATORS = 39,
		JUNK = 40,
		// "|" = 41
		// "." = 42
		// "," = 43
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_NexiLexerTokenTypes_hpp_*/
