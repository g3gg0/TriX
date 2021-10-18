/*
   Mini XML lib

   Traslation from error code to error description 

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_error.c,v 1.3 2007/03/31 15:13:00 geggo Exp $
*/

#include <stdio.h>
#include "mxml.h"

static char *edesc[] =
{
   "Input/output error",
   "Not enough memory",
   "Character outside tags",
   "Invalid character as tag name",
   "Invalid character as attribute name",
   "Malformed attribute definition",
   "Invalid character",
   "Name of tag too long",
   "Name of attribute too long",
   "Value of attribute too long",
   "Unbalanced tag opening",
   "Unbalanced entity opening",
   "Escape/entity '&;' found"
};

char *mxml_error_desc( MXML_ERROR_CODE code )
{
   code --;
   if ( code < 0 || code > sizeof( edesc ) / sizeof( char * ) )
      return NULL;

   return edesc[ code ];
}
