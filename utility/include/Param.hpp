/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

 ** 02/2005 -- dmf Rework to use indri Parameters.
 */

#ifndef _ParamH_
#define _ParamH_

#include "lemur-platform.h"
#include "String.hpp"

// replace with #defines?
namespace lemur 
{
  namespace api
  {
    //  Get routines:
    /// \brief Get a parameter value as a string.
    /*!
      @param s the parameter name.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */
    lemur::utility::String ParamGetString(const lemur::utility::String &s, 
                                          const lemur::utility::String &def);
    /// \brief Get a parameter value as a string.
    /*!
      @param s the parameter name.
      @param value reference to a String to place the value into.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */
    lemur::utility::String ParamGetString(const lemur::utility::String &s, 
                                          lemur::utility::String &value, 
                                          const lemur::utility::String &def);
    /*!
      @param s the parameter name.
      @return the value of the parameter or an empty String if unspecified.
     */
    lemur::utility::String ParamGetString(const lemur::utility::String &s);

    /// \brief Get a parameter value as a string.
    /*!
      @param s the parameter name.
      @param value reference to a String to place the value into.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, 
                    lemur::utility::String &value);

    /// \brief Get a parameter value as a string.
    /*!
      @param s the parameter name.
      @param value reference to a String to place the value into.
      @param def the default value for the parameter.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, 
                    lemur::utility::String &value, 
                    const lemur::utility::String &def);

    /// \brief Get a parameter value as an integer.
    /*!
      @param s the parameter name.
      @param value reference to an int to place the value into.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, int &value);

    /// \brief Get a parameter value as an integer.
    /*!
      @param s the parameter name.
      @param value reference to an int to place the value into.
      @param def the default value for the parameter.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, int &value, 
                    const int &def);

    /// \brief Get a parameter value as a double.
    /*!
      @param s the parameter name.
      @param value reference to an int to place the value into.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, double &value);

    /// \brief Get a parameter value as a double.
    /*!
      @param s the parameter name.
      @param value reference to a double to place the value into.
      @param def the default value for the parameter.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, double &value, 
                    const double &def);

    /// \brief Get a parameter value as a float.
    /*!
      @param s the parameter name.
      @param value reference to a float to place the value into.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, float &value);

    /// \brief Get a parameter value as a float.
    /*!
      @param s the parameter name.
      @param value reference to a float to place the value into.
      @param def the default value for the parameter.
      @return true (1) if the parameter was specified, false (0) otherwise.
     */
    int    ParamGet(const lemur::utility::String &s, float &value, 
                    const float &def);

    /// \brief Get a parameter value as an int.
    /*!
      @param s the parameter name.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */
    int    ParamGetInt(const lemur::utility::String &s, int def);

    /// \brief Get a parameter value as a bit (true or false).
    /*!
      @param s the parameter name.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */

    int    ParamGetBit(const lemur::utility::String &s, int def);

    /// \brief Get a parameter value as a double.
    /*!
      @param s the parameter name.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */
    double ParamGetDouble(const lemur::utility::String &s, double def);

    /// \brief Get a parameter value as a float.
    /*!
      @param s the parameter name.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */
    float  ParamGetFloat(const lemur::utility::String &s, float def);

    /// \brief Get a parameter value as an INT64 (long long).
    /*!
      @param s the parameter name.
      @param def the default value for the parameter.
      @return the value of the parameter or the default value if unspecified.
     */
    INT64  ParamGetLongLong(const lemur::utility::String &s, INT64 def);

    //  Miscellaneous:
    ///\brief Load parameters from a parameter file.
    /*! Adds a set of parameters to the parameter stack.
      @param s the parameter file to load.
      @return true if the file was loaded. false if it was not.
     */
    int    ParamPushFile (const lemur::utility::String &s);

    ///\brief Remove parameters from the parameter stack.
    /*! Removes the most recently pushed parameter file from the stack.
      @return an empty String
     */
    lemur::utility::String ParamPopFile (void);
    /// prints the current parameter keys to cerr.
    void   ParamDisplay();

    ///  Clear the parameter stack and deallocate all memory.
    void   ParamClear();

    /// \brief Add a parameter value pair. 
    /*! If no parameter file has been pushed,
      a new empty Parameters object is put on the stack.
    */
    void ParamSet(const lemur::utility::String &key, 
                  const lemur::utility::String &value);
  }
}

#endif
