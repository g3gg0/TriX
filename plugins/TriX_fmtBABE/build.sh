#!/bin/bash

gcc -fPIC -shared -D_REENTRANT -g -O0 -o ../trix_fmtBABE.txp fmtBABE.c  -I../../include

