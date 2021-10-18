#!/bin/bash

FILES="mxml_attribute mxml_document mxml_error mxml_file mxml_index mxml_iterator mxml_node mxml_output mxml_path mxml_refil mxml_sgs xml"
OUT="../trix_xml.txp"
DEFINES="TRIXPLUG_MODE"

OBJ=""

for file in $FILES; do
    gcc-3.4 -fPIC -c $file.c -o $file.o -I../../include -D $DEFINES
    OBJ="${OBJ}${file}.o " 
done;

gcc-3.4 -fPIC -shared -D_REENTRANT -g -o $OUT $OBJ
