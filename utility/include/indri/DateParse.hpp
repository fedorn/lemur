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
// DateParse
//
// 13 April 2004 -- tds
//

#ifndef INDRI_DATEPARSE_HPP
#define INDRI_DATEPARSE_HPP

class DateParse {
private:
  static int _parseYear( const std::string& year ) {
    return atoi( year.c_str() );
  }

  static int _parseDay( const std::string& day ) {
    return atoi( day.c_str() );
  }

  static int _parseMonth( const std::string& month ) {
    if( month[0] >= '0' && month[0] <= '9' )
      return atoi( month.c_str() );

    char prefix[4];
    memset( prefix, 0, 4 );

    for( unsigned int i=0; i<4 && i<month.size(); i++ ) {
      prefix[i] = tolower( month[i] );
    }
    
    // j f m a m j j a s o n d
    if( prefix[0] == 'j' ) {
      if( prefix[1] == 'a' ) return 1; // january
      if( prefix[2] == 'n' ) return 6; // june
      if( prefix[2] == 'l' ) return 7; // july
      return 0;
    } else if( prefix[0] == 'f' ) {
      return 2; // february
    } else if( prefix[0] == 'a' ) {
      if( prefix[1] == 'p' ) return 4; // april
      if( prefix[1] == 'u' ) return 8; // august
      return 0;
    } else if( prefix[0] == 'm' ) {
      if( prefix[2] == 'r' ) return 3; // march
      if( prefix[2] == 'y' ) return 5; // may
      return 0;
    } else if( prefix[0] == 's' ) {
      return 9; // september
    } else if( prefix[0] == 'o' ) {
      return 10; // october
    } else if( prefix[0] == 'n' ) {
      return 11; // november
    } else if( prefix[0] == 'd' ) {
      return 12;
    }

    return 0;
  }

public:
  // converts year, month, and day from parser to the number of days since 1600
  static UINT64 convertDate( const std::string& year, const std::string& month, const std::string& day ) {
    int numYear = _parseYear( year );
    int numMonth = _parseMonth( month );
    int numDay = _parseDay( day );

    int monthCumulativeDays[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    if( numMonth == 0 || numYear < 1601 || numDay == 0 )
      return 0;

    // let's do days since 1600 here
    UINT64 totalDays = 0;
    UINT64 yearsSince = numYear - 1600;
    
    // every 4 years is a leap year, except every 100 years is not, except every 400 years actually is...
    UINT64 leapDays = yearsSince / 4 -
                      yearsSince / 100 +
                      yearsSince / 400 + 
                      1; // 1 leap day in 1600

    // if this year is a leap year and it's past febuary, add an extra year on
    if( numMonth > 2 && (numYear % 4) == 0 && ( ((numYear % 100) != 0) || (numYear % 400) == 0) )
      leapDays++;

    return (numDay-1) + monthCumulativeDays[numMonth-1] + yearsSince*365 + leapDays;
  }
};

#endif // INDRI_DATEPARSE_HPP

