namespace lemur {
  namespace api {

    // these need to delete themselves.
    %newobject Index::termInfoList;
    %newobject Index::termInfoListSeq;
    %newobject Index::docInfoList;

    
    class Index {
    public:
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Open previously created Index, return true if opened successfully, <tt>indexName</tt> should be the full name of the table-of-content file for the index. E.g., index.key for a KeyfileIncIndex. 

@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      bool open(const std::string &indexName) throw (lemur::api::Exception) = 0;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Convert a term spelling to a termID, returns 0 if out of vocabulary. Valid index starts at 1.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif
      TERMID_T term(const TERM_T &word)  const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Convert a valid termID to its spelling
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      const TERM_T term(TERMID_T termID)  const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Convert a spelling to docID, returns 0 if out of vocabulary. Valid index starts at 1.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      DOCID_T document (const EXDOCID_T &docIDStr) const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Convert a valid docID to its spelling
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      const EXDOCID_T document (DOCID_T docID)  const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
@return the document manager to get at the source
       of the document with this document id
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      const lemur::api::DocumentManager* docManager(DOCID_T docID) const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Total count (i.e., number) of documents in collection
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      COUNT_T docCount () const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Total count of unique terms in collection, i.e., the term vocabulary size
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      COUNT_T termCountUnique () const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Total counts of a term in collection
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      COUNT_T termCount (TERMID_T termID)  const throw (lemur::api::Exception);
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Total counts of all terms in collection
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      COUNT_T termCount () const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Average document length 
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      float docLengthAvg() const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Total counts of doc with a given term
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      COUNT_T docCount(TERMID_T termID) const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       Total counts of terms in a document  
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      COUNT_T docLength (DOCID_T docID) const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       returns a new instance of DocInfoList which represents the doc entries in a term index.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      lemur::api::DocInfoList *docInfoList(TERMID_T termID) const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       returns a new instance of TermInfoList which represents the word entries in a document index.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      lemur::api::TermInfoList *termInfoList(DOCID_T docID) const throw (lemur::api::Exception) ;
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
       returns TermInfoList is sequential representation (not bag of words)
       return NULL list when sequence is not available.
@throws Exception if a lemur::api::Exception was thrown by the JNI library.
*/
public";
#endif

      lemur::api::TermInfoList *termInfoListSeq(DOCID_T docID) const throw (lemur::api::Exception) ;
      %extend {
#ifdef SWIGJAVA
        %javamethodmodifiers  "
/**
Loads the stemmer/stopword properties for the index.
*/
public";
#endif

        /** Loads the stemmer/stopword properties for the index.
         */
        void setProps() 
          {
            const lemur::parse::BasicCollectionProps* props = dynamic_cast<const lemur::parse::BasicCollectionProps*> (self->collectionProps());
            if (props) {
              const lemur::parse::Property* p = NULL;
              props->startIteration();
              while (props->hasMore()) {
                p = props->nextEntry();
                if (p->getType() == lemur::parse::Property::STRING) 
                  lemur::api::ParamSet(p->getName(),(char*)p->getValue());
              }
            }
          }
      };
    };
#ifdef SWIGJAVA
    %nodefault IndexManager;
    %newobject IndexManager::openIndex;
#endif    
    class IndexManager {
    public:
#ifdef SWIGJAVA
      %javamethodmodifiers "
/**
An utility function to open an index, automatically recognizing the indexer based on file extension. 
@param indexTOCFile the name of the index table of contents file, or base
repository directory for an indri repository.
@return reference to the open Index.
@throws Exception if the index fails to open.
*/
public";
#endif
      static Index *openIndex(const std::string &indexTOCFile) throw (lemur::api::Exception);
    };
  }
}
