#!/bin/bash

gcc -fPIC -shared -D_REENTRANT -g -o ../trix_term.txp term.c  -I../../include
