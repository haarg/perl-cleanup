#!./perl 

BEGIN {
    chdir 't' if -d 't';
    @INC = '../lib';
    $ENV{PERL5LIB} = '../lib';
}

$| = 1;

my $tmpfile = "tmp0000";
my $i = 0 ;
1 while -f ++$tmpfile;
END { unlink $tmpfile if $tmpfile; }

my @prgs = () ;

foreach (sort glob("pragma/strict-*")) {

    open F, "<$_" or die "Cannot open $_: $!\n" ;
    while (<F>) {
	last if /^__END__/ ;
    }

    {
        local $/ = undef;
        @prgs = (@prgs, split "\n########\n", <F>) ;
    }
    close F ;
}

undef $/;

print "1..", scalar @prgs, "\n";
 
 
for (@prgs){
    my $switch = "";
    my @temps = () ;
    if (s/^\s*-\w+//){
        $switch = $&;
    }
    my($prog,$expected) = split(/\nEXPECT\n/, $_);
    if ( $prog =~ /--FILE--/) {
        my(@files) = split(/\n--FILE--\s*([^\s\n]*)\s*\n/, $prog) ;
	shift @files ;
	die "Internal error test $i didn't split into pairs, got " . 
		scalar(@files) . "[" . join("%%%%", @files) ."]\n"
	    if @files % 2 ;
	while (@files > 2) {
	    my $filename = shift @files ;
	    my $code = shift @files ;
    	    push @temps, $filename ;
	    open F, ">$filename" or die "Cannot open $filename: $!\n" ;
	    print F $code ;
	    close F ;
	}
	shift @files ;
	$prog = shift @files ;
    }
    open TEST, "| sh -c './perl $switch' >$tmpfile 2>&1";
    print TEST $prog, "\n";
    close TEST;
    my $status = $?;
    my $results = `cat $tmpfile`;
    $results =~ s/\n+$//;
    $expected =~ s/\n+$//;
    my $prefix = ($results =~ s/^PREFIX\n//) ;
    if ( $results =~ s/^SKIPPED\n//) {
	print "$results\n" ;
    }
    elsif (($prefix and $results !~ /^\Q$expected/) or
	   (!$prefix and $results ne $expected)){
        print STDERR "PROG: $switch\n$prog\n";
        print STDERR "EXPECTED:\n$expected\n";
        print STDERR "GOT:\n$results\n";
        print "not ";
    }
    print "ok ", ++$i, "\n";
    foreach (@temps) 
	{ unlink $_ if $_ } 
}