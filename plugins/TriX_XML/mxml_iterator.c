/*
   Mini XML lib

   Iterators

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_iterator.c 3483 2009-04-01 12:53:42Z geggo $
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

MXML_ITERATOR *mxml_iterator_new( MXML_DOCUMENT *doc )
{
   MXML_ITERATOR *it = ( MXML_ITERATOR *) MXML_ALLOCATOR( sizeof( MXML_ITERATOR ) );

   if ( it != NULL )
      mxml_iterator_setup( it, doc );

   return it;
}

MXML_STATUS mxml_iterator_setup( MXML_ITERATOR *it, MXML_DOCUMENT *doc )
{
   it->document = doc;
   it->mode = MXML_ITERATOR_MODE_FORWARD;
   it->root_node = doc->root;

   mxml_iterator_top( it );

   return MXML_STATUS_OK;
}

void mxml_iterator_destroy( MXML_ITERATOR *it )
{
   if ( it != NULL )
   {
      MXML_DELETOR( it );
   }
}

MXML_NODE *mxml_iterator_top( MXML_ITERATOR *it )
{
   it->node = it->root_node;
   it->level = 0;
   return it->node;
}

void mxml_iterator_set_top( MXML_ITERATOR *it, MXML_NODE *node )
{
   it->root_node = node;
   mxml_iterator_top( it );
}

void mxml_iterator_copy( MXML_ITERATOR *dest, MXML_ITERATOR *src )
{
   dest->document = src->document;
   dest->root_node = src->root_node;
   dest->mode = src->mode;
   dest->node = src->node;
   dest->level = src->level;
}

MXML_ITERATOR *mxml_iterator_clone( MXML_ITERATOR *src )
{
   MXML_ITERATOR *it = ( MXML_ITERATOR *) MXML_ALLOCATOR( sizeof( MXML_ITERATOR ) );

   if ( it != NULL )
      mxml_iterator_copy( it, src );

   return it;
}


/**
* Finds the next node in the hyerarcy, scanning all the childrens, then
* all the brothers, and then climbing back to the parent's brothers up
* to esausting all the nodes.
*/

MXML_NODE *mxml_iterator_next( MXML_ITERATOR *it )
{
   MXML_NODE *node = NULL;
   int level = it->level;

   if ( it->node->child != NULL ) {
      node = it->node->child;
      level++;
   }
   else if ( it->node->next != NULL ) {
      node = it->node->next;
   }
   else {
      node = it->node;
      while ( node->parent != NULL ) {
         level--;
         node = node->parent;
         if ( node->next != NULL )
            break;
      }
      node = node->next; // can be NULL
   }

   // have we found a different node to go?
   if ( node != NULL ) {
      it->node = node;
      it->level = level;
      return node;
   }

   // we are done
   return NULL;
}


MXML_NODE *mxml_iterator_next_brother( MXML_ITERATOR *it )
{
   if ( it->node->next != NULL ) {
      it->node = it->node->next;
      return it->node;
   }
   return NULL;
}

MXML_NODE *mxml_iterator_previous_brother( MXML_ITERATOR *it )
{
   if ( it->node->prev != NULL ) {
      it->node = it->node->prev;
      return it->node;
   }
   return NULL;
}

MXML_NODE *mxml_iterator_descend( MXML_ITERATOR *it )
{
   if ( it->node->child != NULL ) {
      it->node = it->node->child;
      it->level++;
      return it->node;
   }
   return NULL;
}

MXML_NODE *mxml_iterator_ascend( MXML_ITERATOR *it )
{
   if ( it->node->parent != NULL ) {
      it->node = it->node->parent;
      it->level--;
      return it->node;
   }
   return NULL;
}

void mxml_iterator_insert_after( MXML_ITERATOR *it, MXML_NODE *node  )
{
   if ( it->node == NULL ) {
      mxml_iterator_add_below( it, node );
   }
   else {
      mxml_node_insert_before( it->node, node );
   }
}

void mxml_iterator_insert_before( MXML_ITERATOR *it, MXML_NODE *node )
{
   if ( it->node == NULL ) {
      mxml_iterator_add_below( it, node );
   }
   else {
      mxml_node_insert_after( it->node, node );
   }
}

/**
* Insert a new node between the iterator pointer and all its children
*/

void mxml_iterator_insert_below( MXML_ITERATOR *it, MXML_NODE *node )
{
   mxml_node_insert_below( it->node , node );
}

/**
* Add a new node (or tree) after the last child of the iterator pointer
* This is the only function capable to add a new node to an empty tree.
*/

void mxml_iterator_add_below( MXML_ITERATOR *it, MXML_NODE *node )
{
   if ( it->node == NULL )
   {
      it->document->root = it->node = node;
      return;
   }

   mxml_node_add_below( it->node, node );
}

/**
* Search for a node with the given name in the rest of the document
* Currently it does not uses indexed search.
*/

MXML_NODE *mxml_iterator_scan_node( MXML_ITERATOR *it, char *name )
{
   MXML_NODE *node = it->node;

   while( node != NULL ) {
      if ( node->name != NULL) {
         if ( strcmp( node->name, name ) == 0 )
            break;
      }
      node = mxml_iterator_next( it );
   }
   /* Notice: it is modified */
   return node;
}

/**
* Search for a node with the given attribute.
* Currently it does not uses indexed search.
*/
MXML_NODE *mxml_iterator_scan_attribute( MXML_ITERATOR *it, char *attrib )
{
   MXML_NODE *node = it->node;

   while( node != NULL ) {
      MXML_ATTRIBUTE *attr = node->attributes;
      while ( attr != NULL) {
         if ( strcmp( attr->name, attrib ) == 0 )
            return node;
         attr = attr->next;
      }
      node = mxml_iterator_next( it );
   }
   /* Notice: it is modified */
   return node;
}



/**
* Search for a node with the given attribute and the given value.
* Currently it does not uses indexed search.
*/
MXML_NODE *mxml_iterator_scan_attribute_value( MXML_ITERATOR *it, char *attrib, char *val )
{
   MXML_NODE *node = it->node;

   while( node != NULL ) {
      MXML_ATTRIBUTE *attr = node->attributes;
      while ( attr != NULL) {
         if ( strcmp( attr->name, attrib ) == 0 &&
            attr->value != NULL &&
            strcmp( attr->value, val ) == 0 )
                  return node;
         attr = attr->next;
      }

      node = mxml_iterator_next( it );
   }
   /* Notice: it is modified */
   return node;
}
