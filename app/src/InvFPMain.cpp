#include "InvFPPushIndex.hpp"
#include "DocumentProps.hpp"
#include "InvFPTerm.hpp"
#include "lex_parser.hpp"

int main(int argc, char* argv[]) {
  FILE* readin = NULL;
  InvFPPushIndex* index = new InvFPPushIndex("ThiTestIndex", atoi(argv[1]));
  DocumentProps* dp = new DocumentProps();
  InvFPTerm* term = new InvFPTerm();

  int tcount = 0; // count of total terms (not unique)
  int dtcount = 0;  // count of total terms in document
  int token;
  for (int i=2;i<argc;i++) {    
    // if there was one, close the previous file pointer
    if (readin != NULL)
      fclose(readin);

    fprintf (stderr, "begin indexing: %s\n", argv[i]);
    readin = fopen(argv[i], "r");
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
