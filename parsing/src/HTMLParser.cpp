/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// HTMLParser
//
// March 2004 -- metzler
//

#include "indri/HTMLParser.hpp"
#include <ctype.h>
#include "lemur-compat.hpp"

void indri::parse::HTMLParser::initialize( UnparsedDocument* unparsed, indri::api::ParsedDocument* parsed ) {
  indri::parse::TaggedTextParser::initialize( unparsed, parsed );

  // clear URL
  url[0] = 0;
  base_url[0] = 0;

  // find the DOCHDR tag, so we can yank out the URL
  for( unsigned int i=0; i<unparsed->metadata.size(); i++ ) {
    if( !strcmp(unparsed->metadata[i].key, "dochdr") ) {
      char* beginURL = (char*) unparsed->metadata[i].value;
      char* endURL = beginURL + strcspn( (char*) unparsed->metadata[i].value, " \t\r\n" );
      int length = lemur_compat::min<int>( endURL-beginURL, sizeof url-1 );
      memcpy( url, beginURL, length );
      url[length] = 0;

      strncpy( base_url, url, sizeof url-1 );
      char* lastSlash = strrchr( base_url, '/' );
      if( lastSlash ) *lastSlash = 0;
      break;
    }
  }

  // set url
  normalizeURL(url);

  // set base_url
  normalizeURL(base_url);

  // get tag definitions
  _absoluteUrlTag = _findTag("absolute-url");
  _relativeUrlTag = _findTag("relative-url");
  _anchorTag = _findTag("a");

  // add URL to metadata
  indri::parse::MetadataPair pair;
  pair.key = "url";
  pair.value = url;
  pair.valueLength = strlen(url)+1;
  parsed->metadata.push_back( pair );
}

void indri::parse::HTMLParser::cleanup( indri::parse::UnparsedDocument* unparsed, indri::api::ParsedDocument* parsed ) {
  indri::parse::TaggedTextParser::cleanup( unparsed, parsed );
}

void indri::parse::HTMLParser::handleTag(char* token, long pos) {
  // <A HREF ...> tag
  int length = strlen(token);

  if(length > 2 && (token[1] == 'A' || token[1] == 'a') && token[2] == ' ') {
    int i;
    for(i = 2; i < length && token[i] == ' '; i++);
    if(i + 3 < length &&
       (token[i] == 'H' || token[i] == 'h') &&
       (token[i+1] == 'R' || token[i+1] == 'r') &&
       (token[i+2] == 'E' || token[i+2] == 'e') &&
       (token[i+3] == 'F' || token[i+3] == 'f')) {
      if(!_anchorTag && !_relativeUrlTag && !_absoluteUrlTag)
        return;

      if(!extractURL(token))
        return;
        
      char tmp_buf[MAX_URL_LENGTH];
      strncpy(tmp_buf, token, lemur_compat::min<int>(length,MAX_URL_LENGTH-1));
      tmp_buf[MAX_URL_LENGTH-1] = 0;

      bool relative = normalizeURL(tmp_buf);

      // if special url tags are requested, we'll
      // store the url of the anchor text in the document itself
        
      const TaggedTextParser::tag_properties* tagProps;
      if( !relative ) {
        tagProps = _absoluteUrlTag;
      } else {
        tagProps = _relativeUrlTag;
      }

      if( tagProps && !tagProps->exclude && !_exclude ) {
        writeToken(tmp_buf);
        addTag(tagProps->name, tagProps->conflation, pos);
        endTag(tagProps->name, tagProps->conflation, pos+1);
      }

      tagProps = _anchorTag;
      if( tagProps && !tagProps->exclude && !_exclude )
        addTag(tagProps->name, tagProps->conflation, pos+1);
    }
    else
      indri::parse::TaggedTextParser::handleTag(token, pos);
  }
  // <BASE HREF ...> tag
  else if (length > 5 &&
           (token[1] == 'B' || token[1] == 'b') &&
           (token[2] == 'A' || token[2] == 'a') &&
           (token[3] == 'S' || token[3] == 's') &&
           (token[4] == 'E' || token[4] == 'e') &&
           token[5] == ' ') {
    int i;
    for(i = 5; i < length && token[i] == ' '; i++);
    if(i + 3 < length &&
       (token[i] == 'H' || token[i] == 'h') &&
       (token[i+1] == 'R' || token[i+1] == 'r') &&
       (token[i+2] == 'E' || token[i+2] == 'e') &&
       (token[i+3] == 'F' || token[i+3] == 'f')) {
      char tmp_buf[MAX_URL_LENGTH];
      strncpy( tmp_buf, token, lemur_compat::min<int>(length,MAX_URL_LENGTH-1) );
      if(!extractURL(tmp_buf))
        return;
      normalizeURL(tmp_buf);
      strncpy(base_url, tmp_buf, lemur_compat::min<int>(length,MAX_URL_LENGTH-1));
      base_url[MAX_URL_LENGTH-1] = 0;
    }
    else
      TaggedTextParser::handleTag(token, pos);
  }
  // any other tag
  else {
    TaggedTextParser::handleTag(token, pos);
  }
}

