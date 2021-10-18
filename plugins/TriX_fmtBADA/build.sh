#!/bin/bash

gcc -fPIC -shared -D_REENTRANT -g -O0 -o ../trix_fmtBADA.txp fmtBADA.c  -I../../include

