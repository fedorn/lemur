/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "InvFPPushIndex.hpp"
#include "DocumentProps.hpp"
#include "InvFPTerm.hpp"
#include "lex_parser.hpp"

int main(int argc, char* argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: indexname cachemem_bytes file1 [... fileN]\n");
    return 0;
  }

  FILE* readin = NULL;
  InvFPPushIndex* index = new InvFPPushIndex(argv[1], atoi(argv[2]));
  DocumentProps* dp = new DocumentProps();
  InvFPTerm* term = new InvFPTerm();

  int tcount = 0; // count of total terms (not unique)
  int dtcount = 0;  // count of total terms in document
  int token;
  for (int i=3;i<argc;i++) {    
    // if there was one, close the previous file pointer
    if (readin != NULL)
      fclose(readin);

    fprintf (stderr, "begin indexing: %s\n", argv[i]);
    readin = fopen(argv[i], "r");
    if (!readin) {
      fprintf(stderr, "Error: could not open source file %s\n", argv[i]);
      continue;
    }

    yyin = readin;
          
    while ((token = yylex()) != 0) {
      switch (token) {
      case DOCID:
        // close out the previous
        if (dtcount != 0) {
          dp->length(dtcount);
          index->endDoc(dp);
          // we want to keep using the same dp object so reset values after they're used
          dp->length(0);
        }
        dp->stringID(yytext);
        index->beginDoc(dp);
        tcount += dtcount;
        dtcount = 0;
        break;
      case TERM:
        dtcount++;
        term->strLength(yyleng);
        term->spelling(yytext);
        term->position(dtcount);
        index->addTerm(*term);
        break;
      default:
        fprintf(stderr, "Main: encountered token type we were not expecting.\n");
        break;
      } // switch
    } // while there is more text
    // close out the last document of this file
    tcount += dtcount;
    dp->length(dtcount);
    index->endDoc(dp);
  }// for each file
  // close collection
  index->endCollection(NULL);

  delete(dp);
  delete(term);
  delete(index);
  fclose(readin);
  return 0;
}
