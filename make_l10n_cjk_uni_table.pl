#!/usr/bin/perl

use strict;

use FileHandle;

my %width = ( N  => 1,
	      Na => 1, # Nallow
	      W  => 2, # Wide
	      H  => 1, # HalfWidth
	      F  => 2, # FullWidth
	      A  => 2, # Anbiguous
	    );

sub main {
    print STDERR "Processing";
    my $ifh = new FileHandle("EastAsianWidth.txt");
    my $bit = new Bit;
    while (<$ifh>) {
	next if (/^\s*\#/);
	my ($from, $to, $type) = /([\da-f]+)(?:\.\.([\da-f]+))?;(\w+)/i
	    or next;
	$to = $from unless defined($to);
	last if (length($to) > 4); # only in BMP
	if ($width{$type} == 2) {
	    foreach my $code (hex($from)..hex($to)) {
		$bit->set($code);
	    }
	}
	print STDERR ".";
    }
    $ifh->close;
    print STDERR "Done\n";
    my $cnt = 0;
    print <<EOF;
#include "l10n_cjk_uni_table.h"

unsigned long cjk_width[CJK_WIDTH_LENGTH] = {
EOF
    foreach my $word (@$bit) {
	print "    " unless $cnt;
	printf("0x%08x", $word);
	$cnt = ($cnt + 1) % 4;
	if ($cnt) {
	    print ", ";
	} else {
	    print ",\n";
	}
    }
    print "};\n";
}

main;

package Bit;

sub new {
    my $class = shift;
    my $self = [];
    @$self = 0x10000 / 32;
    return bless($self, $class);
}

sub set {
    my ($self, $code) = @_;
    my $index = int($code / 32);
    my $bit   = $code % 32;
    $self->[$index] |= (1 << $bit);
}
