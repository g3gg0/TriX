TARGET = c2html
prefix = /usr/local
DESTDIR = $(prefix)/$(TARGET)
BINDIR = $(prefix)/bin
SOURCES =\
	main.c\
	convert.c\
	check_begin.c\
	check_end.c\
	error.c\
	print_info.c\
	write_header_footer.c

INSTALL_FILES=\
	'User manual.pdf'

CFLAGS=-I. -O3 -Wall
CC=gcc

RMDIR=rmdir

include makefile.default
