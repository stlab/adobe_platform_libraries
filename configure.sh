#!/bin/bash

# Run a command, and echo before doing so. Also checks the exit
# status and quits if there was an error.
echo_run ()
{
    echo "EXEC : $@"
    "$@"
    r=$?
    if test $r -ne 0 ; then
        exit $r
    fi
}

cd ..

if [ ! -e 'adobe_source_libraries' ]; then
    echo "INFO : adobe_source_libraries not found: setting up."

    echo_run git clone --depth=50 --branch=master git://github.com/stlab/adobe_source_libraries.git
else
    echo "INFO : adobe_source_libraries found: skipping setup."
fi

echo_run cd adobe_source_libraries

echo_run ./configure.sh

echo "INFO : You are ready to go!"
