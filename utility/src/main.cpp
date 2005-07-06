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



#include "main.hpp"
#include "Exception.hpp"
#include "Param.hpp"
#include "common_headers.hpp"

int main(int argc, char *argv[])
{
  if (argc>=2 && strcmp(argv[1], "-help") &&
      strcmp(argv[1], "--help") &&
      strcmp(argv[1], "-h") ) {  // recognize parameter file as argument
    if (!lemur::api::ParamPushFile(argv[1])) 
      {
        cerr << "Unable to load parameter file: " << argv[1] << endl;;
        exit(1);
      } 
  } else {
    
    // handle "-help", display parameters 
    GetAppParam(); // called only  to "register" parameters
    cerr << "Parameters for " <<  argv[0] << endl;
    lemur::api::ParamDisplay();
    exit(0);
  } 
  GetAppParam();

  int result = 1;
  try {
    result = AppMain(argc,argv);
  } 
  catch (lemur::api::Exception &ex) {
    ex.writeMessage();
    cerr << "Program aborted due to exception" << endl;;
    exit(1);
  } 
  // clean up
  lemur::api::ParamClear();
  return result;
}
