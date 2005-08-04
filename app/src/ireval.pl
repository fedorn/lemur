#!/usr/local/bin/perl



use English;
use Carp;
use Getopt::Long;

sub Usage{
  my $message = shift;

  print STDERR $message, "\n" if $message;
  print STDERR "\nUsage: $0 -q -j(udgments) qrel-file -trec -N num < result \n";

  print STDERR <<'EOM';
	  -j(udgements) filename    Judgment file

	  -t(rec)    TREC format, i.e., four column qrel and six column results
	             other wise, three column qrel and three column results

	  -N num     top number of documents per query (default 1000).

          -q         print information for each query.

          -h(elp) 	: display this message

EOM

    exit(1);

}

if (! &GetOptions("help","trec", "judgments=s", "N:i", "q") or
    $opt_help) {
  &Usage();
}

if ($opt_N == 0) { $opt_N = 1000; }

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
$setTotalRet = 0;
$setRel = 0;
$retCount = 0;
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
            # fill in values for any short list.
            if ($totalRels{$q} != 0) {
                $setTotalRet += $rankCount;
                $retCount = $rankCount;
                while ($rankCount < $opt_N) {
                    $rankCount++;
                    $thisPr = $rel/$rankCount;
                    if ($rankCount == $totalRels{$curQ}) {
                        $exactPr = $thisPr;
                    }
                    for ($i=0; $i<=$#docLevel; $i++) {
                        if ($rankCount == $docLevel[$i]) {
                            $precDoc[$i] = $thisPr;
                        }
                    }
                }
                ## update any missing levels
                if ($opt_N < 1000) {
                    for ($i=0; $i<=$#docLevel; $i++) {
                        if ($rankCount < $docLevel[$i]) {
                            ## need an entry for this value
                            $thisPr = $rel/$docLevel[$i];
                            $precDoc[$i] = $thisPr;
                        }
                    }
                }
            }
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
    if ($rankCount == $opt_N) { 
        next ;  ## don't do more than opt_N
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
    # fill in values for any short list.
    if ($totalRels{$curQ} != 0) { # skip any query with no judgments
        $setTotalRet += $rankCount;
        $retCount = $rankCount;
        while ($rankCount < $opt_N) {
            $rankCount++;
            $thisPr = $rel/$rankCount;
            if ($rankCount == $totalRels{$curQ}) {
                $exactPr = $thisPr;
            }
            for ($i=0; $i<=$#docLevel; $i++) {
                if ($rankCount == $docLevel[$i]) {
                    $precDoc[$i] = $thisPr;
                }
            }
        }
    }
    &PrintFigure;
} 

print STDOUT "Set average over $countQuery topics\n";
print STDOUT "Total number of documents over all queries\n";
printf( STDOUT "      Retrieved: %7d\n", $setTotalRet);
printf( STDOUT "      Relevant:  %7d\n", $setTotalRel);
printf( STDOUT "      Rel_ret:   %7d\n", $setRel);
print STDOUT "Interpolated Recall - Precision\n";

for ($i=0; $i<=$#recLevel; $i++) {
    printf( STDOUT "      at %4.2f\t| %.4f\n", $recLevel[$i], $avgPrecRec[$i]/$countQuery);
}
printf( STDOUT "Average precision (non-interpolated) over all rel docs\n\t\t| %.4f\n", $setAvgPr/$countQuery);

print STDOUT "Precision:\n";

for ($i=0; $i<=$#docLevel; $i++) {
    printf( STDOUT "  At %4d docs:\t| %.4f\n", $docLevel[$i], $avgPrecDoc[$i]/$countQuery);
}
print STDOUT "R-Precision (precision after R (= num_rel for a query) docs retrieved):\n";
printf( STDOUT "Exact:\t\t| %.4f\n",$setExactPr/$countQuery);


sub PrintFigure {
# not sure if this does the right thing.
#    if ($totalRels{$curQ}==0) {
#	print STDERR "Topic $curQ ignored: no judgments found\n";
#	return;
#    }

    if ($totalRels{$curQ}==0) {
#	print STDERR "Topic $curQ ignored: no judgments found\n";
	return;
    }
# have to increment the counter only if there judgements.to match trec_eval.
    $countQuery ++;

    if ($opt_q) {
        print STDOUT "Topic:\t\t$curQ\n";
        print STDOUT "Total number of documents\n";
        printf( STDOUT "      Retrieved: %7d\n", $retCount);
        printf( STDOUT "      Relevant:  %7d\n", $totalRels{$curQ});
        printf( STDOUT "      Rel_ret:   %7d\n", $rel);
    }  
#    $p = $avgPr/$totalRels{$curQ};
# need this if not returning due to no rels
    if ($totalRels{$curQ}==0) {
        $p = 0;
    } else {
        $p = $avgPr/$totalRels{$curQ};
    }

    if ($opt_q) { print STDOUT "Interpolated Recall - Precision\n";}
    $setAvgPr += $p;
    for ($i=0; $i<=$#recLevel; $i++) {
        if ($opt_q) {
            printf( STDOUT "      at %4.2f\t| %.4f\n", $recLevel[$i], 
                    $precRec[$i]);
        }
        $avgPrecRec[$i]+= $precRec[$i];
    }
    if ($opt_q) {
        printf( STDOUT "Average precision (non-interpolated) over all rel docs\n\t\t| %.4f\n", $p);
    print STDOUT "Precision:\n";
    }

    for ($i=0; $i<=$#docLevel; $i++) {
	if ($opt_q) {
            printf( STDOUT "  At %4d docs:\t| %.4f\n", $docLevel[$i], 
                    $precDoc[$i]);
        }
	$avgPrecDoc[$i]+= $precDoc[$i];
    }
    if ($opt_q) {
        print STDOUT "R-Precision (precision after R (= num_rel for a query) docs retrieved):\n";
        printf( STDOUT "Exact:\t\t| %.4f\n", $exactPr);
        print STDOUT "\n";
    }
    $setExactPr += $exactPr;
    $setRel += $rel;
    return;
}

