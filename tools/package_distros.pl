#!/usr/bin/perl -w

use strict;
use warnings;
use English;

use Cwd;
use FindBin qw($Bin);

use lib "$Bin/../../adobe_source_libraries/tools";
use Adobe::ShellUtils qw(cmd_in_dir zip tar copy_dir make_writeable mk_dir build force_rmdir build_msvc);

### Establish root directory:
my $ASL_ROOT="$Bin/../..";

### check version
open(CONFIG_HPP, "< $ASL_ROOT/source_release/adobe/config.hpp") 
  or die "Cannot open $ASL_ROOT/source_release/adobe/config.hpp" ;


my $subminor_version=0;
while(<CONFIG_HPP>){
 if(m/ADOBE_VERSION_SUBMINOR/){
     my @f = split;
     $subminor_version = $f[2];
     last;
  }
}
if ($subminor_version == 0) { die "subminor version == 0"; }

my $VERSION="1.0.$subminor_version";
print "building distros for version $VERSION\n";

##assure empty /distros
if (-d "/distros") {
    force_rmdir "/distros";
}

mk_dir "/distros";

### build, copy, and compress
if ($OSNAME =~ /darwin/) {
    ### Adobe Begin
    build "$ASL_ROOT/adobe_platform_libraries", "test/begin", "debug", "architecture=combined", "clean";
    build "$ASL_ROOT/adobe_platform_libraries", "test/begin", "debug", "architecture=combined";
    copy_dir "$ASL_ROOT/built_artifacts/install/MACOSX/debug/Adobe Begin", "/distros/asl_${VERSION}_begin_mac";
    tar "/distros", "asl_${VERSION}_begin_mac.tgz", "asl_${VERSION}_begin_mac";


    ### pmeval
    build "$ASL_ROOT/adobe_source_libraries", "test/property_model_eval//install_program", "debug", "architecture=combined", "clean";
    build "$ASL_ROOT/adobe_source_libraries", "test/property_model_eval//install_program", "debug", "architecture=combined";
    copy_dir "$ASL_ROOT/built_artifacts/install/MACOSX/debug/pmeval", "/distros/asl_${VERSION}_pmeval_mac";
    tar "/distros", "asl_${VERSION}_pmeval_mac.tgz", "asl_${VERSION}_pmeval_mac";
} else {
    ### Adobe Begin
    build "$ASL_ROOT/adobe_platform_libraries", "test/begin", "release", "clean";    
    build "$ASL_ROOT/adobe_platform_libraries", "test/begin", "release";    
    copy_dir "$ASL_ROOT/built_artifacts/install/NT/release/Adobe Begin", "/distros/asl_${VERSION}_begin_win";
    "/distros/asl_${VERSION}_begin_win";
    copy_dir "C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/redist/x86/Microsoft.VC100.CRT/msvcp100.dll",
    "/distros/asl_${VERSION}_begin_win";
    copy_dir "C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/redist/x86/Microsoft.VC100.CRT/msvcr100.dll",
    "/distros/asl_${VERSION}_begin_win";
    zip "/distros", "asl_${VERSION}_begin_win.zip", "asl_${VERSION}_begin_win";

    ### pmeval
    build "$ASL_ROOT/adobe_source_libraries", "test/property_model_eval//install_program", "release", "clean";
    build "$ASL_ROOT/adobe_source_libraries", "test/property_model_eval//install_program", "release";
    copy_dir "$ASL_ROOT/built_artifacts/install/NT/release/pmeval", "/distros/asl_${VERSION}_pmeval_win";
    zip "/distros", "asl_${VERSION}_pmeval_win.zip", "asl_${VERSION}_pmeval_win";
}

### copy asl_net_setup.sh
open(NET_SETUP_SH_SOURCE, "< $ASL_ROOT/adobe_platform_libraries/tools/asl_net_setup.sh") 
    or die "couldn't open file asl_net_setup.sh: $!";
open(NET_SETUP_SH_TARGET, "+> /distros/asl_${VERSION}_net_setup.sh") 
    or die "couldn't open file asl_${VERSION}_net_setup.sh: $!";

print "copying and setting version in $ASL_ROOT/adobe_platform_libraries/tools/asl_net_setup.sh\n";
while(<NET_SETUP_SH_SOURCE>){
    s/asl_1.0.\d\d/asl_${VERSION}/;
    s/apl_1.0.\d\d/apl_${VERSION}/;
    print NET_SETUP_SH_TARGET $_;
}

### copy asl_net_setup.bat
open(NET_SETUP_BAT_SOURCE, "< $ASL_ROOT/adobe_platform_libraries/tools/asl_net_setup.bat") 
    or die "couldn't open file asl_net_setup.bat: $!";
open(NET_SETUP_BAT_TARGET, "+> /distros/asl_${VERSION}_net_setup.bat") 
    or die "couldn't open file open file asl_${VERSION}_net_setup.bat: $!";

print "copying and setting version in $ASL_ROOT/adobe_platform_libraries/tools/asl_net_setup.bat\n";
while(<NET_SETUP_BAT_SOURCE>){
    s/asl_1.0.\d\d/asl_${VERSION}/;
    s/apl_1.0.\d\d/apl_${VERSION}/;
    print NET_SETUP_BAT_TARGET $_;
}

### zip and tar asl source and platform

make_writeable "${ASL_ROOT}/platform_release/";
zip $ASL_ROOT, "/distros/apl_${VERSION}.zip", "platform_release";
tar $ASL_ROOT, "/distros/apl_${VERSION}.tgz", "platform_release";

make_writeable "${ASL_ROOT}/source_release/";
zip $ASL_ROOT, "/distros/asl_${VERSION}.zip", "source_release";
tar $ASL_ROOT, "/distros/asl_${VERSION}.tgz", "source_release";
