/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "BasicCollectionProps.hpp"
#include <sstream>

string lemur::parse::BasicCollectionProps::getStringProperty(const string& name) const{
  const Property* p = this->getProperty(name);
  string value = "";
  std::stringstream s;
  int v;
  double d;
  if (p) {
    Property::DataType type = p->getType();
    switch (type) {
    case (Property::STDSTRING):
      value = *(string*)p->getValue();
      break;
    case (Property::STRING):
      value = (char*)p->getValue();
      break;
    case (Property::INT):
      v = *(int*)p->getValue();
      s << v;
      value = s.str();
      break;
    case (Property::DOUBLE):    
      d = *(double*)p->getValue();
      s << d;
      value = s.str();
      break;
    }
  }
  return value;
}
