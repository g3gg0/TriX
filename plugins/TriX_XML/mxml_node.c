/*
   Mini XML lib

   Item (node) routines

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   $Id: mxml_node.c 3608 2009-09-05 21:59:31Z kubica.bartek $
*/

//#include <stdio.h>
//#include <ctype.h>

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

// just a shortcut
#define MALFORMED_ERROR( elem, val ) \
   elem->status = MXML_STATUS_MALFORMED;\
   elem->error = val;\
   iStatus = -1;

MXML_NODE *mxml_node_new()
{
   MXML_NODE *node = (MXML_NODE *) MXML_ALLOCATOR( sizeof( MXML_NODE ) );

   if ( node != NULL )
      mxml_node_setup( node );

   return node;
}

MXML_STATUS mxml_node_setup( MXML_NODE *node )
{
   node->name = NULL;
   node->data = NULL;
   node->data_length = 0;
   node->attributes = NULL;
   node->type = MXML_TYPE_TAG;

   node->parent = NULL;
   node->next = NULL;
   node->prev = NULL;
   node->child = NULL;
   node->last_child = NULL;

   return MXML_STATUS_OK;
}

/**
* The unlink function is used to detach a node from the UPPER and PARENT hierarchy.
* The node is "removed" so that siblings node are "squished", and possible parents
* are informed of the changes, so that they are able to get a new child to start
* the tree structure under them. The childs of the unlinked nodes are NOT unlinked,
* thus remains attached to the node: is like removing a branch with all its leaves.
*
*/

void mxml_node_unlink( MXML_NODE *node )
{
   if ( node->prev != NULL )
      node->prev->next = node->next;

   if ( node->next != NULL )
      node->next->prev = node->prev;

   if ( node->parent != NULL && node->parent->child == node )
      node->parent->child = node->next;

   node->parent = NULL;
   node->next = NULL;
   node->prev = NULL;
}
/****************************************************************/

void mxml_node_insert_before( MXML_NODE *tg, MXML_NODE *node )
{
   node->next = tg;
   node->prev = tg->prev;
   node->parent = tg->parent;
   /* puts the node on top of hierarchy if needed */
   if ( tg->parent != NULL && tg->parent->child == tg )
      tg->parent->child = node;

   tg->prev = node;
}

void mxml_node_insert_after( MXML_NODE *tg, MXML_NODE *node )
{
   node->next = tg->next;
   node->prev = tg;
   node->parent = tg->parent;
   /* puts the node on bottom of hierarchy if needed */
   if ( tg->parent != NULL && tg->parent->last_child == tg )
      tg->parent->last_child = node;

   tg->next = node;
}

/**
* Creates a new tree level, so that the given node is added between
* tg and its former children.
*/

void mxml_node_insert_below( MXML_NODE *tg, MXML_NODE *node )
{
   MXML_NODE *child;

   node->parent = tg;
   node->last_child = tg->last_child;
   child = node->child = tg->child;

   while ( child != NULL ) {
      child->parent = node;
      child = child->next;
   }

   tg->last_child = tg->child = node;
}

/**
* Adds a node to the bottom of the children list of tg.
*/

void mxml_node_add_below( MXML_NODE *tg, MXML_NODE *node )
{
   MXML_NODE * child;

   node->parent = tg;
   child = tg->last_child;

   if ( child != NULL ) {
      child->next = node;
      node->prev = child;
      tg->last_child = node;
   }
   else {
      tg->last_child = tg->child = node;
   }
}

/**
* Clones a node, but it does not set the parent, nor the siblings;
* this clone is "floating" out of the tree hierarchy.
*/

MXML_NODE *mxml_node_clone( MXML_NODE *tg )
{
   MXML_NODE *clone = mxml_node_new();
   clone->type = tg->type;

   if ( tg->name != NULL ) {
      int nlen = strlen( tg->name ) + 1;
      clone->name = ( char *) MXML_ALLOCATOR( nlen );
      memcpy( clone->name, tg->name, nlen );
   }

   if ( tg->data != NULL ) {
      int nlen = tg->data_length > 0 ? tg->data_length +1 : strlen( tg->data ) + 1;
      clone->data = ( char *) MXML_ALLOCATOR( nlen );
      memcpy( clone->data, tg->data, nlen );
      clone->data_length = tg->data_length;
   }

   clone->attributes = mxml_attribute_clone( tg->attributes );

   return clone;
}


