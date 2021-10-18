/*
   Mini XML lib

   XML path oriented operations

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_path.c,v 1.4 2007/03/31 15:13:00 geggo Exp $
*/
#include <stdio.h> // for NULL
#include <stdlib.h> // for free
#include <string.h>	// for strlen

#include "mxml.h"

char *mxml_node_get_path_new( MXML_NODE* node )
{
   int pathlen = 0;
   char *path, *path1;
   MXML_NODE *parent = node;

   if ( node->type == MXML_TYPE_DOCUMENT )
      return NULL;

   while( parent != NULL && parent->name != NULL ) {
      pathlen += strlen( parent->name )+1;
      parent = parent->parent;
   }

   path = (char *) MXML_ALLOCATOR( pathlen + 1 );
   path1 = (char *) MXML_ALLOCATOR( pathlen + 1 );

   path[0] = 0;
   parent = node;

   while ( parent != NULL && parent->name != NULL ) {
      sprintf( path1 , "/%s%s", parent->name, path );
      strcpy( path, path1 );
      parent = parent->parent;
   }

   MXML_DELETOR( path1 );
   return path;
}

int mxml_node_get_path_depth( MXML_NODE* node )
{
   int depth = 0;
   MXML_NODE *parent = node;

   while( parent != NULL && parent->type != MXML_TYPE_DOCUMENT ) {
      depth++;
      parent = parent->parent;
   }

   return depth;
}

int mxml_node_get_path_length( MXML_NODE* node )
{
   int length = 0;
   MXML_NODE *parent = node;

   while( parent != NULL && parent->name != NULL) {
      length += strlen( parent->name )+1;
      parent = parent->parent;
   }

   return length;
}

int mxml_node_get_path( MXML_NODE* node, char *path )
{
   char *reverse_path[ MXML_MAX_DEPTH ];
   int pos = 0;
   int i;
   int plen = 0;
   MXML_NODE *parent = node;

   while( parent != NULL && parent->name != NULL) {
      reverse_path[pos++] = parent->name;
      parent = parent->parent;
   }

   for ( i = pos-1; i >= 0; i-- ) {
      sprintf( path + plen, "/%s", reverse_path[ i ] );
      plen += strlen( reverse_path[ i ] )+1;
   }

   path[ plen ] = 0;
   return plen;
}

