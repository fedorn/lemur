#ifndef _DOCUMENTPROPS_HPP
#define _DOCUMENTPROPS_HPP

//! Class for set of Document Properties

/*! this is a supporting class for classes that want to pass one object
around that would have access methods for properties that a document might
have.  this class should be subclassed, adding access methods for the particular information needs for a particular index.

*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 ==========================================================================*/
class DocumentProps {
public:
  DocumentProps() { len=0; sid=NULL;};
  ~DocumentProps() {};
  
  /// set document length
  void length(int dl) { len=dl; };

  /// get document length
  int length() { return len; };

  /// set docID, we're not making a copy
  void stringID(char* did) { sid = did; };

  /// get docID
  char* stringID() { return sid; };

private:
  int len;
  char* sid;
};

#endif
