namespace lemur {
  namespace api {

    class DocumentManager {
    public:
      virtual char* getDoc(const std::string &docID) const= 0;
    // need ElemDocMgr to support the elem bits
    %extend {
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
