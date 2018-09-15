#!/usr/bin/env bash

# This script generates active objects from the ./interfaces folder
# into the ./activeObjects folder and compares them to the files
# in the ./activeObjectsReferences folder.



# Generate the active objects
for INTERFACE in `ls ./interfaces/`
do
    ././../../clang-active-object.sh ./interfaces/$INTERFACE ISimpleClass > ./activeObjects/$INTERFACE
done

# Compare active objects to references
for ACTIVE_OBJECT in `ls ./activeObjects/`
do
    DIFF=`diff ./activeObjects/$ACTIVE_OBJECT ./activeObjectReferences/$ACTIVE_OBJECT`
    echo $DIFF
done