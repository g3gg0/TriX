/*
   Mini XML lib

   Document routines

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_index.c 3483 2009-04-01 12:53:42Z geggo $
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

#include "mxml.h"

MXML_INDEX *mxml_index_new()
{
   MXML_INDEX *index = (MXML_INDEX *) MXML_ALLOCATOR( sizeof( MXML_INDEX ) );

   if ( index != NULL )
      mxml_index_setup( index );

   return index;
}

MXML_STATUS mxml_index_setup( MXML_INDEX *index )
{
   index->length = 0;
   index->allocated = 0;
   index->data = NULL;

   return MXML_STATUS_OK;
}

void mxml_index_destroy( MXML_INDEX *index )
{
   /* Posix free() can be used with NULL, but nothing is known for other
      free() provided by users */
   if ( index->data != NULL )
      MXML_DELETOR( index->data );

   MXML_DELETOR( index );
}
