namespace lemur {
  namespace api {

    class DocumentManager {
    public:
#ifdef SWIGJAVA
      %javamethodmodifiers  "
/**
@return the document with the given docID (usually in raw format)
*/
public";
#endif

      virtual char* getDoc(const std::string &docID) const= 0;
      // need ElemDocMgr to support the elem bits
      %extend {
#ifdef SWIGJAVA
        %javamethodmodifiers  "
/**
       Get an element of a document with element name

@param docid the external document id
@param elt the name of the element to return 
*/
public";
#endif

        char *docElement(const std::string &docid, const std::string &elt) 
          {
            lemur::parse::ElemDocMgr *dm = dynamic_cast<lemur::parse::ElemDocMgr *>(self);
            if (dm)
              return dm->getElement(docid.c_str(), elt.c_str());
            else
              return NULL;
          }
      }
    };
  }
}
