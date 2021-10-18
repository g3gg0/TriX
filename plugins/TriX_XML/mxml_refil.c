/*
   Mini XML lib

   Refiller routines

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_refil.c,v 1.4 2007/03/31 15:13:00 geggo Exp $
*/


#include <stdio.h>
#include "mxml.h"

/**
* Creates a new refiller object.
* If buf is null, then buflen is ignored and set to 0; the first retrival
* of a character will then lead to refil func calling.
* If the function is null, once the data has been read the reader returns
* eof. If both func and buf are NULL, the creation fails, and the function
* retunrs NULL.
*/
MXML_REFIL *mxml_refil_new( MXML_REFIL_FUNC func, char *buf, int buflen,
   int bufsize )
{
   MXML_REFIL * ret = (MXML_REFIL* ) MXML_ALLOCATOR( sizeof( MXML_REFIL ) );

   if ( ret == NULL )
      return NULL;

   if ( mxml_refil_setup( ret, func, buf, buflen, bufsize ) == MXML_STATUS_OK )
      return ret;

   MXML_DELETOR( ret );
   return NULL;
}

/**
* Sets up refiller parameters.
* If buf is null, then buflen is ignored and set to 0; the first retrival
* of a character will then lead to refil func calling. Bufsize is the size
* of the allocated memory, while buflen is the count of currently valid
* characters in that buffer.
* If the function is null, once the data has been read the reader returns
* eof. If both func and buf are NULL, the function fails and returns
* MXML_STATUS_ERROR. On success, returns MXML_STATUS_OK.
* Notice: ref->data member is left to fill to the
* calling program, if this is needed.
*/

MXML_STATUS mxml_refil_setup( MXML_REFIL *ref, MXML_REFIL_FUNC func,
   char *buf, int buflen, int bufsize )
{

   if ( buf == NULL && func == NULL )
      return MXML_STATUS_ERROR;

   ref->refil_func = func;
   ref->buffer = buf;

   ref->status = MXML_STATUS_OK;
   ref->error = MXML_ERROR_NONE;

   if (buf == NULL)
      ref->buflen = ref->bufsize = 0;
   else {
      ref->buflen = buflen;
      ref->bufsize = bufsize;
   }

   ref->bufpos = 0;

   //stream length is left for the program to implement progress indicators
   ref->streamlen = 0;
   ref->streampos = 0;

   //theese are for ungetc operations
   ref->sparechar = MXML_EOF;

   //data is left to fill for the program
   return MXML_STATUS_OK;
}


void mxml_refil_destroy ( MXML_REFIL *ref ) {
   if ( ref != NULL )
   {
      MXML_DELETOR( ref );
   }
}

int mxml_refil_getc( MXML_REFIL *ref )
{
   if ( ref->sparechar != MXML_EOF ) {
      int chr = ref->sparechar;
      ref->sparechar = MXML_EOF;
      return chr;
   }

   if ( ref->bufpos >= ref->buflen ) {
      if ( ref->refil_func != NULL ) {
         ref->refil_func( ref );
         if ( ref->status != MXML_STATUS_OK || ref->buflen == 0)
            return MXML_EOF;
      }
      else
         return MXML_EOF;
   }

   return ref->buffer[ ref->bufpos++ ];
}


/* implemented as a macro
void mxml_refil_ungetc( MXML_REFIL *ref, int chr )
{
   ref->sparechar = chr;
}
*/

/**
* Useful "fill" function that reads from a buffer
*/

void mxml_refill_from_buffer_func( MXML_REFIL *ref )
{
	t_buf_refill *info = (t_buf_refill *) ref->data;
	int len;

	if ( info->length >= info->pos + ref->bufsize )
		len = ref->bufsize;
	else
		len = info->length - info->pos;

	memcpy ( ref->buffer, &(info->buffer[info->pos]), len );

	info->pos += len;

	ref->buflen = len;
	ref->bufpos = 0;

}

/**
* Useful "fill" function that reads from a stream
*/

void mxml_refill_from_stream_func( MXML_REFIL *ref )
{
   FILE *fp = (FILE *) ref->data;
   int len;

   len = fread( ref->buffer, 1, ref->bufsize, fp );

   if ( ferror( fp ) ) {
      ref->status = MXML_STATUS_ERROR;
      ref->error = MXML_ERROR_IO;
   }
   else {
      ref->buflen = len;
      ref->bufpos = 0;
   }
}

/**
* Useful "fill" function that reads from a file handle
*/

void mxml_refill_from_handle_func( MXML_REFIL *ref )
{
   int fh = (int) ref->data;
   int len;

   len = read( fh, ref->buffer, ref->bufsize );

   if ( len == -1 ) {
      ref->status = MXML_STATUS_ERROR;
      ref->error = MXML_ERROR_IO;
   }
   else {
      ref->buflen = len;
      ref->bufpos = 0;
   }
}