/**
* Clones a node and all its subtree, but it does not set the parent, nor the siblings;
* this clone is "floating" out of the tree hierarchy.
*/

MXML_NODE *mxml_node_clone_tree( MXML_NODE *tg )
{
   MXML_NODE *clone = mxml_node_clone( tg );
   MXML_NODE *node = tg->child;

   if ( node != NULL ) {
      clone->child = mxml_node_clone_tree( node );
      clone->child->parent = clone;
      clone->last_child = clone->child;
      node = node->next;

      while ( node != NULL ) {
         clone->last_child->next = mxml_node_clone_tree( node );
         clone->last_child->next->parent = clone->last_child->parent;
         clone->last_child = clone->last_child->next;
         node = node->next;
      }
   }

   return clone;
}

/**
* Destroy a node, including all its children and brothers in the next branch.
* To destroy a subtree, unlink the root node of the subtree and then destroy it.
* To destroy a single node, unlink it and then move its children elsewhere;
* then destroy the node.
*/

void mxml_node_destroy( MXML_NODE *node )
{
   if ( node->name != NULL )
      MXML_DELETOR( node->name );

   if ( node->data != NULL )
      MXML_DELETOR( node->data );

   if ( node->attributes != NULL )
      mxml_attribute_destroy( node->attributes );

   if ( node->next != NULL )
      mxml_node_destroy( node->next );

   if ( node->child != NULL )
      mxml_node_destroy( node->child );

   MXML_DELETOR( node );
}

/**
* Scans a node for given attribute
*/
char *mxml_node_value_of( MXML_NODE *pNode, char *attrib )
{
   MXML_ATTRIBUTE *attr = pNode->attributes;
   char *ret = NULL;

   while( attr != NULL ) {
      if ( strcmp( attrib, attr->name) == 0 ) {
         ret = attr->value;
         break;
      }
      attr = attr->next;
   }

   return ret;
}

/**
* Scans a node for given attribute, returning a default value if
* the attribute can't be found.
*/
char *mxml_node_value_of_default( MXML_NODE *pNode, char *attrib, char *val_default )
{
   MXML_ATTRIBUTE *attr = pNode->attributes;
   char *ret = val_default;

   while( attr != NULL ) {
      if ( strcmp( attrib, attr->name) == 0 ) {
         ret = attr->value;
         break;
      }
      attr = attr->next;
   }

   return ret;
}


/**
  Reads a data node
*/
static void mxml_node_read_data( MXML_REFIL *ref, MXML_NODE *pNode,
      MXML_DOCUMENT *doc, int iStyle )
{
   char *buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
   char *bufc;
   int iAllocated = MXML_ALLOC_BLOCK;
   int iPos = 0;
   int chr;
   int iStatus = 0, iPosAmper;

   chr = mxml_refil_getc( ref );
   while ( chr != MXML_EOF ) {

      // still in a data element
      if ( chr != '<' ) {

         // verify entity or escape
         if ( chr == '&' && ! (iStyle & MXML_STYLE_NOESCAPE) ) {

            if ( iStatus == 0 ) {
               iStatus = 1;
               iPosAmper = iPos;
            }
            else {
               //error - we have something like &amp &amp
               doc->status = MXML_STATUS_MALFORMED;
               doc->error = MXML_ERROR_UNCLOSEDENTITY;
               return;
            }
         }

         // rightful closing of an entity
         if ( chr == ';' && iStatus == 1 ) {
            int iAmpLen = iPos - iPosAmper - 2;
            char *bp = buf + iPosAmper + 1;

            if ( iAmpLen <= 0 ) {
               //error! - we have "&;"
               doc->status = MXML_STATUS_MALFORMED;
               doc->error = MXML_ERROR_WRONGENTITY;
               return;
            }

            iStatus = 0;

            if ( strncmp( bp, "amp", iAmpLen ) == 0 ) chr = '&';
            else if ( strncmp( bp, "lt", iAmpLen ) == 0 ) chr = '<';
            else if ( strncmp( bp, "gt", iAmpLen ) == 0 ) chr = '>';
            else if ( strncmp( bp, "quot", iAmpLen ) == 0 ) chr = '"';
            else if ( strncmp( bp, "apos", iAmpLen ) == 0 ) chr = '\'';

            // if yes, we must put it at the place of the amper, and restart
            // from there
            if ( chr != ';' ) iPos = iPosAmper;
         }

         buf[ iPos++ ] = chr;
         if ( iPos >= iAllocated ) {
            iAllocated += MXML_ALLOC_BLOCK;
            buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
         }

         if ( chr == MXML_LINE_TERMINATOR )
            doc->iLine++;

      }
      else {
         mxml_refil_ungetc( ref, chr );
         break;
      }

      chr = mxml_refil_getc( ref );
   }

   // see if we had an entity open: <item> &amp hello</item> is an error
   if ( iStatus != 0 ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_UNCLOSEDENTITY;
      return;
   }

   if ( ref->status != MXML_STATUS_OK) {
      doc->status = ref->status;
      doc->error = ref->error;
      return;
   }

   // trimming unneded spaces
//   while ( iPos >1 && buf[iPos-1] == ' ' || buf[iPos-1] == '\t' )
//      iPos--;
   buf[ iPos ] = 0;
   pNode->type = MXML_TYPE_DATA;
   pNode->data = buf;
   pNode->data_length = iPos;

}

