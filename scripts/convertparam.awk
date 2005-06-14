#==========================================================================
# * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
# *
# * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
# * is subject to the terms of the software license set forth in the LICENSE
# * file included with this software, and also available at
# * http://www.lemurproject.org/license.html
# *
#==========================================================================
# Usage:
# gawk -f convertparam.awk paramfile > newparamfile
# converts lemur parameter files from pre 4.0 to the xml format used by 4.0

BEGIN {
# split on param = value;
  FS="=|;";
# all parameter files contain a single parameters element at top level
  print "<parameters>";
}
END {
# close the top level element
  print "</parameters>";
}
/^[ \t\f\v]*[a-zA-Z].+=.+;/ {
#strip leading and trailing whitespace characters on param and value
  sub(/^[ \t\f\v]+/, "", $1);
  sub(/[ \t\f\v]+$/, "", $1);
  sub(/^[ \t\f\v]+/, "", $2);
  sub(/[ \t\f\v]+$/, "", $2);
  param = $1;
  val = $2;
# if the line contains a comment, emit it before the parameter
  if (match($0,/\/\*(.+)\*\//)) {
    com = substr($0, RSTART+2, RLENGTH-4);
    print "<!-- " com " -->";
  }
  print "<" param ">" val "</" param ">"
}
/^[ \t\f\v]*\/\*(.+)\*\// {
#emit comment lines
  print "<!-- " substr($0, 3, length - 4) " -->";
}
