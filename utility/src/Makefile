#define this module
MODULE	= utility
# IN GENERAL,  DO NOT CHANGE ANY LINES BELOW THIS POINT
include ../../MakeMod
#ALLHEADER += ../include/greedy_vector ../include/count_iterator
INDRIHEADER += ../include/indri/greedy_vector ../include/indri/count_iterator
stamp:
	awk -f ../src/version-stamper "stamp=`date`" ../include/indri/indri-platform.h > ../include/indri/indri-platform.h2
	mv ../include/indri/indri-platform.h2 ../include/indri/indri-platform.h