static MXML_STATUS mxml_node_read_name( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   char buf[ MXML_MAX_NAME_LEN + 1];
   int iPos = 0;
   int chr;
   int iStatus = 0;

   chr = 1;
   while ( iStatus < 2 && iPos < MXML_MAX_NAME_LEN ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         case 0:
            if ( isalpha( chr ) ) {
               buf[ iPos++ ] = chr;
               iStatus = 1;
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
               return MXML_STATUS_MALFORMED;
            }
         break;

         case 1:
            if ( isalnum( chr ) || chr == '_' || chr == '-' || chr == ':' ) {
               buf[ iPos++ ] = chr;
            }
            else if ( chr == '>' || chr == ' ' || chr == '/' || chr == '\r'
                  || chr == '\t' || chr == '\n' ) {
               mxml_refil_ungetc( ref, chr );
               iStatus = 2;
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
               return MXML_STATUS_MALFORMED;
            }
         break;
      }
  }

   if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
      return doc->status;
   }

   if ( iStatus != 2  ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_NAMETOOLONG;
      return MXML_STATUS_ERROR;
   }

   pNode->name = ( char *) MXML_ALLOCATOR( iPos + 1);
   memcpy( pNode->name, buf, iPos );
   pNode->name[ iPos ] = 0;

   return MXML_STATUS_OK;
}

static MXML_STATUS mxml_node_read_attributes( MXML_REFIL *ref, MXML_NODE *pNode,
         MXML_DOCUMENT *doc, int style )
{

   MXML_ATTRIBUTE *head, *tail;

   head = tail = mxml_attribute_read( ref, doc, style );

   while ( doc->status == MXML_STATUS_OK && tail != NULL ) {
      tail->next = mxml_attribute_read( ref, doc, style );
      tail = tail->next;
   }

   if ( ref->status == MXML_STATUS_OK ) {
      pNode->attributes = head;
   }
   else {
      doc->status = ref->status;
      doc->error = ref->error;
   }

   return doc->status;
}

static void mxml_node_read_directive( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   char *buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
   int iAllocated = MXML_ALLOC_BLOCK;
   int iPos = 0;
   int chr;

   pNode->type = MXML_TYPE_DIRECTIVE;
   if ( mxml_node_read_name( ref, pNode, doc ) == MXML_STATUS_OK ) {
      chr = mxml_refil_getc( ref );
      while ( chr != MXML_EOF && chr != '>') {
         if ( iPos > 0 || ( chr != ' ' && chr != '\t' && chr != '\r' && chr != '\n' ) )
            buf[ iPos++ ] = chr;

         if ( iPos >= iAllocated ) {
            iAllocated += MXML_ALLOC_BLOCK;
            buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
         }

         if ( chr == MXML_LINE_TERMINATOR )
            doc->iLine++;

         chr = mxml_refil_getc( ref );
      }

      if ( ref->status == MXML_STATUS_OK ) {
         buf[ iPos ] = 0;
         pNode->data = buf;
         pNode->data_length = iPos;
      }
      else {
         doc->status = ref->status;
         doc->error = ref->error;
      }

   }
}

