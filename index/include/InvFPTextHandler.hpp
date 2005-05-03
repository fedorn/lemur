/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

#include "TextHandler.hpp"
#include "InvPushIndex.hpp"
#include "InvFPPushIndex.hpp"
#include "BasicCollectionProps.hpp"

#ifndef _INVFPTEXTHANDLER_HPP
#define _INVFPTEXTHANDLER_HPP

namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  InvFPTextHandler builds an InvFPIndex using InvFPPushIndex.
    ///  This class is a destination TextHandler.  
    ///

    class InvFPTextHandler : public lemur::api::TextHandler {

    public:
      // want to subclass this class for incremental.
      InvFPTextHandler() { }
  
      /// Create a InvFPTextHandler with index name filename 
      /// (minus the .ifp extension) and specified buffer size.
      InvFPTextHandler(const string &filename, int bufferSize, 
                       bool countStopWds = false, int ind = 1);
      ~InvFPTextHandler();

      /// Pushes the doc into the InvFPPushIndex.
      char * handleDoc(char * docno);
      /// Pushes the word into the InvFPPushIndex.
      char * handleWord(char * word);

      /// passes mgrID onto index
      void setDocManager(const string &mgrID);
      // want to subclass this class for incremental.
    protected:
      /// Ends a document in the collection
      void endDoc();
      void endCollection();

      /// The indexer.
      lemur::index::PushIndex * index;
      /// Document properties object to be reused over repeated calls.
      DocumentProps * dp;
      /// Term object to be reused over repeated calls.
      lemur::index::InvFPTerm * term;
      /// Keeps track of document length.
      int docLength;
      /// Keeps track of term positions.
      int pos;
      /// Indicates whether or not to count stop words in doc length
      bool countStopWds;

      /// Indicates whether object is before first doc
      /// true = yes, false = foundDoc has been called once.
      bool first;

    };
  }
}

#endif

