/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _INQUERYOPPARSER_HPP
#define _INQUERYOPPARSER_HPP
#define NESTING_LIMIT 20
#include "Parser.hpp"
#include "TextHandler.hpp"
namespace lemur 
{
  namespace parse 
  {
    
    // Source code in InQueryOpParser.lex

    class InQueryOpParser : public lemur::api::Parser {

    public:
      static const string identifier;

      InQueryOpParser();

      /// Parse a file.
      void parseFile (const string &filename);

      /// Parse a buffer of len length
      void parseBuffer (char * buf, int len);

      /// Gives current byte position offset into file being parsed.
      /// Don't use with parseBuffer
      long fileTell() const;
      /// The workhorse.
      void doParse();
      /// parse nested query nodes until the next enclosing RIGHT_PAREN
      void finishing_nesting_qnote(int top);
      /// parse nested proximity query nodes until the next enclosing RIGHT_PAREN
      void finishing_prox_qnote(int top);
      /// begin parsing nested query nodes
      void parseNestingOp(char *op);
      /// parse proximity operator node
      void parseProxOp(char *op, char *size);
      /// parse passage operator node
      void parsePassageOp(char *op, char *size);
      /// parse synonym operator node
      void parseSynOp(char *op);
      /// parse property operator node
      void parsePropOp(char *op);
      /// parse term node
      void parseTermOp(char *op);
      /// parse query node
      void parseQryNode(int tok);
      /// parse proximity query node
      void parseProxNode(int tok);

    private:

      /// The state of the parser.
      int state;
    };
  }
}

#endif
