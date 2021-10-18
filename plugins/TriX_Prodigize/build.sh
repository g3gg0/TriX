#!/bin/bash

FILES="prodigize interface"
OUT="../trix_prodigize.txp"
DEFINES="TRIXPLUG_MODE"

OBJ="static/LOWCNTR.static static/LOWDEV.static static/LOWFLASH.static static/LOWMBUS.static static/UART.static "

for file in $FILES; do
    gcc -fPIC -g -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ+="${file}.o " 
done;

gcc -fPIC -shared -g -D_REENTRANT -g -o $OUT $OBJ
