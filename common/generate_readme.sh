#!/bin/sh
#
# Constuct a README.md from the source code and output
#
TEMPLATE=README.md.template
NAME=$(basename $PWD)
cat $TEMPLATE | sed '/NOTE-BEGIN/,/NOTE-READ-CODE/!d' | grep -v "^NOTE-"

#
# Add the source wit DOC changed to a C++ comment
#
cat main.cpp | \
    grep -v "define DOC" | \
    sed -e 's/DOC(\"\(.*\)\"); *$/\/\/ \1/g' \
        -e 's/OK(\"SUCCESS: \(.*\)\"); *$/\/\/ \1/g' \
        -e 's/ERR(\"ERROR: \(.*\)\"); *$/\/\/ \1/g'
cat $TEMPLATE | sed '/NOTE-READ-CODE/,/NOTE-BUILD-CODE/!d' | grep -v "^NOTE-"

#
# Now add the build output
#
echo "cd $NAME"
make clean all

#
# Now add the executable output
#
echo "./example"
cat $TEMPLATE | sed '/NOTE-BUILD-CODE/,/NOTE-RUN-CODE/!d' | grep -v "^NOTE-"

#
# remove ansi colors in the output and convert to bold for gitub markdown
#
# use $'...' when you want escape sequences to be interpreted by the shell.
# as bsd sed cannot grok \x01b
#
./example | sed -e $'s,[\x01-\x1F\x7F]\[[0-9;]*[a-zA-Z],# ,g' \
                -e 's/# *$//g'

#
# Remove the markers
#
cat $TEMPLATE | sed '/NOTE-RUN-CODE/,/NOTE-END/!d' | grep -v "^NOTE-"