// extracts a URL (in place) from a tag of the form
// <\w+[ ]*=[ ]*"{0,1\}URL"{0,1}.*>
// returns true on success, false otherwise
bool indri::parse::HTMLParser::extractURL(char *token) {
  int i,j;
  for(i = 0; token[i] != '='; i++) {
    if(token[i] == '\0') {
      return false;
    }
  }
  for(i++; token[i] == ' ' || token[i] == '\"'; i++) {
    if(token[i] == '\0') {
      return false;
    }
  }
  for(j = 0; token[i] && token[i] != ' ' && token[i] != '\n' && token[i] != '\"' && token[i] != '>'; i++)
    token[j++] = token[i];

  token[j] = '\0';
  return true;
}

// normalizes a URL (in place)
// largely based on information contained in RFC 1808
// Note: returns true if the URL was a relative one, false if it was absolute
//       the return value is not an error code here; the function should always succeed
bool indri::parse::HTMLParser:: normalizeURL(char *s) {
  char *normurl = s;

  // remove the fragment identifier, query information and parameter information
  char *c;
  for(c = s; *c != '\0' && *c != '#' && *c != '?' && *c != ';'; c++);
  *c = '\0';

  // extract scheme, if given
  bool found_scheme = false;
  char scheme[MAX_URL_LENGTH];
  char netloc[MAX_URL_LENGTH];
  char path[MAX_URL_LENGTH];
  int path_len = 0;
  int scheme_len = 0;
  int netloc_len = 0;

  for(c = s; *c != '\0'; c++) {
    // scheme must have length > 0 and end with a ':'
    if(scheme_len > 0 && *c == ':') {
      found_scheme = true;
      strncpy(scheme, s, scheme_len);
      scheme[scheme_len] = '\0';
      // convert scheme to lowercase
      for(int i = 0; i < scheme_len; i++) {
        if( scheme[i] >='A' && scheme[i] <='Z' )
          scheme[i] = scheme[i] + ('a' - 'A');
      }
      c++;

      // extract network location
      if(*c == '/' && *(c+1) == '/') c+=2; // skip "//"
      char *netloc_begin = c;
      for(; *c != '\0' && *c != '/'; c++)
        netloc_len++;
      strncpy(netloc, netloc_begin, netloc_len);
      netloc[netloc_len] = '\0';
      // convert netloc to lowercase
      int colon_loc = -1;
      for(int i = 0; i < netloc_len; i++) {
        if(netloc[i] == ':')
          colon_loc = i;
        else if(colon_loc > -1 && !isdigit((unsigned char) netloc[i]))
          colon_loc = -1;

        if( netloc[i] > 'A' && netloc[i] < 'Z' )
          netloc[i] = netloc[i] + ('a' - 'A');
      }
      if(colon_loc > -1)
        netloc[colon_loc] = '\0';

      // extract the path
      for(; *c == '/'; c++); // skip leading slashes
      path_len = s + strlen(s) - c;
      if(path_len > 0) {
        strncpy(path, c, path_len);
        path[path_len] = '\0';
      }
      break;
    }

    char ch = *c;

    // only alpha + num + '+' + '-' + '.' are allowed to appear in scheme
    if( ((ch < 'A' || ch > 'Z') && (ch < 'a' || ch > 'z')) && // not alpha
        (ch < '0' || ch > '9') && // not digit
        ch != '+' &&
        ch != '-' && 
        ch != '.' )
      break;

    scheme_len++;
  }

  // absolute url
  int dotCleanStart = -1;

  if(found_scheme) {
    if(strcmp(scheme, "http") == 0) {
      if(path_len == 0) {
        memcpy( normurl, scheme, scheme_len );
        memcpy( normurl + scheme_len, "://", 3 );
        memcpy( normurl + scheme_len + 3, netloc, netloc_len+1 );
      } else {
        strcpy( normurl, scheme );
        strcat( normurl, "://" );
        strcat( normurl, netloc );
        strcat( normurl, "/" );
        strcat( normurl, path );
        dotCleanStart = scheme_len + 3 + netloc_len + 1;
      }
    }
    else {
      sprintf(normurl, "%s:%s", scheme, netloc);
    }
  }
  // relative url
  else {
    char tmp_buf[MAX_URL_LENGTH];
    strncpy(tmp_buf, s, MAX_URL_LENGTH-1);
    tmp_buf[MAX_URL_LENGTH-1] = 0;
    if(*s == '/') {
      normurl[0] = 0;
      strcat( normurl, base_url );
      strcat( normurl, tmp_buf );
    } else {
      normurl[0] = 0;
      strcat( normurl, base_url );
      strcat( normurl, "/" );
      strcat( normurl, tmp_buf );
    }
    
    char* colonSlashSlash = 0;
    char* slash = 0;

    colonSlashSlash = strstr( normurl, "://" );
    if( colonSlashSlash )
      slash = strstr( colonSlashSlash + 3, "/" );
    if( slash )
      dotCleanStart = slash - normurl;
  }

  indri::utility::greedy_vector<int,32> slashes;
  indri::utility::greedy_vector<bool,32> usable;
  int normLength = strlen(normurl);

  // get rid of ".." directories
  if( dotCleanStart >= 0 ) {
    bool unusableFound = false;

    // find slashes
    // if the space between two slashes is "../", that's not useful to us,
    // so mark that fact
    for( int i=dotCleanStart; i<normLength; i++ ) {
      if( normurl[i] == '/' ) {
        if( slashes.size() ) {
          int lastSlash = slashes[slashes.size()-1];
          if( !strncmp( "../", normurl + lastSlash + 1, 3 ) ) {
            usable.push_back(false);
            unusableFound = true;
          } else {
            usable.push_back(true);
          }
        }
        
        slashes.push_back(i);
      }
    }

    if( normLength && normurl[normLength-1] != '/' ) {
      if( strncmp( "..", normurl + normLength - 2, 2 ) == 0 ) {
        unusableFound = true;
        usable.push_back(false);
      } else {
        usable.push_back(true);
      }
    }

    // some ".." dir was found, so we're going to clean it out
    if( unusableFound ) {
      for( int i=0; i<(int)usable.size(); i++ ) {
        if( !usable[i] ) {
          // search back to find something to mark false
          // to account for this '..'
          for( int j=i; j>=0; j-- ) {
            if( usable[j] ) {
              usable[j] = false;
              break;
            }
          }
        }
      }

      // now that we've marked the usable spots, we just have to copy
      // the url parts we want

      char tmp_buf[MAX_URL_LENGTH];
      tmp_buf[0] = 0;
      strncpy(tmp_buf, normurl, dotCleanStart);
      tmp_buf[dotCleanStart] = 0;

      for( int i=0; i<(int)usable.size(); i++ ) {
        if( usable[i] ) {
          if( slashes.size() == i+1 )
            strcat( tmp_buf, normurl + slashes[i] );
          else
            strncat( tmp_buf, normurl + slashes[i], slashes[i+1] - slashes[i] );
        }
      }

      strncpy(s, tmp_buf, MAX_URL_LENGTH-1);
      s[MAX_URL_LENGTH-1] = 0;
    }
  }

  return !found_scheme;
}

