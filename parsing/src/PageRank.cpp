/*==========================================================================
 * Copyright (c) 2002-2008 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
//
// PageRank
//
// 17 July 2005 -- dam
// Jan 2008 -- dmf
//

#include "indri/PageRank.hpp"
#include <algorithm>
const double
indri::parse::PageRank::_intToProb[11] = {log(2.21916E-10), 
                                          log(2.21916E-10),
                                          log(5.61952E-10),
                                          log(1.94786E-09),
                                          log(3.62742E-09), 
                                          log(1.00745E-08),
                                          log(1.97616E-08),
                                          log(3.46944E-08),
                                          log(4.39146E-08),
                                          log(1.74226E-07),
                                          log(2.28983E-07) };

// this is somewhat slow, but it gets the job done
// of course, it'd be faster if we had an index built...
void indri::parse::PageRank::_computeColLen() {
  _colLen = 0;
  indri::file::FileTreeIterator files( _corpusPath );
  for( ; files != indri::file::FileTreeIterator::end(); files++ ) {
    std::string filePath = *files;
    indri::parse::TaggedDocumentIterator iterator;
    iterator.open( filePath );
    iterator.setTags( "<DOC>",             // startDocTag
                      "</DOC>\n",          // endDocTag
                      "</DOCHDR>" );       // endMetadataTag
    indri::parse::UnparsedDocument* unparsed;
    while( (unparsed = iterator.nextDocument()) != 0 )
      _colLen++;
    // close up everything
    iterator.close();
  }
}

float indri::parse::PageRank::_readPageRankFromFile( std::ifstream& src, const std::string& sourceDoc ) {
  int lengthBuf;
  char docBuf[4096];
  float prBuf;

  if( !src.good() )
    return 1.0 / (double)_colLen;

  while( !src.eof() ) {
    src.read( (char *)&lengthBuf, sizeof( int ) ); // get length of document name
    src.read( docBuf, lengthBuf ); // get document name
    src.read( (char *)&prBuf, sizeof( float ) ); // read page rank

    std::string docName = docBuf;

    if( docName > sourceDoc ) {
      int relPos = -( sizeof( int ) + lengthBuf + sizeof( float ) );
      src.seekg( relPos, std::ios::cur );
      return ( 1.0 - _c ) / (double)_colLen;
    }

    if( docName == sourceDoc )
      return prBuf;
  }

  return ( 1.0 - _c ) / (double)_colLen;
}

void indri::parse::PageRank::_writePageRankToFile( std::ofstream& src, const std::string& destDoc, const float pr ) {
  int lengthBuf;

  lengthBuf = destDoc.length();
  src.write( (char *)&lengthBuf, sizeof( int ) ); // document name length
  src.write( destDoc.c_str(), lengthBuf ); // document name
  src.write( (char *)&pr, sizeof( float ) ); // page rank
}

void indri::parse::PageRank::_updatePageRank( std::ifstream& src, std::ofstream& dest, Links& links ) {
  src.seekg( 0, std::ios::beg );

  PageRankVector destPR;

  // update PageRank for each destination in links
  Links::iterator iter;
  for( iter = links.begin(); iter != links.end(); ++iter ) {
    std::string sourceDoc = iter->first;
    int outDegree = iter->second.first;
    std::vector< std::string > destDocs = iter->second.second;

    float srcPR = _readPageRankFromFile( src, sourceDoc );

    std::vector< std::string >::iterator destIter;
    for( destIter = destDocs.begin(); destIter != destDocs.end(); ++destIter )
      destPR[ *destIter ] += srcPR / double( outDegree );
  }

  // dampen PageRanks
  PageRankVector::iterator PRiter;
  for( PRiter = destPR.begin(); PRiter != destPR.end(); ++PRiter ) {
    PRiter->second = _c * PRiter->second + ( 1.0 - _c ) / (double)_colLen;
    _writePageRankToFile( dest, PRiter->first, PRiter->second );
  }
}

void indri::parse::PageRank::_computeOutDegrees( Links& links ) {
  char docno[512];
  char docUrl[4096];
  char linkCountText[256];
  char text[65536];
  char linkDocno[512];

  indri::file::FileTreeIterator input( _linkPath );
  Links::iterator iter;

  for( ; input != indri::file::FileTreeIterator::end(); input++ ) {
    std::string path = *input;
    std::ifstream in;

    in.open( path.c_str() );

    // fill in the links structure for the documents currently under consideration
    while( !in.eof() ) {
      in.getline( docno, sizeof docno );
      in.getline( docUrl, sizeof docUrl );
      in.getline( linkCountText, sizeof linkCountText );

      if( strncmp( docno, "DOC", 3 ) != 0 )
        break;

      int linkCount = atoi( linkCountText + sizeof "LINKS=" - 1 );

      for( int i=0; i<linkCount; i++ ) {
        in.getline( linkDocno, sizeof linkDocno );
        in.getline( text, sizeof text ); // ignore LINKFROM
        in.getline( text, sizeof text ); // ignore TEXT

        // see if this is one of our sources
        iter = links.find( linkDocno + 10 );

        // if so, update outdegree count
        if( iter != links.end() ) {
          iter->second.first++;
        }
      }
    }

    in.close();
  }
}

void indri::parse::PageRank::_doPageRankIter( const int docsPerIter, const std::string& srcFile, const std::string& destFile ) {
  char docno[512];
  char docUrl[4096];
  char linkCountText[256];
  char linkDocno[512];
  char text[65536];

  int docsAdded = 0;
  Links links;

  std::ifstream src( srcFile.c_str(), std::ios::in | std::ios::binary );
  std::ofstream dest( destFile.c_str(), std::ios::out | std::ios::binary );

  // we need to iterate over the corpus path directories to ensure that we
  // keep things in the correct sorted order
  indri::file::FileTreeIterator input( _corpusPath );

  for( ; input != indri::file::FileTreeIterator::end(); input++ ) {
    std::string filePath = *input;
    std::ifstream in;

    std::string relative = indri::file::Path::relative( _corpusPath, filePath );
    std::string linkFile = indri::file::Path::combine( _linkPath, relative );

    in.open( linkFile.c_str() );

    // grab the doc ids of the source documents we're interested in
    while( !in.eof() ) {
      in.getline( docno, sizeof docno );
      in.getline( docUrl, sizeof docUrl );
      in.getline( linkCountText, sizeof linkCountText );

      if( strncmp( docno, "DOC", 3 ) != 0 )
        break;

      docsAdded++;

      int linkCount = atoi( linkCountText + sizeof "LINKS=" - 1 );

      // skip this stuff
      for( int i = 0; i < linkCount; i++ ) {
        in.getline( linkDocno, sizeof linkDocno );
        in.getline( text, sizeof text ); // ignore LINKFROM
        in.getline( text, sizeof text ); // ignore TEXT

        links[ linkDocno + 10 ].first = 0;
        links[ linkDocno + 10 ].second.push_back( docno + 6 );
      }

      if( docsAdded == docsPerIter ) {
        _computeOutDegrees( links );
        _updatePageRank( src, dest, links );
        links.clear();
        docsAdded = 0;
      }
    }

    in.close();
  }

  // do the remainder of the documents
  if( docsAdded > 0 ) {
    _computeOutDegrees( links );
    _updatePageRank( src, dest, links );
  }

  src.close();
  dest.close();
}

void indri::parse::PageRank::_raw2int(std::vector<pagerank> &ranks)
{
  // Metzler's rawToInt.pl
  // sort the raw scores
  std::sort(ranks.begin(), ranks.end(), pagerank::pagerank_greater());
  int numDocs = ranks.size();
  // max page rank, could be a parameter.
  // if changed, the _intToProb table will need to change.
  int max_pr = 10;
  int rank = max_pr;
  double B = pow((numDocs + 1.0),(1.0/max_pr));
  int baseNum = int(B - 1);
  int num = baseNum;
  for (int i = 0; i < ranks.size(); i++) {
    if (num > 0) {
      ranks[i].int_val = rank;
      num--;
    }
    if (num == 0) {
      baseNum = int(ceil(B*baseNum));
      num = baseNum;
      if (rank > 1) rank--; // don't allow rank 0
    }
  }
}

void indri::parse::PageRank::_loadRanks( const std::string& dest,
                                         std::vector<pagerank> &pageranks) {
  std::ifstream in( dest.c_str(), std::ios::in | std::ios::binary );

  int lengthBuf;
  char docBuf[4096];
  float prBuf;
  // read in the values
  while( !in.eof() ) {
    in.read( (char *)&lengthBuf, sizeof( int ) ); // get length of document name
    in.read( docBuf, lengthBuf ); // get document name
    in.read( (char *)&prBuf, sizeof( float ) ); // read page rank
    docBuf[lengthBuf] = '\0';

    // insert into a vector
    indri::parse::pagerank p;
    p.doc = docBuf;
    p.val = prBuf;
    pageranks.push_back(p);
  }
  in.close();
}

void indri::parse::PageRank::writeRaw( const std::string& dest, const std::string &rawFile ) {
  std::ofstream out(rawFile.c_str(), std::ios::out);
  out.setf( std::ios_base::fixed );
  out.precision( 32 );

  std::vector<pagerank> pageranks;
  _loadRanks(dest, pageranks);
  for (int i = 0; i < pageranks.size(); i++) {
    out << pageranks[i].doc << " " << pageranks[i].val<< std::endl;
  }
}

// merge these two
void indri::parse::PageRank::writeRanks( const std::string& dest, const std::string &ranksFile ) {
  std::ofstream out(ranksFile.c_str(), std::ios::out);
  out.setf( std::ios_base::fixed );
  out.precision( 13 );

  std::vector<pagerank> pageranks;
  _loadRanks(dest, pageranks);
  // bin the values to integer page ranks
  _raw2int(pageranks);
  for (int i = 0; i < pageranks.size(); i++) {
    out << pageranks[i].doc << " " << pageranks[i].int_val<< std::endl;
  }
  out.close();
}

void indri::parse::PageRank::writePriors( const std::string& dest, const std::string &priorFile ) {
  std::ofstream out(priorFile.c_str(), std::ios::out);
  out.setf( std::ios_base::fixed );
  out.precision( 13 );

  std::vector<pagerank> pageranks;
  _loadRanks(dest, pageranks);
  // sort the ranks
  std::sort(pageranks.begin(), pageranks.end(), pagerank::pagerank_greater());
  // bin the values to integer page ranks
  _raw2int(pageranks);
  // assign log probabilities to the bins.
  for (int i = 0; i < pageranks.size(); i++) {
    out << pageranks[i].doc << " " << _intToProb[pageranks[i].int_val]<< std::endl;
  }
  out.close();
}

void indri::parse::PageRank::computePageRank( const std::string& outputFile, const int maxIters, const int docsPerIter, const double c ) {
  static const std::string _tmpFile = "PageRank.tmp";
  _c = c;

  std::string src = _tmpFile;
  std::string dest = outputFile;

  // ensure outputFile is always the final destination
  if( maxIters % 2 == 1 )
    _swap( src, dest );

  for( int iter = 0; iter < maxIters; iter++ ) {
    _swap( src, dest ); // alternate between source and destination files
    _doPageRankIter( docsPerIter, src, dest );
  }

  // delete the temp file
  ::remove( _tmpFile.c_str() );
}
