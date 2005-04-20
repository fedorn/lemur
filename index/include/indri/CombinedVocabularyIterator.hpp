
//
// CombinedVocabularyIterator
//
// 20 January 2005 -- tds
//

#ifndef INDRI_COMBINEDVOCABULARYITERATOR_HPP
#define INDRI_COMBINEDVOCABULARYITERATOR_HPP

#include "indri/VocabularyIterator.hpp"
#include "indri/DiskTermData.hpp"

namespace indri {
  namespace index {
    class CombinedVocabularyIterator : public VocabularyIterator {
    private:
      VocabularyIterator* _first;
      VocabularyIterator* _second;
      int _secondBase;
      bool _finished;
      bool _usingSecond;

    public:
      CombinedVocabularyIterator( VocabularyIterator* first, VocabularyIterator* second, int secondBase );
      ~CombinedVocabularyIterator();

      void startIteration();
      bool nextEntry();
      DiskTermData* currentEntry();
      bool finished();
    };
  }
}

#endif // INDRI_COMBINEDVOCABULARYITERATOR_HPP

