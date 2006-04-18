/*==========================================================================
 * Copyright (c) 2003-2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// UTF8CaseNormalizationTransformation
//
// 16 September 2005 -- mwb
//

#include "indri/UTF8CaseNormalizationTransformation.hpp"
#include <map>
#include <vector>

indri::parse::UTF8CaseNormalizationTransformation::UTF8CaseNormalizationTransformation() : _handler(0) { _initHT();}

indri::parse::UTF8CaseNormalizationTransformation::~UTF8CaseNormalizationTransformation() {

  for ( std::vector<char*>::iterator i = _buffers_allocated.begin();
        i != _buffers_allocated.end(); i++ )
    delete[] (*i);
}

void indri::parse::UTF8CaseNormalizationTransformation::handle( indri::api::ParsedDocument* document ) {

  _handler->handle( transform( document ) );
}

void indri::parse::UTF8CaseNormalizationTransformation::setHandler( ObjectHandler<indri::api::ParsedDocument>& handler ) {

  _handler = &handler;
}

indri::api::ParsedDocument* indri::parse::UTF8CaseNormalizationTransformation::transform( indri::api::ParsedDocument* document ) {

  for ( std::vector<char*>::iterator i = _buffers_allocated.begin();
        i != _buffers_allocated.end(); i++ )
    delete[] (*i);
  _buffers_allocated.clear();
  

  // Here, detect UTF-8 strings and downcase them.

  // We don't have access to the original term buffer where these
  // term strings live, so this function will allocate a new
  // buffer for any UTF-8 term that needs to be downcased.
  // These buffers are cleaned up on each call to transform.

  for ( int i = 0; i < document->terms.size(); i++ ) {

    char *term = document->terms[i];
    if ( ! term ) continue;

    // Test for bytes with high bit set; if at least
    // one exists, the string must be UTF-8 analyzed.

    bool utf_8_detected = false;

    for ( char *c = term; *c != '\0'; c++ )
      if ( (unsigned char)*c > 0x7F ) { utf_8_detected = true; break; }

    if ( ! utf_8_detected ) { // In-place ASCII downcase

      for ( char *c = term; *c != '\0'; c++ )
        *c = tolower( *c );

      continue;

    }

    // Must create a new buffer for the term in case the length
    // of the encoded string changes.  This situation is not
    // common, but it may be too expensive to scan each UTF-8
    // string for the cases in which it happens.

    int len = strlen( term );

    char *new_term = new char[len * 2 + 1];
    _buffers_allocated.push_back( new_term );
   
    UINT64* unicode_chars = new UINT64[len + 1];
    _transcoder.utf8_decode( term, &unicode_chars, NULL, NULL,
                             NULL, NULL );

    int buf_index = 0;
    for ( int j = 0; j < len && unicode_chars[j]; j++ ) {

      UINT64* x = _downcase.find( unicode_chars[j] );
      if ( x ) unicode_chars[j] = *x;
              
      // Re-encode and copy into new buffer
      _transcoder.utf8_encode( unicode_chars[j],  
                               new_term + buf_index, NULL );
    }
          
    term = new_term;
    delete[] unicode_chars;
  }

  return document;
}

void indri::parse::UTF8CaseNormalizationTransformation::_initHT() {

  // This map does not include downcasing of ASCII characters
  // (<= 0x7F).

  if ( _downcase.size() == 0 ) {

    // ASCII

    _downcase.insert( 0x41, 0x61 ); _downcase.insert( 0x42, 0x62 );
    _downcase.insert( 0x43, 0x63 ); _downcase.insert( 0x44, 0x64 );
    _downcase.insert( 0x45, 0x65 ); _downcase.insert( 0x46, 0x66 );
    _downcase.insert( 0x47, 0x67 ); _downcase.insert( 0x48, 0x68 );
    _downcase.insert( 0x49, 0x69 ); _downcase.insert( 0x4A, 0x6A );
    _downcase.insert( 0x4B, 0x6B ); _downcase.insert( 0x4C, 0x6C );
    _downcase.insert( 0x4D, 0x6D ); _downcase.insert( 0x4E, 0x6E );
    _downcase.insert( 0x4F, 0x6F ); _downcase.insert( 0x50, 0x70 );
    _downcase.insert( 0x51, 0x71 ); _downcase.insert( 0x52, 0x72 );
    _downcase.insert( 0x53, 0x73 ); _downcase.insert( 0x54, 0x74 );
    _downcase.insert( 0x55, 0x75 ); _downcase.insert( 0x56, 0x76 );
    _downcase.insert( 0x57, 0x77 ); _downcase.insert( 0x58, 0x78 );
    _downcase.insert( 0x59, 0x79 ); _downcase.insert( 0x5A, 0x7A );

    // Multibyte

    _downcase.insert( 0x00C0, 0x00E0 ); _downcase.insert( 0x00C1, 0x00E1 );
    _downcase.insert( 0x00C2, 0x00E2 ); _downcase.insert( 0x00C3, 0x00E3 );
    _downcase.insert( 0x00C4, 0x00E4 ); _downcase.insert( 0x00C5, 0x00E5 );
    _downcase.insert( 0x00C6, 0x00E6 ); _downcase.insert( 0x00C7, 0x00E7 );
    _downcase.insert( 0x00C8, 0x00E8 ); _downcase.insert( 0x00C9, 0x00E9 );
    _downcase.insert( 0x00CA, 0x00EA ); _downcase.insert( 0x00CB, 0x00EB );
    _downcase.insert( 0x00CC, 0x00EC ); _downcase.insert( 0x00CD, 0x00ED );
    _downcase.insert( 0x00CE, 0x00EE ); _downcase.insert( 0x00CF, 0x00EF );
    _downcase.insert( 0x00D0, 0x00F0 ); _downcase.insert( 0x00D1, 0x00F1 );
    _downcase.insert( 0x00D2, 0x00F2 ); _downcase.insert( 0x00D3, 0x00F3 );
    _downcase.insert( 0x00D4, 0x00F4 ); _downcase.insert( 0x00D5, 0x00F5 );
    _downcase.insert( 0x00D6, 0x00F6 ); _downcase.insert( 0x00D8, 0x00F8 );
    _downcase.insert( 0x00D9, 0x00F9 ); _downcase.insert( 0x00DA, 0x00FA );
    _downcase.insert( 0x00DB, 0x00FB ); _downcase.insert( 0x00DC, 0x00FC );
    _downcase.insert( 0x00DD, 0x00FD ); _downcase.insert( 0x00DE, 0x00FE );
    _downcase.insert( 0x0100, 0x0101 ); _downcase.insert( 0x0102, 0x0103 );
    _downcase.insert( 0x0104, 0x0105 ); _downcase.insert( 0x0106, 0x0107 );
    _downcase.insert( 0x0108, 0x0109 ); _downcase.insert( 0x010A, 0x010B );
    _downcase.insert( 0x010C, 0x010D ); _downcase.insert( 0x010E, 0x010F );
    _downcase.insert( 0x0110, 0x0111 ); _downcase.insert( 0x0112, 0x0113 );
    _downcase.insert( 0x0114, 0x0115 ); _downcase.insert( 0x0116, 0x0117 );
    _downcase.insert( 0x0118, 0x0119 ); _downcase.insert( 0x011A, 0x011B );
    _downcase.insert( 0x011C, 0x011D ); _downcase.insert( 0x011E, 0x011F );
    _downcase.insert( 0x0120, 0x0121 ); _downcase.insert( 0x0122, 0x0123 );
    _downcase.insert( 0x0124, 0x0125 ); _downcase.insert( 0x0126, 0x0127 );
    _downcase.insert( 0x0128, 0x0129 ); _downcase.insert( 0x012A, 0x012B );
    _downcase.insert( 0x012C, 0x012D ); _downcase.insert( 0x012E, 0x012F );
    _downcase.insert( 0x0130, 0x0069 ); _downcase.insert( 0x0132, 0x0133 );
    _downcase.insert( 0x0134, 0x0135 ); _downcase.insert( 0x0136, 0x0137 );
    _downcase.insert( 0x0139, 0x013A ); _downcase.insert( 0x013B, 0x013C );
    _downcase.insert( 0x013D, 0x013E ); _downcase.insert( 0x013F, 0x0140 );
    _downcase.insert( 0x0141, 0x0142 ); _downcase.insert( 0x0143, 0x0144 );
    _downcase.insert( 0x0145, 0x0146 ); _downcase.insert( 0x0147, 0x0148 );
    _downcase.insert( 0x014A, 0x014B ); _downcase.insert( 0x014C, 0x014D );
    _downcase.insert( 0x014E, 0x014F ); _downcase.insert( 0x0150, 0x0151 );
    _downcase.insert( 0x0152, 0x0153 ); _downcase.insert( 0x0154, 0x0155 );
    _downcase.insert( 0x0156, 0x0157 ); _downcase.insert( 0x0158, 0x0159 );
    _downcase.insert( 0x015A, 0x015B ); _downcase.insert( 0x015C, 0x015D );
    _downcase.insert( 0x015E, 0x015F ); _downcase.insert( 0x0160, 0x0161 );
    _downcase.insert( 0x0162, 0x0163 ); _downcase.insert( 0x0164, 0x0165 );
    _downcase.insert( 0x0166, 0x0167 ); _downcase.insert( 0x0168, 0x0169 );
    _downcase.insert( 0x016A, 0x016B ); _downcase.insert( 0x016C, 0x016D );
    _downcase.insert( 0x016E, 0x016F ); _downcase.insert( 0x0170, 0x0171 );
    _downcase.insert( 0x0172, 0x0173 ); _downcase.insert( 0x0174, 0x0175 );
    _downcase.insert( 0x0176, 0x0177 ); _downcase.insert( 0x0178, 0x00FF );
    _downcase.insert( 0x0179, 0x017A ); _downcase.insert( 0x017B, 0x017C );
    _downcase.insert( 0x017D, 0x017E ); _downcase.insert( 0x0181, 0x0253 );
    _downcase.insert( 0x0182, 0x0183 ); _downcase.insert( 0x0184, 0x0185 );
    _downcase.insert( 0x0186, 0x0254 ); _downcase.insert( 0x0187, 0x0188 );
    _downcase.insert( 0x0189, 0x0256 ); _downcase.insert( 0x018A, 0x0257 );
    _downcase.insert( 0x018B, 0x018C ); _downcase.insert( 0x018E, 0x01DD );
    _downcase.insert( 0x018F, 0x0259 ); _downcase.insert( 0x0190, 0x025B );
    _downcase.insert( 0x0191, 0x0192 ); _downcase.insert( 0x0193, 0x0260 );
    _downcase.insert( 0x0194, 0x0263 ); _downcase.insert( 0x0196, 0x0269 );
    _downcase.insert( 0x0197, 0x0268 ); _downcase.insert( 0x0198, 0x0199 );
    _downcase.insert( 0x019C, 0x026F ); _downcase.insert( 0x019D, 0x0272 );
    _downcase.insert( 0x019F, 0x0275 ); _downcase.insert( 0x01A0, 0x01A1 );
    _downcase.insert( 0x01A2, 0x01A3 ); _downcase.insert( 0x01A4, 0x01A5 );
    _downcase.insert( 0x01A6, 0x0280 ); _downcase.insert( 0x01A7, 0x01A8 );
    _downcase.insert( 0x01A9, 0x0283 ); _downcase.insert( 0x01AC, 0x01AD );
    _downcase.insert( 0x01AE, 0x0288 ); _downcase.insert( 0x01AF, 0x01B0 );
    _downcase.insert( 0x01B1, 0x028A ); _downcase.insert( 0x01B2, 0x028B );
    _downcase.insert( 0x01B3, 0x01B4 ); _downcase.insert( 0x01B5, 0x01B6 );
    _downcase.insert( 0x01B7, 0x0292 ); _downcase.insert( 0x01B8, 0x01B9 );
    _downcase.insert( 0x01BC, 0x01BD ); _downcase.insert( 0x01C4, 0x01C6 );
    _downcase.insert( 0x01C5, 0x01C6 ); _downcase.insert( 0x01C7, 0x01C9 );
    _downcase.insert( 0x01C8, 0x01C9 ); _downcase.insert( 0x01CA, 0x01CC );
    _downcase.insert( 0x01CB, 0x01CC ); _downcase.insert( 0x01CD, 0x01CE );
    _downcase.insert( 0x01CF, 0x01D0 ); _downcase.insert( 0x01D1, 0x01D2 );
    _downcase.insert( 0x01D3, 0x01D4 ); _downcase.insert( 0x01D5, 0x01D6 );
    _downcase.insert( 0x01D7, 0x01D8 ); _downcase.insert( 0x01D9, 0x01DA );
    _downcase.insert( 0x01DB, 0x01DC ); _downcase.insert( 0x01DE, 0x01DF );
    _downcase.insert( 0x01E0, 0x01E1 ); _downcase.insert( 0x01E2, 0x01E3 );
    _downcase.insert( 0x01E4, 0x01E5 ); _downcase.insert( 0x01E6, 0x01E7 );
    _downcase.insert( 0x01E8, 0x01E9 ); _downcase.insert( 0x01EA, 0x01EB );
    _downcase.insert( 0x01EC, 0x01ED ); _downcase.insert( 0x01EE, 0x01EF );
    _downcase.insert( 0x01F1, 0x01F3 ); _downcase.insert( 0x01F2, 0x01F3 );
    _downcase.insert( 0x01F4, 0x01F5 ); _downcase.insert( 0x01F6, 0x0195 );
    _downcase.insert( 0x01F7, 0x01BF ); _downcase.insert( 0x01F8, 0x01F9 );
    _downcase.insert( 0x01FA, 0x01FB ); _downcase.insert( 0x01FC, 0x01FD );
    _downcase.insert( 0x01FE, 0x01FF ); _downcase.insert( 0x0200, 0x0201 );
    _downcase.insert( 0x0202, 0x0203 ); _downcase.insert( 0x0204, 0x0205 );
    _downcase.insert( 0x0206, 0x0207 ); _downcase.insert( 0x0208, 0x0209 );
    _downcase.insert( 0x020A, 0x020B ); _downcase.insert( 0x020C, 0x020D );
    _downcase.insert( 0x020E, 0x020F ); _downcase.insert( 0x0210, 0x0211 );
    _downcase.insert( 0x0212, 0x0213 ); _downcase.insert( 0x0214, 0x0215 );
    _downcase.insert( 0x0216, 0x0217 ); _downcase.insert( 0x0218, 0x0219 );
    _downcase.insert( 0x021A, 0x021B ); _downcase.insert( 0x021C, 0x021D );
    _downcase.insert( 0x021E, 0x021F ); _downcase.insert( 0x0220, 0x019E );
    _downcase.insert( 0x0222, 0x0223 ); _downcase.insert( 0x0224, 0x0225 );
    _downcase.insert( 0x0226, 0x0227 ); _downcase.insert( 0x0228, 0x0229 );
    _downcase.insert( 0x022A, 0x022B ); _downcase.insert( 0x022C, 0x022D );
    _downcase.insert( 0x022E, 0x022F ); _downcase.insert( 0x0230, 0x0231 );
    _downcase.insert( 0x0232, 0x0233 ); _downcase.insert( 0x023B, 0x023C );
    _downcase.insert( 0x023D, 0x019A ); _downcase.insert( 0x0241, 0x0294 );
    _downcase.insert( 0x0386, 0x03AC ); _downcase.insert( 0x0388, 0x03AD );
    _downcase.insert( 0x0389, 0x03AE ); _downcase.insert( 0x038A, 0x03AF );
    _downcase.insert( 0x038C, 0x03CC ); _downcase.insert( 0x038E, 0x03CD );
    _downcase.insert( 0x038F, 0x03CE ); _downcase.insert( 0x0391, 0x03B1 );
    _downcase.insert( 0x0392, 0x03B2 ); _downcase.insert( 0x0393, 0x03B3 );
    _downcase.insert( 0x0394, 0x03B4 ); _downcase.insert( 0x0395, 0x03B5 );
    _downcase.insert( 0x0396, 0x03B6 ); _downcase.insert( 0x0397, 0x03B7 );
    _downcase.insert( 0x0398, 0x03B8 ); _downcase.insert( 0x0399, 0x03B9 );
    _downcase.insert( 0x039A, 0x03BA ); _downcase.insert( 0x039B, 0x03BB );
    _downcase.insert( 0x039C, 0x03BC ); _downcase.insert( 0x039D, 0x03BD );
    _downcase.insert( 0x039E, 0x03BE ); _downcase.insert( 0x039F, 0x03BF );
    _downcase.insert( 0x03A0, 0x03C0 ); _downcase.insert( 0x03A1, 0x03C1 );
    _downcase.insert( 0x03A3, 0x03C3 ); _downcase.insert( 0x03A4, 0x03C4 );
    _downcase.insert( 0x03A5, 0x03C5 ); _downcase.insert( 0x03A6, 0x03C6 );
    _downcase.insert( 0x03A7, 0x03C7 ); _downcase.insert( 0x03A8, 0x03C8 );
    _downcase.insert( 0x03A9, 0x03C9 ); _downcase.insert( 0x03AA, 0x03CA );
    _downcase.insert( 0x03AB, 0x03CB ); _downcase.insert( 0x03D8, 0x03D9 );
    _downcase.insert( 0x03DA, 0x03DB ); _downcase.insert( 0x03DC, 0x03DD );
    _downcase.insert( 0x03DE, 0x03DF ); _downcase.insert( 0x03E0, 0x03E1 );
    _downcase.insert( 0x03E2, 0x03E3 ); _downcase.insert( 0x03E4, 0x03E5 );
    _downcase.insert( 0x03E6, 0x03E7 ); _downcase.insert( 0x03E8, 0x03E9 );
    _downcase.insert( 0x03EA, 0x03EB ); _downcase.insert( 0x03EC, 0x03ED );
    _downcase.insert( 0x03EE, 0x03EF ); _downcase.insert( 0x03F4, 0x03B8 );
    _downcase.insert( 0x03F7, 0x03F8 ); _downcase.insert( 0x03F9, 0x03F2 );
    _downcase.insert( 0x03FA, 0x03FB ); _downcase.insert( 0x0400, 0x0450 );
    _downcase.insert( 0x0401, 0x0451 ); _downcase.insert( 0x0402, 0x0452 );
    _downcase.insert( 0x0403, 0x0453 ); _downcase.insert( 0x0404, 0x0454 );
    _downcase.insert( 0x0405, 0x0455 ); _downcase.insert( 0x0406, 0x0456 );
    _downcase.insert( 0x0407, 0x0457 ); _downcase.insert( 0x0408, 0x0458 );
    _downcase.insert( 0x0409, 0x0459 ); _downcase.insert( 0x040A, 0x045A );
    _downcase.insert( 0x040B, 0x045B ); _downcase.insert( 0x040C, 0x045C );
    _downcase.insert( 0x040D, 0x045D ); _downcase.insert( 0x040E, 0x045E );
    _downcase.insert( 0x040F, 0x045F ); _downcase.insert( 0x0410, 0x0430 );
    _downcase.insert( 0x0411, 0x0431 ); _downcase.insert( 0x0412, 0x0432 );
    _downcase.insert( 0x0413, 0x0433 ); _downcase.insert( 0x0414, 0x0434 );
    _downcase.insert( 0x0415, 0x0435 ); _downcase.insert( 0x0416, 0x0436 );
    _downcase.insert( 0x0417, 0x0437 ); _downcase.insert( 0x0418, 0x0438 );
    _downcase.insert( 0x0419, 0x0439 ); _downcase.insert( 0x041A, 0x043A );
    _downcase.insert( 0x041B, 0x043B ); _downcase.insert( 0x041C, 0x043C );
    _downcase.insert( 0x041D, 0x043D ); _downcase.insert( 0x041E, 0x043E );
    _downcase.insert( 0x041F, 0x043F ); _downcase.insert( 0x0420, 0x0440 );
    _downcase.insert( 0x0421, 0x0441 ); _downcase.insert( 0x0422, 0x0442 );
    _downcase.insert( 0x0423, 0x0443 ); _downcase.insert( 0x0424, 0x0444 );
    _downcase.insert( 0x0425, 0x0445 ); _downcase.insert( 0x0426, 0x0446 );
    _downcase.insert( 0x0427, 0x0447 ); _downcase.insert( 0x0428, 0x0448 );
    _downcase.insert( 0x0429, 0x0449 ); _downcase.insert( 0x042A, 0x044A );
    _downcase.insert( 0x042B, 0x044B ); _downcase.insert( 0x042C, 0x044C );
    _downcase.insert( 0x042D, 0x044D ); _downcase.insert( 0x042E, 0x044E );
    _downcase.insert( 0x042F, 0x044F ); _downcase.insert( 0x0460, 0x0461 );
    _downcase.insert( 0x0462, 0x0463 ); _downcase.insert( 0x0464, 0x0465 );
    _downcase.insert( 0x0466, 0x0467 ); _downcase.insert( 0x0468, 0x0469 );
    _downcase.insert( 0x046A, 0x046B ); _downcase.insert( 0x046C, 0x046D );
    _downcase.insert( 0x046E, 0x046F ); _downcase.insert( 0x0470, 0x0471 );
    _downcase.insert( 0x0472, 0x0473 ); _downcase.insert( 0x0474, 0x0475 );
    _downcase.insert( 0x0476, 0x0477 ); _downcase.insert( 0x0478, 0x0479 );
    _downcase.insert( 0x047A, 0x047B ); _downcase.insert( 0x047C, 0x047D );
    _downcase.insert( 0x047E, 0x047F ); _downcase.insert( 0x0480, 0x0481 );
    _downcase.insert( 0x048A, 0x048B ); _downcase.insert( 0x048C, 0x048D );
    _downcase.insert( 0x048E, 0x048F ); _downcase.insert( 0x0490, 0x0491 );
    _downcase.insert( 0x0492, 0x0493 ); _downcase.insert( 0x0494, 0x0495 );
    _downcase.insert( 0x0496, 0x0497 ); _downcase.insert( 0x0498, 0x0499 );
    _downcase.insert( 0x049A, 0x049B ); _downcase.insert( 0x049C, 0x049D );
    _downcase.insert( 0x049E, 0x049F ); _downcase.insert( 0x04A0, 0x04A1 );
    _downcase.insert( 0x04A2, 0x04A3 ); _downcase.insert( 0x04A4, 0x04A5 );
    _downcase.insert( 0x04A6, 0x04A7 ); _downcase.insert( 0x04A8, 0x04A9 );
    _downcase.insert( 0x04AA, 0x04AB ); _downcase.insert( 0x04AC, 0x04AD );
    _downcase.insert( 0x04AE, 0x04AF ); _downcase.insert( 0x04B0, 0x04B1 );
    _downcase.insert( 0x04B2, 0x04B3 ); _downcase.insert( 0x04B4, 0x04B5 );
    _downcase.insert( 0x04B6, 0x04B7 ); _downcase.insert( 0x04B8, 0x04B9 );
    _downcase.insert( 0x04BA, 0x04BB ); _downcase.insert( 0x04BC, 0x04BD );
    _downcase.insert( 0x04BE, 0x04BF ); _downcase.insert( 0x04C1, 0x04C2 );
    _downcase.insert( 0x04C3, 0x04C4 ); _downcase.insert( 0x04C5, 0x04C6 );
    _downcase.insert( 0x04C7, 0x04C8 ); _downcase.insert( 0x04C9, 0x04CA );
    _downcase.insert( 0x04CB, 0x04CC ); _downcase.insert( 0x04CD, 0x04CE );
    _downcase.insert( 0x04D0, 0x04D1 ); _downcase.insert( 0x04D2, 0x04D3 );
    _downcase.insert( 0x04D4, 0x04D5 ); _downcase.insert( 0x04D6, 0x04D7 );
    _downcase.insert( 0x04D8, 0x04D9 ); _downcase.insert( 0x04DA, 0x04DB );
    _downcase.insert( 0x04DC, 0x04DD ); _downcase.insert( 0x04DE, 0x04DF );
    _downcase.insert( 0x04E0, 0x04E1 ); _downcase.insert( 0x04E2, 0x04E3 );
    _downcase.insert( 0x04E4, 0x04E5 ); _downcase.insert( 0x04E6, 0x04E7 );
    _downcase.insert( 0x04E8, 0x04E9 ); _downcase.insert( 0x04EA, 0x04EB );
    _downcase.insert( 0x04EC, 0x04ED ); _downcase.insert( 0x04EE, 0x04EF );
    _downcase.insert( 0x04F0, 0x04F1 ); _downcase.insert( 0x04F2, 0x04F3 );
    _downcase.insert( 0x04F4, 0x04F5 ); _downcase.insert( 0x04F6, 0x04F7 );
    _downcase.insert( 0x04F8, 0x04F9 ); _downcase.insert( 0x0500, 0x0501 );
    _downcase.insert( 0x0502, 0x0503 ); _downcase.insert( 0x0504, 0x0505 );
    _downcase.insert( 0x0506, 0x0507 ); _downcase.insert( 0x0508, 0x0509 );
    _downcase.insert( 0x050A, 0x050B ); _downcase.insert( 0x050C, 0x050D );
    _downcase.insert( 0x050E, 0x050F ); _downcase.insert( 0x0531, 0x0561 );
    _downcase.insert( 0x0532, 0x0562 ); _downcase.insert( 0x0533, 0x0563 );
    _downcase.insert( 0x0534, 0x0564 ); _downcase.insert( 0x0535, 0x0565 );
    _downcase.insert( 0x0536, 0x0566 ); _downcase.insert( 0x0537, 0x0567 );
    _downcase.insert( 0x0538, 0x0568 ); _downcase.insert( 0x0539, 0x0569 );
    _downcase.insert( 0x053A, 0x056A ); _downcase.insert( 0x053B, 0x056B );
    _downcase.insert( 0x053C, 0x056C ); _downcase.insert( 0x053D, 0x056D );
    _downcase.insert( 0x053E, 0x056E ); _downcase.insert( 0x053F, 0x056F );
    _downcase.insert( 0x0540, 0x0570 ); _downcase.insert( 0x0541, 0x0571 );
    _downcase.insert( 0x0542, 0x0572 ); _downcase.insert( 0x0543, 0x0573 );
    _downcase.insert( 0x0544, 0x0574 ); _downcase.insert( 0x0545, 0x0575 );
    _downcase.insert( 0x0546, 0x0576 ); _downcase.insert( 0x0547, 0x0577 );
    _downcase.insert( 0x0548, 0x0578 ); _downcase.insert( 0x0549, 0x0579 );
    _downcase.insert( 0x054A, 0x057A ); _downcase.insert( 0x054B, 0x057B );
    _downcase.insert( 0x054C, 0x057C ); _downcase.insert( 0x054D, 0x057D );
    _downcase.insert( 0x054E, 0x057E ); _downcase.insert( 0x054F, 0x057F );
    _downcase.insert( 0x0550, 0x0580 ); _downcase.insert( 0x0551, 0x0581 );
    _downcase.insert( 0x0552, 0x0582 ); _downcase.insert( 0x0553, 0x0583 );
    _downcase.insert( 0x0554, 0x0584 ); _downcase.insert( 0x0555, 0x0585 );
    _downcase.insert( 0x0556, 0x0586 ); _downcase.insert( 0x10A0, 0x2D00 );
    _downcase.insert( 0x10A1, 0x2D01 ); _downcase.insert( 0x10A2, 0x2D02 );
    _downcase.insert( 0x10A3, 0x2D03 ); _downcase.insert( 0x10A4, 0x2D04 );
    _downcase.insert( 0x10A5, 0x2D05 ); _downcase.insert( 0x10A6, 0x2D06 );
    _downcase.insert( 0x10A7, 0x2D07 ); _downcase.insert( 0x10A8, 0x2D08 );
    _downcase.insert( 0x10A9, 0x2D09 ); _downcase.insert( 0x10AA, 0x2D0A );
    _downcase.insert( 0x10AB, 0x2D0B ); _downcase.insert( 0x10AC, 0x2D0C );
    _downcase.insert( 0x10AD, 0x2D0D ); _downcase.insert( 0x10AE, 0x2D0E );
    _downcase.insert( 0x10AF, 0x2D0F ); _downcase.insert( 0x10B0, 0x2D10 );
    _downcase.insert( 0x10B1, 0x2D11 ); _downcase.insert( 0x10B2, 0x2D12 );
    _downcase.insert( 0x10B3, 0x2D13 ); _downcase.insert( 0x10B4, 0x2D14 );
    _downcase.insert( 0x10B5, 0x2D15 ); _downcase.insert( 0x10B6, 0x2D16 );
    _downcase.insert( 0x10B7, 0x2D17 ); _downcase.insert( 0x10B8, 0x2D18 );
    _downcase.insert( 0x10B9, 0x2D19 ); _downcase.insert( 0x10BA, 0x2D1A );
    _downcase.insert( 0x10BB, 0x2D1B ); _downcase.insert( 0x10BC, 0x2D1C );
    _downcase.insert( 0x10BD, 0x2D1D ); _downcase.insert( 0x10BE, 0x2D1E );
    _downcase.insert( 0x10BF, 0x2D1F ); _downcase.insert( 0x10C0, 0x2D20 );
    _downcase.insert( 0x10C1, 0x2D21 ); _downcase.insert( 0x10C2, 0x2D22 );
    _downcase.insert( 0x10C3, 0x2D23 ); _downcase.insert( 0x10C4, 0x2D24 );
    _downcase.insert( 0x10C5, 0x2D25 ); _downcase.insert( 0x1E00, 0x1E01 );
    _downcase.insert( 0x1E02, 0x1E03 ); _downcase.insert( 0x1E04, 0x1E05 );
    _downcase.insert( 0x1E06, 0x1E07 ); _downcase.insert( 0x1E08, 0x1E09 );
    _downcase.insert( 0x1E0A, 0x1E0B ); _downcase.insert( 0x1E0C, 0x1E0D );
    _downcase.insert( 0x1E0E, 0x1E0F ); _downcase.insert( 0x1E10, 0x1E11 );
    _downcase.insert( 0x1E12, 0x1E13 ); _downcase.insert( 0x1E14, 0x1E15 );
    _downcase.insert( 0x1E16, 0x1E17 ); _downcase.insert( 0x1E18, 0x1E19 );
    _downcase.insert( 0x1E1A, 0x1E1B ); _downcase.insert( 0x1E1C, 0x1E1D );
    _downcase.insert( 0x1E1E, 0x1E1F ); _downcase.insert( 0x1E20, 0x1E21 );
    _downcase.insert( 0x1E22, 0x1E23 ); _downcase.insert( 0x1E24, 0x1E25 );
    _downcase.insert( 0x1E26, 0x1E27 ); _downcase.insert( 0x1E28, 0x1E29 );
    _downcase.insert( 0x1E2A, 0x1E2B ); _downcase.insert( 0x1E2C, 0x1E2D );
    _downcase.insert( 0x1E2E, 0x1E2F ); _downcase.insert( 0x1E30, 0x1E31 );
    _downcase.insert( 0x1E32, 0x1E33 ); _downcase.insert( 0x1E34, 0x1E35 );
    _downcase.insert( 0x1E36, 0x1E37 ); _downcase.insert( 0x1E38, 0x1E39 );
    _downcase.insert( 0x1E3A, 0x1E3B ); _downcase.insert( 0x1E3C, 0x1E3D );
    _downcase.insert( 0x1E3E, 0x1E3F ); _downcase.insert( 0x1E40, 0x1E41 );
    _downcase.insert( 0x1E42, 0x1E43 ); _downcase.insert( 0x1E44, 0x1E45 );
    _downcase.insert( 0x1E46, 0x1E47 ); _downcase.insert( 0x1E48, 0x1E49 );
    _downcase.insert( 0x1E4A, 0x1E4B ); _downcase.insert( 0x1E4C, 0x1E4D );
    _downcase.insert( 0x1E4E, 0x1E4F ); _downcase.insert( 0x1E50, 0x1E51 );
    _downcase.insert( 0x1E52, 0x1E53 ); _downcase.insert( 0x1E54, 0x1E55 );
    _downcase.insert( 0x1E56, 0x1E57 ); _downcase.insert( 0x1E58, 0x1E59 );
    _downcase.insert( 0x1E5A, 0x1E5B ); _downcase.insert( 0x1E5C, 0x1E5D );
    _downcase.insert( 0x1E5E, 0x1E5F ); _downcase.insert( 0x1E60, 0x1E61 );
    _downcase.insert( 0x1E62, 0x1E63 ); _downcase.insert( 0x1E64, 0x1E65 );
    _downcase.insert( 0x1E66, 0x1E67 ); _downcase.insert( 0x1E68, 0x1E69 );
    _downcase.insert( 0x1E6A, 0x1E6B ); _downcase.insert( 0x1E6C, 0x1E6D );
    _downcase.insert( 0x1E6E, 0x1E6F ); _downcase.insert( 0x1E70, 0x1E71 );
    _downcase.insert( 0x1E72, 0x1E73 ); _downcase.insert( 0x1E74, 0x1E75 );
    _downcase.insert( 0x1E76, 0x1E77 ); _downcase.insert( 0x1E78, 0x1E79 );
    _downcase.insert( 0x1E7A, 0x1E7B ); _downcase.insert( 0x1E7C, 0x1E7D );
    _downcase.insert( 0x1E7E, 0x1E7F ); _downcase.insert( 0x1E80, 0x1E81 );
    _downcase.insert( 0x1E82, 0x1E83 ); _downcase.insert( 0x1E84, 0x1E85 );
    _downcase.insert( 0x1E86, 0x1E87 ); _downcase.insert( 0x1E88, 0x1E89 );
    _downcase.insert( 0x1E8A, 0x1E8B ); _downcase.insert( 0x1E8C, 0x1E8D );
    _downcase.insert( 0x1E8E, 0x1E8F ); _downcase.insert( 0x1E90, 0x1E91 );
    _downcase.insert( 0x1E92, 0x1E93 ); _downcase.insert( 0x1E94, 0x1E95 );
    _downcase.insert( 0x1EA0, 0x1EA1 ); _downcase.insert( 0x1EA2, 0x1EA3 );
    _downcase.insert( 0x1EA4, 0x1EA5 ); _downcase.insert( 0x1EA6, 0x1EA7 );
    _downcase.insert( 0x1EA8, 0x1EA9 ); _downcase.insert( 0x1EAA, 0x1EAB );
    _downcase.insert( 0x1EAC, 0x1EAD ); _downcase.insert( 0x1EAE, 0x1EAF );
    _downcase.insert( 0x1EB0, 0x1EB1 ); _downcase.insert( 0x1EB2, 0x1EB3 );
    _downcase.insert( 0x1EB4, 0x1EB5 ); _downcase.insert( 0x1EB6, 0x1EB7 );
    _downcase.insert( 0x1EB8, 0x1EB9 ); _downcase.insert( 0x1EBA, 0x1EBB );
    _downcase.insert( 0x1EBC, 0x1EBD ); _downcase.insert( 0x1EBE, 0x1EBF );
    _downcase.insert( 0x1EC0, 0x1EC1 ); _downcase.insert( 0x1EC2, 0x1EC3 );
    _downcase.insert( 0x1EC4, 0x1EC5 ); _downcase.insert( 0x1EC6, 0x1EC7 );
    _downcase.insert( 0x1EC8, 0x1EC9 ); _downcase.insert( 0x1ECA, 0x1ECB );
    _downcase.insert( 0x1ECC, 0x1ECD ); _downcase.insert( 0x1ECE, 0x1ECF );
    _downcase.insert( 0x1ED0, 0x1ED1 ); _downcase.insert( 0x1ED2, 0x1ED3 );
    _downcase.insert( 0x1ED4, 0x1ED5 ); _downcase.insert( 0x1ED6, 0x1ED7 );
    _downcase.insert( 0x1ED8, 0x1ED9 ); _downcase.insert( 0x1EDA, 0x1EDB );
    _downcase.insert( 0x1EDC, 0x1EDD ); _downcase.insert( 0x1EDE, 0x1EDF );
    _downcase.insert( 0x1EE0, 0x1EE1 ); _downcase.insert( 0x1EE2, 0x1EE3 );
    _downcase.insert( 0x1EE4, 0x1EE5 ); _downcase.insert( 0x1EE6, 0x1EE7 );
    _downcase.insert( 0x1EE8, 0x1EE9 ); _downcase.insert( 0x1EEA, 0x1EEB );
    _downcase.insert( 0x1EEC, 0x1EED ); _downcase.insert( 0x1EEE, 0x1EEF );
    _downcase.insert( 0x1EF0, 0x1EF1 ); _downcase.insert( 0x1EF2, 0x1EF3 );
    _downcase.insert( 0x1EF4, 0x1EF5 ); _downcase.insert( 0x1EF6, 0x1EF7 );
    _downcase.insert( 0x1EF8, 0x1EF9 ); _downcase.insert( 0x1F08, 0x1F00 );
    _downcase.insert( 0x1F09, 0x1F01 ); _downcase.insert( 0x1F0A, 0x1F02 );
    _downcase.insert( 0x1F0B, 0x1F03 ); _downcase.insert( 0x1F0C, 0x1F04 );
    _downcase.insert( 0x1F0D, 0x1F05 ); _downcase.insert( 0x1F0E, 0x1F06 );
    _downcase.insert( 0x1F0F, 0x1F07 ); _downcase.insert( 0x1F18, 0x1F10 );
    _downcase.insert( 0x1F19, 0x1F11 ); _downcase.insert( 0x1F1A, 0x1F12 );
    _downcase.insert( 0x1F1B, 0x1F13 ); _downcase.insert( 0x1F1C, 0x1F14 );
    _downcase.insert( 0x1F1D, 0x1F15 ); _downcase.insert( 0x1F28, 0x1F20 );
    _downcase.insert( 0x1F29, 0x1F21 ); _downcase.insert( 0x1F2A, 0x1F22 );
    _downcase.insert( 0x1F2B, 0x1F23 ); _downcase.insert( 0x1F2C, 0x1F24 );
    _downcase.insert( 0x1F2D, 0x1F25 ); _downcase.insert( 0x1F2E, 0x1F26 );
    _downcase.insert( 0x1F2F, 0x1F27 ); _downcase.insert( 0x1F38, 0x1F30 );
    _downcase.insert( 0x1F39, 0x1F31 ); _downcase.insert( 0x1F3A, 0x1F32 );
    _downcase.insert( 0x1F3B, 0x1F33 ); _downcase.insert( 0x1F3C, 0x1F34 );
    _downcase.insert( 0x1F3D, 0x1F35 ); _downcase.insert( 0x1F3E, 0x1F36 );
    _downcase.insert( 0x1F3F, 0x1F37 ); _downcase.insert( 0x1F48, 0x1F40 );
    _downcase.insert( 0x1F49, 0x1F41 ); _downcase.insert( 0x1F4A, 0x1F42 );
    _downcase.insert( 0x1F4B, 0x1F43 ); _downcase.insert( 0x1F4C, 0x1F44 );
    _downcase.insert( 0x1F4D, 0x1F45 ); _downcase.insert( 0x1F59, 0x1F51 );
    _downcase.insert( 0x1F5B, 0x1F53 ); _downcase.insert( 0x1F5D, 0x1F55 );
    _downcase.insert( 0x1F5F, 0x1F57 ); _downcase.insert( 0x1F68, 0x1F60 );
    _downcase.insert( 0x1F69, 0x1F61 ); _downcase.insert( 0x1F6A, 0x1F62 );
    _downcase.insert( 0x1F6B, 0x1F63 ); _downcase.insert( 0x1F6C, 0x1F64 );
    _downcase.insert( 0x1F6D, 0x1F65 ); _downcase.insert( 0x1F6E, 0x1F66 );
    _downcase.insert( 0x1F6F, 0x1F67 ); _downcase.insert( 0x1F88, 0x1F80 );
    _downcase.insert( 0x1F89, 0x1F81 ); _downcase.insert( 0x1F8A, 0x1F82 );
    _downcase.insert( 0x1F8B, 0x1F83 ); _downcase.insert( 0x1F8C, 0x1F84 );
    _downcase.insert( 0x1F8D, 0x1F85 ); _downcase.insert( 0x1F8E, 0x1F86 );
    _downcase.insert( 0x1F8F, 0x1F87 ); _downcase.insert( 0x1F98, 0x1F90 );
    _downcase.insert( 0x1F99, 0x1F91 ); _downcase.insert( 0x1F9A, 0x1F92 );
    _downcase.insert( 0x1F9B, 0x1F93 ); _downcase.insert( 0x1F9C, 0x1F94 );
    _downcase.insert( 0x1F9D, 0x1F95 ); _downcase.insert( 0x1F9E, 0x1F96 );
    _downcase.insert( 0x1F9F, 0x1F97 ); _downcase.insert( 0x1FA8, 0x1FA0 );
    _downcase.insert( 0x1FA9, 0x1FA1 ); _downcase.insert( 0x1FAA, 0x1FA2 );
    _downcase.insert( 0x1FAB, 0x1FA3 ); _downcase.insert( 0x1FAC, 0x1FA4 );
    _downcase.insert( 0x1FAD, 0x1FA5 ); _downcase.insert( 0x1FAE, 0x1FA6 );
    _downcase.insert( 0x1FAF, 0x1FA7 ); _downcase.insert( 0x1FB8, 0x1FB0 );
    _downcase.insert( 0x1FB9, 0x1FB1 ); _downcase.insert( 0x1FBA, 0x1F70 );
    _downcase.insert( 0x1FBB, 0x1F71 ); _downcase.insert( 0x1FBC, 0x1FB3 );
    _downcase.insert( 0x1FC8, 0x1F72 ); _downcase.insert( 0x1FC9, 0x1F73 );
    _downcase.insert( 0x1FCA, 0x1F74 ); _downcase.insert( 0x1FCB, 0x1F75 );
    _downcase.insert( 0x1FCC, 0x1FC3 ); _downcase.insert( 0x1FD8, 0x1FD0 );
    _downcase.insert( 0x1FD9, 0x1FD1 ); _downcase.insert( 0x1FDA, 0x1F76 );
    _downcase.insert( 0x1FDB, 0x1F77 ); _downcase.insert( 0x1FE8, 0x1FE0 );
    _downcase.insert( 0x1FE9, 0x1FE1 ); _downcase.insert( 0x1FEA, 0x1F7A );
    _downcase.insert( 0x1FEB, 0x1F7B ); _downcase.insert( 0x1FEC, 0x1FE5 );
    _downcase.insert( 0x1FF8, 0x1F78 ); _downcase.insert( 0x1FF9, 0x1F79 );
    _downcase.insert( 0x1FFA, 0x1F7C ); _downcase.insert( 0x1FFB, 0x1F7D );
    _downcase.insert( 0x1FFC, 0x1FF3 ); _downcase.insert( 0x2126, 0x03C9 );
    _downcase.insert( 0x212A, 0x006B ); _downcase.insert( 0x212B, 0x00E5 );
    _downcase.insert( 0x2160, 0x2170 ); _downcase.insert( 0x2161, 0x2171 );
    _downcase.insert( 0x2162, 0x2172 ); _downcase.insert( 0x2163, 0x2173 );
    _downcase.insert( 0x2164, 0x2174 ); _downcase.insert( 0x2165, 0x2175 );
    _downcase.insert( 0x2166, 0x2176 ); _downcase.insert( 0x2167, 0x2177 );
    _downcase.insert( 0x2168, 0x2178 ); _downcase.insert( 0x2169, 0x2179 );
    _downcase.insert( 0x216A, 0x217A ); _downcase.insert( 0x216B, 0x217B );
    _downcase.insert( 0x216C, 0x217C ); _downcase.insert( 0x216D, 0x217D );
    _downcase.insert( 0x216E, 0x217E ); _downcase.insert( 0x216F, 0x217F );
    _downcase.insert( 0x24B6, 0x24D0 ); _downcase.insert( 0x24B7, 0x24D1 );
    _downcase.insert( 0x24B8, 0x24D2 ); _downcase.insert( 0x24B9, 0x24D3 );
    _downcase.insert( 0x24BA, 0x24D4 ); _downcase.insert( 0x24BB, 0x24D5 );
    _downcase.insert( 0x24BC, 0x24D6 ); _downcase.insert( 0x24BD, 0x24D7 );
    _downcase.insert( 0x24BE, 0x24D8 ); _downcase.insert( 0x24BF, 0x24D9 );
    _downcase.insert( 0x24C0, 0x24DA ); _downcase.insert( 0x24C1, 0x24DB );
    _downcase.insert( 0x24C2, 0x24DC ); _downcase.insert( 0x24C3, 0x24DD );
    _downcase.insert( 0x24C4, 0x24DE ); _downcase.insert( 0x24C5, 0x24DF );
    _downcase.insert( 0x24C6, 0x24E0 ); _downcase.insert( 0x24C7, 0x24E1 );
    _downcase.insert( 0x24C8, 0x24E2 ); _downcase.insert( 0x24C9, 0x24E3 );
    _downcase.insert( 0x24CA, 0x24E4 ); _downcase.insert( 0x24CB, 0x24E5 );
    _downcase.insert( 0x24CC, 0x24E6 ); _downcase.insert( 0x24CD, 0x24E7 );
    _downcase.insert( 0x24CE, 0x24E8 ); _downcase.insert( 0x24CF, 0x24E9 );
    _downcase.insert( 0x2C00, 0x2C30 ); _downcase.insert( 0x2C01, 0x2C31 );
    _downcase.insert( 0x2C02, 0x2C32 ); _downcase.insert( 0x2C03, 0x2C33 );
    _downcase.insert( 0x2C04, 0x2C34 ); _downcase.insert( 0x2C05, 0x2C35 );
    _downcase.insert( 0x2C06, 0x2C36 ); _downcase.insert( 0x2C07, 0x2C37 );
    _downcase.insert( 0x2C08, 0x2C38 ); _downcase.insert( 0x2C09, 0x2C39 );
    _downcase.insert( 0x2C0A, 0x2C3A ); _downcase.insert( 0x2C0B, 0x2C3B );
    _downcase.insert( 0x2C0C, 0x2C3C ); _downcase.insert( 0x2C0D, 0x2C3D );
    _downcase.insert( 0x2C0E, 0x2C3E ); _downcase.insert( 0x2C0F, 0x2C3F );
    _downcase.insert( 0x2C10, 0x2C40 ); _downcase.insert( 0x2C11, 0x2C41 );
    _downcase.insert( 0x2C12, 0x2C42 ); _downcase.insert( 0x2C13, 0x2C43 );
    _downcase.insert( 0x2C14, 0x2C44 ); _downcase.insert( 0x2C15, 0x2C45 );
    _downcase.insert( 0x2C16, 0x2C46 ); _downcase.insert( 0x2C17, 0x2C47 );
    _downcase.insert( 0x2C18, 0x2C48 ); _downcase.insert( 0x2C19, 0x2C49 );
    _downcase.insert( 0x2C1A, 0x2C4A ); _downcase.insert( 0x2C1B, 0x2C4B );
    _downcase.insert( 0x2C1C, 0x2C4C ); _downcase.insert( 0x2C1D, 0x2C4D );
    _downcase.insert( 0x2C1E, 0x2C4E ); _downcase.insert( 0x2C1F, 0x2C4F );
    _downcase.insert( 0x2C20, 0x2C50 ); _downcase.insert( 0x2C21, 0x2C51 );
    _downcase.insert( 0x2C22, 0x2C52 ); _downcase.insert( 0x2C23, 0x2C53 );
    _downcase.insert( 0x2C24, 0x2C54 ); _downcase.insert( 0x2C25, 0x2C55 );
    _downcase.insert( 0x2C26, 0x2C56 ); _downcase.insert( 0x2C27, 0x2C57 );
    _downcase.insert( 0x2C28, 0x2C58 ); _downcase.insert( 0x2C29, 0x2C59 );
    _downcase.insert( 0x2C2A, 0x2C5A ); _downcase.insert( 0x2C2B, 0x2C5B );
    _downcase.insert( 0x2C2C, 0x2C5C ); _downcase.insert( 0x2C2D, 0x2C5D );
    _downcase.insert( 0x2C2E, 0x2C5E ); _downcase.insert( 0x2C80, 0x2C81 );
    _downcase.insert( 0x2C82, 0x2C83 ); _downcase.insert( 0x2C84, 0x2C85 );
    _downcase.insert( 0x2C86, 0x2C87 ); _downcase.insert( 0x2C88, 0x2C89 );
    _downcase.insert( 0x2C8A, 0x2C8B ); _downcase.insert( 0x2C8C, 0x2C8D );
    _downcase.insert( 0x2C8E, 0x2C8F ); _downcase.insert( 0x2C90, 0x2C91 );
    _downcase.insert( 0x2C92, 0x2C93 ); _downcase.insert( 0x2C94, 0x2C95 );
    _downcase.insert( 0x2C96, 0x2C97 ); _downcase.insert( 0x2C98, 0x2C99 );
    _downcase.insert( 0x2C9A, 0x2C9B ); _downcase.insert( 0x2C9C, 0x2C9D );
    _downcase.insert( 0x2C9E, 0x2C9F ); _downcase.insert( 0x2CA0, 0x2CA1 );
    _downcase.insert( 0x2CA2, 0x2CA3 ); _downcase.insert( 0x2CA4, 0x2CA5 );
    _downcase.insert( 0x2CA6, 0x2CA7 ); _downcase.insert( 0x2CA8, 0x2CA9 );
    _downcase.insert( 0x2CAA, 0x2CAB ); _downcase.insert( 0x2CAC, 0x2CAD );
    _downcase.insert( 0x2CAE, 0x2CAF ); _downcase.insert( 0x2CB0, 0x2CB1 );
    _downcase.insert( 0x2CB2, 0x2CB3 ); _downcase.insert( 0x2CB4, 0x2CB5 );
    _downcase.insert( 0x2CB6, 0x2CB7 ); _downcase.insert( 0x2CB8, 0x2CB9 );
    _downcase.insert( 0x2CBA, 0x2CBB ); _downcase.insert( 0x2CBC, 0x2CBD );
    _downcase.insert( 0x2CBE, 0x2CBF ); _downcase.insert( 0x2CC0, 0x2CC1 );
    _downcase.insert( 0x2CC2, 0x2CC3 ); _downcase.insert( 0x2CC4, 0x2CC5 );
    _downcase.insert( 0x2CC6, 0x2CC7 ); _downcase.insert( 0x2CC8, 0x2CC9 );
    _downcase.insert( 0x2CCA, 0x2CCB ); _downcase.insert( 0x2CCC, 0x2CCD );
    _downcase.insert( 0x2CCE, 0x2CCF ); _downcase.insert( 0x2CD0, 0x2CD1 );
    _downcase.insert( 0x2CD2, 0x2CD3 ); _downcase.insert( 0x2CD4, 0x2CD5 );
    _downcase.insert( 0x2CD6, 0x2CD7 ); _downcase.insert( 0x2CD8, 0x2CD9 );
    _downcase.insert( 0x2CDA, 0x2CDB ); _downcase.insert( 0x2CDC, 0x2CDD );
    _downcase.insert( 0x2CDE, 0x2CDF ); _downcase.insert( 0x2CE0, 0x2CE1 );
    _downcase.insert( 0x2CE2, 0x2CE3 ); _downcase.insert( 0xFF21, 0xFF41 );
    _downcase.insert( 0xFF22, 0xFF42 ); _downcase.insert( 0xFF23, 0xFF43 );
    _downcase.insert( 0xFF24, 0xFF44 ); _downcase.insert( 0xFF25, 0xFF45 );
    _downcase.insert( 0xFF26, 0xFF46 ); _downcase.insert( 0xFF27, 0xFF47 );
    _downcase.insert( 0xFF28, 0xFF48 ); _downcase.insert( 0xFF29, 0xFF49 );
    _downcase.insert( 0xFF2A, 0xFF4A ); _downcase.insert( 0xFF2B, 0xFF4B );
    _downcase.insert( 0xFF2C, 0xFF4C ); _downcase.insert( 0xFF2D, 0xFF4D );
    _downcase.insert( 0xFF2E, 0xFF4E ); _downcase.insert( 0xFF2F, 0xFF4F );
    _downcase.insert( 0xFF30, 0xFF50 ); _downcase.insert( 0xFF31, 0xFF51 );
    _downcase.insert( 0xFF32, 0xFF52 ); _downcase.insert( 0xFF33, 0xFF53 );
    _downcase.insert( 0xFF34, 0xFF54 ); _downcase.insert( 0xFF35, 0xFF55 );
    _downcase.insert( 0xFF36, 0xFF56 ); _downcase.insert( 0xFF37, 0xFF57 );
    _downcase.insert( 0xFF38, 0xFF58 ); _downcase.insert( 0xFF39, 0xFF59 );
    _downcase.insert( 0xFF3A, 0xFF5A ); _downcase.insert( 0x10400, 0x10428 );
    _downcase.insert( 0x10401, 0x10429 ); _downcase.insert( 0x10402, 0x1042A );
    _downcase.insert( 0x10403, 0x1042B ); _downcase.insert( 0x10404, 0x1042C );
    _downcase.insert( 0x10405, 0x1042D ); _downcase.insert( 0x10406, 0x1042E );
    _downcase.insert( 0x10407, 0x1042F ); _downcase.insert( 0x10408, 0x10430 );
    _downcase.insert( 0x10409, 0x10431 ); _downcase.insert( 0x1040A, 0x10432 );
    _downcase.insert( 0x1040B, 0x10433 ); _downcase.insert( 0x1040C, 0x10434 );
    _downcase.insert( 0x1040D, 0x10435 ); _downcase.insert( 0x1040E, 0x10436 );
    _downcase.insert( 0x1040F, 0x10437 ); _downcase.insert( 0x10410, 0x10438 );
    _downcase.insert( 0x10411, 0x10439 ); _downcase.insert( 0x10412, 0x1043A );
    _downcase.insert( 0x10413, 0x1043B ); _downcase.insert( 0x10414, 0x1043C );
    _downcase.insert( 0x10415, 0x1043D ); _downcase.insert( 0x10416, 0x1043E );
    _downcase.insert( 0x10417, 0x1043F ); _downcase.insert( 0x10418, 0x10440 );
    _downcase.insert( 0x10419, 0x10441 ); _downcase.insert( 0x1041A, 0x10442 );
    _downcase.insert( 0x1041B, 0x10443 ); _downcase.insert( 0x1041C, 0x10444 );
    _downcase.insert( 0x1041D, 0x10445 ); _downcase.insert( 0x1041E, 0x10446 );
    _downcase.insert( 0x1041F, 0x10447 ); _downcase.insert( 0x10420, 0x10448 );
    _downcase.insert( 0x10421, 0x10449 ); _downcase.insert( 0x10422, 0x1044A );
    _downcase.insert( 0x10423, 0x1044B ); _downcase.insert( 0x10424, 0x1044C );
    _downcase.insert( 0x10425, 0x1044D ); _downcase.insert( 0x10426, 0x1044E );
    _downcase.insert( 0x10427, 0x1044F );
  }
}
