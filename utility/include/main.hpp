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

#ifndef _MAIN_HPP
#define _MAIN_HPP

/*!
  This package defines a default Lemur main() which 
  calls the application's main function AppMain(), takes
  care of parameter display, and
  handles uncaught exceptions.

  An application is supposed to supply two functions

  1. GetAppParam
  
  This function is to load/read/get the parameter values required by an application.

  2. AppMain

  This is the main function of the application.
  
*/

/// User's application must define this function
extern void GetAppParam();

/// User's application must define this function
extern int AppMain(int argc, char *argv[]);

#endif 
