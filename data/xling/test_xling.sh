#!/bin/sh
#######################################################################
# THIS IS THE TEST SCRIPT FOR Cross lingual retrieval                 #
#######################################################################
BIN=../../app/obj
RET=$BIN/XlingRetEval
DICT=$BIN/PDictManager
INDEX=$BIN/BuildIndex
rm -f indt* res.xling t1.dict*
# index source language collection
$INDEX bt1.parm t1.sgml
# index target language collection
$INDEX bt2.parm t2.sgml
# create translation dictionary
$DICT -create t1.dict t1.in ';'
# retrieve target language documents for two source language queries
$RET xling.param     
