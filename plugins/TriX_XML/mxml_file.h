/*
   Mini XML lib
   Header files

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   File oriented operations on mxml libs. They are mainly shortcut,
   to be used in final program but not in other libraries.

   $Id: mxml_file.h,v 1.3 2007/03/31 15:13:00 geggo Exp $
*/

#ifndef MXML_FILE_H
#define MXML_FILE_H


/* Operations on the whole doc strucuture */
MXML_DOCUMENT *mxml_buffer ( char *buffer, int style );
MXML_DOCUMENT *mxml_read( char *filename, int style );
MXML_STATUS mxml_read_file( FILE *fp, MXML_DOCUMENT *doc, int style );

MXML_STATUS mxml_write( MXML_DOCUMENT *doc, char *filename, int style );
MXML_STATUS mxml_write_file( MXML_DOCUMENT *doc, FILE *fp, int style );

#endif
