#!/bin/bash

FILES=""
OUT="../trix_UFS.txp"
DEFINES="TRIXPLUG_MODE"

OBJ=""

for file in $FILES; do
    gcc -fPIC -g -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ+="${file}.o " 
done;

gcc -fPIC -shared -g -D_REENTRANT -g -o $OUT $OBJ
