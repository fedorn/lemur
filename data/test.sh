#!/bin/sh


#######################################################################
# The following command builds an index of the CACM database          #
#######################################################################

../app/obj/BuildBasicIndex buildparam

#######################################################################
# The following command computes the necessary support information    # 
# to be used for document language model smoothing                    #
#######################################################################

../app/obj/GenerateSmoothSupport suppparam


#######################################################################
# The following scripts show how to run simple and feedback retrieval #
# experiments with different retrieval model and different parameters #
#######################################################################

# simple TFIDF retrieval example
../app/obj/RetEval simple_tfidf_param
../app/obj/trec_eval -a qrel res.simple_tfidf > pr.simple_tfidf

# TFIDF feedback retrieval example
../app/obj/RetEval fb_tfidf_param
../app/obj/trec_eval -a qrel res.fb_tfidf > pr.fb_tfidf

# simple Okapi retrieval example
../app/obj/RetEval simple_okapi_param
../app/obj/trec_eval -a qrel res.simple_okapi > pr.simple_okapi

# TFIDF feedback retrieval example
../app/obj/RetEval fb_okapi_param
../app/obj/trec_eval -a qrel res.fb_okapi > pr.fb_okapi

# simple language model (KL-divergence) retrieval example, using JM smoothing
../app/obj/RetEval simple_kl_jm_param
../app/obj/trec_eval -a qrel res.simple_kl_jm > pr.simple_kl_jm

# simple language model (KL-divergence) retrieval example, using Dirichlet smoothing
../app/obj/RetEval simple_kl_dir_param
../app/obj/trec_eval -a qrel res.simple_kl_dir > pr.simple_kl_dir


# Language model (KL-divergence) feedback retrieval example, using collection mixture method and Dirichlet smoothing
../app/obj/RetEval mixfb_kl_dir_param
../app/obj/trec_eval -a qrel res.mixfb_kl_dir > pr.mixfb_kl_dir

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
../app/obj/trec_eval -a qrel res.mixfb_kl > pr.mixfb_kl


# Construct a query model using the divergence minimization method based on 
# previously retrieved result file "res.simple_kl_dir", store the expanded
# query in query.dmfb_kl.
../app/obj/GenerateQueryModel dmfb_model_param
# Then evaluate it with Dirichlet smoothing
../app/obj/QueryModelEval dmfb_model_eval_param
../app/obj/trec_eval -a qrel res.dmfb_kl > pr.dmfb_kl