static void mxml_node_read_pi( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   int iPos = 0, iAllocated;
   int chr;
   char *buf;
   int iStatus = 0;

   pNode->type = MXML_TYPE_PI;
   // let's read the xml PI instruction
   if ( mxml_node_read_name( ref, pNode, doc ) != MXML_STATUS_OK )
      return;

   // and then we'll put all the "data" into the data member, up to ?>

   buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
   iAllocated = MXML_ALLOC_BLOCK ;
   chr = 1;
   while ( iStatus < 2 ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         // scanning for ?>
         case 0:
            if ( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine++;
               buf[ iPos ++ ] = chr;
            }
            else if ( chr == '?' )
               iStatus = 1;
            else {
               if ( iPos > 0 || ( chr != ' ' && chr != '\n' ) )
		       // hm, MXML_LINE_TERMINATOR ?
                  buf[ iPos++ ] = chr;
            }
         break;

         case 1:
            if ( chr == '>' )
               iStatus = 2;
            else {
               iStatus = 0;
               buf[ iPos++ ] = '?';
               mxml_refil_ungetc( ref, chr );
            }
         break;

      }

      if ( iPos == iAllocated ) {
         iAllocated += MXML_ALLOC_BLOCK;
         buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
      }
   }

   if ( ref->status == MXML_STATUS_OK ) {
      buf[iPos] = 0;
      pNode->data = buf;
      pNode->data_length = iPos;
   }
   else {
      doc->status = ref->status;
      doc->error = ref->error;
   }
}

static void mxml_node_read_tag( MXML_REFIL *ref, MXML_NODE *pNode,
      MXML_DOCUMENT *doc, int style )
{
   char chr;

   pNode->type = MXML_TYPE_TAG;

   if ( mxml_node_read_name( ref, pNode, doc ) == MXML_STATUS_OK ) {
      mxml_node_read_attributes( ref, pNode, doc, style );
   }

   // if the list of attributes terminates with a '/', the last '>' is
   // left unread. This means the current node is complete.
   chr = mxml_refil_getc( ref );
   if ( ref->status == MXML_STATUS_OK && chr != '>' ) {
      mxml_refil_ungetc( ref, chr );
      // recurse
      mxml_node_read( ref, pNode, doc, style );
   }
   else if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
   }

   //else the node is complete
}

static void mxml_node_read_comment( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   int iPos = 0, iAllocated;
   int chr;
   char *buf;
   int iStatus = 0;

   pNode->type = MXML_TYPE_COMMENT;
   //  we'll put all the comment into the data member, up to ->

   chr = 1;
   buf = (char *) MXML_ALLOCATOR( MXML_ALLOC_BLOCK );
	// flo buf == NULL ?
   iAllocated = MXML_ALLOC_BLOCK;

   while ( iStatus < 3 ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      switch ( iStatus ) {
         // scanning for ->
         case 0:
            if ( chr == MXML_LINE_TERMINATOR ) {
               doc->iLine++;
               buf[ iPos ++ ] = chr;
            }
            else if ( chr == '-' )
               iStatus = 1;
            else
               buf[ iPos++ ] = chr;
         break;

         case 1:
            if ( chr == '-' )
               iStatus = 2;
            else {
               iStatus = 0;
               buf[ iPos++ ] = '-';
               mxml_refil_ungetc( ref, chr );
            }
         break;

         case 2:
            if ( chr == '>' )
               iStatus = 3;
            else {
               iStatus = 0;
               buf[ iPos++ ] = '-';
               mxml_refil_ungetc( ref, chr );
            }
         break;

      }

      if ( iPos == iAllocated ) {
         iAllocated += MXML_ALLOC_BLOCK;
         buf = (char *) MXML_REALLOCATOR( buf, iAllocated );
      }
   }

   if ( ref->status == MXML_STATUS_OK ) {
      buf[ iPos ] = 0;
      pNode->data = buf;
      pNode->data_length = iPos;
   }
   else {
      doc->status = ref->status;
      doc->error = ref->error;
   }
}

