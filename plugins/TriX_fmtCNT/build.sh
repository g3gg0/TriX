#!/bin/bash

gcc -fPIC -shared -D_REENTRANT -g -O0 -o ../trix_fmtCNT.txp fmtCNT.c  -I../../include

