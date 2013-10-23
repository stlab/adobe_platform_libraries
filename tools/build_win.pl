#!/usr/bin/perl -w

use strict;
use warnings;

use Cwd;
use FindBin qw($Bin);

use lib "$Bin/../../adobe_source_libraries/tools";
use Adobe::ShellUtils qw(cmd_in_dir zip tar copy_dir make_writeable mk_dir build force_rmdir build_msvc);

build "$Bin/../../adobe_source_libraries/", "debug", "msvc-10.0", "--without-python" ;
build "$Bin/../../adobe_source_libraries/", "release", "msvc-10.0", "--without-python" ;
build "$Bin/../../adobe_source_libraries/", "debug", "msvc-10.0", "address-model=64", "--without-python" ;
build "$Bin/../../adobe_source_libraries/", "release", "msvc-10.0", "address-model=64", "--without-python" ;

build "$Bin/../", "debug", "msvc-10.0", "--without-python" ;
build "$Bin/../", "release", "msvc-10.0", "--without-python" ;
build "$Bin/../", "debug", "msvc-10.0", "address-model=64", "--without-python" ;
build "$Bin/../", "release", "msvc-10.0", "address-model=64", "--without-python" ;

#build "$Bin/../", "debug", "msvc-8.0", "--without-python" ;
#build "$Bin/../", "release", "msvc-8.0", , "--without-python" ;

build "$Bin/../../adobe_source_libraries/", "debug", "release", "gcc-4.5.0", "instruction-set=i686", "--without-python" ;

## signal issues?

build_msvc "$Bin/../msvc_ide", "begin.sln", "/build", "debug";
build_msvc "$Bin/../msvc_ide", "begin.sln", "/build", "release";
