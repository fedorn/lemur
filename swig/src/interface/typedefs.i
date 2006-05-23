namespace lemur {
  namespace api {
    // typedefs from IndexTypes.hpp
    // should probably just provide separate typemaps.
    typedef int   FILEID_T;
    typedef int TERMID_T;
    typedef TERMID_T LOC_T;
    typedef TERMID_T DOCID_T;
    typedef TERMID_T COUNT_T;
    typedef float SCORE_T;
    typedef std::string TERM_T;
    typedef std::string EXDOCID_T;
    typedef int POS_T;  // Used by DocLists and TermLists

    typedef lemur::utility::WeightedIDSet DocIDSet;
  }
}
