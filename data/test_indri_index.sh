#!/bin/sh
## Edit this to point to your version of trec_eval
## if not found, it will be set to echo
TREC_EVAL=/usr/mel0/tmp1/dfisher/trec_eval/trec_eval
if ! test -x $TREC_EVAL ; then
  TREC_EVAL="echo not found: $TREC_EVAL"
fi

#######################################################################
# THIS IS THE TEST SCRIPT FOR THE INDRI QUERY LANGUAGE                #
#######################################################################
#edit this if necessary
OBJ=../app/obj
# First erase any existing files 
./clean.sh
mkdir indri_index
# Then prepare parameters
cp indriparam/*_param .
#######################################################################
# The following command builds a position index of the CACM database  #
#  directly from a simple SGML format source                          #
# Note that the version of database.sgml used has been modified to    #
# include <TEXT> tags around the body of each document.               #
# Uses the stopword list provided in smallstop_param.                 #
#######################################################################

$OBJ/IndriBuildIndex build_param smallstop_param

#######################################################################
# The following shows how to run simple retrieval                     #
# experiments with indri query language queries.                      #
#######################################################################

# simple query retrieval example
$OBJ/IndriRunQuery ret_param indriparam/query.indri > res.indri
$TREC_EVAL -o -q qrel res.indri  > pr.indri
