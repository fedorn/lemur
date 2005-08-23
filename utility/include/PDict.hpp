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


/* 
 * dmf 02/2004
 */
#ifndef _LEMUR_PDICT_HPP
#define _LEMUR_PDICT_HPP

#include "common_headers.hpp"
#include "algorithm"
#include "Keyfile.hpp"
#include "File.hpp"
#include "TextHandlerManager.hpp"
namespace lemur 
{
  /// probabilistic dictionary components.
  namespace dictionary 
  {
    
    /*! \brief container for dictionary entries.
     */
    class DictEntry {
    public:
      /// constructor
      DictEntry();

      /// constructor
      DictEntry(const string &targ, const string &typ, double pr) : target(targ), type(typ), 
                                                                    prob(pr) { }
  
      /// \brief Encode for binary writing.
      /// @param numBytes returns length of return value.
      /// @return this entry encoded as an array of bytes.
      char *toBytes(int &numBytes) const;

      /// \brief Encode for binary writing.
      /// @param buffer char to encode this entry into.
      /// @return number of bytes used.
      int toBytes(char *buffer) const;

      /// \brief Determine size needed to encode for binary writing.
      /// @return number of bytes needed to encode as an array of bytes.
      int numBytes() const;
      /// \brief Initialize from an array of bytes
      /// @param buffer data to initialize from
      /// @return number of bytes consumed.
      int fromBytes(char *buffer);
      /// Equality on target and type attributes only
      bool operator==(const DictEntry & a) const { 
        return (target == a.target && type == a.type); 
      }
      string toString(string delim = ";") const ;
    
      /// The target language term
      string target;
      /// POS, etc.
      string type;
      /// freq or Prob
      double prob;
    };

    /*!
      \brief Filters on dictionary entries.
    */
    class DictEntryFilter {
    public:
      /// \brief Accept or reject an entry.
      /// @param entry The entry to filter
      virtual bool accept(const DictEntry &entry) const = 0;
      virtual ~DictEntryFilter() {}
    } ;


    /*!
      \brief Accept all entries.
    */
    class AllDictEntryFilter : public DictEntryFilter {
    public:
      /// \brief Accept or reject an entry.
      /// @param entry The entry to filter
      bool accept(const DictEntry &entry) const { return true; }
    };


    /*!
      \brief Accept all entries above a certain probability threshold.
    */
    class ProbDictEntryFilter : public DictEntryFilter {
    public:
      /// \brief Initialize threshold for filter.
      /// @param thresh the value of the filter's threshold. Default 0.
      ProbDictEntryFilter(double thresh = 0.0) : threshold(thresh) {
      }
      /// \brief Accept or reject an entry based on probability threshold.
      /// @param entry The entry to filter
      /// @return true if the entry's probability is greater than threshold, otherwise false.
      bool accept(const DictEntry &entry) const { return entry.prob > threshold; }
    private:
      double threshold;
    };


    /*!
      \brief Accept all entries with a specified type.
    */
    class TypeDictEntryFilter : public DictEntryFilter {
    public:
      /// \brief Initialize filter type value.
      /// @param filtType the type string to filter on.
      TypeDictEntryFilter(const string &filtType) : type(filtType) {
      }
      /// \brief Accept or reject an entry based on type.
      /// @param entry The entry to filter
      /// @return true if the entry's type is equal to the filter type value.
      bool accept(const DictEntry &entry) const { return entry.type == type; }
    private:
      string type;
    };

    /*!
      \brief Accept all entries not on the given stopword list.
      This filters on the target language.
    */
    class StopwordDictEntryFilter : public DictEntryFilter {
    public:
      /// \brief Initialize the Stopper to use.
      /// @param stopwords the stopword list to filter against.
      StopwordDictEntryFilter(const string &stopwords) {
        stopper = lemur::api::TextHandlerManager::createStopper(stopwords);
      }
      /// \brief Accept or reject an entry based on type.
      /// @param entry The entry to filter
      /// @return true if the entry's type is equal to the filter type value.
      bool accept(const DictEntry &entry) const { 
        return !(stopper->stopWord(entry.target.c_str())); 
      }
    private:
      lemur::api::Stopper *stopper;
    };

    /// Vector of dictionary entries, supports sorting on the prob attribute.
    class DictEntryVector : public vector<DictEntry> {
    public:
      DictEntryVector() : vector<DictEntry>() {
      }
      DictEntryVector(char *buffer, DictEntryFilter *filter);
      /// sort the scores in descending order
      void sortScores() {
        sort(this->begin(), this->end(), cmpFn);
      }
      /// \brief Add an entry, overwriting if it already exists
      /// @param entry the new entry to add
      /// @param compose the function to use to combine this entries probability/frequency value with if there is an existing entry in the dictionary. Default is to sum.
      /// @return true if this is a new entry, false if overwriting an existing entry.
      bool addEntry(DictEntry &entry, double (*compose)(double, double) = NULL);

      /// \brief Remove an entry
      /// @param entry the entry to remove
      /// @return true if the entry is removed, false if not found.
      bool removeEntry(DictEntry &entry);

      /// \brief Encode for binary writing.
      /// @param numBytes returns length of return value.
      /// @return this entry encoded as an array of bytes.
      char *toBytes(int &numBytes) const;
      /// \brief Initialize from an array of bytes
      /// @param buffer data to initialize from
      /// @return number of bytes consumed.

      /// \brief Encode for binary writing.
      /// @param buffer to encode into.
      /// Used by normalize to update values.
      void toBytes(char *buffer) const;

      void fromBytes(char *buffer, DictEntryFilter *filter);

      /// \brief Number of active entries. May be <= size().
      /// @return number of active entries in the vector.
      int numEntries() const;

