#!/usr/bin/perl
use strict;
use CGI;
use Time::HiRes qw(time);
require File::Temp;
use File::Temp ();
use File::Temp qw/ tempfile tempdir /;

print CGI::header();

my $query = CGI->new();

=pod
my @names = $query->param();
foreach my $name (@names) {
	print $name,
		" : ",
		$query->param($name);

}
=cut

#my $previousPV = $query->param("P");
my $history = $query->param("h");
my $move = $query->param("m");
my $wtime = $query->param("wtime");
my $btime = $query->param("btime");
my $movestogo = $query->param("movestogo");
my $checkOnly = $query->param("checkOnly");

my $tmpFile = File::Temp->new();
print $tmpFile "uci\n";
print $tmpFile "setoption name WebMode value true\n";

if (defined $checkOnly) {
    print $tmpFile "setoption name MaxDepth value 0\n";
}

if (defined $history) {
    $history = " moves " . $history;
}
print $tmpFile "position startpos" . $history . "\n";
print $tmpFile "go wtime $wtime btime $btime movestogo $movestogo\n";
print $tmpFile "quit\n";

my $cmd = "cat $tmpFile | bin/chessika";
#print $cmd
my $res = `$cmd`;
print $res;
