/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// AnchorTextHarvester
//
// 03 Mar 2008 - mjh
//

#ifndef INDRI_ANCHORTEXTHARVESTER_HPP
#define INDRI_ANCHORTEXTHARVESTER_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include "indri/ParsedDocument.hpp"
#include "indri/ObjectHandler.hpp"
#include "indri/Path.hpp"
#include "lemur-compat.hpp"
#include "Keyfile.hpp"
#include "SortMergeTextFiles.hpp"
#include "SHA1.hpp"

namespace indri
{
  namespace parse
  {
    /*! Writes anchor text from a parsed document out to a file. */
    class AnchorTextHarvester : public ObjectHandler<indri::api::ParsedDocument> {
    private:
      // holds destinationURL->(array of <sourceURL, anchor text> pairs)
      std::ofstream _linkFile;

      // docOrder file (document URL->DOCNO)
      std::ofstream _docOrder;

      char linkFileOutBuffer[5*1024*1024];
      char docOrderOutBuffer[3*1024*1024];

      lemur::file::Keyfile *_docNoKeyfile;
      lemur::file::Keyfile *_redirectKeyfile;

      lemur::utility::SHA1 SHA1Hasher;

    public:
      AnchorTextHarvester(const std::string &linkFilePath, const std::string& docOrderPath, lemur::file::Keyfile *docNoKeyfile, lemur::file::Keyfile *redirectKeyfile=NULL );
      ~AnchorTextHarvester();

      void handle( indri::api::ParsedDocument* document );

    }; // end class AnchorTextHarvester

  } // end namespace parse
} // end namespace indri



#endif // #define INDRI_ANCHORTEXTHARVESTER_HPP
