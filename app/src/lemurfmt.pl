#!/usr/bin/perl

use English;
use Carp;
use Getopt::Long;

$debug=0;

sub Usage{
  my $message = shift;

  print STDERR $message, "\n" if $message;
  print STDERR "\nUsage: $0 -d(atabase) DatabaseName -m(ode) Mode\n";

  print STDERR <<'EOM';

         formatted source is written to standard  output

          -d(atabase)      database to format 
	                  (e.g., source) 
			   a format file with the same name
			   as the database file and with the
			   extension ".fmt" is assume dto exist
			       (e.g., source.fmt), 
			   and it provides tag specification
			      
			   The fields recognized in the format file include
                             TITLE, IGNORE, DOCBEGIN,
			      DOCEND, IDBEGIN, IDEND

			    An example of the format file is:

			    IGNORE: tag-to-ignore
			    IDBEGIN: DOCID
			      IDEND: /DOCID

			    IGNORE causes the content tagged by "tag-to-ignore"
				(i.e., anything between <tag-to-ignore> and
                                </tag-to-ignore>) to be ignored. 
			    There can be multiple "IGNORE" entries. 

	    -m(ode)     mode: 
	                    fmtonly : do not remove extra tags, do not ignore
			      ignoreonly : ignore some fields
				removeonly: remove tags
				  fullfmt: do all

          -h(elp) 	: display this message
	      
	      
	      

EOM

    exit(1);

}

if (! &GetOptions("help", "database=s", "mode=s") or
    $opt_help) {
  &Usage();
}

open(DBFMT, $opt_database . ".fmt") || die "can't open format file:$opt_database",".fmt\n";

$docBegin="DOC";
$docEnd="\/DOC";
$idBegin="DOCNO";
$idEnd="\/DOCNO";
while (<DBFMT>) {
    print STDERR if $debug;
    if (/^\s*TITLE\s*:\s*([^\s]+)/) {
	$title{$1}=1;
    } elsif (/^\s*IGNORE\s*:\s*([^\s]+)/) {
	$ignore{$1}=1;
    } elsif (/^\s*DOCBEGIN\s*:\s*([^\s]+)/) {
	$docBegin=$1;
    } elsif (/^\s*DOCEND\s*:\s*([^\s]+)/) {
	$docEnd=$1;
    } elsif (/^\s*IDBEGIN\s*:\s*([^\s]+)/) {
	$idBegin=$1;
    } elsif (/^\s*IDEND\s*:\s*([^\s]+)/) {
	$idEnd=$1;
    }
}
#print STDERR "=$idBegin=$idEnd\n";
close (DBFMT);
# $debug =1;
$removeTag= ($opt_mode eq "removeonly") || ($opt_mode eq "fullfmt");
$ignoreTag= ($opt_mode eq "ignoreonly") || ($opt_mode eq "fullfmt");

open(DB, $opt_database) || die "can't open database\n";


while (<DB>) {
    print STDERR if ($debug);
	while (/(<\/[^>]+>)|(<[^>]+>)|([^><]+)/go) {
	    $closeTag=$1;
	    $beginTag=$2;
	    $content=$3;
	    print STDERR "===$1 $2 $3===\n" if ($debug);
	    # $1 is closing tag, $2 is beginning tag, $3 is field content
	    if ($beginTag =~ /<$docBegin>/o) {
		$docTitle="";
		$docID="";
		$docBody="";
	    } elsif ($closeTag =~ /<$docEnd>/o) {
		

		$docID =~ s/\_/X/g;

		# print "***\n$docID\n";
		print "<DOC $docID>\n";
		$docTitle =~ s/[^a-zA-Z\s]+//g;
		$docBody =~ s/[^a-zA-Z\s]+//g;
		print "\n$docTitle\n$docBody\n<\/DOC>\n";
	    } elsif ($beginTag=~ /<$idBegin>/o) {
		$inDOCNO=1;
	    } elsif ($closeTag =~ /<$idEnd>/o ) {
		$inDOCNO=0;
	    } elsif (($beginTag=~/<([^>\s]+)>/o) && $title{$1}) {
		# beginning of title
		$inTITLE=1;
	    } elsif (($closeTag=~/<\/([^>\s]+)>/o) && $title{$1}) {	
		$inTITLE=0;
	    } elsif ($inIGNORE && ($closeTag=~/<\/([^>\s]+)>/o) && ($1 eq $ignoreTagName )) {
		print STDERR "finding ignore closing tag: $closeTag\n" if ($debug);
		$inIGNORE=0;
		$ignoreTagName="";
		$ignoredTxt .= $closeTag;
	    } elsif ($inIGNORE && (($beginTag =~ /<([^>\s])/o) || ($closeTag=~ /<\/([^>\s]+)>/o))) {
		# seeing tags within ignored fields, ignore them.
		$ignoredTxt .= $beginTag if ($beginTag);
		$ignoredTxt .= $closeTag if ($closeTag);
	    } elsif ($ignoreTag && ($beginTag=~/<([^>\s]+)/o) && $ignore{$1}) {
		# seeing new begining tag specified as ignored
		$inIGNORE=1;
		print STDERR "finding ignore begin tag: $beginTag\n" if ($debug);
		$ignoreTagName=$1;
		$ignoredTxt .= "\n$beginTag";
	    } elsif ($ignoreTag &&($closeTag=~/<\/([^>\s]+)>/o) && $ignore{$1}) {	
		$inIGNORE=0;
		print STDERR "Shouldn't print this! something is wrong! $closeTag\n";
		$ignoredTxt .= "$closeTag\n";
	    } elsif ($removeTag && $closeTag) {
		$skippedTags .=" $closeTag\n";
	    } elsif ( $removeTag && $beginTag) {
		$skippedTags .=" $beginTag\n";
	    } elsif ($closeTag) {
		$docBody .= "$closeTag\n";
	    } elsif ($beginTag) {
		$docBody .= "$beginTag\n";
	    } else { # in contents!
		if ($inTITLE) {
		    $docTitle .= "$content\n";
		} elsif ($inDOCNO) {
		    $docID = $content;
		} elsif (!$inIGNORE) {
		    $docBody .= "$content";
		} else {
		    $ignoredTxt .= "$content\n";
		}
	    }
	}
}

open(LOG, ">$opt_database.ignore") || die "can't open log file\n";
print LOG "==== Skipped tags ===\n$skippedTags\n";
print LOG "==== Skipped contents ===\n$ignoredTxt\n";
close (LOG);
