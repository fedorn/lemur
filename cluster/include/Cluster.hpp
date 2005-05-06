/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

// David Fisher
// init: 11/18/2002
#ifndef _CLUSTER_HPP
#define _CLUSTER_HPP
#include <string>
#include "common_headers.hpp"
#include "Index.hpp"
#include "ClusterRep.hpp"
#include "SimilarityMethod.hpp"
#include "ClusterParam.hpp"
namespace lemur
{
  namespace cluster
  {
    // need an enumeration of types
#define DOC_ELT 1
#define CLUSTER_ELT 2
    // maximum length of a cluster name string.
#define NAMESIZE 50
    /// Cluster element representation.
    class ClusterElt {
    public:
      bool operator==(const ClusterElt& second) const {
        return (myType == second.myType && id == second.id);
      }
      int id;
      int myType;
    };

    /// Cluster representation, a vector of cluster elements.
    class Cluster {
    public:
      ///create empty cluster with specific id.
      Cluster(int cid, const lemur::api::Index &ind, 
              const SimilarityMethod &sim);
      /// delete cluster
      virtual ~Cluster();
      /// set cluster id.
      void setId(int cid);
      /// set cluster symbolic name.
      void setName(const string &newName);
      /// get cluster id.
      int getId() const;
      /// get cluster symbolic name.
      const string &getName() const;
      /// get vector of elements in this cluster.
      const vector <ClusterElt> *getIds() const;
      /// get vector of doc ids in this cluster.
      vector <lemur::api::DOCID_T> getDocIds() const;
      /// \brief Get the ClusterRep for this Cluster for scoring. 
      /// The Rep will have been weighted by the SimilarityMethod. 
      /// Caller responsible for deleting.
      virtual ClusterRep *getClusterRep() const = 0;
      /// score a document against this cluster, given the rep.
      virtual double score(const ClusterRep *rep) const = 0;
      /// add the element to this cluster
      virtual void add(const ClusterElt &elt);
      /// Add a list of documents to a cluster
      virtual void add(const vector<lemur::api::DOCID_T> docids);
      /// remove the element from this cluster
      virtual void remove(const ClusterElt &elt);
      /// Split this cluster into subclusters.
      virtual vector<Cluster *> split(int numParts = 2);
      /// merge in a cluster
      virtual void merge(const Cluster *c);
      /// read a cluster in from the cluster db file.
      virtual bool read(ifstream &in);
      /// write a cluster to the cluster db file.
      virtual void write(ofstream &out);
      /// pretty print to stdout.
      void print() const ;  
      /// How many elements
      int  getSize() const { return size;}
      /// Sum of squared values in the cluster's term vector.
      virtual double sum2() const { return 0; }
      /// Return the top numTerms terms in the cluster (tf.idf scores).
      virtual string getKeyWords(int numTerms = 10) const ;
  
    protected:
      /// internal id
      int id;
      /// external symbolic name.
      string name;
      /// vector of cluster elements in this cluster.
      vector<ClusterElt> ids;
      /// The SimilarityMethod to use for weighting.
      const SimilarityMethod &similarity;
      /// size of the cluster
      int size;
      double weight;
      const lemur::api::Index &ind;
      fstream *datFile;
      long offset;
    };
  }
}

#endif
