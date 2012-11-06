#!/usr/bin/perl -w

use strict;
use warnings;

use File::Basename;

use Cwd;
use FindBin qw($Bin);

use lib "$Bin/../../adobe_source_libraries/tools";
use Adobe::ShellUtils qw(run_perl_script p4_edit);

die "usage: update_adobe_codexes ldap-username ldap-password" unless @ARGV == 2;

## REVISIT (mmarcus) : Calculate from current date and $ADOBE_ROOT/adobe_source_libraries/adobe/config.hpp

my ($asl_date, $ldap_username, $ldap_password)=@ARGV;
my $adobe_root=dirname(dirname("$Bin"));


### check version
open(CONFIG_HPP, "< $adobe_root/adobe_source_libraries/adobe/config.hpp") 
  or die "Cannot open $adobe_root/adobe_source_libraries/adobe/config.hpp" ;


my $subminor_version=0;
while(<CONFIG_HPP>){
 if(m/ADOBE_VERSION_SUBMINOR/){
     my @f = split;
     $subminor_version = $f[2];
     last;
  }
}
if ($subminor_version == 0) { die "subminor version == 0"; }

my $asl_version="1.0.$subminor_version";
print "building codexes for version $asl_version\n";

##Update ASL Codex:

print p4_edit "$adobe_root/adobe_source_libraries/tools", "$adobe_root/adobe_source_libraries/tools/VersionInfo.xml";

run_perl_script "$adobe_root/adobe_source_libraries/tools", 
    "asl_codex.pl", 
    "$asl_version";

run_perl_script "$adobe_root/adobe_source_libraries/tools", 
    "post_build.pl", 
    "$asl_version", 
    "$ldap_username",
    "$ldap_password";

##Update APL Codex:

p4_edit "$adobe_root/adobe_platform_libraries/tools", "VersionInfo.xml";

run_perl_script "$adobe_root/adobe_platform_libraries/tools", 
    "apl_codex.pl", 
    "$asl_version";
run_perl_script "$adobe_root/adobe_platform_libraries/tools", 
    "post_build.pl", 
    "$asl_version",  
    "$ldap_username",
    "$ldap_password";


