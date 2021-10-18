#!/bin/bash

FILES="fat fate fattime"
OUT="../trix_fate.txp"
DEFINES="TRIXPLUG_MODE"

OBJ=""

for file in $FILES; do
    gcc -g -O0 -fPIC -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ+="${file}.o " 
done;

gcc -fPIC -shared -D_REENTRANT -g -O0 -o $OUT $OBJ
