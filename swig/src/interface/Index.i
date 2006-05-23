
namespace lemur {
  namespace api {

    // these need to delete themselves.
    %newobject Index::termInfoList;
    %newobject Index::termInfoListSeq;
    %newobject Index::docInfoList;

    
    class Index {
    public:
      bool open(const std::string &indexName) throw (lemur::api::Exception) = 0;
      TERMID_T term(const TERM_T &word)  const throw (lemur::api::Exception) ;
      const TERM_T term(TERMID_T termID)  const throw (lemur::api::Exception) ;
      DOCID_T document (const EXDOCID_T &docIDStr) const throw (lemur::api::Exception) ;
      const EXDOCID_T document (DOCID_T docID)  const throw (lemur::api::Exception) ;
      const lemur::api::DocumentManager* docManager(DOCID_T docID) const throw (lemur::api::Exception) ;
      COUNT_T docCount () const throw (lemur::api::Exception) ;
      COUNT_T termCountUnique () const throw (lemur::api::Exception) ;
      COUNT_T termCount (TERMID_T termID)  const throw (lemur::api::Exception);
      COUNT_T termCount () const throw (lemur::api::Exception) ;
      float docLengthAvg() const throw (lemur::api::Exception) ;
      COUNT_T docCount(TERMID_T termID) const throw (lemur::api::Exception) ;
      COUNT_T docLength (DOCID_T docID) const throw (lemur::api::Exception) ;
      lemur::api::DocInfoList *docInfoList(TERMID_T termID) const throw (lemur::api::Exception) ;
      lemur::api::TermInfoList *termInfoList(DOCID_T docID) const throw (lemur::api::Exception) ;
      lemur::api::TermInfoList *termInfoListSeq(DOCID_T docID) const throw (lemur::api::Exception) ;
      // probably don't want this one
      //      const lemur::parse::CollectionProps* collectionProps() const ;
%extend {
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
    %nodefault IndexManager;
    %newobject IndexManager::openIndex;
#ifdef SWIGJAVA
%javamethodmodifiers IndexManager::openIndex(const std::string &indexTOCFile) "
/**
An utility function to open an index, automatically recognizing the indexer based on file extension. 
@param indexTOCFile the name of the index table of contents file, or base
repository directory for an indri repository.
@return reference to the open Index.
@throws Exception if the index fails to open.
*/
public";
#endif    
    class IndexManager {
    public:
      static Index *openIndex(const std::string &indexTOCFile) throw (lemur::api::Exception);
    };
  }
}
