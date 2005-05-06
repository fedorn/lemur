/*==========================================================================
 *
 * Original source copyright (c) 2000, Carnegie Mellon University.  
 * See copyright.cmu for details.
 * Modifications copyright (c) 2002, University of Massachusetts.
 * See copyright.umass for details.
 *
 *==========================================================================
 */


#ifndef _INDEXEDREAL_HPP
#define _INDEXEDREAL_HPP

#include "common_headers.hpp"
#include <algorithm>

namespace lemur 
{
  namespace api 
  {
    
    /// A list of indexed real numbers (similar to IndexProb)

    /*!

    CLASSES: IndexedReal, IndexedRealVector

    IndexedReal: a real number with an integer index
    IndexedRealVector: A vector of IndexedReal
 
    Possible use: (1) sorting doc scores; (2) representing query vector;
    (3) sorting probabilities

    */

    // CLASSES: IndexedReal, IndexedRealVector (C. Zhai, 12/1/2000)
    // Normalize and LogToPosterior (added dmf, 09/2002)

    struct IndexedReal {
      int ind;
      double val;
    };

    /// \brief A vector of IndexedReal.
    /// 
    class IndexedRealVector : public vector<IndexedReal> {
    public:
  
      IndexedRealVector() : vector<IndexedReal>() {}
      IndexedRealVector(int size) : vector<IndexedReal>(size) {}
      virtual ~IndexedRealVector() {}
      virtual iterator FindByIndex(int index);

      /// increase the value for "index", add the entry if not existing.
      // return true iff the entry already exists.
      virtual bool IncreaseValueFor(int index, double value);

      /// push a value 
      virtual void PushValue(int index, double value);

      /// sort all the values, default is descending order
      virtual void Sort(bool descending = true);

      /// normalize values in a range (0:1), and sum to 1
      virtual void NormalizeValues();

      /// consider input values as log(x), mapping to exp(log(x)), and
      /// normalize x values in a range (0:1), and sum to 1
      virtual void LogToPosterior();


    private:

      /// Function object types, 
      /// defines how to compare IndexedReal objects (for sorting/searching)

      class IndexedRealAscending {
      public:
        bool operator()(const IndexedReal & a, const IndexedReal & b) {
          return a.val < b.val;  // based on the real value
        }
      };

      class IndexedRealDescending { 
      public: 
        bool operator()(const IndexedReal & a, const IndexedReal & b) {
          return a.val > b.val;  // based on the real value
        }
      };

      static IndexedRealAscending ascendOrder;
      static IndexedRealDescending descendOrder;

    };
  }
}
#endif //_INDEXEDREAL_HPP
