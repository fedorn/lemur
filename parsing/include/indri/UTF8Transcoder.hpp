/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// UTF8Transcoder
//
// 15 September 2005 -- mwb
//

// Simple class that converts Unicode characters to and from UTF-8
// encoding.  This class is used by the TextTokenizer for interpreting
// strings identified as possibly being in UTF-8 encoding, and by the
// UTF8CaseNormalizationTransformation.

#ifndef INDRI_UTF8TRANSCODER_HPP
#define INDRI_UTF8TRANSCODER_HPP

#include "indri/indri-platform.h"
#include "indri/HashTable.hpp"
#include <string.h>

namespace indri {
  namespace parse {
    
    namespace CharClass {

      const int apostrophe = 1;
      const int percent = 2;
      const int control = 3;
      const int currency = 4;
      const int symbol = 5;
      const int letter = 6;
      const int digit = 7;
      const int punctuation = 8;
      const int whitespace = 9;
      const int decimal = 10;
      const int hyphen = 11;
      const int thousand = 12;
    }

    class UTF8Transcoder {

    private:

      // This function checks for a sequence of bytes between and
      // inclusive of 0x80 and 0xBF and returns how many exist.

      int _count_bytes( unsigned char* buf, int index, int max_index, int how_many ) {

        int count = 0;

        for ( int i = index + 1; i <= index + how_many; i++ )
          if ( i < max_index && buf[i] && buf[i] != '\0' && 
               0x80 <= buf[i] && buf[i] <= 0xBF ) 
            count++;

        return count;
      }

      indri::utility::HashTable<UINT64,const int> u;
      void store_interval( indri::utility::HashTable<UINT64,const int>&
                                  table, UINT64 start, UINT64 end, 
                                  const int cls ) {

        for ( UINT64 i = start; i <= end; i++ )
          table.insert( i, cls );
      }