      /// \brief Normalize probabilities of entries to sum to one
      void normalize();
  
    private:
      class DictEntryProbDescending { 
      public: 
        bool operator()(const DictEntry & a, const DictEntry & b) {
          return a.prob > b.prob;
        }
      };
      static DictEntryProbDescending cmpFn;
    };

    /// structure for dictionary statistics in keyfile.
    struct dictStats {
      /// total number of entries.
      int dictSize;
      /// size of source vocabulary, unique terms.
      int sourceSize;
      /// size of target vocabulary, unique terms.
      int targetSize;
    };

    /*!
      \brief Probabilistic dictionary using Keyfile for data storage.
    */
    class PDict {
    public:
      /// default constructor 
      PDict();
  
      /// clean up
      ~PDict();

      /// \brief Get dictionary entries (translations) for a term.
      /// @param term the term to lookup.
      /// @param filter to apply to the entries. If unspecified, defaults to NULL.
      /// @return Pointer to the vector of dictionary entries for the term. 
      /// Caller is responsible for deleting. 
      DictEntryVector *getTranslations(const string &term, 
                                       DictEntryFilter *filter=NULL) const ;
      /// \brief Get the number dictionary entries (translations) for a term.
      /// @param term the term to lookup.
      /// @param filter to apply to the entries. If unspecified, defaults to NULL.
      /// @return Number of dictionary entries for the term after filtering.
      int numTranslations(const string &term, 
                          DictEntryFilter *filter=NULL) const;
      /// \brief Get the total size of the dictionary
      /// @return Total number of pairs in the dictionary
      int getNumPairs() const;

      /// \brief Get the number of unique terms in the source vocabulary
      /// @return Total number of unique source term entries in the dictionary
      int getSourceCount() const;

      /// \brief Get the number of unique terms in the target vocabulary
      /// @return Total number of unique target term entries in the dictionary
      int getTargetCount() const ;

      /// \brief Get the name of the dictionary
      /// @return The name of the dictionary
      const string &getName() const {return name;}

      /// \brief Is the dictionary using counts or probabilities.
      /// @return true if the dictionary contains frequencies, otherwise false.
      bool isUsingCounts() const {return usingCounts;}
  
      /// \brief Set the flag for using counts or probabilities.
      /// @param val true if the dictionary contains frequencies otherwise false.
      void setUsingCounts(bool val) {usingCounts = val;}
  
      /// \brief Add an entry for a term
      /// @param source the key for the entry
      /// @param value the value to add
      /// @param compose the function to use to combine this entries probability/frequency value with if there is an existing entry in the dictionary. Default is to sum.
      /// Replaces the entry for value if one exists.
      void add(const string &source, DictEntry &value, 
               double (*compose)(double, double) = NULL);

      /// \brief Remove an entry for a term
      /// @param source the key for the entry
      /// @param value the value to delete
      void remove(const string &source, DictEntry &value);

      /// \brief Remove all entries for a term
      /// @param source the key for the entry
      void remove(const string &source);

      /// \brief Output dictionary as plain text, separator delimited values.
      /// @param outputName the name of the file to write to.
      /// @param delim the delimiter to use. 
      /// NB single char delimiter ? Escape in source/target.
      void write(const string &outputName, const string &delim);

      /// \brief Input a dictionary from plain text, separator delimited values.
      /// The input file must contain 4 columns. The columns are:<br>
      /// sourceterm;type;targetterm;probability;<br>
      /// where type is an arbitrary symbol, such as a part of speech tag.
      /// @param dictName the file to read
      /// @param delim the delimiter to use.
      /// @param counts true if the input file contains frequencies. Default is false.
      /// @return true if created successfully. Otherwise false.
      /// 
      bool read(const string &dictName, const string &delim, bool counts = false);

      /// \brief Open an existing probabilistic dictionary.
      /// @param dictName the dictionary file to open.
      /// @return true if opened successfully. Otherwise false.
      /// Open an existing dictionary.
      bool open(const string &dictName);

      /// \brief Create a new, empty probabilistic dictionary.
      /// @param dictName the dictionary file to create.
      /// @return true if created successfully. Otherwise false.
      /// Create a new dictionary.
      bool create(const string &dictName);

      /// \brief Close the dictionary.
      /// Flushes all buffers and closes all files.
      void close();
  
      /// \brief Normalize probabilities of entries to sum to one
      /// Normalizes all entries, updating the dictionary.
      void normalize();

      /// \brief Initialize for iteration over all keys.
      void startIteration() {dict.setFirst();}

      /// \brief Get next key's dictionary entry (translations).
      /// @param term set to the term for this entry.
      /// @param filter to apply to the entries. If unspecified, defaults to NULL.
      /// @return Pointer to the vector of dictionary entries for the term. Caller is responsible for deleting. Returns NULL at end of file.
      DictEntryVector *nextTranslations(string &term, 
                                        DictEntryFilter *filter=NULL) const;

    private:
      /// write toc file
      void writeTOC() const;  
      /// known term?
      bool contains(const string &term, lemur::file::Keyfile &keyfile) const;
      /// flush the current entry to table
      void flush();
      /// dictionary statistics
      dictStats stats;
      /// Current term's entry vector
      DictEntryVector* currentVec;
      /// are we storing frequencies or probabilities?
      bool usingCounts;
      /// Current term
      string currentTerm;
      /// base name for dictionary  
      string name;
      /// btree for dictionary entry records.  
      mutable lemur::file::Keyfile dict;
      /// target vocab termName -> freq table (delete if f == 0).
      mutable lemur::file::Keyfile targetIDs;
      /// File for entry data
      mutable lemur::file::File dictEntries;
    };
  }
}

#endif // _LEMUR_PDICT_HPP
