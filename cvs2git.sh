#!/bin/bash

echo "Processing directory $1"

FILES=`cat $1/CVS/Entries | grep "^/" | cut -d / -f 2`
DIRS=`cat $1/CVS/Entries | grep "^D/" | cut -d / -f 2`

for FILE in $FILES; do
    echo "  Adding $FILE";
    git add $1/$FILE;
done

for DIR in $DIRS; do
    echo "  Processing $DIR";
    ./cvs2git.sh $1/$DIR;
done
