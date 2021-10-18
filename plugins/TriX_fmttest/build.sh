#!/bin/bash

gcc -fPIC -shared -D_REENTRANT -g -o ../trix_fmttest.txp fmttest.c  -I../../include
