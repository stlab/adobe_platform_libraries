#!/bin/bash

function usage {
    echo "$0 -- Adobe quick-and-dirty code cleaner"
    echo
    echo "Usage:"
    echo "    $0 [file|directory]"
    echo
    echo "Current Features:"
    echo "    - Automatic Perforce checkout (if you are logged in)"
    echo "    - Sets 'horizontal rules' (/***...***/) to 80 characters in width"
    echo "    - Warns of lines in source files longer than 80 characters"
    echo
    echo "Limitations:"
    echo "    - Only processes files ending in .c, .h, .cp, .hp, .cpp and .hpp"
}

function clean_horizontal_rules {
    local RESULT=`sed -e 's/^[/][*]*[/]$/\/******************************************************************************\//g' "$1"`

    echo "$RESULT" > "$1"
}

function check_for_long_lines {
    local RESULTS=`grep -n -e "^.\{81,\}$" $1`

    if [ "$RESULTS" != "" ]; then
        echo "$1 has lines over 80 characters wide:"
        echo "$RESULTS"
    fi
}

function discern_and_clean {
    if [ -d "$1" ]; then
        process_directory `dirname $1/.` # the dirname command eliminates a trailing slash (if present)
    elif [ -f "$1" ]; then
        process_file $1
    else
        echo "I do not know what the path \"$1\" points to - skipping."
    fi
}

function process_file {
    echo $1 | grep -i -e "^.\+[.][ch]p\{0,2\}$" > /dev/null 2> /dev/null

    if [ $? == 0 ]; then
        local OK_TO_MODIFY=1

        if [ $IS_P4_OK == 1 ]; then
            p4 edit "$1" 2>&1 | grep -q -i -e "not on client"

            OK_TO_MODIFY=$?
        fi

        if [ $OK_TO_MODIFY == 1 ]; then
            clean_horizontal_rules $1

            check_for_long_lines $1
        fi
    fi
}

function process_directory {
    for i in $( ls $1 ); do
        discern_and_clean "$1/$i"
    done
}

function detect_perforce {
    local WHICH_P4=`which p4`

    IS_P4_OK=0;

    printf "Perforce... "
    
    if [ "$WHICH_P4" == "" ]; then
        echo "not detected"
    else
        p4 login -s > /dev/null 2> /dev/null
        if [ $? == 0 ]; then
            echo "detected and logged in"
            IS_P4_OK=1;
        else
            echo "detected but not logged in; files will not be checked out"
        fi
    fi
}

if [ "$1" == "" ]; then
    usage
    exit 1
fi

NOW=`date`
echo "Beginning execution at $NOW"

detect_perforce

discern_and_clean $1

if [ $IS_P4_OK == 1 ]; then
    OPEN_FILE_SET=`p4 opened 2>&1`

    if [ "$OPEN_FILE_SET" != 'File(s) not opened on this client.' ]; then
        p4 revert -a //...
    fi

    OPEN_FILE_SET=`p4 opened 2>&1`

    if [ "$OPEN_FILE_SET" != 'File(s) not opened on this client.' ]; then
        echo Perforce-managed files still need to be checked in
    fi
fi

NOW=`date`

echo "Finished execution at $NOW"

exit 0
