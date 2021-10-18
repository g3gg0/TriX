/*
   Mini XML lib

   Document routines

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_document.c 3483 2009-04-01 12:53:42Z geggo $
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

MXML_DOCUMENT *mxml_document_new()
{
	MXML_DOCUMENT *doc =(MXML_DOCUMENT *) MXML_ALLOCATOR( sizeof( MXML_DOCUMENT ) );

	if ( !doc )
		return NULL;

	doc->iLine = 0;
	doc->name = NULL;
	doc->index = NULL;
	doc->node_count = 0;
	doc->root = NULL;

	mxml_document_setup( doc );

	return doc;
}

MXML_STATUS mxml_document_setup( MXML_DOCUMENT *doc )
{
	CHECK_AND_FREE ( doc->index);
	CHECK_AND_FREE ( doc->root);
   doc->iLine = 0;
   doc->name = NULL;
   doc->index = mxml_index_new();
   doc->node_count = 0;
   doc->root = mxml_node_new();

   if ( doc->root != NULL && doc->index != NULL ) {
      doc->root->type = MXML_TYPE_DOCUMENT;
      doc->status = MXML_STATUS_OK;
      doc->error = MXML_ERROR_NONE;
      return MXML_STATUS_OK;
   }

   doc->status = MXML_STATUS_ERROR;
   doc->error = MXML_ERROR_NOMEM;
   return MXML_STATUS_ERROR;
}

void mxml_document_destroy( MXML_DOCUMENT *doc )
{
	mxml_node_destroy ( doc->root );
	mxml_index_destroy( doc->index );
	MXML_DELETOR( doc );
}


MXML_STATUS mxml_document_find( MXML_ITERATOR *it, char *path )
{
   return MXML_STATUS_OK;
}
