#!/bin/bash

ASL_DISTRO_NAME=asl_1.0.39.tgz
ASL_NAME=source_release
APL_DISTRO_NAME=apl_1.0.39.tgz
APL_NAME=platform_release
ASL_DISTRO_SITE=easynews.dl.sourceforge.net
BOOST_NAME=boost_1_44_0
BOOST_DISTRO_NAME="$BOOST_NAME".tar.gz
#-L protects against redirects
CURL_CMD=curl\ -L 
INTEL_TBB_NAME=tbb30_018oss
INTEL_TBB_DISTRO_NAME="$INTEL_TBB_NAME"_src.tgz

# Run a command, and echo before doing so. Also checks the exit
# status and quits if there was an error.
#
# WARNING : Does *not* work when the command uses the redirection operator (>)
#
echo_run ()
{
    echo "$@"
    "$@"
    r=$?
    if test $r -ne 0 ; then
        exit $r
    fi
}

test_path()
{
    hash $1 1>/dev/null 2>/dev/null
}

WD=`pwd`
MACHINE=`uname`
HERE=`dirname $0`

cd $HERE

if [ "$1" != "" ]; then
    ASL_DISTRO_SITE="$1"
fi

if [ ! -e $ASL_DISTRO_NAME ]; then
    echo_run ${CURL_CMD} http://$ASL_DISTRO_SITE/sourceforge/adobe-source/$ASL_DISTRO_NAME -o $ASL_DISTRO_NAME
fi

if [ ! -e $APL_DISTRO_NAME ]; then
    echo_run ${CURL_CMD} http://$ASL_DISTRO_SITE/sourceforge/adobe-source/$APL_DISTRO_NAME -o $APL_DISTRO_NAME
fi

if [ ! -e $BOOST_DISTRO_NAME ]; then
    echo_run ${CURL_CMD} http://easynews.dl.sourceforge.net/sourceforge/boost/$BOOST_DISTRO_NAME -o $BOOST_DISTRO_NAME
fi

if [ ! -e $INTEL_TBB_DISTRO_NAME ]; then
    echo_run ${CURL_CMD} http://www.threadingbuildingblocks.org/uploads/78/154/3.0/$INTEL_TBB_DISTRO_NAME -o $INTEL_TBB_DISTRO_NAME
fi

if [ ! -d adobe_root ]; then
    echo_run mkdir adobe_root
fi

if [ ! -d adobe_root/adobe_source_libraries ]; then
   echo_run tar -xzf $ASL_DISTRO_NAME
   echo_run mv $ASL_NAME adobe_root/adobe_source_libraries
fi

if [ ! -d adobe_root/adobe_platform_libraries ]; then
   echo_run tar -xzf $APL_DISTRO_NAME
   echo_run mv $APL_NAME adobe_root/adobe_platform_libraries
fi

if [ ! -e adobe_root/boost_libraries/INSTALL ]; then
    # move the boost distro into place
    echo_run tar -xzf $BOOST_DISTRO_NAME
    echo_run rm -rf adobe_root/boost_libraries
    echo_run mv $BOOST_NAME adobe_root/boost_libraries
fi

if [ ! -e adobe_root/intel_tbb_libraries/README ]; then
    # move the intel_tbb distro into place
    echo_run tar -xzf $INTEL_TBB_DISTRO_NAME
    echo_run rm -rf adobe_root/intel_tbb_libraries
    echo_run mv $INTEL_TBB_NAME adobe_root/intel_tbb_libraries
fi

cd $HERE
echo_run adobe_root/adobe_source_libraries/tools/patch_boost.sh

if [[ $MACHINE == "Darwin" ]]
then
    cd adobe_root/adobe_platform_libraries
else
    cd adobe_root/adobe_source_libraries
fi

echo_run ../adobe_source_libraries/tools/build.sh

echo Done!

exit 0
