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

#ifndef _FREQCOUNTER_HPP
#define _FREQCOUNTER_HPP

#include <map>
#include <set>
#include "InvFPTypes.hpp"
#include "TextHandler.hpp"
#include "Stopper.hpp"
namespace lemur 
{
  namespace distrib 
  {
    
    /// Specifies query terms selected proportional to collection term frequency.
#define R_CTF 0
    /// Specifies query terms selected proportional to document frequency.
#define R_DF 1
    /// Specifies query terms selected proportional to average term frequency.
#define R_AVE_TF 2
    /// Specifies query terms selected with uniform probability.
#define R_UNIFORM 3

    /// Internally used structure for storing term statistics.
    typedef struct freqinfo_tt {
      char * word;
      int ctf;
      int df;
    } freqinfo_t;

    /// A map from words to frequency information.
    typedef map<std::string , freqinfo_t, less<std::string> > freqmap;
    /// A set of words.
    typedef set<std::string , less<std::string> > stringset;


    /*!
     * Counts collection term frequencies and document frequencies.  Also
     * provides a means for selecting random words.  The FreqCounter can 
     * use a stopword list.
     */
    class FreqCounter : public lemur::api::TextHandler {

    public:
      /// Create a frequency counter with the specified stopword list.
      /// The stopWords parameter is optional.
      FreqCounter(const lemur::api::Stopper * stopWords = NULL);
      /// Create a frequency counter (loading it from file) with the
      /// specified stopword list.  Thes stopWords parameter is optional.
      FreqCounter(const string &filename, const lemur::api::Stopper * stopWords = NULL);
  
      /// Delete the freqency counter.
      ~FreqCounter();

      /// Clear the frequency counter (set all counts to 0).
      void clear();

      /// Output the frequency information to a file.
      void output(const string &filename) const;

      /*! Get a random word from the distribution specified by setRandomMode. 
       * The random word is unique since the last clear operation. */
      char * randomWord();
      /*! Set the random word selection mode:
       *  R_CTF     - select using collection term frequency
       *  R_DF      - select using document frequency
       *  R_AVE_TF  - select using average term frequency
       *  R_UNIFORM - select each word with equal probability
       */
      void setRandomMode(int mode);
      /*! Gets the current random word mode.  See setRandomMode(...) */
      int getRandomMode() const;

      /*! Select a word at random using collection term frequency. This
       * word is not guarenteed to be unique from other calls to this function. */
      char * randomCtf() const;
      /*! Select a word at random using document frequency. This word is not
       * guarenteed to be unique from other calls to this function. */
      char * randomDf() const;
      /*! Select a word at random using average term frequency. This word is no
       * guarenteed to be unique from other calls to this function. */
      char * randomAveTf() const;
      /*! Select a word at random with equal probability for each word. This
       * word is not guarenteed to be unique from other calls to this funtion. */
      char * randomUniform() const;


      /*! Return the number of unique words seen across all documents processed. */
      int numWords() const;
      /*! Return the total words seen across all documents processed. */
      int totWords() const;

      /*! Get a reference to the internal frequency count map. */
      const freqmap * getFreqInfo() const;

      /*! Get the collection term frequency for a word. */
      int getCtf(const char * word) const;
      /*! Get the document frequency for a word. */
      int getDf(const char * word) const;
      /*! Get the average term frequency for a word. */
      double getAveTf(const char * word) const;

      /*! Compare lm1 to this language model, returning the ctf ratio. */
      double ctfRatio(FreqCounter & lm1) const;

      /// Overridden from TextHandler
      char * handleDoc(char * docno);
      /// Overridden from TextHandler - increments collection term frequencies.
      char * handleWord(char * word);

      /// Specifies end of a document - updates document frequencies.
      void endDoc();

      /// Set the name of language model described by the frequency counter.
      void setName(const string &freqCounterName);
      /// Get the counter's name.
      const string & getName() const;

      /// Prune least frequent words, keeping only topWords most frequent words.
      void pruneBottomWords(int topWords);
  

    protected:
      /* Loads a language model from file. */
      void input(const string &filename);

      /* Collection term frequencies. */
      mutable freqmap freqInfo;

      /* Words in a doc. */
      stringset doc;
      /* Random words returned so far. */
      stringset randdone;

      /* The frequency counter's name. */
      string name;

      /* Stopword list */
      const lemur::api::Stopper * stopper;


      /* used for calculating probabilities when
       * selecting a random word
       */
      /* Sum over words of ctf. */
      long ctfTot;
      /* Sum over words of df. */
      int dfTot;  
      /* Sum over words of average tf. */
      mutable long double avetfTot;
      /* Indicates whether avetfTot is valid (true)
       * or needs to be recalculated (false). */
      mutable bool atfValid;
      /* Random selection mode. */
      int randomMode;
      /* Number of unique words. */
      int nWords;


    };
 
  }
}


#endif
