#ifndef _DOCUMENTPROPS_HPP
#define _DOCUMENTPROPS_HPP

//! this Abstract Class for set of Document Properties

/*! this is a supporting class for classes that want to pass one object
around that would have access methods for properties that a document might
have.  this class should be subclassed, adding access methods for the particular information needs for a particular index.

See BasicDocumentProps for example

*/

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 ==========================================================================*/

class DocumentProps {
public:
  virtual ~DocumentProps() {};

};

#endif