      void _initHT() 
      {        
        if ( u.size() == 0 ) {

          store_interval( u, 0xFF0C, 0xFF0C, CharClass::thousand );
          store_interval( u, 0x002C, 0x002C, CharClass::thousand ); 
          store_interval( u, 0x066C, 0x066C, CharClass::thousand );

          store_interval( u, 0x002D, 0x002D, CharClass::hyphen );
          store_interval( u, 0x2010, 0x2011, CharClass::hyphen );
          store_interval( u, 0xFF0D, 0xFF0D, CharClass::hyphen );

          store_interval( u, 0x002E, 0x002E, CharClass::decimal );
          store_interval( u, 0x066B, 0x066B, CharClass::decimal );
          store_interval( u, 0xFF0E, 0xFF0E, CharClass::decimal );

          store_interval( u, 0x2000, 0x200A, CharClass::whitespace );
          store_interval( u, 0x2028, 0x2029, CharClass::whitespace );
          store_interval( u, 0x202F, 0x202F, CharClass::whitespace );
          store_interval( u, 0x205F, 0x205F, CharClass::whitespace );
          store_interval( u, 0x0020, 0x0020, CharClass::whitespace );
          store_interval( u, 0x1680, 0x1680, CharClass::whitespace );
          store_interval( u, 0x180E, 0x180E, CharClass::whitespace );
          store_interval( u, 0x00A0, 0x00A0, CharClass::whitespace );
          store_interval( u, 0x3000, 0x3000, CharClass::whitespace );

          store_interval( u, 0x17D4, 0x17D6, CharClass::punctuation );
          store_interval( u, 0x17D8, 0x17DA, CharClass::punctuation );
          store_interval( u, 0x10A50, 0x10A58, CharClass::punctuation );
          store_interval( u, 0x0021, 0x0023, CharClass::punctuation );
          store_interval( u, 0x0026, 0x0026, CharClass::punctuation );
          store_interval( u, 0x0028, 0x002A, CharClass::punctuation );
          store_interval( u, 0x002F, 0x002F, CharClass::punctuation );
          store_interval( u, 0x003A, 0x003B, CharClass::punctuation );
          store_interval( u, 0x003F, 0x0040, CharClass::punctuation );
          store_interval( u, 0x005B, 0x0060, CharClass::punctuation );
          store_interval( u, 0x007B, 0x007B, CharClass::punctuation );
          store_interval( u, 0x007D, 0x007E, CharClass::punctuation );
          store_interval( u, 0x1039F, 0x1039F, CharClass::punctuation );
          store_interval( u, 0x0E4F, 0x0E4F, CharClass::punctuation );
          store_interval( u, 0x0E5A, 0x0E5B, CharClass::punctuation );
          store_interval( u, 0x0374, 0x0375, CharClass::punctuation );
          store_interval( u, 0x037E, 0x037E, CharClass::punctuation );
          store_interval( u, 0x0384, 0x0385, CharClass::punctuation );
          store_interval( u, 0x0387, 0x0387, CharClass::punctuation );
          store_interval( u, 0x10FB, 0x10FB, CharClass::punctuation );
          store_interval( u, 0x0DF4, 0x0DF4, CharClass::punctuation );
          store_interval( u, 0x1800, 0x180A, CharClass::punctuation );
          store_interval( u, 0x060C, 0x060D, CharClass::punctuation );
          store_interval( u, 0x061B, 0x061B, CharClass::punctuation );
          store_interval( u, 0x061E, 0x061F, CharClass::punctuation );
          store_interval( u, 0x066D, 0x066D, CharClass::punctuation );
          store_interval( u, 0x06D4, 0x06D4, CharClass::punctuation );
          store_interval( u, 0xFD3E, 0xFD3F, CharClass::punctuation );
          store_interval( u, 0xFE10, 0xFE19, CharClass::punctuation );
          store_interval( u, 0xFE30, 0xFE52, CharClass::punctuation );
          store_interval( u, 0xFE54, 0xFE61, CharClass::punctuation );
          store_interval( u, 0xFE63, 0xFE63, CharClass::punctuation );
          store_interval( u, 0xFE68, 0xFE68, CharClass::punctuation );
          store_interval( u, 0xFE6A, 0xFE6B, CharClass::punctuation );
          store_interval( u, 0x02C2, 0x02C5, CharClass::punctuation );
          store_interval( u, 0x02D2, 0x02DF, CharClass::punctuation );
          store_interval( u, 0x02E5, 0x02ED, CharClass::punctuation );
          store_interval( u, 0x02EF, 0x02FF, CharClass::punctuation );
          store_interval( u, 0x0F04, 0x0F12, CharClass::punctuation );
          store_interval( u, 0x0F3A, 0x0F3D, CharClass::punctuation );
          store_interval( u, 0x0F85, 0x0F85, CharClass::punctuation );
          store_interval( u, 0x0FD0, 0x0FD1, CharClass::punctuation );
          store_interval( u, 0x1735, 0x1736, CharClass::punctuation );
          store_interval( u, 0x3001, 0x3003, CharClass::punctuation );
          store_interval( u, 0x3008, 0x3011, CharClass::punctuation );
          store_interval( u, 0x3014, 0x301F, CharClass::punctuation );
          store_interval( u, 0x3030, 0x3030, CharClass::punctuation );
          store_interval( u, 0x303D, 0x303D, CharClass::punctuation );
          store_interval( u, 0x30A0, 0x30A0, CharClass::punctuation );
          store_interval( u, 0x30FB, 0x30FB, CharClass::punctuation );
          store_interval( u, 0xFF65, 0xFF65, CharClass::punctuation );
          store_interval( u, 0x169B, 0x169C, CharClass::punctuation );
          store_interval( u, 0x1361, 0x1368, CharClass::punctuation );
          store_interval( u, 0x0589, 0x058A, CharClass::punctuation );
          store_interval( u, 0x19DE, 0x19DF, CharClass::punctuation );
          store_interval( u, 0x2012, 0x2015, CharClass::punctuation );
          store_interval( u, 0x2018, 0x2018, CharClass::punctuation );
          store_interval( u, 0x201A, 0x201F, CharClass::punctuation );
          store_interval( u, 0x2026, 0x2026, CharClass::punctuation );
          store_interval( u, 0x2039, 0x203A, CharClass::punctuation );
          store_interval( u, 0x203C, 0x2040, CharClass::punctuation );
          store_interval( u, 0x2044, 0x2049, CharClass::punctuation );
          store_interval( u, 0x204F, 0x204F, CharClass::punctuation );
          store_interval( u, 0x2053, 0x2054, CharClass::punctuation );
          store_interval( u, 0x207D, 0x207E, CharClass::punctuation );
          store_interval( u, 0x208D, 0x208E, CharClass::punctuation );
          store_interval( u, 0x2768, 0x2775, CharClass::punctuation );
          store_interval( u, 0x27C5, 0x27C6, CharClass::punctuation );
          store_interval( u, 0x27E6, 0x27EB, CharClass::punctuation );
          store_interval( u, 0x2983, 0x2998, CharClass::punctuation );
          store_interval( u, 0x29D8, 0x29DB, CharClass::punctuation );
          store_interval( u, 0x29FC, 0x29FD, CharClass::punctuation );
          store_interval( u, 0x2E00, 0x2E17, CharClass::punctuation );
          store_interval( u, 0x2E1C, 0x2E1D, CharClass::punctuation );
          store_interval( u, 0xFF01, 0xFF03, CharClass::punctuation );
          store_interval( u, 0xFF06, 0xFF06, CharClass::punctuation );
          store_interval( u, 0xFF08, 0xFF0A, CharClass::punctuation );
          store_interval( u, 0xFF0F, 0xFF0F, CharClass::punctuation );
          store_interval( u, 0xFF1A, 0xFF1B, CharClass::punctuation );
          store_interval( u, 0xFF1F, 0xFF20, CharClass::punctuation );
          store_interval( u, 0xFF3B, 0xFF40, CharClass::punctuation );
          store_interval( u, 0xFF5B, 0xFF64, CharClass::punctuation );
          store_interval( u, 0x2CF9, 0x2CFC, CharClass::punctuation );
          store_interval( u, 0x2CFE, 0x2CFF, CharClass::punctuation );
          store_interval( u, 0x0964, 0x0965, CharClass::punctuation );
          store_interval( u, 0x0970, 0x0970, CharClass::punctuation );
          store_interval( u, 0x0700, 0x070D, CharClass::punctuation );
          store_interval( u, 0x00A1, 0x00A1, CharClass::punctuation );
          store_interval( u, 0x00A8, 0x00A8, CharClass::punctuation );
          store_interval( u, 0x00AB, 0x00AB, CharClass::punctuation );
          store_interval( u, 0x00AF, 0x00AF, CharClass::punctuation );
          store_interval( u, 0x00B4, 0x00B4, CharClass::punctuation );
          store_interval( u, 0x00B7, 0x00B8, CharClass::punctuation );
          store_interval( u, 0x00BB, 0x00BB, CharClass::punctuation );
          store_interval( u, 0x00BF, 0x00BF, CharClass::punctuation );
          store_interval( u, 0x05BE, 0x05BE, CharClass::punctuation );
          store_interval( u, 0x05C0, 0x05C0, CharClass::punctuation );
          store_interval( u, 0x05C3, 0x05C3, CharClass::punctuation );
          store_interval( u, 0x05C6, 0x05C6, CharClass::punctuation );
          store_interval( u, 0x05F3, 0x05F4, CharClass::punctuation );
          store_interval( u, 0x10100, 0x10101, CharClass::punctuation );
          store_interval( u, 0x1A1E, 0x1A1F, CharClass::punctuation );
          store_interval( u, 0x104A, 0x104F, CharClass::punctuation );
          store_interval( u, 0x16EB, 0x16ED, CharClass::punctuation );
          store_interval( u, 0x1944, 0x1945, CharClass::punctuation );

          store_interval( u, 0x1369, 0x137C, CharClass::digit );
          store_interval( u, 0x0660, 0x0669, CharClass::digit );
          store_interval( u, 0x06F0, 0x06F9, CharClass::digit );
          store_interval( u, 0x1040, 0x1049, CharClass::digit );
          store_interval( u, 0x0AE6, 0x0AEF, CharClass::digit );
          store_interval( u, 0x0D66, 0x0D6F, CharClass::digit );
          store_interval( u, 0x1810, 0x1819, CharClass::digit );
          store_interval( u, 0x0BE6, 0x0BF2, CharClass::digit );
          store_interval( u, 0x1946, 0x194F, CharClass::digit );
          store_interval( u, 0x09E6, 0x09EF, CharClass::digit );
          store_interval( u, 0x09F4, 0x09F9, CharClass::digit );
          store_interval( u, 0x104A0, 0x104A9, CharClass::digit );
          store_interval( u, 0x0ED0, 0x0ED9, CharClass::digit );
          store_interval( u, 0x0F20, 0x0F33, CharClass::digit );
          store_interval( u, 0x0B66, 0x0B6F, CharClass::digit );
          store_interval( u, 0x10107, 0x10133, CharClass::digit );
          store_interval( u, 0x10320, 0x10323, CharClass::digit );
          store_interval( u, 0x00B2, 0x00B3, CharClass::digit );
          store_interval( u, 0x00B9, 0x00B9, CharClass::digit );
          store_interval( u, 0x00BC, 0x00BE, CharClass::digit );
          store_interval( u, 0x1034A, 0x1034A, CharClass::digit );
          store_interval( u, 0x1D7CE, 0x1D7FF, CharClass::digit );
          store_interval( u, 0x0C66, 0x0C6F, CharClass::digit );
          store_interval( u, 0x19D0, 0x19D9, CharClass::digit );
          store_interval( u, 0x0030, 0x0039, CharClass::digit );
          store_interval( u, 0x16EE, 0x16F0, CharClass::digit );
          store_interval( u, 0x0E50, 0x0E59, CharClass::digit );
          store_interval( u, 0x17E0, 0x17E9, CharClass::digit );
          store_interval( u, 0x17F0, 0x17F9, CharClass::digit );
          store_interval( u, 0x3007, 0x3007, CharClass::digit );
          store_interval( u, 0x3021, 0x3029, CharClass::digit );
          store_interval( u, 0x3038, 0x303A, CharClass::digit );
          store_interval( u, 0x3192, 0x3195, CharClass::digit );
          store_interval( u, 0x3220, 0x3229, CharClass::digit );
          store_interval( u, 0x3251, 0x325F, CharClass::digit );
          store_interval( u, 0x3280, 0x3289, CharClass::digit );
          store_interval( u, 0x32B1, 0x32BF, CharClass::digit );
          store_interval( u, 0x0A66, 0x0A6F, CharClass::digit );
          store_interval( u, 0x2070, 0x2070, CharClass::digit );
          store_interval( u, 0x2074, 0x2079, CharClass::digit );
          store_interval( u, 0x2080, 0x2089, CharClass::digit );
          store_interval( u, 0x2153, 0x2183, CharClass::digit );
          store_interval( u, 0x2460, 0x249B, CharClass::digit );
          store_interval( u, 0x24EA, 0x24FF, CharClass::digit );
          store_interval( u, 0x2776, 0x2793, CharClass::digit );
          store_interval( u, 0xFF10, 0xFF19, CharClass::digit );
          store_interval( u, 0x2CFD, 0x2CFD, CharClass::digit );
          store_interval( u, 0x0966, 0x096F, CharClass::digit );
          store_interval( u, 0x103D1, 0x103D5, CharClass::digit );
          store_interval( u, 0x0CE6, 0x0CEF, CharClass::digit );
          store_interval( u, 0x10140, 0x10178, CharClass::digit );
          store_interval( u, 0x1018A, 0x1018A, CharClass::digit );

          store_interval( u, 0x0981, 0x0983, CharClass::letter );
          store_interval( u, 0x09BC, 0x09BC, CharClass::letter );
          store_interval( u, 0x09BE, 0x09C4, CharClass::letter );
          store_interval( u, 0x09C7, 0x09C8, CharClass::letter );
          store_interval( u, 0x09CB, 0x09CD, CharClass::letter );
          store_interval( u, 0x09D7, 0x09D7, CharClass::letter );
          store_interval( u, 0x09E2, 0x09E3, CharClass::letter );
          store_interval( u, 0x10400, 0x1044F, CharClass::letter );
          store_interval( u, 0x10800, 0x10805, CharClass::letter );
          store_interval( u, 0x10808, 0x10808, CharClass::letter );
          store_interval( u, 0x1080A, 0x10835, CharClass::letter );
          store_interval( u, 0x10837, 0x10838, CharClass::letter );
          store_interval( u, 0x1083C, 0x1083C, CharClass::letter );
          store_interval( u, 0x1083F, 0x1083F, CharClass::letter );
          store_interval( u, 0x0300, 0x036F, CharClass::letter );
          store_interval( u, 0x1DC0, 0x1DC3, CharClass::letter );
          store_interval( u, 0x0904, 0x0939, CharClass::letter );
          store_interval( u, 0x093D, 0x093D, CharClass::letter );
          store_interval( u, 0x0950, 0x0950, CharClass::letter );
          store_interval( u, 0x0958, 0x0961, CharClass::letter );
          store_interval( u, 0x097D, 0x097D, CharClass::letter );
          store_interval( u, 0x0610, 0x0615, CharClass::letter );
          store_interval( u, 0x064B, 0x065E, CharClass::letter );
          store_interval( u, 0x0670, 0x0670, CharClass::letter );
          store_interval( u, 0x06D6, 0x06DC, CharClass::letter );
          store_interval( u, 0x06DE, 0x06E4, CharClass::letter );
          store_interval( u, 0x06E7, 0x06E8, CharClass::letter );
          store_interval( u, 0x06EA, 0x06ED, CharClass::letter );
          store_interval( u, 0xFE00, 0xFE0F, CharClass::letter );
          store_interval( u, 0xFE20, 0xFE23, CharClass::letter );
          store_interval( u, 0x05D0, 0x05EA, CharClass::letter );
          store_interval( u, 0x05F0, 0x05F2, CharClass::letter );
          store_interval( u, 0xFB1D, 0xFB1D, CharClass::letter );
          store_interval( u, 0xFB1F, 0xFB28, CharClass::letter );
          store_interval( u, 0xFB2A, 0xFB36, CharClass::letter );
          store_interval( u, 0xFB38, 0xFB3C, CharClass::letter );
          store_interval( u, 0xFB3E, 0xFB3E, CharClass::letter );
          store_interval( u, 0xFB40, 0xFB41, CharClass::letter );
          store_interval( u, 0xFB43, 0xFB44, CharClass::letter );
          store_interval( u, 0xFB46, 0xFB4F, CharClass::letter );
          store_interval( u, 0x0A05, 0x0A0A, CharClass::letter );
          store_interval( u, 0x0A0F, 0x0A10, CharClass::letter );
          store_interval( u, 0x0A13, 0x0A28, CharClass::letter );
          store_interval( u, 0x0A2A, 0x0A30, CharClass::letter );
          store_interval( u, 0x0A32, 0x0A33, CharClass::letter );
          store_interval( u, 0x0A35, 0x0A36, CharClass::letter );
          store_interval( u, 0x0A38, 0x0A39, CharClass::letter );
          store_interval( u, 0x0A59, 0x0A5C, CharClass::letter );
          store_interval( u, 0x0A5E, 0x0A5E, CharClass::letter );
          store_interval( u, 0x0A72, 0x0A74, CharClass::letter );
          store_interval( u, 0x180B, 0x180D, CharClass::letter );
          store_interval( u, 0x18A9, 0x18A9, CharClass::letter );
          store_interval( u, 0x0250, 0x02C1, CharClass::letter );
          store_interval( u, 0x02C6, 0x02D1, CharClass::letter );
          store_interval( u, 0x02E0, 0x02E4, CharClass::letter );
          store_interval( u, 0x02EE, 0x02EE, CharClass::letter );
          store_interval( u, 0x0C01, 0x0C03, CharClass::letter );
          store_interval( u, 0x0C3E, 0x0C44, CharClass::letter );
          store_interval( u, 0x0C46, 0x0C48, CharClass::letter );
          store_interval( u, 0x0C4A, 0x0C4D, CharClass::letter );
          store_interval( u, 0x0C55, 0x0C56, CharClass::letter );
          store_interval( u, 0x2D30, 0x2D65, CharClass::letter );
          store_interval( u, 0x2D6F, 0x2D6F, CharClass::letter );
          store_interval( u, 0x037A, 0x037A, CharClass::letter );
          store_interval( u, 0x0386, 0x0386, CharClass::letter );
          store_interval( u, 0x0388, 0x038A, CharClass::letter );
          store_interval( u, 0x038C, 0x038C, CharClass::letter );
          store_interval( u, 0x038E, 0x03A1, CharClass::letter );
          store_interval( u, 0x03A3, 0x03CE, CharClass::letter );
          store_interval( u, 0x03D0, 0x03F5, CharClass::letter );
          store_interval( u, 0x03F7, 0x03FF, CharClass::letter );
          store_interval( u, 0x302A, 0x302F, CharClass::letter );
          store_interval( u, 0x3099, 0x309A, CharClass::letter );
          store_interval( u, 0x1920, 0x192B, CharClass::letter );
          store_interval( u, 0x1930, 0x193B, CharClass::letter );
          store_interval( u, 0x0E01, 0x0E30, CharClass::letter );
          store_interval( u, 0x0E32, 0x0E33, CharClass::letter );
          store_interval( u, 0x0E40, 0x0E46, CharClass::letter );
          store_interval( u, 0x1780, 0x17B3, CharClass::letter );
          store_interval( u, 0x17D7, 0x17D7, CharClass::letter );
          store_interval( u, 0x17DC, 0x17DC, CharClass::letter );
          store_interval( u, 0x07A6, 0x07B0, CharClass::letter );
          store_interval( u, 0x10380, 0x1039D, CharClass::letter );
          store_interval( u, 0x0483, 0x0486, CharClass::letter );
          store_interval( u, 0x0488, 0x0489, CharClass::letter );
          store_interval( u, 0x17B6, 0x17D3, CharClass::letter );
          store_interval( u, 0x17DD, 0x17DD, CharClass::letter );
          store_interval( u, 0x0E31, 0x0E31, CharClass::letter );
          store_interval( u, 0x0E34, 0x0E3A, CharClass::letter );
          store_interval( u, 0x0E47, 0x0E4E, CharClass::letter );
          store_interval( u, 0x3005, 0x3006, CharClass::letter );
          store_interval( u, 0x3031, 0x3035, CharClass::letter );
          store_interval( u, 0x303B, 0x303C, CharClass::letter );
          store_interval( u, 0x3041, 0x3096, CharClass::letter );
          store_interval( u, 0x309D, 0x309F, CharClass::letter );
          store_interval( u, 0x30A1, 0x30FA, CharClass::letter );
          store_interval( u, 0x30FC, 0x30FF, CharClass::letter );
          store_interval( u, 0x3105, 0x312C, CharClass::letter );
          store_interval( u, 0x3131, 0x318E, CharClass::letter );
          store_interval( u, 0x31A0, 0x31B7, CharClass::letter );
          store_interval( u, 0x31F0, 0x31FF, CharClass::letter );
          store_interval( u, 0x3400, 0x4DB5, CharClass::letter );
          store_interval( u, 0x4E00, 0x9FBB, CharClass::letter );
          store_interval( u, 0xAC00, 0xD7A3, CharClass::letter );
          store_interval( u, 0xF900, 0xFAD9, CharClass::letter );
          store_interval( u, 0xFF66, 0xFFBE, CharClass::letter );
          store_interval( u, 0xFFC2, 0xFFC7, CharClass::letter );
          store_interval( u, 0xFFCA, 0xFFCF, CharClass::letter );
          store_interval( u, 0xFFD2, 0xFFD7, CharClass::letter );
          store_interval( u, 0xFFDA, 0xFFDC, CharClass::letter );
          store_interval( u, 0x20000, 0x2A6D6, CharClass::letter );
          store_interval( u, 0x2F800, 0x2FA1D, CharClass::letter );
          store_interval( u, 0x0A85, 0x0A8D, CharClass::letter );
          store_interval( u, 0x0A8F, 0x0A91, CharClass::letter );
          store_interval( u, 0x0A93, 0x0AA8, CharClass::letter );
          store_interval( u, 0x0AAA, 0x0AB0, CharClass::letter );
          store_interval( u, 0x0AB2, 0x0AB3, CharClass::letter );
          store_interval( u, 0x0AB5, 0x0AB9, CharClass::letter );
          store_interval( u, 0x0ABD, 0x0ABD, CharClass::letter );
          store_interval( u, 0x0AD0, 0x0AD0, CharClass::letter );
          store_interval( u, 0x0AE0, 0x0AE1, CharClass::letter );
          store_interval( u, 0x0531, 0x0556, CharClass::letter );
          store_interval( u, 0x0559, 0x0559, CharClass::letter );
          store_interval( u, 0x055B, 0x055F, CharClass::letter );
          store_interval( u, 0x0561, 0x0587, CharClass::letter );
          store_interval( u, 0xFB13, 0xFB17, CharClass::letter );
          store_interval( u, 0x0D05, 0x0D0C, CharClass::letter );
          store_interval( u, 0x0D0E, 0x0D10, CharClass::letter );
          store_interval( u, 0x0D12, 0x0D28, CharClass::letter );
          store_interval( u, 0x0D2A, 0x0D39, CharClass::letter );
          store_interval( u, 0x0D60, 0x0D61, CharClass::letter );
          store_interval( u, 0x19B0, 0x19C0, CharClass::letter );
          store_interval( u, 0x19C8, 0x19C9, CharClass::letter );
          store_interval( u, 0x0901, 0x0903, CharClass::letter );
          store_interval( u, 0x093C, 0x093C, CharClass::letter );
          store_interval( u, 0x093E, 0x094D, CharClass::letter );
          store_interval( u, 0x0951, 0x0954, CharClass::letter );
          store_interval( u, 0x0962, 0x0963, CharClass::letter );
          store_interval( u, 0x1681, 0x169A, CharClass::letter );
          store_interval( u, 0x0D85, 0x0D96, CharClass::letter );
          store_interval( u, 0x0D9A, 0x0DB1, CharClass::letter );
          store_interval( u, 0x0DB3, 0x0DBB, CharClass::letter );
          store_interval( u, 0x0DBD, 0x0DBD, CharClass::letter );
          store_interval( u, 0x0DC0, 0x0DC6, CharClass::letter );
          store_interval( u, 0x1820, 0x1877, CharClass::letter );
          store_interval( u, 0x1880, 0x18A8, CharClass::letter );
          store_interval( u, 0x0B83, 0x0B83, CharClass::letter );
          store_interval( u, 0x0B85, 0x0B8A, CharClass::letter );
          store_interval( u, 0x0B8E, 0x0B90, CharClass::letter );
          store_interval( u, 0x0B92, 0x0B95, CharClass::letter );
          store_interval( u, 0x0B99, 0x0B9A, CharClass::letter );
          store_interval( u, 0x0B9C, 0x0B9C, CharClass::letter );
          store_interval( u, 0x0B9E, 0x0B9F, CharClass::letter );
          store_interval( u, 0x0BA3, 0x0BA4, CharClass::letter );
          store_interval( u, 0x0BA8, 0x0BAA, CharClass::letter );
          store_interval( u, 0x0BAE, 0x0BB9, CharClass::letter );
          store_interval( u, 0x1900, 0x191C, CharClass::letter );
          store_interval( u, 0x1100, 0x1159, CharClass::letter );
          store_interval( u, 0x115F, 0x11A2, CharClass::letter );
          store_interval( u, 0x11A8, 0x11F9, CharClass::letter );
          store_interval( u, 0x1F00, 0x1F15, CharClass::letter );
          store_interval( u, 0x1F18, 0x1F1D, CharClass::letter );
          store_interval( u, 0x1F20, 0x1F45, CharClass::letter );
          store_interval( u, 0x1F48, 0x1F4D, CharClass::letter );
          store_interval( u, 0x1F50, 0x1F57, CharClass::letter );
          store_interval( u, 0x1F59, 0x1F59, CharClass::letter );
          store_interval( u, 0x1F5B, 0x1F5B, CharClass::letter );
          store_interval( u, 0x1F5D, 0x1F5D, CharClass::letter );
          store_interval( u, 0x1F5F, 0x1F7D, CharClass::letter );
          store_interval( u, 0x1F80, 0x1FB4, CharClass::letter );
          store_interval( u, 0x1FB6, 0x1FC4, CharClass::letter );
          store_interval( u, 0x1FC6, 0x1FD3, CharClass::letter );
          store_interval( u, 0x1FD6, 0x1FDB, CharClass::letter );
          store_interval( u, 0x1FDD, 0x1FEC, CharClass::letter );
          store_interval( u, 0x1FF2, 0x1FF4, CharClass::letter );
          store_interval( u, 0x1FF6, 0x1FFE, CharClass::letter );
          store_interval( u, 0x1712, 0x1714, CharClass::letter );
          store_interval( u, 0xA000, 0xA48C, CharClass::letter );
          store_interval( u, 0x0591, 0x05B9, CharClass::letter );
          store_interval( u, 0x05BB, 0x05BD, CharClass::letter );
          store_interval( u, 0x05BF, 0x05BF, CharClass::letter );
          store_interval( u, 0x05C1, 0x05C2, CharClass::letter );
          store_interval( u, 0x05C4, 0x05C5, CharClass::letter );
          store_interval( u, 0x05C7, 0x05C7, CharClass::letter );
          store_interval( u, 0xFB1E, 0xFB1E, CharClass::letter );
          store_interval( u, 0x1401, 0x1676, CharClass::letter );
          store_interval( u, 0x1A00, 0x1A16, CharClass::letter );
          store_interval( u, 0x1732, 0x1734, CharClass::letter );
          store_interval( u, 0x1772, 0x1773, CharClass::letter );
          store_interval( u, 0x0985, 0x098C, CharClass::letter );
          store_interval( u, 0x098F, 0x0990, CharClass::letter );
          store_interval( u, 0x0993, 0x09A8, CharClass::letter );
          store_interval( u, 0x09AA, 0x09B0, CharClass::letter );
          store_interval( u, 0x09B2, 0x09B2, CharClass::letter );
          store_interval( u, 0x09B6, 0x09B9, CharClass::letter );
          store_interval( u, 0x09BD, 0x09BD, CharClass::letter );
          store_interval( u, 0x09CE, 0x09CE, CharClass::letter );
          store_interval( u, 0x09DC, 0x09DD, CharClass::letter );
          store_interval( u, 0x09DF, 0x09E1, CharClass::letter );
          store_interval( u, 0x09F0, 0x09F1, CharClass::letter );
          store_interval( u, 0x10480, 0x1049D, CharClass::letter );
          store_interval( u, 0x0E81, 0x0E82, CharClass::letter );
          store_interval( u, 0x0E84, 0x0E84, CharClass::letter );
          store_interval( u, 0x0E87, 0x0E88, CharClass::letter );
          store_interval( u, 0x0E8A, 0x0E8A, CharClass::letter );
          store_interval( u, 0x0E8D, 0x0E8D, CharClass::letter );
          store_interval( u, 0x0E94, 0x0E97, CharClass::letter );
          store_interval( u, 0x0E99, 0x0E9F, CharClass::letter );
          store_interval( u, 0x0EA1, 0x0EA3, CharClass::letter );
          store_interval( u, 0x0EA5, 0x0EA5, CharClass::letter );
          store_interval( u, 0x0EA7, 0x0EA7, CharClass::letter );
          store_interval( u, 0x0EAA, 0x0EAB, CharClass::letter );
          store_interval( u, 0x0EAD, 0x0EB0, CharClass::letter );
          store_interval( u, 0x0EB2, 0x0EB3, CharClass::letter );
          store_interval( u, 0x0EBD, 0x0EBD, CharClass::letter );
          store_interval( u, 0x0EC0, 0x0EC4, CharClass::letter );
          store_interval( u, 0x0EC6, 0x0EC6, CharClass::letter );
          store_interval( u, 0x0EDC, 0x0EDD, CharClass::letter );
          store_interval( u, 0x0F00, 0x0F00, CharClass::letter );
          store_interval( u, 0x0F40, 0x0F47, CharClass::letter );
          store_interval( u, 0x0F49, 0x0F6A, CharClass::letter );
          store_interval( u, 0x0F88, 0x0F8B, CharClass::letter );
          store_interval( u, 0x0A81, 0x0A83, CharClass::letter );
          store_interval( u, 0x0ABC, 0x0ABC, CharClass::letter );
          store_interval( u, 0x0ABE, 0x0AC5, CharClass::letter );
          store_interval( u, 0x0AC7, 0x0AC9, CharClass::letter );
          store_interval( u, 0x0ACB, 0x0ACD, CharClass::letter );
          store_interval( u, 0x0AE2, 0x0AE3, CharClass::letter );
          store_interval( u, 0x0B05, 0x0B0C, CharClass::letter );
          store_interval( u, 0x0B13, 0x0B28, CharClass::letter );
          store_interval( u, 0x0B2A, 0x0B30, CharClass::letter );
          store_interval( u, 0x0B32, 0x0B33, CharClass::letter );
          store_interval( u, 0x0B35, 0x0B39, CharClass::letter );
          store_interval( u, 0x0B3D, 0x0B3D, CharClass::letter );
          store_interval( u, 0x0B5C, 0x0B5D, CharClass::letter );
          store_interval( u, 0x0B5F, 0x0B61, CharClass::letter );
          store_interval( u, 0x0B71, 0x0B71, CharClass::letter );
          store_interval( u, 0x1752, 0x1753, CharClass::letter );
          store_interval( u, 0x0780, 0x07A5, CharClass::letter );
          store_interval( u, 0x07B1, 0x07B1, CharClass::letter );
          store_interval( u, 0x10300, 0x1031E, CharClass::letter );
          store_interval( u, 0x0710, 0x0710, CharClass::letter );
          store_interval( u, 0x0712, 0x072F, CharClass::letter );
          store_interval( u, 0x1760, 0x176C, CharClass::letter );
          store_interval( u, 0x176E, 0x1770, CharClass::letter );
          store_interval( u, 0x0B0F, 0x0B10, CharClass::letter );
          store_interval( u, 0x1200, 0x1248, CharClass::letter );
          store_interval( u, 0x124A, 0x124D, CharClass::letter );
          store_interval( u, 0x1250, 0x1256, CharClass::letter );
          store_interval( u, 0x1258, 0x1258, CharClass::letter );
          store_interval( u, 0x125A, 0x125D, CharClass::letter );
          store_interval( u, 0x1260, 0x1288, CharClass::letter );
          store_interval( u, 0x128A, 0x128D, CharClass::letter );
          store_interval( u, 0x1290, 0x12B0, CharClass::letter );
          store_interval( u, 0x12B2, 0x12B5, CharClass::letter );
          store_interval( u, 0x12B8, 0x12BE, CharClass::letter );
          store_interval( u, 0x12C0, 0x12C0, CharClass::letter );
          store_interval( u, 0x12C2, 0x12C5, CharClass::letter );
          store_interval( u, 0x12C8, 0x12D6, CharClass::letter );
          store_interval( u, 0x12D8, 0x1310, CharClass::letter );
          store_interval( u, 0x1312, 0x1315, CharClass::letter );
          store_interval( u, 0x1318, 0x135A, CharClass::letter );
          store_interval( u, 0x1380, 0x138F, CharClass::letter );
          store_interval( u, 0x2D80, 0x2D96, CharClass::letter );
          store_interval( u, 0x2DA0, 0x2DA6, CharClass::letter );
          store_interval( u, 0x2DA8, 0x2DAE, CharClass::letter );
          store_interval( u, 0x2DB0, 0x2DB6, CharClass::letter );
          store_interval( u, 0x2DB8, 0x2DBE, CharClass::letter );
          store_interval( u, 0x2DC0, 0x2DC6, CharClass::letter );
          store_interval( u, 0x2DC8, 0x2DCE, CharClass::letter );
          store_interval( u, 0x2DD0, 0x2DD6, CharClass::letter );
          store_interval( u, 0x2DD8, 0x2DDE, CharClass::letter );
          store_interval( u, 0x1E00, 0x1E9B, CharClass::letter );
          store_interval( u, 0x1EA0, 0x1EF9, CharClass::letter );
          store_interval( u, 0x1950, 0x196D, CharClass::letter );
          store_interval( u, 0x1970, 0x1974, CharClass::letter );
          store_interval( u, 0x0EB1, 0x0EB1, CharClass::letter );
          store_interval( u, 0x0EB4, 0x0EB9, CharClass::letter );
          store_interval( u, 0x0EBB, 0x0EBC, CharClass::letter );
          store_interval( u, 0x0EC8, 0x0ECD, CharClass::letter );
          store_interval( u, 0x1D00, 0x1DBF, CharClass::letter );
          store_interval( u, 0x0621, 0x063A, CharClass::letter );
          store_interval( u, 0x0640, 0x064A, CharClass::letter );
          store_interval( u, 0x066E, 0x066F, CharClass::letter );
          store_interval( u, 0x0671, 0x06D3, CharClass::letter );
          store_interval( u, 0x06D5, 0x06D5, CharClass::letter );
          store_interval( u, 0x06E5, 0x06E6, CharClass::letter );
          store_interval( u, 0x06EE, 0x06EF, CharClass::letter );
          store_interval( u, 0x06FA, 0x06FC, CharClass::letter );
          store_interval( u, 0x06FF, 0x06FF, CharClass::letter );
          store_interval( u, 0x0750, 0x076D, CharClass::letter );
          store_interval( u, 0xFB50, 0xFBB1, CharClass::letter );
          store_interval( u, 0xFBD3, 0xFD3D, CharClass::letter );
          store_interval( u, 0xFD50, 0xFD8F, CharClass::letter );
          store_interval( u, 0xFD92, 0xFDC7, CharClass::letter );
          store_interval( u, 0xFDF0, 0xFDFB, CharClass::letter );
          store_interval( u, 0xFE70, 0xFE74, CharClass::letter );
          store_interval( u, 0xFE76, 0xFEFC, CharClass::letter );
          store_interval( u, 0x0C82, 0x0C83, CharClass::letter );
          store_interval( u, 0x0CBC, 0x0CBC, CharClass::letter );
          store_interval( u, 0x0CBE, 0x0CC4, CharClass::letter );
          store_interval( u, 0x0CC6, 0x0CC8, CharClass::letter );
          store_interval( u, 0x0CCA, 0x0CCD, CharClass::letter );
          store_interval( u, 0x0CD5, 0x0CD6, CharClass::letter );
          store_interval( u, 0x0B82, 0x0B82, CharClass::letter );
          store_interval( u, 0x0BBE, 0x0BC2, CharClass::letter );
          store_interval( u, 0x0BC6, 0x0BC8, CharClass::letter );
          store_interval( u, 0x0BCA, 0x0BCD, CharClass::letter );
          store_interval( u, 0x0BD7, 0x0BD7, CharClass::letter );
          store_interval( u, 0x10A0, 0x10C5, CharClass::letter );
          store_interval( u, 0x10D0, 0x10FA, CharClass::letter );
          store_interval( u, 0x10FC, 0x10FC, CharClass::letter );
          store_interval( u, 0x2D00, 0x2D25, CharClass::letter );
          store_interval( u, 0x0F18, 0x0F19, CharClass::letter );
          store_interval( u, 0x0F35, 0x0F35, CharClass::letter );
          store_interval( u, 0x0F37, 0x0F37, CharClass::letter );
          store_interval( u, 0x0F39, 0x0F39, CharClass::letter );
          store_interval( u, 0x0F3E, 0x0F3F, CharClass::letter );
          store_interval( u, 0x0F71, 0x0F84, CharClass::letter );
          store_interval( u, 0x0F86, 0x0F87, CharClass::letter );
          store_interval( u, 0x0F90, 0x0F97, CharClass::letter );
          store_interval( u, 0x0F99, 0x0FBC, CharClass::letter );
          store_interval( u, 0x0FC6, 0x0FC6, CharClass::letter );
          store_interval( u, 0xA800, 0xA801, CharClass::letter );
          store_interval( u, 0xA803, 0xA805, CharClass::letter );
          store_interval( u, 0xA807, 0xA80A, CharClass::letter );
          store_interval( u, 0xA80C, 0xA822, CharClass::letter );
          store_interval( u, 0x1000, 0x1021, CharClass::letter );
          store_interval( u, 0x1023, 0x1027, CharClass::letter );
          store_interval( u, 0x1029, 0x102A, CharClass::letter );
          store_interval( u, 0x1050, 0x1055, CharClass::letter );
          store_interval( u, 0x20D0, 0x20EB, CharClass::letter );
          store_interval( u, 0x00AA, 0x00AA, CharClass::letter );
          store_interval( u, 0x00B5, 0x00B5, CharClass::letter );
          store_interval( u, 0x00BA, 0x00BA, CharClass::letter );
          store_interval( u, 0x00C0, 0x00D6, CharClass::letter );
          store_interval( u, 0x00D8, 0x00F6, CharClass::letter );
          store_interval( u, 0x00F8, 0x0241, CharClass::letter );
          store_interval( u, 0x2071, 0x2071, CharClass::letter );
          store_interval( u, 0x207F, 0x207F, CharClass::letter );
          store_interval( u, 0x2090, 0x2094, CharClass::letter );
          store_interval( u, 0xFB00, 0xFB06, CharClass::letter );
          store_interval( u, 0xFF21, 0xFF3A, CharClass::letter );
          store_interval( u, 0xFF41, 0xFF5A, CharClass::letter );
          store_interval( u, 0x0711, 0x0711, CharClass::letter );
          store_interval( u, 0x0730, 0x074A, CharClass::letter );
          store_interval( u, 0x074D, 0x074F, CharClass::letter );
          store_interval( u, 0x10330, 0x10349, CharClass::letter );
          store_interval( u, 0x10000, 0x1000B, CharClass::letter );
          store_interval( u, 0x1000D, 0x10026, CharClass::letter );
          store_interval( u, 0x10028, 0x1003A, CharClass::letter );
          store_interval( u, 0x1003C, 0x1003D, CharClass::letter );
          store_interval( u, 0x1003F, 0x1004D, CharClass::letter );
          store_interval( u, 0x10050, 0x1005D, CharClass::letter );
          store_interval( u, 0x10080, 0x100FA, CharClass::letter );
          store_interval( u, 0x1D400, 0x1D454, CharClass::letter );
          store_interval( u, 0x1D456, 0x1D49C, CharClass::letter );
          store_interval( u, 0x1D49E, 0x1D49F, CharClass::letter );
          store_interval( u, 0x1D4A2, 0x1D4A2, CharClass::letter );
          store_interval( u, 0x1D4A5, 0x1D4A6, CharClass::letter );
          store_interval( u, 0x1D4A9, 0x1D4AC, CharClass::letter );
          store_interval( u, 0x1D4AE, 0x1D4B9, CharClass::letter );
          store_interval( u, 0x1D4BB, 0x1D4BB, CharClass::letter );
          store_interval( u, 0x1D4BD, 0x1D4C3, CharClass::letter );
          store_interval( u, 0x1D4C5, 0x1D505, CharClass::letter );
          store_interval( u, 0x1D507, 0x1D50A, CharClass::letter );
          store_interval( u, 0x1D50D, 0x1D514, CharClass::letter );
          store_interval( u, 0x1D516, 0x1D51C, CharClass::letter );
          store_interval( u, 0x1D51E, 0x1D539, CharClass::letter );
          store_interval( u, 0x1D53B, 0x1D53E, CharClass::letter );
          store_interval( u, 0x1D540, 0x1D544, CharClass::letter );
          store_interval( u, 0x1D546, 0x1D546, CharClass::letter );
          store_interval( u, 0x1D54A, 0x1D550, CharClass::letter );
          store_interval( u, 0x1D552, 0x1D6A5, CharClass::letter );
          store_interval( u, 0x1D6A8, 0x1D6C0, CharClass::letter );
          store_interval( u, 0x1D6C2, 0x1D6DA, CharClass::letter );
          store_interval( u, 0x1D6DC, 0x1D6FA, CharClass::letter );
          store_interval( u, 0x1D6FC, 0x1D714, CharClass::letter );
          store_interval( u, 0x1D716, 0x1D734, CharClass::letter );
          store_interval( u, 0x1D736, 0x1D74E, CharClass::letter );
          store_interval( u, 0x1D750, 0x1D76E, CharClass::letter );
          store_interval( u, 0x1D770, 0x1D788, CharClass::letter );
          store_interval( u, 0x1D78A, 0x1D7A8, CharClass::letter );
          store_interval( u, 0x1D7AA, 0x1D7C2, CharClass::letter );
          store_interval( u, 0x1D7C4, 0x1D7C9, CharClass::letter );
          store_interval( u, 0x10A00, 0x10A00, CharClass::letter );
          store_interval( u, 0x10A10, 0x10A13, CharClass::letter );
          store_interval( u, 0x10A15, 0x10A17, CharClass::letter );
          store_interval( u, 0x10A19, 0x10A33, CharClass::letter );
          store_interval( u, 0x10A01, 0x10A03, CharClass::letter );
          store_interval( u, 0x10A05, 0x10A06, CharClass::letter );
          store_interval( u, 0x10A0C, 0x10A0F, CharClass::letter );
          store_interval( u, 0x10A38, 0x10A3B, CharClass::letter );
          store_interval( u, 0x10A3F, 0x10A47, CharClass::letter );
          store_interval( u, 0x0D82, 0x0D83, CharClass::letter );
          store_interval( u, 0x0DCA, 0x0DCA, CharClass::letter );
          store_interval( u, 0x0DCF, 0x0DD4, CharClass::letter );
          store_interval( u, 0x0DD6, 0x0DD6, CharClass::letter );
          store_interval( u, 0x0DD8, 0x0DDF, CharClass::letter );
          store_interval( u, 0x0DF2, 0x0DF3, CharClass::letter );
          store_interval( u, 0x2102, 0x2102, CharClass::letter );
          store_interval( u, 0x2107, 0x2107, CharClass::letter );
          store_interval( u, 0x210A, 0x2113, CharClass::letter );
          store_interval( u, 0x2115, 0x2115, CharClass::letter );
          store_interval( u, 0x2119, 0x211D, CharClass::letter );
          store_interval( u, 0x2124, 0x2124, CharClass::letter );
          store_interval( u, 0x2126, 0x2126, CharClass::letter );
          store_interval( u, 0x2128, 0x2128, CharClass::letter );
          store_interval( u, 0x212A, 0x212D, CharClass::letter );
          store_interval( u, 0x212F, 0x2131, CharClass::letter );
          store_interval( u, 0x2133, 0x2139, CharClass::letter );
          store_interval( u, 0x213C, 0x213F, CharClass::letter );
          store_interval( u, 0x2145, 0x2149, CharClass::letter );
          store_interval( u, 0x1720, 0x1731, CharClass::letter );
          store_interval( u, 0x2C80, 0x2CE4, CharClass::letter );
          store_interval( u, 0x1740, 0x1751, CharClass::letter );
          store_interval( u, 0x2C00, 0x2C2E, CharClass::letter );
          store_interval( u, 0x2C30, 0x2C5E, CharClass::letter );
          store_interval( u, 0x0A01, 0x0A03, CharClass::letter );
          store_interval( u, 0x0A3C, 0x0A3C, CharClass::letter );
          store_interval( u, 0x0A3E, 0x0A42, CharClass::letter );
          store_interval( u, 0x0A47, 0x0A48, CharClass::letter );
          store_interval( u, 0x0A4B, 0x0A4D, CharClass::letter );
          store_interval( u, 0x0A70, 0x0A71, CharClass::letter );
          store_interval( u, 0x13A0, 0x13F4, CharClass::letter );
          store_interval( u, 0x10450, 0x1047F, CharClass::letter );
          store_interval( u, 0x0B01, 0x0B03, CharClass::letter );
          store_interval( u, 0x0B3C, 0x0B3C, CharClass::letter );
          store_interval( u, 0x0B3E, 0x0B43, CharClass::letter );
          store_interval( u, 0x0B47, 0x0B48, CharClass::letter );
          store_interval( u, 0x0B4B, 0x0B4D, CharClass::letter );
          store_interval( u, 0x0B56, 0x0B57, CharClass::letter );
          store_interval( u, 0x0C85, 0x0C8C, CharClass::letter );
          store_interval( u, 0x0C8E, 0x0C90, CharClass::letter );
          store_interval( u, 0x0C92, 0x0CA8, CharClass::letter );
          store_interval( u, 0x0CAA, 0x0CB3, CharClass::letter );
          store_interval( u, 0x0CB5, 0x0CB9, CharClass::letter );
          store_interval( u, 0x0CBD, 0x0CBD, CharClass::letter );
          store_interval( u, 0x0CDE, 0x0CDE, CharClass::letter );
          store_interval( u, 0x0CE0, 0x0CE1, CharClass::letter );
          store_interval( u, 0x103A0, 0x103C3, CharClass::letter );
          store_interval( u, 0x103C8, 0x103D0, CharClass::letter );
          store_interval( u, 0x135F, 0x135F, CharClass::letter );
          store_interval( u, 0x0D02, 0x0D03, CharClass::letter );
          store_interval( u, 0x0D3E, 0x0D43, CharClass::letter );
          store_interval( u, 0x0D46, 0x0D48, CharClass::letter );
          store_interval( u, 0x0D4A, 0x0D4D, CharClass::letter );
          store_interval( u, 0x0D57, 0x0D57, CharClass::letter );
          store_interval( u, 0x0C05, 0x0C0C, CharClass::letter );
          store_interval( u, 0x0C0E, 0x0C10, CharClass::letter );
          store_interval( u, 0x0C12, 0x0C28, CharClass::letter );
          store_interval( u, 0x0C2A, 0x0C33, CharClass::letter );
          store_interval( u, 0x0C35, 0x0C39, CharClass::letter );
          store_interval( u, 0x0C60, 0x0C61, CharClass::letter );
          store_interval( u, 0x1700, 0x170C, CharClass::letter );
          store_interval( u, 0x170E, 0x1711, CharClass::letter );
          store_interval( u, 0x0400, 0x0481, CharClass::letter );
          store_interval( u, 0x048A, 0x04CE, CharClass::letter );
          store_interval( u, 0x04D0, 0x04F9, CharClass::letter );
          store_interval( u, 0x0500, 0x050F, CharClass::letter );
          store_interval( u, 0x1980, 0x19A9, CharClass::letter );
          store_interval( u, 0x19C1, 0x19C7, CharClass::letter );
          store_interval( u, 0x102C, 0x1032, CharClass::letter );
          store_interval( u, 0x1036, 0x1039, CharClass::letter );
          store_interval( u, 0x1056, 0x1059, CharClass::letter );
          store_interval( u, 0xA802, 0xA802, CharClass::letter );
          store_interval( u, 0xA806, 0xA806, CharClass::letter );
          store_interval( u, 0xA80B, 0xA80B, CharClass::letter );
          store_interval( u, 0xA823, 0xA827, CharClass::letter );
          store_interval( u, 0x1A17, 0x1A1B, CharClass::letter );
          store_interval( u, 0x0041, 0x005A, CharClass::letter );
          store_interval( u, 0x0061, 0x007A, CharClass::letter );
          store_interval( u, 0x16A0, 0x16EA, CharClass::letter );

          store_interval( u, 0x1D100, 0x1D126, CharClass::symbol );
          store_interval( u, 0x1D12A, 0x1D1DD, CharClass::symbol );
          store_interval( u, 0x1D300, 0x1D356, CharClass::symbol );
          store_interval( u, 0x10179, 0x10189, CharClass::symbol );
          store_interval( u, 0x002B, 0x002B, CharClass::symbol );
          store_interval( u, 0x003C, 0x003E, CharClass::symbol );
          store_interval( u, 0x007C, 0x007C, CharClass::symbol );
          store_interval( u, 0x1FED, 0x1FEF, CharClass::symbol );
          store_interval( u, 0x1940, 0x1940, CharClass::symbol );
          store_interval( u, 0x09FA, 0x09FA, CharClass::symbol );
          store_interval( u, 0x0B70, 0x0B70, CharClass::symbol );
          store_interval( u, 0x2CE5, 0x2CEA, CharClass::symbol );
          store_interval( u, 0x10102, 0x10102, CharClass::symbol );
          store_interval( u, 0x10137, 0x1013F, CharClass::symbol );
          store_interval( u, 0x1D6C1, 0x1D6C1, CharClass::symbol );
          store_interval( u, 0x1D6DB, 0x1D6DB, CharClass::symbol );
          store_interval( u, 0x1D6FB, 0x1D6FB, CharClass::symbol );
          store_interval( u, 0x1D715, 0x1D715, CharClass::symbol );
          store_interval( u, 0x1D735, 0x1D735, CharClass::symbol );
          store_interval( u, 0x1D74F, 0x1D74F, CharClass::symbol );
          store_interval( u, 0x1D76F, 0x1D76F, CharClass::symbol );
          store_interval( u, 0x1D789, 0x1D789, CharClass::symbol );
          store_interval( u, 0x1D7A9, 0x1D7A9, CharClass::symbol );
          store_interval( u, 0x1D7C3, 0x1D7C3, CharClass::symbol );
          store_interval( u, 0xA828, 0xA82B, CharClass::symbol );
          store_interval( u, 0x060E, 0x060F, CharClass::symbol );
          store_interval( u, 0x06E9, 0x06E9, CharClass::symbol );
          store_interval( u, 0x06FD, 0x06FE, CharClass::symbol );
          store_interval( u, 0xFDFD, 0xFDFD, CharClass::symbol );
          store_interval( u, 0xFE62, 0xFE62, CharClass::symbol );
          store_interval( u, 0xFE64, 0xFE66, CharClass::symbol );
          store_interval( u, 0x2016, 0x2017, CharClass::symbol );
          store_interval( u, 0x2020, 0x2025, CharClass::symbol );
          store_interval( u, 0x2032, 0x2038, CharClass::symbol );
          store_interval( u, 0x203B, 0x203B, CharClass::symbol );
          store_interval( u, 0x2041, 0x2043, CharClass::symbol );
          store_interval( u, 0x204A, 0x204E, CharClass::symbol );
          store_interval( u, 0x2050, 0x2052, CharClass::symbol );
          store_interval( u, 0x2055, 0x205E, CharClass::symbol );
          store_interval( u, 0x207A, 0x207C, CharClass::symbol );
          store_interval( u, 0x208A, 0x208C, CharClass::symbol );
          store_interval( u, 0x2100, 0x2101, CharClass::symbol );
          store_interval( u, 0x2103, 0x2106, CharClass::symbol );
          store_interval( u, 0x2108, 0x2109, CharClass::symbol );
          store_interval( u, 0x2114, 0x2114, CharClass::symbol );
          store_interval( u, 0x2116, 0x2118, CharClass::symbol );
          store_interval( u, 0x211E, 0x2123, CharClass::symbol );
          store_interval( u, 0x2125, 0x2125, CharClass::symbol );
          store_interval( u, 0x2127, 0x2127, CharClass::symbol );
          store_interval( u, 0x2129, 0x2129, CharClass::symbol );
          store_interval( u, 0x212E, 0x212E, CharClass::symbol );
          store_interval( u, 0x2132, 0x2132, CharClass::symbol );
          store_interval( u, 0x213A, 0x213B, CharClass::symbol );
          store_interval( u, 0x2140, 0x2144, CharClass::symbol );
          store_interval( u, 0x214A, 0x214C, CharClass::symbol );
          store_interval( u, 0x2190, 0x23DB, CharClass::symbol );
          store_interval( u, 0x2400, 0x2426, CharClass::symbol );
          store_interval( u, 0x2440, 0x244A, CharClass::symbol );
          store_interval( u, 0x249C, 0x24E9, CharClass::symbol );
          store_interval( u, 0x2500, 0x269C, CharClass::symbol );
          store_interval( u, 0x26A0, 0x26B1, CharClass::symbol );
          store_interval( u, 0x2701, 0x2704, CharClass::symbol );
          store_interval( u, 0x2706, 0x2709, CharClass::symbol );
          store_interval( u, 0x270C, 0x2727, CharClass::symbol );
          store_interval( u, 0x2729, 0x274B, CharClass::symbol );
          store_interval( u, 0x274D, 0x274D, CharClass::symbol );
          store_interval( u, 0x274F, 0x2752, CharClass::symbol );
          store_interval( u, 0x2756, 0x2756, CharClass::symbol );
          store_interval( u, 0x2758, 0x275E, CharClass::symbol );
          store_interval( u, 0x2761, 0x2767, CharClass::symbol );
          store_interval( u, 0x2794, 0x2794, CharClass::symbol );
          store_interval( u, 0x2798, 0x27AF, CharClass::symbol );
          store_interval( u, 0x27B1, 0x27BE, CharClass::symbol );
          store_interval( u, 0x27C0, 0x27C4, CharClass::symbol );
          store_interval( u, 0x27D0, 0x27E5, CharClass::symbol );
          store_interval( u, 0x27F0, 0x2982, CharClass::symbol );
          store_interval( u, 0x2999, 0x29D7, CharClass::symbol );
          store_interval( u, 0x29DC, 0x29FB, CharClass::symbol );
          store_interval( u, 0x29FE, 0x2B13, CharClass::symbol );
          store_interval( u, 0xFF0B, 0xFF0B, CharClass::symbol );
          store_interval( u, 0xFF1C, 0xFF1E, CharClass::symbol );
          store_interval( u, 0x2E80, 0x2E99, CharClass::symbol );
          store_interval( u, 0x2E9B, 0x2EF3, CharClass::symbol );
          store_interval( u, 0x2F00, 0x2FD5, CharClass::symbol );
          store_interval( u, 0x2FF0, 0x2FFB, CharClass::symbol );
          store_interval( u, 0x3004, 0x3004, CharClass::symbol );
          store_interval( u, 0x3012, 0x3013, CharClass::symbol );
          store_interval( u, 0x3020, 0x3020, CharClass::symbol );
          store_interval( u, 0x3036, 0x3037, CharClass::symbol );
          store_interval( u, 0x303E, 0x303F, CharClass::symbol );
          store_interval( u, 0x309B, 0x309C, CharClass::symbol );
          store_interval( u, 0x3200, 0x321E, CharClass::symbol );
          store_interval( u, 0x322A, 0x3243, CharClass::symbol );
          store_interval( u, 0x32C0, 0x32FE, CharClass::symbol );
          store_interval( u, 0x3300, 0x33FF, CharClass::symbol );
          store_interval( u, 0xA700, 0xA716, CharClass::symbol );
          store_interval( u, 0xFFE2, 0xFFE4, CharClass::symbol );
          store_interval( u, 0xFFE8, 0xFFEE, CharClass::symbol );
          store_interval( u, 0xFFFC, 0xFFFD, CharClass::symbol );
          store_interval( u, 0x1360, 0x1360, CharClass::symbol );
          store_interval( u, 0x1390, 0x1399, CharClass::symbol );
          store_interval( u, 0x0F01, 0x0F03, CharClass::symbol );
          store_interval( u, 0x0F13, 0x0F17, CharClass::symbol );
          store_interval( u, 0x0F1A, 0x0F1F, CharClass::symbol );
          store_interval( u, 0x0F34, 0x0F34, CharClass::symbol );
          store_interval( u, 0x0F36, 0x0F36, CharClass::symbol );
          store_interval( u, 0x0F38, 0x0F38, CharClass::symbol );
          store_interval( u, 0x0FBE, 0x0FC5, CharClass::symbol );
          store_interval( u, 0x0FC7, 0x0FCC, CharClass::symbol );
          store_interval( u, 0x0FCF, 0x0FCF, CharClass::symbol );
          store_interval( u, 0x1D200, 0x1D245, CharClass::symbol );
          store_interval( u, 0x4DC0, 0x4DFF, CharClass::symbol );
          store_interval( u, 0xFB29, 0xFB29, CharClass::symbol );
          store_interval( u, 0xA490, 0xA4C6, CharClass::symbol );
          store_interval( u, 0x00A6, 0x00A7, CharClass::symbol );
          store_interval( u, 0x00A9, 0x00A9, CharClass::symbol );
          store_interval( u, 0x00AC, 0x00AC, CharClass::symbol );
          store_interval( u, 0x00AE, 0x00AE, CharClass::symbol );
          store_interval( u, 0x00B0, 0x00B1, CharClass::symbol );
          store_interval( u, 0x00B6, 0x00B6, CharClass::symbol );
          store_interval( u, 0x00D7, 0x00D7, CharClass::symbol );
          store_interval( u, 0x00F7, 0x00F7, CharClass::symbol );
          store_interval( u, 0x0482, 0x0482, CharClass::symbol );
          store_interval( u, 0x1D000, 0x1D0F5, CharClass::symbol );
          store_interval( u, 0x3190, 0x3191, CharClass::symbol );
          store_interval( u, 0x3196, 0x319F, CharClass::symbol );
          store_interval( u, 0x31C0, 0x31CF, CharClass::symbol );
          store_interval( u, 0x3260, 0x327F, CharClass::symbol );
          store_interval( u, 0x328A, 0x32B0, CharClass::symbol );
          store_interval( u, 0x19E0, 0x19FF, CharClass::symbol );
          store_interval( u, 0x03F6, 0x03F6, CharClass::symbol );
          store_interval( u, 0x0BF3, 0x0BF8, CharClass::symbol );
          store_interval( u, 0x0BFA, 0x0BFA, CharClass::symbol );
          store_interval( u, 0x3250, 0x3250, CharClass::symbol );

          store_interval( u, 0x09F2, 0x09F3, CharClass::currency );
          store_interval( u, 0x060B, 0x060B, CharClass::currency );
          store_interval( u, 0xFDFC, 0xFDFC, CharClass::currency );
          store_interval( u, 0xFE69, 0xFE69, CharClass::currency );
          store_interval( u, 0xFFE0, 0xFFE1, CharClass::currency );
          store_interval( u, 0xFFE5, 0xFFE6, CharClass::currency );
          store_interval( u, 0x17DB, 0x17DB, CharClass::currency );
          store_interval( u, 0x0E3F, 0x0E3F, CharClass::currency );
          store_interval( u, 0x0AF1, 0x0AF1, CharClass::currency );
          store_interval( u, 0x0BF9, 0x0BF9, CharClass::currency );
          store_interval( u, 0x0024, 0x0024, CharClass::currency );
          store_interval( u, 0x20A0, 0x20B5, CharClass::currency );
          store_interval( u, 0xFF04, 0xFF04, CharClass::currency );
          store_interval( u, 0x00A2, 0x00A5, CharClass::currency );

          store_interval( u, 0x0000, 0x001F, CharClass::control );
          store_interval( u, 0x007F, 0x007F, CharClass::control );
          store_interval( u, 0x070F, 0x070F, CharClass::control );
          store_interval( u, 0x200B, 0x200F, CharClass::control );
          store_interval( u, 0x2027, 0x2027, CharClass::control );
          store_interval( u, 0x202A, 0x202E, CharClass::control );
          store_interval( u, 0x2060, 0x2063, CharClass::control );
          store_interval( u, 0x206A, 0x206F, CharClass::control );
          store_interval( u, 0xFEFF, 0xFEFF, CharClass::control );
          store_interval( u, 0xFFF9, 0xFFFB, CharClass::control );
          store_interval( u, 0xE0001, 0xE0001, CharClass::control );
          store_interval( u, 0xE0020, 0xE007F, CharClass::control );
          store_interval( u, 0xE0100, 0xE01EF, CharClass::control );
          store_interval( u, 0x0080, 0x009F, CharClass::control );
          store_interval( u, 0x00AD, 0x00AD, CharClass::control );
          store_interval( u, 0x0600, 0x0603, CharClass::control );
          store_interval( u, 0x06DD, 0x06DD, CharClass::control );
          store_interval( u, 0x17B4, 0x17B5, CharClass::control );

          store_interval( u, 0x066A, 0x066A, CharClass::percent );
          store_interval( u, 0x2030, 0x2031, CharClass::percent );
          store_interval( u, 0xFF05, 0xFF05, CharClass::percent );
          store_interval( u, 0x0025, 0x0025, CharClass::percent );

          store_interval( u, 0x055A, 0x055A, CharClass::apostrophe );
          store_interval( u, 0x0027, 0x0027, CharClass::apostrophe );
          store_interval( u, 0x2019, 0x2019, CharClass::apostrophe );
          store_interval( u, 0xFF07, 0xFF07, CharClass::apostrophe );

        }
      }

