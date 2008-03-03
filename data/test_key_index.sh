#!/bin/sh
## Edit this to point to your version of trec_eval
## if not found, it will be set to echo
IREVAL=../ireval/obj/ireval.jar
TREC_EVAL="java -jar $IREVAL"

#######################################################################
# THIS IS THE TEST SCRIPT FOR THE POSITION INDEXER                    #
#   (For the basic indexer, use test_basic_index.sh)                  #
#######################################################################

# First erasing any existing files 
./clean.sh

# Then prepare parameters
cp keyparam/*_param .


#######################################################################
# The following command converts the queries from a simple            #
# SGML format to the basic lemur format  (output file: query)         #
#######################################################################

../app/obj/ParseToFile parse_query_param query.sgml

#######################################################################
# The following command builds a position index of the CACM database  #
#  directly from a simple SGML format source                          #
#######################################################################

../app/obj/BuildIndex build_param database.sgml

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
$TREC_EVAL  res.simple_tfidf  qrel > pr.simple_tfidf

# TFIDF feedback retrieval example
../app/obj/RetEval fb_tfidf_param
$TREC_EVAL  res.fb_tfidf qrel > pr.fb_tfidf

# simple Okapi retrieval example
../app/obj/RetEval simple_okapi_param
$TREC_EVAL  res.simple_okapi  qrel > pr.simple_okapi

# TFIDF feedback retrieval example
../app/obj/RetEval fb_okapi_param
$TREC_EVAL  res.fb_okapi qrel > pr.fb_okapi

# simple language model (KL-divergence) retrieval example, using JM smoothing
../app/obj/RetEval simple_kl_jm_param
$TREC_EVAL  res.simple_kl_jm  qrel > pr.simple_kl_jm

# simple language model (KL-divergence) retrieval example, using Dirichlet smoothing
../app/obj/RetEval simple_kl_dir_param
$TREC_EVAL  res.simple_kl_dir qrel > pr.simple_kl_dir

# simple language model (KL-divergence) retrieval example, using absolute discounting smoothing
../app/obj/RetEval simple_kl_abs_param
$TREC_EVAL  res.simple_kl_abs qrel > pr.simple_kl_abs


# Language model (KL-divergence) feedback retrieval example, using collection mixture method and Dirichlet smoothing
../app/obj/RetEval mixfb_kl_dir_param
$TREC_EVAL  res.mixfb_kl_dir  qrel > pr.mixfb_kl_dir

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
# query in query_mixfb_kl.
../app/obj/GenerateQueryModel mixfb_model_param

# Then evaluate it with Dirichlet smoothing
../app/obj/QueryModelEval mixfb_model_eval_param
$TREC_EVAL  res.mixfb_kl qrel > pr.mixfb_kl


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
$TREC_EVAL  res.rerank_simple_tfidf qrel > pr.rerank_simple_tfidf

# This re-ranks res_simple_tfidf with KL-div.+ Dirichlet. Note how this improves the precision
../app/obj/RetEval rerank_simple_kl_dir_param
$TREC_EVAL  res.rerank_simple_kl_dir qrel > pr.rerank_simple_kl_dir

# This re-ranks the res_simple_tfidf with tfidf+Rocchio pseudo feedback
../app/obj/RetEval rerank_fb_tfidf_param
$TREC_EVAL   res.rerank_fb_tfidf qrel > pr.rerank_fb_tfidf

######################################################################
#								     #
# The following examples show how one can do relevance feedback, as  #
# opposed to pseudo feedback                                         #
######################################################################

# This performs relevance feedback using the mixture model approach
# You can use any other retrieval methods
../app/obj/RelFBEval mixrelfb_kl_dir_param
$TREC_EVAL   res.mixrelfb_kl_dir qrel > pr.mixrelfb_kl_dir



######################################################################
#								     #
# The following examples show how one can do iterative feedback      #
# with the KL-divergence model. Basically, GenerateQueryModel allows #
# you to expand any previously saved query model; the newly          #
# expanded query is saved in a (different) file                      #
#                                                                    #
######################################################################

# We do relevance feedback to expand the saved query from pseudo feedback 
# The saved query is generated using GenerateQueryModel with parameter file mixfb_model_param, see above
# The new expanded query is saved in "query_it"
../app/obj/GenerateQueryModel mixrelfb_it_param

# Then use the expanded query "query_it" to retrieve documents 
../app/obj/QueryModelEval mixrelfb_it_eval_param
$TREC_EVAL  res.mixrelfb_it qrel > pr.mixrelfb_it




######################################################################
#								     #
# The following examples show how one can do automatic estimation    #
# of retrieval parameters using the two-stage smoothing method       #
#                                                                    #
######################################################################

# We first estimate the 1st-stage Dirichlet prior parameter 
# ("prior sample size"). This does not depend on the query
../app/obj/EstimateDirPrior est_dirprior_param
# After it is finishes, it will print out the estimated value

# Then, we estimate the 2nd-stage query noise parameter
# We have let the Dirichlet prior parameter set to the estimated
# value in the parameter file "twostage_param"
../app/obj/TwoStageRetEval twostage_param
$TREC_EVAL  res.twostage qrel > pr.twostage



