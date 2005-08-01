
/**************************************************************************/
/**************************************************************************/
/**************            ARABIC STEMMER HEADER FILE         *************/
/**************************************************************************/
/**************************************************************************/

/*

Copyright (c) 2001 UMASS CIIR All rights reserved.
Written by Nick Dufresne (nickd@cs.brandeis.edu)
	
08/01/2005 -- rewrite as standalone thread safe clase.	
*/
#include <string>
#include <set>

namespace lemur
{
  namespace parse
  {
    class Arabic_Stemmer 
    {
    public:
      Arabic_Stemmer(std::string stemFunc);
      ~Arabic_Stemmer();
      // stem a term.
      void stemTerm(char *, char *);

    private:
      void (Arabic_Stemmer::*stem_fct)(char *, char *) ;
      void arabic_remove_diacritics (char *, char *);
      //removes diacritics from word
      void arabic_stop(char *, char *);  // only removes stops
      void no_stem(char *, char *) ;	   // doesn't do anything
      //normalize arabic word
      void arabic_norm2(char *, char *);
      void arabic_norm2_stop(char *, char *);
      void arabic_light10(char *, char *);   
      void arabic_light10_stop(char *, char *);         
      // stopwords hash table.
      struct ltstr {
        bool operator()(const char* s1, const char* s2) const {
          return strcmp(s1, s2) < 0;
        }
      };
      std::set<const char *, ltstr> stop_words_ht;
      bool on_stop_list (char *word);
    // pointer to member function
      typedef struct {
        const char *option;
        void (Arabic_Stemmer::*stem_fct)(char *, char *) ;
      } stem_info_t;
      static stem_info_t stemtable[];
      static const int ArabicVowel[256];
      static const int Norm3Char[256];
      static const int NormChar[256];
      static const int isWhitespace[256];
      static char *stopwords[];
      static char *suffixes[];
      static char *defarticles[];
      int is_whitespace (const char c);
      void remove_definite_articles(char *word, char *result);
      void remove_all_suffixes(char *word, char *result, size_t lenlimit);
    };
  }
}

    
