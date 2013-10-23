#!/bin/bash

HERE=`dirname $0`

cd $HERE/../../

MODE="debug"

if [ "$1" != "" ]; then
    MODE="$1"
fi

if [ "$MODE" == "debug" ]; then
    BOOSTLIBEXT="-d"
else
    BOOSTLIBEXT=""
fi

LIBRARY_SET=(\
    third_party/boost_tp/boost/bin.v2/libs/filesystem/build/darwin/$MODE/arch-fat/link-static/threading-multi/libboost_filesystem-mt"$BOOSTLIBEXT"-1_34_1.a \
    third_party/boost_tp/boost/bin.v2/libs/signals/build/darwin/$MODE/arch-fat/link-static/threading-multi/libboost_signals-mt"$BOOSTLIBEXT"-1_34_1.a \
    third_party/boost_tp/boost/bin.v2/libs/thread/build/darwin/$MODE/arch-fat/link-static/threading-multi/libboost_thread-mt"$BOOSTLIBEXT"-1_34_1.a \
    bin.v2/adobe/build/darwin/$MODE/arch-fat/asl-dev/link-static/threading-multi/libasl_dev.a \
    bin.v2/adobe/future/widgets/darwin/$MODE/arch-fat/asl-dev/link-static/threading-multi/user-interface-gui/libadobe_widgets.a \
)

SET_COUNT=$((${#LIBRARY_SET[@]}))
COUNTER=0

while [ $COUNTER != $SET_COUNT ]; do

    CURRENT_LIB="${LIBRARY_SET[$COUNTER]}"

    if [ ! -e $CURRENT_LIB ]; then

        BASENAME=`basename $CURRENT_LIB`
        DIRNAME=`dirname $CURRENT_LIB`

        if [ `uname` == "Darwin" ]; then

            osascript -e 'tell application "Finder"' -e 'activate' -e 'beep' -e "display alert \"Cannot find required file:\\n\\n\\t$BASENAME\\n\\nfor the $MODE build.\\n\\nPlease check the read_me.txt file in the ASL distribution for information on how to build this library.\" message \"Looked in directory \\\"$DIRNAME\\\"\" buttons \"OK\" default button \"OK\" as warning" -e "end tell"

        else

            echo "Cannot find required library file \"$BASENAME\" for the $MODE build. Please check the read_me.txt file in the ASL distribution for information on how to build this library."

        fi

        exit 1

#   else

#       echo "Found \"`basename $CURRENT_LIB`\""

    fi

    let COUNTER=COUNTER+1

done

#echo "Found everything OK! You're good to go"

exit 0