    public:
      UTF8Transcoder() { _initHT();}
      ~UTF8Transcoder() {}

      // This function computes the UTF-8 byte sequence for the
      // specified Unicode character code.  The bytes are written into
      // the specified buffer, which must be large enough to hold the
      // byte sequence (always <= 6 bytes in length) as well as the
      // terminating null.  The number of octets, which equals the
      // number of bytes written to the buffer not including the
      // terminating null, is stored in the octets integer.

      void utf8_encode( UINT64 code, char* buf, int* octets ) {

        int bytes = 0;

        if ( code <= 0x7F ) {

          if ( buf ) {

            buf[0] = (char)code;
            buf[1] = '\0';
          }

          bytes = 1;

        } else if ( code >= 0x80 && code <= 0x7FF ) {

          if ( buf ) {

            buf[0] = 0xC0 | ( 0x1F & ( code >> 6 ) );
            buf[1] = 0x80 | ( 0x3F & code );
            buf[2] = '\0';
          }

          bytes = 2;

        } else if ( code >= 0x800 && code <= 0xFFFF ) {

          if ( buf ) {

            buf[0] = 0xE0 | ( 0xF & ( code >> 12 ) );
            buf[1] = 0x80 | ( 0x3F & ( code >> 6 ) );
            buf[2] = 0x80 | ( 0x3F & code );
            buf[3] = '\0';
          }

          bytes = 3;

        } else if ( code >= 0x1000 && code <= 0x1FFFFF ) {

          if ( buf ) {

            buf[0] = 0xF0 | ( 0x7 & ( code >> 18 ) );
            buf[1] = 0x80 | ( 0x3F & ( code >> 12 ) );
            buf[2] = 0x80 | ( 0x3F & ( code >> 6 ) );
            buf[3] = 0x80 | ( 0x3F & code );
            buf[4] = '\0';
          }

          bytes = 4;

        } else if ( code >= 0x200000 && code <= 0x3FFFFFF ) {

          if ( buf ) {

            buf[0] = 0xF8 | ( 0x7 & ( code >> 24 ) );
            buf[1] = 0x80 | ( 0x3F & ( code >> 18 ) );
            buf[2] = 0x80 | ( 0x3F & ( code >> 12 ) );
            buf[3] = 0x80 | ( 0x3F & ( code >> 6 ) );
            buf[4] = 0x80 | ( 0x3F & code );
            buf[5] = '\0';
          }

          bytes = 5;

        } else if ( code >= 0x4000000 && code <= 0x7FFFFFFF ) {

          if ( buf ) {

            buf[0] = 0xFC | ( 0x1 & ( code >> 30 ) );
            buf[1] = 0x80 | ( 0x3F & ( code >> 24 ) );
            buf[2] = 0x80 | ( 0x3F & ( code >> 18 ) );
            buf[3] = 0x80 | ( 0x3F & ( code >> 12 ) );
            buf[4] = 0x80 | ( 0x3F & ( code >> 6 ) );
            buf[5] = 0x80 | ( 0x3F & code );
            buf[6] = '\0';
          }

          bytes = 5;

        } else {

          if ( buf ) buf[0] = '\0';
        }

        if ( octets ) *octets = bytes;
      }

