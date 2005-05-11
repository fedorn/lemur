#!/usr/local/bin/perl



use English;
use Carp;
use Getopt::Long;

sub Usage{
  my $message = shift;

  print STDERR $message, "\n" if $message;
  print STDERR "\nUsage: $0 -j(udgments) qrel-file -trec < result \n";

  print STDERR <<'EOM';
	  -j(udgements) filename    Judgment file

	  -t(rec)    TREC format, i.e., four column qrel and six column results
	             other wise, three column qrel and three column results

          -h(elp) 	: display this message

EOM

    exit(1);

}

if (! &GetOptions("help","trec", "judgments=s") or
    $opt_help) {
  &Usage();
}



open(T,"$opt_judgments") ||  die "can't open judgment file: $opt_judgments\n";

while (<T>) {
    if ($opt_trec) {
	($q,$dummy, $d,$v) = split;
    } else {
	($q,$d,$v) = split;
    }
    $dict{$q ."=".$d} =$v;
    # some qrel files use values >1 for rel
#    if ($v == 1) {
    if ($v != 0) {
	$totalRels{$q} ++;
    }
}

$curQ="";

@recLevel = (0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7,0.8, 0.9, 1.0);
@docLevel = (5, 10, 15, 20, 30, 100, 200, 500, 1000);

for ($i=0; $i<=$#recLevel; $i++) {
    $precRec[$i] = 0;
    $avgPrecRec[$i] = 0;
}

for ($i=0; $i<=$#docLevel; $i++) {
    $precDoc[$i] = 0;
    $avgPrecDoc[$i] = 0;
}


$countQuery = 0;
$setAvgPr = 0;
$setExactPr = 0;
$setTotalRel = 0;
$setRel = 0;

$rel = 0;
$avgPr = 0;
$exactPr = 0;
while (<stdin>) {

    if ($opt_trec) {
	($q, $dummy1, $d, $dummy2, $s, $dummy3) =split;    
    } else {
	($q,$d,$s) =split;
    }

    if ($q ne $curQ) { # new query
	$setTotalRel += $totalRels{$q};
	if ($curQ ne "") {
	    &PrintFigure;
	} 

	$curQ=$q;
	$avgPr = 0;
	$rankCount=0;
	$rel=0;

	for ($i=0; $i<=$#recLevel; $i++) {
	    $precRec[$i] = 0;
	}

	for ($i=0; $i<=$#docLevel; $i++) {
	    $precDoc[$i] = 0;
	}
    }
    
    $rankCount++;
# not sure if this does the right thing.
#    if ($totalRels{$q}==0) { next; } # skip any query with no judgments
    if ($totalRels{$q}==0) { next; } # skip any query with no judgments
    $judge = $dict{$q ."=".$d};
    if ($judge) {
	$rel++;
    }
    $thisPr = $rel/$rankCount;
    if ($judge) {
	$avgPr += $thisPr;
	
	for ($i=0; $i<=$#recLevel; $i++) {
	    if ($rel >= $recLevel[$i]*$totalRels{$q} && $thisPr >$precRec[$i]) {
		$precRec[$i] = $thisPr;
	    }
	}

    }
    if ($rankCount == $totalRels{$q}) {
	$exactPr = $thisPr;
    }
    for ($i=0; $i<=$#docLevel; $i++) {
	if ($rankCount == $docLevel[$i]) {
	    $precDoc[$i] = $thisPr;
	}
	
    }



}

if ($curQ ne "") {


    &PrintFigure;
} 

print STDOUT "Set average over $countQuery topics\n";
print STDOUT "Set average (non-interpolated) precision = ", $setAvgPr/$countQuery, "\n";
print STDOUT "Set total number of relevant docs = $setTotalRel\n";
print STDOUT "Set total number of retrieved relevant docs = $setRel\n";
print STDOUT "Set average interpolated precision at recalls:\n";


for ($i=0; $i<=$#recLevel; $i++) {
    print STDOUT " avg prec at $recLevel[$i] = ",$avgPrecRec[$i]/$countQuery,"\n";
}
print STDOUT "Set non-interpolated precsion at docs:\n";

for ($i=0; $i<=$#docLevel; $i++) {
    print STDOUT " avg prec at $docLevel[$i] = ",$avgPrecDoc[$i]/$countQuery,"\n";
}
print STDOUT "Set breakeven precision = ", $setExactPr/$countQuery,"\n";


sub PrintFigure {
# have to increment the counter even if there were not judgements.
    $countQuery ++;
# not sure if this does the right thing.
#    if ($totalRels{$curQ}==0) {
#	print STDERR "Topic $curQ ignored: no judgments found\n";
#	return;
#    }

    if ($totalRels{$curQ}==0) {
#	print STDERR "Topic $curQ ignored: no judgments found\n";
	return;
    }

    print STDOUT "Topic: $curQ\n";
    print STDOUT "Total number of relevant docs = ", $totalRels{$curQ},"\n";
    print STDOUT "Total number of retrieved relevant docs = ", $rel,"\n";
	  
#    $p = $avgPr/$totalRels{$curQ};
# need this if not returning due to no rels
    if ($totalRels{$curQ}==0) {
        $p = 0;
    } else {
        $p = $avgPr/$totalRels{$curQ};
    }
    print STDOUT "Average (non-interpolated) precision = $p\n";
    print STDOUT "Interpolated precsion at recalls:\n";
    $setAvgPr += $p;
    for ($i=0; $i<=$#recLevel; $i++) {
	print STDOUT "  prec at ", $recLevel[$i], " = $precRec[$i]\n";
        $avgPrecRec[$i]+= $precRec[$i];
    }
    print STDOUT "Non-interpolated precsion at docs:\n";

    for ($i=0; $i<=$#docLevel; $i++) {
	print STDOUT "  prec at ", $docLevel[$i], " = $precDoc[$i]\n";
	$avgPrecDoc[$i]+= $precDoc[$i];
    }
    print STDOUT "Breakeven Precision: $exactPr\n";
    $setExactPr += $exactPr;
    $setRel += $rel;
    print STDOUT "\n";
    return;
}

