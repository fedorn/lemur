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
// FileClassEnvironmentFactory
//
// 23 August 2004 -- tds
//

#ifndef INDRI_FILECLASSENVIRONMENTFACTORY_HPP
#define INDRI_FILECLASSENVIRONMENTFACTORY_HPP

#include "indri/FileClassEnvironment.hpp"
#include <string>
#include "indri/HashTable.hpp"

class FileClassEnvironmentFactory {
private:
  std::map<std::string, struct file_class_environment_spec_stl*> _userTable;

public:
  ~FileClassEnvironmentFactory();

  FileClassEnvironment* get( const std::string& name );
  void addFileClass( const std::string& name, 
                     const std::string& iterator,
                     const std::string& parser,
                     const std::string& startDocTag,
                     const std::string& endDocTag,
                     const std::string& endMetadataTag,
                     const std::vector<std::string>& include,
                     const std::vector<std::string>& exclude,
                     const std::vector<std::string>& index,
                     const std::vector<std::string>& metadata, 
                     const std::map<std::string,std::string>& conflations );
};

#endif // INDRI_FILECLASSENVIRONMENTFACTORY_HPP

