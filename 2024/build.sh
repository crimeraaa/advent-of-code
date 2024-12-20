#! /usr/bin/bash

SCRIPT_DIR=$(dirname $(readlink --canonicalize $0))

# Does PART expand to a zero length string?
# Note that you can also call the form: PART=2 build.sh <args>
if [ -z "$PART" ]; then
    PART=1
fi

if [ -z "$1" ]; then
    echo "No Odin source directory specified."
    exit 1
fi

odin build "$SCRIPT_DIR/$1" -debug -define:PART=$PART -out:"$SCRIPT_DIR/bin/$1"
