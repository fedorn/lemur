/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "TextHandler.hpp"
#include "WordSet.hpp"
namespace lemur
{
  namespace api
  {
    ///
    ///  \brief Provides a generic parser interface.
    /*! 
      Assumes that the parser uses an acronym list.  If, when developing
      your parser, you do not use an acronym list, you can
      just provide an empty implementation of the setAcroList
      function.
    */

    class Parser : public TextHandler {
    public:
      static const string category;
      static const string identifier;

      Parser();
      virtual ~Parser();

      /// Parse a file.  
      /// use parseFile.  this method will be deprecated in future
      virtual void parse(const string &filename);
  
      /// Parse a file.
      /// implementing subclasses should set parsefile string
      virtual void parseFile(const string &filename) = 0;

      /// Parse a buffer.  
      virtual void parseBuffer(char * buf, int len) = 0;

      /// Set the acronym list.  Can be an empty implementation if
      /// the parser is not designed to deal with acronyms by
      /// using a list. WordSet still belongs to the caller
      virtual void setAcroList(const lemur::utility::WordSet * acronyms);

      /// Set the acronym list from this file.
      virtual void setAcroList(string filename);

      /// return the current byte position of the file being parsed
      virtual long fileTell() const = 0;

      /// return the byte position at the beginning of the current document
      virtual long getDocBytePos() const { return docpos; }

      /// return the name of the file being parsed
      virtual const string getParseFile() const { return parsefile; }

    protected: 
      /// Checks to see if the word is in the acronym list.
      /// Returns false if the list is not set.
      bool isAcronym(const char * word);
      /// clears internal acronym list
      void clearAcros();

      long docpos; 

      string parsefile;
    private:
      /// The acronym list.
      lemur::utility::WordSet * myacros;
      const lemur::utility::WordSet* borrowedacros;
    };
  }
}

#endif