      // This function decodes a char[] assumed to be in UTF-8
      // encoding.  Results are stored in as unicode codes in the
      // supplied UINT64[] array, which must have as many elements as
      // the char[] array because in the worst case, it contains an
      // ASCII string.  The number of characters decoded is stored in
      // the characters integer, and the number of malformed bytes
      // skipped is stored in the malformed integer.  Offsets and
      // lengths are optional parameters; if not NULL, they will be
      // filled with byte offsets where the UTF-8 characters begin and
      // lengths of each encoding in bytes as they occurr in the input
      // buffer.  Offsets and lengths must have as many elements as
      // the input buffer has bytes.

      void utf8_decode( const char* buf_in, UINT64** codes, int* characters,
                        int* malformed, int** offsets, int** lengths ) {

        int len = strlen( buf_in );
        int i = 0; // index into codes array
        int j = 0; // index into input buf
        UINT64 code = 0;
        bool code_ok;
        int bytes;

        unsigned char* buf = (unsigned char*)buf_in;
        
        while ( j < len ) {

          bool code_ok = false;
          bytes = 0;
          
          if ( buf[j] <= 0x7F ) { // 7-bit ASCII character (One-byte sequence)

            code = (UINT64)buf[j];
            bytes = 1;
            code_ok = true;

          } else if ( 0xC0 <= buf[j] && buf[j] <= 0xDF ) { // Two-byte sequence

            bytes = 1 + _count_bytes( buf, j, len, 1 );

            if ( bytes == 2 ) {

              code = ( ( 0x1F & buf[j] ) << 6 ) + ( 0x3F & buf[j + 1] );
              
              if ( code < 0x80 ) { // Overlong sequence detected

                if ( malformed ) *malformed += 2;

              } else {

                code_ok = true;
              }
            } else { // Subsequent bytes of sequence malformed

              bytes = 1;
              if ( malformed ) *malformed++;
            }

          } else if ( 0xE0 <= buf[j] && buf[j] <= 0xEF ) { // Three-byte sequence

            bytes = 1 + _count_bytes( buf, j, len, 2 );

            if ( bytes == 3 ) {

              code = ( ( 0xF & buf[j] ) << 12 ) + ( ( 0x3F & buf[j + 1] ) << 6 ) +
                ( 0x3F & buf[j + 2] );

              if ( code < 0x800 ) { // Overlong sequence detected
                
                if ( malformed ) *malformed += 3;

              } else {
                
                code_ok = true;
              }
            } else { // Subsequent bytes of sequence malformed
          
              bytes = 1;
              if ( malformed ) *malformed += 3;
            }

          } else if ( 0xF0 <= buf[j] && buf[j] <= 0xF7 ) { // Four-byte sequence

            bytes = 1 + _count_bytes( buf, j, len, 3 );

            if ( bytes == 4 ) {

              code = ( ( 0x7 & buf[j] ) << 18 ) + ( ( 0x3F & buf[j + 1] ) << 12 ) +
                ( ( 0x3F & buf[j + 2] ) << 6 ) + ( 0x3F & buf[j + 3] );

              if ( code < 0x10000 ) { // Overlong sequence detected
                
                if ( malformed ) *malformed += 4;

              } else {

                code_ok = true;
              }
            } else { // Subsequent bytes of sequence malformed

              bytes = 1;
              if ( malformed ) *malformed += 4;
            }

          } else if ( 0xF8 <= buf[j] && buf[j] <= 0xFB ) { // Five-byte sequence

            bytes = 1 + _count_bytes( buf, j, len, 4 );

            if ( bytes == 5 ) {

              code = ( ( 0x3 & buf[j] ) << 24 ) + ( ( 0x3F & buf[j + 1] ) << 18 ) +
                ( ( 0x3F & buf[j + 2] ) << 12 ) + ( ( 0x3F & buf[j + 3] ) << 6 ) +
                ( 0x3F & buf[j + 4] );
              
              if ( code < 0x200000 ) { // Overlong sequence detected
                
                if ( malformed ) *malformed += 5;
              } else {

                code_ok = true;
              }
            } else { // Subsequent bytes of sequence malformed

              bytes = 1;
              if ( malformed ) *malformed += 5;
            }

          } else if ( 0xFC <= buf[j] && buf[j] <= 0xFD ) { // Six-byte sequence

            bytes = 1 + _count_bytes( buf, j, len, 5 );

            if ( bytes == 6 ) {

              code = ( ( 0x1 & buf[j] ) << 30 ) + ( ( 0x3F & buf[j + 1] ) << 24 ) +
                ( ( 0x3F & buf[j + 2] ) << 18 ) + ( ( 0x3F & buf[j + 3] ) << 12 ) + 
                ( ( 0x3F & buf[j + 4] ) << 6 ) + ( 0x3F & buf[j + 5] );

              if ( code < 0x4000000 ) { // Overlong sequence detected
              
                if ( malformed ) *malformed += 6;

              } else {

                code_ok = true;
              }
            } else { // Subsequent bytes of sequence malformed

              bytes = 1;
              if ( malformed ) *malformed += 6;
            }

          } else { // Bytes 0xFE and 0xFF are invalid in UTF-8
            // Bytes 0x80-0xBF are invalid as the first byte of a
            // sequence.

            bytes = 1;
            if ( malformed ) *malformed += 1;
          }

          if ( code_ok ) {

            if ( offsets ) (*offsets)[i] = j;
            if ( lengths ) (*lengths)[i] = bytes;
            if ( codes ) (*codes)[i] = code;
            i++;
            if ( characters ) (*characters)++;
          }

          j += bytes;
        }

        // Terminate the arrays:
        if ( offsets ) (*offsets)[i] = 0;
        if ( lengths ) (*lengths)[i] = 0;
        if ( codes ) (*codes)[i] = 0;
      }
      
      indri::utility::HashTable<UINT64,const int>& unicode() {
        return u;
      }      
    };

  }
}

#endif // INDRI_UTF8TRANSCODER_HPP

