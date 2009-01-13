/*==========================================================================
 * Copyright (c) 2002-2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "Arabic_Stemmer.hpp"
#include "Exception.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

#define CHAR_WAW 0xe6
#define NUMSTEMMERS 6
namespace lemur
{
  namespace parse 
  {    
    // data tables
    const char *Arabic_Stemmer::defarticles[] = {"«·", "Ê«·","»«·", "ﬂ«·", 
                                           "›«·", "··", "\0"};
    const char *Arabic_Stemmer::suffixes[] = {"Â«","«‰","« ","Ê‰","Ì‰","ÌÂ","Ì…",
                                        "Â","…","Ì","\0"};  
    const char *Arabic_Stemmer::stopwords[] = 
      { "«‰","»⁄œ", "÷œ", "Ì·Ì", "«·Ï", "›Ì", "„‰", "Õ Ï", "ÊÂÊ", "ÌﬂÊ‰",
        "»Â", "Ê·Ì”", "√Õœ", "⁄·Ï", "Êﬂ«‰", " ·ﬂ", "ﬂ–·ﬂ", "«· Ì", "Ê»Ì‰",
        "›ÌÂ«", "⁄·ÌÂ«", "≈‰", "Ê⁄·Ï", "·ﬂ‰", "⁄‰", "„”«¡", "·Ì”", "„‰–",
        "«·–Ì", "√„«", "ÕÌ‰", "Ê„‰", "·«", "·Ì”»", "Êﬂ«‰ ", "√Ì", "„«", "⁄‰Â",
        "ÕÊ·", "œÊ‰", "„⁄", "·ﬂ‰Â", "Ê·ﬂ‰", "·Â", "Â–«", "Ê«· Ì","›ﬁÿ", "À„",
        "Â–Â", "√‰Â", " ﬂÊ‰", "ﬁœ", "»Ì‰", "Ãœ«", "·‰", "‰ÕÊ", "ﬂ«‰", "·Â„",
        "·√‰", "«·ÌÊ„", "·„", "Âƒ·«¡", "›≈‰", "›ÌÂ", "–·ﬂ", "·Ê", "⁄‰œ",
        "«··–Ì‰", "ﬂ·", "»œ", "·œÏ", "ÊÀÌ", "√‰", "Ê„⁄", "›ﬁœ", "»·", "ÂÊ",
        "⁄‰Â«", "„‰Â", "»Â«", "Ê›Ì", "›ÂÊ", " Õ ", "·Â«", "√Ê", "≈–", "⁄·Ì",
        "⁄·ÌÂ", "ﬂ„«", "ﬂÌ›", "Â‰«", "Êﬁœ", "ﬂ«‰ ", "·–·ﬂ", "√„«„", "Â‰«ﬂ",
        "ﬁ»·", "„⁄Â", "ÌÊ„", "„‰Â«", "≈·Ï", "≈–«", "Â·", "ÕÌÀ", "ÂÌ", "«–«",
        "«Ê", "Ê", "„«", "·«", "«·Ì", "≈·Ì", "„«“«·", "·«“«·", "·«Ì“«·",
        "„«Ì“«·", "«’»Õ", "√’»Õ", "√„”Ï", "«„”Ï", "√÷ÕÏ", "«÷ÕÏ", "Ÿ·",
        "„«»—Õ", "„«› ∆", "„««‰›ﬂ", "»« ", "’«—", "·Ì”", "≈‰", "ﬂ√‰",
        "·Ì ", "·⁄·", "·«”Ì„«", "Ê·«Ì“«·", "«·Õ«·Ì", "÷„‰", "«Ê·", "Ê·Â",
        "–« ", "«Ì", "»œ·«", "«·ÌÂ«", "«‰Â", "«·–Ì‰", "›«‰Â", "Ê«‰",
        "Ê«·–Ì", "ÊÂ–«", "·Â–«", "«·«", "›ﬂ«‰", "” ﬂÊ‰", "„„«", "√»Ê",
        "»≈‰", "«·–Ì", "«·ÌÂ", "Ì„ﬂ‰", "»Â–«", "·œÌ", "Ê√‰", "ÊÂÌ", "Ê√»Ê",
        "¬·", "«·–Ì", "Â‰", "«·–Ï", NULL };

    /* isWhitespace is the same
       java aproximation of 
       Character.isWhitespace();
    */
    const int Arabic_Stemmer::isWhitespace[256] = {
      0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
      1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    /* Classifies arabic characters for normalization
       Combines removing diacritics, punctuation,
       nonletters, and substitutions  */

    const int Arabic_Stemmer::NormChar[256] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0x81,0,0,0,0,0,0,0,0,0x8a,0,0,0x8d,0x8e,0x8f,
      0x90,0,0,0,0,0,0,0,0x98,0,0x9a,0,0,0,0,0x9f,
      0,0,0,0,0,0,0,0,0,0,0xaa,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0xc0,0xc1,0xc7,0xc7,0xc4,0xc7,0xc6,0xc7,0xc8,0xe5,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
      0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,   0,0xd8,0xd9,0xda,0xdb,   0,0xdd,0xde,0xdf,
      0,0xe1,   0,0xe3,0xe4,0xe5,0xe6,   0,   0,   0,   0,   0,0xed, 0xed,   0,   0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    const int Arabic_Stemmer::Norm3Char[256] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0x81,0,0,0,0,0,0,0,0,0x8a,0,0,0x8d,0x8e,0x8f,
      0x90,0,0,0,0,0,0,0,0x98,0,0x9a,0,0,0,0,0x9f,
      0,0,0,0,0,0,0,0,0,0,0xaa,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0xc0,0xc7,0xc7,0xc7,0xc7,0xc7,0xc7,0xc7,0xc8,0xe5,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
      0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,   0,0xd8,0xd9,0xda,0xdb,   0,0xdd,0xde,0xdf,
      0,0xe1,   0,0xe3,0xe4,0xe5,0xe6,   0,   0,   0,   0,   0,0xed, 0xed,   0,   0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    const int Arabic_Stemmer::ArabicVowel[256] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,   0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0xe6,0,0,0,0,0,0xec,0xed,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    Arabic_Stemmer::stem_info_t Arabic_Stemmer::stemtable[NUMSTEMMERS] = {
      {"none", &Arabic_Stemmer::no_stem},
      {"arabic_stop", &Arabic_Stemmer::arabic_stop},
      {"arabic_norm2", &Arabic_Stemmer::arabic_norm2},
      {"arabic_norm2_stop", &Arabic_Stemmer::arabic_norm2_stop},
      {"arabic_light10", &Arabic_Stemmer::arabic_light10}, 
      {"arabic_light10_stop", &Arabic_Stemmer::arabic_light10_stop}
    };

    Arabic_Stemmer::Arabic_Stemmer(std::string stemFunc) {
      // initialize stemmer function and stopwords list.
      stem_fct = 0;
      for (int i = 0; i < NUMSTEMMERS; i++) {
        if (stemFunc == stemtable[i].option)
          // call with (this->*stem_fct)(args)
          stem_fct = stemtable[i].stem_fct;
      }
      if (stem_fct == 0) {
        // bad things here, throw an Exception.
        LEMUR_THROW(LEMUR_BAD_PARAMETER_ERROR, "Arabic_Stemmer: unknown stem function: "+stemFunc);
      }
      for (int i = 0; stopwords[i]; i++) {
        stop_words_ht.insert(stopwords[i]);
      }
    }

    Arabic_Stemmer::~Arabic_Stemmer() {
    }

    /**************************************************************************/
    /**********************  STEMMING FUNCTIONS *******************************/
    /**************************************************************************/

    int Arabic_Stemmer::is_whitespace (const char c) {
      int character;
      character = c;
      if (character < 0) character = character + 256;
      return isWhitespace[character];
    }

    /************************************************************
     *  REMOVE_DEFINITE_ARTICLES
     *  corrected version of REMOVE_DEFINITE_ARTICLE
     *     should be done after alef's are normalized
     ************************************************************/
    void Arabic_Stemmer::remove_definite_articles(char *word, char *result) {
      size_t len;
      size_t wordlen=strlen(word);
      const char **nextart=defarticles ;
      strcpy(result,word);

      for ( ; (len=strlen(*nextart)) > 0 ; nextart++) {
        if (wordlen > len+1 && strncmp(word, *nextart, len)==0) {
          strcpy(result, word+len);
          break ;  // only want to find one
        }
      }
    }

    /************************************************************
     *  REMOVE_suffixes
     *     should be done after alef's are normalized and prefixes removed
     *    this removes one suffix
     *    REMOVE_ALL_SUFFIXES removes all suffixes, order is significant
     ************************************************************/

    void Arabic_Stemmer::remove_all_suffixes(char *word, char *result, 
                             size_t lenlimit) /* min size for remainder */ {
      size_t suflen;
      size_t wordlen=strlen(word);
      const char **nextsuf=suffixes ;

      if (wordlen==0) {
        result[0] = '\0';
        return;
      }
      strcpy (result, word);

      for ( ; (suflen=strlen(*nextsuf)) > 0 ; nextsuf++) {
        wordlen=strlen(result);
        if (wordlen > suflen+lenlimit &&
            strncmp(word+wordlen-suflen, *nextsuf, suflen) == 0) {
          result[wordlen-suflen]='\0' ;
        }
      }
    }

    //*******************************************************************

    void Arabic_Stemmer::no_stem(char *word, char *result) {
      strcpy(result, word);
    }

    /* on_stop_list.  If the current word is a stopword then return true. */
    bool Arabic_Stemmer::on_stop_list (char *word) {
      return (stop_words_ht.find(word) != stop_words_ht.end());  
    }

    void Arabic_Stemmer::arabic_stop (char *word, char *result) {
      if (on_stop_list(word)) result[0]='\0';
      else strcpy(result, word);
    }

    /**********************************************************/
    // Version of normalization that uses NormChar table
    void Arabic_Stemmer::arabic_norm2(char *word, char *result) {
      unsigned char *i=(unsigned char *)word ;
      unsigned char *o=(unsigned char *)result ;
      while (*i) {
        // can't ever be true with 8-bit unsigned char
        if (*i > 256) 
          *i &= 0xff ;
        if (*i <= 'z') *o++ = *i; else
        if (NormChar[*i])
          *o++ = NormChar[*i] ;
        i++;
      }
      *o='\0';
    }

    void Arabic_Stemmer::arabic_norm2_stop (char *word, char *result) {
      arabic_norm2(word, result);
      if (on_stop_list(result))
        *result='\0' ;
    }

    void Arabic_Stemmer::arabic_light10 (char *word, char *result) {
      unsigned char *tempstring=(unsigned char *)strdup(word) ;
      unsigned char *tempstring2=(unsigned char *)strdup(word);

      char *begin ;
      *tempstring2='\0';
      arabic_norm2 (word, (char *)tempstring) ;

      // Should remove stop words here

      begin=(char *)tempstring;

      // Remove waw (advance pointer) if remainder >= 3 chars
      if (*tempstring == CHAR_WAW && strlen((char *)tempstring) > 3) begin++ ;

      // Remove definite article if remainder >=2 chars
      remove_definite_articles(begin, (char *)tempstring2);

      // Remove suffixes from small suffix list
      if (strlen((char *)tempstring2) > 2)
        remove_all_suffixes((char *)tempstring2, result, (size_t)1);  
      else 
        strcpy(result, (char *)tempstring2);
      free (tempstring) ;
      free (tempstring2) ;
    }

    void Arabic_Stemmer::arabic_light10_stop (char *word, char *result) {
      unsigned char *tempstring=(unsigned char *)strdup(word) ;
      unsigned char *tempstring2=(unsigned char *)strdup(word);

      char *begin ;
      *tempstring2='\0';
      arabic_norm2 (word, (char *)tempstring) ;
   
      if (on_stop_list((char *)tempstring)) {
        result[0]='\0';
        free (tempstring) ;
        free (tempstring2) ;
        return;
      }
      // Not a stop word   
      begin=(char *)tempstring;
      // Remove waw (advance pointer) if remainder >= 3 chars
      if (*tempstring == CHAR_WAW && strlen((char *)tempstring) > 3) begin++ ;

      // Remove definite article if remainder >=2 chars
      remove_definite_articles(begin, (char *)tempstring2);

      // Remove suffixes from small suffix list
      if (strlen((char *)tempstring2) > 2)
        remove_all_suffixes((char *)tempstring2, result, (size_t)1);  
      else 
        strcpy(result, (char *)tempstring2);
      free (tempstring) ;
      free (tempstring2) ;
    }
    void Arabic_Stemmer::stemTerm(char *word, char *result) {
      (this->*stem_fct)(word, result);
    }
  }
}
