#!/bin/bash

FILES="armulate emu"
OUT="../trix_armulate.txp"
DEFINES="TRIXPLUG_MODE"

OBJ=""

rm $OUT

for file in $FILES; do
    rm ${file}.o
    gcc -ggdb -fPIC -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ+="${file}.o " 
done;

gcc -ggdb -fPIC -shared -D_REENTRANT -g -o $OUT $OBJ
