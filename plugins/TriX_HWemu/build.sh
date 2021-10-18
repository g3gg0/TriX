#!/bin/bash

FILES="hw_accif hw_accpll hw_brain hw_cbusc hw_cbus_devices hw_ctsi hw_dspmif hw_dsppll hw_ebusc hwemu hw_flash hw_fsig hw_lead3mmi hw_mcupll hw_mfi hw_osmo hw_pup hw_simif hw_sti hw_testif hw_uif"
OUT="../trix_hwemu.txp"
DEFINES="TRIXPLUG_MODE"

OBJ=""

for file in $FILES; do
    gcc -fPIC -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ+="${file}.o " 
done;

gcc -fPIC -shared -D_REENTRANT -g -o $OUT $OBJ
