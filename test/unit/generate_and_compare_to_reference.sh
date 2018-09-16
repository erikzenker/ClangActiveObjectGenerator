#!/usr/bin/env bash

# This script generates active objects from the ./interfaces folder
# into the ./activeObjects folder and compares them to the files
# in the ./activeObjectsReferences folder.



# Generate the active objects
for INTERFACE in `ls ./interfaces/`
do
    clang-active-object-generator ./interfaces/$INTERFACE ISimpleClass > ./activeObjects/$INTERFACE
done

# Compare active objects to references
for ACTIVE_OBJECT in `ls ./activeObjects/`
do
    diff --color -c ./activeObjects/$ACTIVE_OBJECT ./activeObjectReferences/$ACTIVE_OBJECT
    RETURN_CODE=$?
    if [ $RETURN_CODE = 1 ]; then exit 1; fi
done