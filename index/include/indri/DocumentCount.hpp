
//
// DocumentCount
// 
// 24 September 2004 -- tds
//

#ifndef INDRI_DOCUMENTCOUNT_HPP
#define INDRI_DOCUMENTCOUNT_HPP

struct DocumentCount {
  DocumentCount() {}

  DocumentCount( int document, int count ) {
    this->document = document;
    this->count = count;
  }

  int document;
  int count;
};

struct DocumentContextCount {
  DocumentContextCount( int document, int count, int contextSize ) {
    this->document = document;
    this->count = count;
    this->contextSize = contextSize;
  }

  int document;
  int count;
  int contextSize;
};

#endif // INDRI_DOCUMENTCOUNT_HPP