// checking closing tag
static void mxml_node_read_closing( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc )
{
   char buf[ MXML_MAX_NAME_LEN + 1];
   int iPos = 0;
   int chr;
   int iStatus = 0;

   chr = 1;
   while ( iPos <= MXML_MAX_NAME_LEN ) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF || chr == '>') break;

      buf[ iPos++ ] = chr;
   }

   if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
      return;
   }

   if ( iPos > MXML_MAX_NAME_LEN ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_NAMETOOLONG;
   }

   buf[ iPos ] = 0;
   if ( chr != '>' || !pNode->name || strcmp( pNode->name, buf ) != 0 ) {
      doc->status = MXML_STATUS_MALFORMED;
      doc->error = MXML_ERROR_UNCLOSED;
   }
   // all fine
}

MXML_STATUS mxml_node_read( MXML_REFIL *ref, MXML_NODE *pNode, MXML_DOCUMENT *doc, int iStyle )
{
   int nLen = 0;
   MXML_NODE *node, *child_node, *data_node;
   int chr;
   /* Stateful machine status */
   int iStatus = 0;

   chr = 1;
   while ( iStatus >= 0) {
      chr = mxml_refil_getc( ref );
      if ( chr == MXML_EOF ) break;

      // resetting new node foundings
      node = NULL;

      switch ( iStatus ) {

         case 0:  // outside nodes
            switch ( chr ) {
               case MXML_LINE_TERMINATOR: doc->iLine++; break;
               // We repeat line terminator here for portability
               case MXML_SOFT_LINE_TERMINATOR: break;
               //case ' ': case '\t': break;
               case '<': iStatus = 1; break;
               default:  // it is a data node
                  mxml_refil_ungetc( ref, chr );
                  node = mxml_node_new();
                  mxml_node_read_data( ref, node, doc, iStyle );
            }
         break;

         case 1: //inside a node, first character
            if ( chr == '/' ) {
               // This can be met only inside current tag
               iStatus = -1; // done
            }
            else if ( chr == '!' ) {
               iStatus = 2;
            }
            else if ( chr == '?' ) {
               node = mxml_node_new();
               mxml_node_read_pi( ref, node, doc );
            }
            else if ( isalpha( chr ) ) {
               mxml_refil_ungetc( ref, chr );
               node = mxml_node_new();
               mxml_node_read_tag( ref, node, doc, iStatus );
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
            }
         break;

         case 2: //inside a possible comment (<!-/<!?)
            if ( chr == '-') {
               iStatus = 3;
            }
            else if ( isalpha( chr ) ) {
               node = mxml_node_new();
               mxml_refil_ungetc( ref, chr );
               mxml_node_read_directive( ref, node, doc );
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
            }
         break;

         case 3:
            if ( chr == '-') {
               node = mxml_node_new();
               mxml_node_read_comment( ref, node, doc );
            }
            else {
               MALFORMED_ERROR( doc, MXML_ERROR_INVNODE );
            }
         break;
      }

      // have I to add a node below our structure ?
      if ( node != NULL ) {
         if ( ref->status == MXML_STATUS_OK ) {
            mxml_node_add_below( pNode, node );
            // beginning again - a new node is born
            doc->node_count++;
            iStatus = 0;
         }
         else {
            doc->status = ref->status;
            doc->error = ref->error;
            mxml_node_destroy( node );
            return doc->status;
         }
      }

   }

   // if we have an hard error on stream
   if ( ref->status != MXML_STATUS_OK ) {
      doc->status = ref->status;
      doc->error = ref->error;
      return doc->status;
   }

   if ( iStatus == -1 ) { // ARE WE DONE?
      /* Time to close current node. We must verify:
         1) If the closing tag is coherent with the opened tag name.
         2) If the tag has just one data node as child.
         if we have only one data node among the children, the data
         node is destroyed and the data element is moved to the
         "data" field of current node, to simplify the tree structure
         in the most common config oriented XML files.
      */
      mxml_node_read_closing( ref, pNode, doc );
      // malformed closing tag?
      if ( ref->status != MXML_STATUS_OK ) {
         doc->status = ref->status;
         doc->error = ref->error;
         return doc->status;
      }

      //checking for data nodes
      child_node = pNode->child;
      data_node = NULL;
      while ( child_node != NULL ) {
         if ( child_node->type == MXML_TYPE_DATA ) {
            // first data node ?
            if ( data_node == NULL )
               data_node = child_node;
            // ... or have we more than a data node?
            else {
               data_node = NULL;
               break;
            }
         }
         child_node = child_node->next;
      }

      if ( data_node != NULL ) {
         pNode->data = data_node->data;
         pNode->data_length = data_node->data_length;
         data_node->data = NULL;

         mxml_node_unlink( data_node );
         mxml_node_destroy( data_node );
         doc->node_count--;
      }

   }

   return MXML_STATUS_OK;
}

