/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

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
		NUMBER = 24,
		FLOAT = 25,
		STAR = 26,
		O_PAREN = 27,
		C_PAREN = 28,
		O_ANGLE = 29,
		C_ANGLE = 30,
		O_SQUARE = 31,
		C_SQUARE = 32,
		DBL_QUOTE = 33,
		QUOTE = 34,
		DOT = 35,
		COMMA = 36,
		DASH = 37,
		COLON = 38,
		TAB = 39,
		CR = 40,
		LF = 41,
		SPACE = 42,
		HIGH_CHAR = 43,
		ASCII_LETTER = 44,
		ASCII_LETTER_NO_B = 45,
		SAFE_CHAR = 46,
		BASESIXFOUR_CHAR = 47,
		TEXT_TERM = 48,
		TERM = 49,
		ENCODED_QUOTED_TERM = 50,
		ENCODED_TERM = 51,
		OPERATOR = 52,
		JUNK = 53,
		SLASH = 54,
		NULL_TREE_LOOKAHEAD = 3
	};
#ifdef __cplusplus
};
#endif
ANTLR_END_NAMESPACE
ANTLR_END_NAMESPACE
#endif /*INC_QueryLexerTokenTypes_hpp_*/
