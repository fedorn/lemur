/*==========================================================================
 * Copyright (c) 2006 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

//
// CreateCosineValues
//
// 28 September 2006 -- mhoy
//

#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/QueryEnvironment.hpp"
#include <iostream>
#include <fstream>
#include <math.h>

void usage() {
  std::cout << "CreateCosineValules <repository> [<body_output_filename> [ <anchor_output_filename> [ <url_output_filename> ] ] ]\n\n";
  std::cout << "A simple utility to output cosine correlation values for documents in\nIndri indexes.\n";
  std::cout << "Parameters:\n";
  std::cout << "  <repository>: the Indri repository\n";
  std::cout << "  <body_output_filename>: (optional) the filename to write the cosine values\n";
  std::cout << "                          for the main body text of the documents to.\n";
  std::cout << "                          If this is not specified, then the main body\n";
  std::cout << "                          cosine values will be written to STDOUT\n";
  std::cout << "  <anchor_output_filename>: (optional) the filename to write the\n";
  std::cout << "                            cosine values for the anchor text tags\n";
  std::cout << "                            (inlinks) of the documents to\n";
  std::cout << "  <url_output_filename>: (optional) the filename to write the cosine\n";
  std::cout << "                         values for the URLs (of trecweb) documents to\n";
  std::cout << "\n";
  std::cout << "The output files are all in the format of:\n";
  std::cout << "doc_id\tcosine_value\n";
  std::cout << "\n";
}

#define REQUIRE_ARGS(n) { if( argc < n ) { usage(); return -1; } }
#define MAX_ARGS(n) { if( argc > n ) { usage(); return -1; } }

int main( int argc, char** argv ) {
  try {
		// there must be at least two arguments
    REQUIRE_ARGS(2);
    // but no more than five!
    MAX_ARGS(5);

		// open the repository
    indri::collection::Repository _repository;
    _repository.openRead(argv[1]);

    std::string bodyOutFilename="";
    std::string anchorOutFilename="";
    std::string urlOutFilename="";

    ofstream bodyOutfile;
    ofstream anchorOutfile;
    ofstream urlOutfile;

	// set up our output streams...
	if (argc > 2) {
      bodyOutFilename=argv[2];
      bodyOutfile.open(bodyOutFilename.c_str());
    }
    if (argc > 3) {
      anchorOutFilename=argv[3];
      anchorOutfile.open(anchorOutFilename.c_str());
    }
    if (argc > 4) {
      urlOutFilename=argv[4];
      urlOutfile.open(urlOutFilename.c_str());
    }

    indri::server::LocalQueryServer localServer(_repository);
    UINT64 docCount=localServer.documentCount();

    std::map<int, int> bodyTFCount;
    std::map<int, int> anchorTFCount;
    std::map<int, int> urlTFCount;

    for (UINT64 docID=1; docID <= docCount; docID++) {
      // for each document in the repository...
      std::vector<lemur::api::DOCID_T> documentIDs;
      documentIDs.clear();
      documentIDs.push_back(docID);

			// get the document vectors...
      indri::server::QueryServerVectorsResponse* response = localServer.documentVectors( documentIDs );

      int bodyLength=0;
      int anchorLength=0;
      int urlLength=0;

      bodyTFCount.clear();
      anchorTFCount.clear();
      urlTFCount.clear();

      int bodyFieldStart=0;
      int bodyFieldEnd=0;
      int anchorFieldStart=999999999;
      int anchorFieldEnd=0;
      int urlFieldStart=0;
      int urlFieldEnd=0;

      // get the field start / end values
      if( response->getResults().size() ) {
        indri::api::DocumentVector* docVector = response->getResults()[0];

        int i;

        for( i=0; i<docVector->fields().size(); i++ ) {
          const indri::api::DocumentVector::Field& field = docVector->fields()[i];
          if (field.name=="mainbody") {
            bodyFieldStart=field.begin;
            bodyFieldEnd=field.end;
          } else if (field.name=="inlink") {
            // may have more than one!
            if (field.begin < anchorFieldStart) { anchorFieldStart=field.begin; }
            if (field.end > anchorFieldEnd) { anchorFieldEnd=field.end; }
          } else if (field.name=="url") {
            urlFieldStart=field.begin;
            urlFieldEnd=field.end;
          }
        } // end for( i=0; i<docVector->fields().size(); i++ )

        if ((urlFieldStart > 0) && (bodyFieldEnd > urlFieldStart)) { bodyFieldEnd=urlFieldStart; }
        if (anchorFieldStart==999999999) { anchorFieldStart=0; }

        bodyLength=bodyFieldEnd-bodyFieldStart;
        anchorLength=anchorFieldEnd-anchorFieldStart;
        urlLength=urlFieldEnd-urlFieldStart;

        // get the body items...
        for (i=bodyFieldStart; i < bodyFieldEnd; i++) {
          int position = docVector->positions()[i];
          if (position > 0) {
            std::map<int, int>::iterator di=bodyTFCount.find(position);
            if (di!=bodyTFCount.end()) {
              int thisCount=di->second;
              bodyTFCount.insert(make_pair(position, thisCount+1));
            } else {
              bodyTFCount.insert(make_pair(position, 1));
            }
          } // end if (position > 0)
        } // end for (i=bodyFieldStart; i < bodyFieldEnd; i++)

        // get the inlink items...
        for (i=anchorFieldStart; i < anchorFieldEnd; i++) {
          int position = docVector->positions()[i];
          if (position > 0) {
            std::map<int, int>::iterator di=anchorTFCount.find(position);
            if (di!=anchorTFCount.end()) {
              int thisCount=di->second;
              anchorTFCount.insert(make_pair(position, thisCount+1));
            } else {
              anchorTFCount.insert(make_pair(position, 1));
            }
          } // end if (position > 0)
        } // end for (i=anchorFieldStart; i < anchorFieldEnd; i++)

        // get the url items...
        for (i=urlFieldStart; i < urlFieldEnd; i++) {
          int position = docVector->positions()[i];
          if (position > 0) {
            std::map<int, int>::iterator di=urlTFCount.find(position);
            if (di!=urlTFCount.end()) {
              int thisCount=di->second;
              urlTFCount.insert(make_pair(position, thisCount+1));
            } else {
              urlTFCount.insert(make_pair(position, 1));
            }
          } // end if (position > 0)
        } // end for (i=urlFieldStart; i < urlFieldEnd; i++)

        // ok - we now have our counts - we can calculate...
        // cos sim. = sqrt(sum(Tf/doclen));
        // first, for the body text
        double bodyCalc=0.0;
        if (bodyLength > 0) {
          std::map<int, int>::iterator di=bodyTFCount.begin();
          while (di!=bodyTFCount.end()) {
            double logScore=log((double)di->second)+1.0;
            bodyCalc += (logScore*logScore);
            di++;
          }
          if (bodyCalc > 0.0) {
            bodyCalc=sqrt(bodyCalc);
          }
        } // end if (bodyLength > 0)

        // now, for anchor tags
        double anchorCalc=0.0;
        if (anchorLength > 0) {
          std::map<int, int>::iterator di=anchorTFCount.begin();
          while (di!=anchorTFCount.end()) {
            double logScore=log((double)di->second)+1.0;
            anchorCalc += (logScore*logScore);
            di++;
          }
          if (anchorCalc > 0.0) {
            anchorCalc=sqrt(anchorCalc);
          }
        } // end if (anchorLength > 0)

        // finally, for the URL
        double urlCalc=0.0;
        if (urlLength > 0) {
          std::map<int, int>::iterator di=urlTFCount.begin();
          while (di!=urlTFCount.end()) {
            double logScore=log((double)di->second)+1.0;
            urlCalc += (logScore*logScore);
            di++;
          }
          if (urlCalc > 0.0) {
            urlCalc=sqrt(urlCalc);
          }
        } // end if (urlLength > 0)

        // output our data...
        if (bodyOutfile.is_open()) {
          bodyOutfile << docID << "\t" << bodyCalc << "\n";
				} else {
					std::cout << docID << "\t" << bodyCalc << "\n";
				} // end if (bodyOutfile.is_open())

        if (anchorOutfile.is_open()) { anchorOutfile << docID << "\t" << anchorCalc << "\n"; }
        if (urlOutfile.is_open()) { urlOutfile << docID << "\t" << urlCalc << "\n"; }

        delete docVector;
      } // end if( response->getResults().size() )

      delete response;
    } // end for (DOCID_T docID=1; docID <= docCount; docID++)

    if (bodyOutfile.is_open()) { bodyOutfile.close(); }
    if (anchorOutfile.is_open()) { anchorOutfile.close(); }
    if (urlOutfile.is_open()) { urlOutfile.close(); }

    _repository.close();
    return 0;
  } catch( lemur::api::Exception& e ) {
    LEMUR_ABORT(e);
  }
}

