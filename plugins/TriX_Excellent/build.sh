#!/bin/bash

FILES="excellent"
OUT="../trix_excellent.txp"
DEFINES="TRIXPLUG_MODE"

OBJ=""

for file in $FILES; do
    gcc -fPIC -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ+="${file}.o " 
done;

gcc -fPIC -shared -D_REENTRANT -g -o $OUT $OBJ
