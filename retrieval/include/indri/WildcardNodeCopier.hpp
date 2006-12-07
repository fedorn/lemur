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
// WildcardNodeCopier
//
// Copier for going through a parsed query
// and transforming any wildcard nodes into
// synonym lists.
//
// 10/26/2006 -- mhoy
//

#ifndef INDRI_WILDCARDNODECOPIER_HPP
#define INDRI_WILDCARDNODECOPIER_HPP

#include "indri/QuerySpec.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/delete_range.hpp"
namespace indri
{
  namespace lang
  {

    class WildcardNodeCopier : public indri::lang::Copier {
    protected:
      std::vector<indri::lang::Node*> _nodes;
			indri::server::LocalQueryServer *_env;

      class SingleChildWalker : public indri::lang::Walker {
      private:
        indri::lang::Node* _child;
        int _children;
        bool _seenRoot;

      public:
        SingleChildWalker() : _seenRoot(false), _children(0), _child(0) {}

        void defaultBefore( indri::lang::Node* n ) {
          if( !_seenRoot ) {
            _seenRoot = true;
          } else {
            _children++;
            _child = n;
          }
        }

        bool onlyOneChild() {
          return _children == 1;
        }

        indri::lang::Node* getChild() {
          return _child;
        }
      };

    public:
			WildcardNodeCopier(indri::server::LocalQueryServer *env) : Copier() {
				_env=env;
			}

      ~WildcardNodeCopier() {
        indri::utility::delete_vector_contents( _nodes );
      }

			indri::lang::Node *after(indri::lang::IndexTerm *oldNode, indri::lang::IndexTerm *newNode) {

				std::string term=oldNode->getText();

				// does this term have a wildcard characeter in it?
				std::string::size_type wildcardPos=term.find("*");
				if (wildcardPos != std::string::npos) {
					// ensure it's not at the first position...
					if (wildcardPos==0) {
						LEMUR_THROW( LEMUR_PARSE_ERROR, "Error in parsing wildcard terms. The wildcard character cannot be the first character in the term.");
					}

					// ensure we only have one
					if (term.find("*", wildcardPos+1)!=std::string::npos) {
						LEMUR_THROW( LEMUR_PARSE_ERROR, "Error in parsing wildcard terms. The wildcard character can only appear once per term.");
					}

					// ensure the wildcard is at the end of the term...
					if (term.length()!=(wildcardPos+1)) {
						LEMUR_THROW( LEMUR_PARSE_ERROR, "Error in parsing wildcard terms. The wildcard character will only operate on suffixes (i.e. the wildcard must be at the end of the term)");
					}

					// ok - we're good!
					// create a synonym list from this term and return it...

					std::vector<std::string> termList=_env->getWildcardTermList(term);

					// iterate through the term list and create our replacement...
					// if we don't have any - it'll get removed later via UnnecessaryNodeRemover
					indri::lang::ExtentOr *synNode=new indri::lang::ExtentOr();
					for (std::vector<std::string>::iterator tIter=termList.begin(); tIter!=termList.end(); tIter++) {
						synNode->addChild(new indri::lang::IndexTerm(*tIter));
					}

					// remove the original copy...
					delete newNode;

					// return out synonym.
					return synNode;

				}

				return newNode;
			}

    }; // end class WildcardNodeCopier
  }
}

#endif // INDRI_WILDCARDNODECOPIER_HPP

