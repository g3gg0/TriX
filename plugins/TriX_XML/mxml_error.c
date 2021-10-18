/*
   Mini XML lib

   Traslation from error code to error description 

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_error.c 3483 2009-04-01 12:53:42Z geggo $
*/

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"
#include "mem.h"
#include "treenode.h"
#include "trixplug.h"
extern struct trix_functions *ft;
#include "trixplug_wrapper.h"

//#include <stdio.h>
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
