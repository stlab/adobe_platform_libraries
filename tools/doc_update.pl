#!/usr/bin/perl

use warnings ;
use strict ;
use FindBin qw($Bin);

use lib "$Bin/../../adobe_source_libraries/tools";
use Adobe::ShellUtils qw(p4_edit p4_revert_unchanged doxygen);
use Cwd;

### Establish root directory:
my $ASL_ROOT=Cwd::realpath("$Bin/../..");
my $DOCS=Cwd::realpath("$ASL_ROOT/adobe_source_libraries/documentation");

print p4_edit "$DOCS", "$DOCS/html/...";

chdir(Cwd::realpath("$DOCS/html"));

open(XARGS, "| xargs rm") or die "can't open xargs rm: $!";
print XARGS join(" ", <*>);
close(XARGS)      || die "can't close XARGS : $!";

print doxygen "$DOCS", "doxyfile";

open(XARGS, "| xargs p4 add") or die "can't open xargs rm: $!";
print XARGS join(" ", <*>);
close(XARGS)      || die "can't close XARGS : $!";

print p4_revert_unchanged "$DOCS", "$DOCS/html/...";