void mxml_node_write_attributes( MXML_OUTPUT *out, MXML_ATTRIBUTE *attr, int style )
{
   while ( attr != NULL && out->status == MXML_STATUS_OK) {
      mxml_output_char( out, ' ' );
      mxml_attribute_write( out, attr, style );
      attr = attr->next;
   }
}

static void mxml_node_file_indent( MXML_OUTPUT *out, int depth, int style )
{
   int i;

   for ( i = 0; i < depth; i++ ) {
      if ( style & MXML_STYLE_TAB )
         mxml_output_char( out, '\t');
      else if (  style & MXML_STYLE_THREESPACES )
         mxml_output_string_len( out, "   ", 3 );
      else
         mxml_output_char( out, ' ' );
   }
}


MXML_STATUS mxml_node_write( MXML_OUTPUT *out, MXML_NODE *pNode, int style )
{
   MXML_NODE *child;
   int depth = 0;
   int mustIndent = 0;

   if ( style & MXML_STYLE_INDENT ) {
      depth = mxml_node_get_path_depth( pNode )-1;
      mxml_node_file_indent( out, depth, style );
   }

   switch( pNode->type ) {
      case MXML_TYPE_TAG:

         mxml_output_char( out, '<' );
         mxml_output_string( out, pNode->name );

         if ( pNode->attributes != NULL )
            mxml_node_write_attributes( out, pNode->attributes, style );
         if ( pNode->data == NULL && pNode->child == NULL ) {
            mxml_output_string_len( out, "/>\n", 3 );
         }
         else {
            mxml_output_char( out, '>' );

            child = pNode->child;
            if ( child != NULL ) {
               mustIndent = 1;
               mxml_output_char( out, '\n' );

               while ( child != NULL ) {
                  mxml_node_write( out, child, style );
                  child = child->next;
               }
            }

            if ( pNode->data != NULL ) {
               if ( mustIndent && ( style & MXML_STYLE_INDENT ) )
                  mxml_node_file_indent( out, depth+1, style );
               if ( style & MXML_STYLE_NOESCAPE )
                  mxml_output_string( out, pNode->data );
               else
                  mxml_output_string_escape( out, pNode->data );
            }

            if ( mustIndent && ( style & MXML_STYLE_INDENT ))
               mxml_node_file_indent( out, depth, style );

            mxml_output_string_len( out, "</", 2 );
            mxml_output_string( out, pNode->name );
            mxml_output_string_len( out, ">\n", 2 );
         }
      break;

      case MXML_TYPE_COMMENT:
            mxml_output_string_len( out, "<!-- ", 5 );
            mxml_output_string( out, pNode->data );
            mxml_output_string_len( out, " -->\n", 5 );
      break;

      case MXML_TYPE_DATA:
         if ( style & MXML_STYLE_NOESCAPE )
            mxml_output_string( out, pNode->data );
         else
            mxml_output_string_escape( out, pNode->data );
      break;

      case MXML_TYPE_DIRECTIVE:
         mxml_output_string_len( out, "<!", 2 );
         mxml_output_string( out, pNode->name );
         if ( pNode->data != NULL ) {
            mxml_output_char( out, ' ' );
            mxml_output_string( out, pNode->data );
         }
         mxml_output_string_len( out, ">\n", 2 );
      break;

      case MXML_TYPE_PI:
         mxml_output_string_len( out, "<?", 2 );
         mxml_output_string( out, pNode->name );
         if( pNode->data != NULL ) {
            mxml_output_char( out, ' ' );
            mxml_output_string( out, pNode->data );
         }
         mxml_output_string_len( out, "?>\n", 3 );
      break;

      case MXML_TYPE_DOCUMENT:
         child = pNode->child;
         while ( child != NULL ) {
               mxml_node_write( out, child, style );
               child = child->next;
         }
         mxml_output_char( out, '\n' );
      break;

   }

   if ( out->status != MXML_STATUS_OK ) {
      return out->status;
   }

   // just for progress indicators
   out->node_done++;

   return MXML_STATUS_OK;
}
