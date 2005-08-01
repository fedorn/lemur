/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
// dmf
// C++ thread safe implementation of the Krovetz stemmer.
// requires no external data files.
// 07/29/2005
#ifndef _KROVETZ_STEMMER_H_
#define _KROVETZ_STEMMER_H_
#ifdef WIN32
#include <hash_map>
#else
#include <ext/hash_map>
// 3.3 does not use __gnu_cxx, 3.4+ does.
using namespace __gnu_cxx;
#endif
#include "indri/Mutex.hpp"
#include "indri/ScopedLock.hpp"

// fix this
#define MAX_WORD_LENGTH 25
namespace indri
{
  namespace parse 
  {
    class KrovetzStemmer 
    {
    public:
      KrovetzStemmer();
      ~KrovetzStemmer();
      
      // const ness...
      char * kstem_stemmer(char *term);
      int kstem_stem_tobuffer(char *term, char *buffer);
      // indri's KrovetzStemmerTransformation may use this
      void kstem_add_table_entry(const char* variant, const char* word, 
                                 bool exc=false);
    private:
      indri::thread::Mutex _stemLock; /// protects stem calls
      typedef struct dictEntry {
        bool exception;      /* is the word an exception to stemming rules? */
        const char *root; /* used for direct lookup (e.g. irregular variants) */
      } dictEntry;
      // this allows two terms to hash to an entry.
      typedef struct cacheEntry {
        char flag; // first or second last used.
        char word1[MAX_WORD_LENGTH];
        char stem1[MAX_WORD_LENGTH];
        char word2[MAX_WORD_LENGTH];
        char stem2[MAX_WORD_LENGTH];
      } cacheEntry;

      // operatoes on atribute word.
      bool ends(const char *s, int sufflen);
      void setsuff(const char *str, int length);
      dictEntry *getdep(char *word);
      bool lookup(char *word);
      bool cons(int i);
      bool vowelinstem();
      bool vowel(int i);
      bool doublec(int i);
      void plural();
      void past_tense();
      void aspect();
      void ion_endings();
      void er_and_or_endings ();
      void ly_endings ();
      void al_endings() ;
      void ive_endings() ;
      void ize_endings() ;
      void ment_endings() ;
      void ity_endings() ;
      void ble_endings() ;
      void ness_endings() ;
      void ism_endings();
      void ic_endings();
      void ncy_endings();
      void nce_endings();
      // maint.
      void loadTables();
#ifdef WIN32
      struct ltstr {
        bool operator()(const char* s1, const char* s2) const {
          return strcmp(s1, s2) < 0;
        }
      };
      //studio 7 hash_map provides hash_compare, rather than hash
      // needing an < predicate, rather than an == predicate.
      typedef stdext::hash_map<const char *, dictEntry, stdext::hash_compare<const char *, ltstr> > dictTable;
#else
      struct eqstr {
        bool operator()(const char* s1, const char* s2) const {
          return strcmp(s1, s2) == 0;
        }
      };
      typedef hash_map<const char *, dictEntry, hash<const char *>, eqstr> dictTable;
#endif
      dictTable dictEntries;
      // this needs to be a bounded size cache.
      // kstem.cpp uses size 30013 entries.
      cacheEntry *stemCache;
      // size
      int stemhtsize;
      // state
      // k = wordlength - 1
      int k;
      // j is stemlength - 1
      int j;
      // pointer to the output buffer
      char *word;
      // used by kstem_stemmer to return a safe value.
      char stem[MAX_WORD_LENGTH];
    };
  }
}
#endif /* _KROVETZ_STEMMER_H_*/
