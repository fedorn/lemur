 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "main.hpp"
#include "Exception.hpp"
#include "Param.hpp"
#include <iostream.h>


int main(int argc, char *argv[])
{
  if (argc==2 && strcmp(argv[1], "-help") &&
      strcmp(argv[1], "--help") &&
      strcmp(argv[1], "-h") ) {  // recognize parameter file as argument
    ParamPushFile(argv[1]);
  } else if (argc==2) { // handle "-help", display parameters 
    GetAppParam(); // called only  to "register" parameters
    cerr << "Parameters for " <<  argv[0] << endl;
    ParamDisplay();
    exit(0);
  } 
  GetAppParam();

  int result = 1;
  try {
    result = AppMain(argc,argv);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    cerr << "Program aborted due to exception" << endl;;
  } 
  return result;
}
