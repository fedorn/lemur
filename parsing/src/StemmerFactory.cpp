/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// StemmerFactory
//
// 5 August 2004 -- tds
//

#include "indri/StemmerFactory.hpp"
#include "indri/PorterStemmerTransformation.hpp"
#include "indri/KrovetzStemmerTransformation.hpp"
#include "Exception.hpp"

#define STEMMER_PORTER ( "Porter" )
#define STEMMER_KROVETZ ( "Krovetz" )

//
// Directions for adding your own stemmer:
//   1. Modify preferredName() to return the normalized name for your stemmer.
///       The idea here is to admit as many names as possible (including possibly misspellings)
//        in parameter files, but still keeping a nice name around in case someone wants to
//        print something to the screen.
//   2. Modify get() to return a copy of your stemmer.
//        Don't keep a pointer to your stemmer around; it is the caller's job to delete the object.
//        You may use the stemmerParams object if you like for additional stemmer options.
//

Transformation* StemmerFactory::get( const std::string& stemmerName, Parameters& stemmerParams ) {
  std::string name = preferredName( stemmerName );

  if( name == STEMMER_PORTER ) {
    return new PorterStemmerTransformation();
  } else if( name == STEMMER_KROVETZ ) {
    return new KrovetzStemmerTransformation( stemmerParams );
  }
  
  LEMUR_THROW( LEMUR_RUNTIME_ERROR, stemmerName + " is not a known stemmer." );
  return 0;
}

std::string StemmerFactory::preferredName( const std::string& name ) {
  if( name[0] == 'k' || name[0] == 'K' ) {
    return STEMMER_KROVETZ;
  } else if( name[0] == 'p' || name[0] == 'P' ) {
    return STEMMER_PORTER;
  }

  return STEMMER_KROVETZ;
}
