#!/bin/sh


#######################################################################
# THIS IS THE TEST SCRIPT FOR THE BASIC INDEXER                       #
#   (For the position indexer, use test_pos_index.sh                  #
#######################################################################

# First erasing any existing files 
./clean.sh

# Then prepare parameters
cp basicparam/*_param .

#######################################################################
# The following command converts the CACM database from a simple      #
# SGML format to the basic lemur format  (output file: source)        #
#######################################################################

../app/obj/ParseToFile parse_doc_param database.sgml

#######################################################################
# The following command converts the queries from a simple            #
# SGML format to the basic lemur format  (output file: query)         #
#######################################################################

../app/obj/ParseToFile parse_query_param query.sgml

#######################################################################
# The following command builds an index of the CACM database (source) #
#######################################################################

../app/obj/BuildBasicIndex build_param

#######################################################################
# The following command computes the necessary support information    # 
# to be used for document language model smoothing                    #
# (output file:  index.supp )                                         #
#######################################################################

../app/obj/GenerateSmoothSupport supp_param


#######################################################################
# The following scripts show how to run simple and feedback retrieval #
# experiments with different retrieval model and different parameters #
#######################################################################

# simple TFIDF retrieval example
../app/obj/RetEval simple_tfidf_param
../app/src/ireval.pl -j qrel -trec < res.simple_tfidf  > pr.simple_tfidf

# TFIDF feedback retrieval example
../app/obj/RetEval fb_tfidf_param
../app/src/ireval.pl -j qrel -trec < res.fb_tfidf > pr.fb_tfidf

# simple Okapi retrieval example
../app/obj/RetEval simple_okapi_param
../app/src/ireval.pl -j qrel -trec < res.simple_okapi  > pr.simple_okapi

# TFIDF feedback retrieval example
../app/obj/RetEval fb_okapi_param
../app/src/ireval.pl -j qrel -trec < res.fb_okapi > pr.fb_okapi

# simple language model (KL-divergence) retrieval example, using JM smoothing
../app/obj/RetEval simple_kl_jm_param
../app/src/ireval.pl -j qrel -trec < res.simple_kl_jm  > pr.simple_kl_jm

# simple language model (KL-divergence) retrieval example, using Dirichlet smoothing
../app/obj/RetEval simple_kl_dir_param
../app/src/ireval.pl -j qrel -trec < res.simple_kl_dir > pr.simple_kl_dir

# simple language model (KL-divergence) retrieval example, using absolute discounting smoothing
../app/obj/RetEval simple_kl_abs_param
../app/src/ireval.pl -j qrel -trec < res.simple_kl_abs > pr.simple_kl_abs


# Language model (KL-divergence) feedback retrieval example, using collection mixture method and Dirichlet smoothing
../app/obj/RetEval mixfb_kl_dir_param
../app/src/ireval.pl -j qrel -trec < res.mixfb_kl_dir  > pr.mixfb_kl_dir

######################################################################
#								     #
# The following examples show how one can separate the query model   #
# construction (with feedback docs) from the query model evaluation. #
# That is, we can construct different query models based on a        #
# pre-computed result, so as to avoid performing initial retrieval   #
# every time. And the constructed query model can be evaluated       #
# with different retrieval (smoothing) parameters                    #
#                                                                    #
######################################################################

# Construct a query model using the collection mixture method based on 
# previously retrieved result file "res.simple_kl_dir", store the expanded
# query in query.mixfb_kl.
../app/obj/GenerateQueryModel mixfb_model_param

# Then evaluate it with Dirichlet smoothing
../app/obj/QueryModelEval mixfb_model_eval_param
../app/src/ireval.pl -j qrel -trec < res.mixfb_kl > pr.mixfb_kl


######################################################################
#								     #
# The following examples show how one can re-rank a subset of docs   #
# e.g., when the scoring method is computationally complex.          #
######################################################################

# ==!!! First, we need to convert the TREC format result to
#       a simple format so that it can be used as a working set
awk '{print $1, $3, $5;}'  < res.simple_tfidf > res_simple_tfidf

# This re-ranks res_simple_tfidf with the tf-idf method  
../app/obj/RetEval rerank_tfidf_param
../app/src/ireval.pl -j qrel -trec < res.rerank_simple_tfidf > pr.rerank_simple_tfidf

# This re-ranks res_simple_tfidf with KL-div.+ Dirichlet. Note how this improves the precision
../app/obj/RetEval rerank_simple_kl_dir_param
../app/src/ireval.pl -j qrel -trec < res.rerank_simple_kl_dir > pr.rerank_simple_kl_dir

# This re-ranks the res_simple_tfidf with tfidf+Rocchio pseudo feedback
../app/obj/RetEval rerank_fb_tfidf_param
../app/src/ireval.pl -j qrel -trec  < res.rerank_fb_tfidf > pr.rerank_fb_tfidf




