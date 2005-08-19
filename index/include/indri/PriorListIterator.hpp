
//
// PriorListIterator.hpp
//
// 22 July 2005 -- tds
//

#ifndef INDRI_PRIORLISTITERATOR_HPP
#define INDRI_PRIORLISTITERATOR_HPP

#include "indri/SequentialReadBuffer.hpp"
#include "indri/greedy_vector"

namespace indri {
  namespace collection {
    class PriorListIterator {
    public:
      struct Entry {
        int document;
        double score;
      };
    
    private:
      indri::file::SequentialReadBuffer* _file;
      Entry _entry;
      indri::utility::greedy_vector<double> _lookup;
      
      bool _finished;
      UINT32 _entryCount;
      UINT32 _entryLength;
      UINT32 _tableLength;
      
    public:
      PriorListIterator( indri::file::SequentialReadBuffer* file );
      ~PriorListIterator();
    
      void startIteration();
      void nextEntry();
      void nextEntry( int document );
      Entry* currentEntry();
      bool finished();
    };
  }
}

#endif // INDRI_PRIORLISTITERATOR_HPP
