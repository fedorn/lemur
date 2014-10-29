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
