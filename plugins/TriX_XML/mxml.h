/*
   Mini XML lib
   Header files

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   This is a minimal XML parser / interpreter to load XML data without
   relying on external library.

   Currently, validating XML is NOT a priority.

   $Id: mxml.h,v 1.6 2007/03/31 15:13:00 geggo Exp $
*/

#ifndef MINI_XML_LIB_H
#define MINI_XML_LIB_H

#ifdef __cplus__
extern "C" {
#endif

#include "mxml_defs.h"


/* Document operations  */

MXML_DOCUMENT *mxml_document_new();
MXML_STATUS mxml_document_setup( MXML_DOCUMENT *doc );
void mxml_document_destroy( MXML_DOCUMENT *doc );

/* Index oriented operations */
MXML_INDEX *mxml_index_new();
MXML_STATUS mxml_index_setup( MXML_INDEX *doc );
void mxml_index_destroy( MXML_INDEX *doc );

/* Tag oriented operations */
MXML_NODE *mxml_node_new();
MXML_STATUS mxml_node_setup( MXML_NODE *tag );
MXML_NODE *mxml_node_clone( MXML_NODE *tg );
MXML_NODE *mxml_node_clone_tree( MXML_NODE *tg );

void mxml_node_destroy( MXML_NODE *tag );
void mxml_node_unlink( MXML_NODE *tag );

void mxml_node_insert_before( MXML_NODE *tg, MXML_NODE *node );
void mxml_node_insert_after( MXML_NODE *tg, MXML_NODE *node );
void mxml_node_insert_below( MXML_NODE *tg, MXML_NODE *node );
void mxml_node_add_below( MXML_NODE *tg, MXML_NODE *node );

char *mxml_node_value_of_default( MXML_NODE *pNode, char *attrib, char *val_default );
char *mxml_node_value_of( MXML_NODE *pNode, char *attrib );

MXML_STATUS mxml_node_read( MXML_REFIL *data, MXML_NODE *node, MXML_DOCUMENT *doc, int iStyle );

char *mxml_node_get_path_new( MXML_NODE* node );
int mxml_node_get_path( MXML_NODE* node, char *path );
int mxml_node_get_path_length( MXML_NODE* node );
int mxml_node_get_path_depth( MXML_NODE* node );

MXML_STATUS mxml_node_write( MXML_OUTPUT *out, MXML_NODE *pNode, int style );


/* Attribute oriented operations */
MXML_ATTRIBUTE *mxml_attribute_new();
MXML_STATUS mxml_attribute_setup( MXML_ATTRIBUTE *attrib );
void mxml_attribute_destroy( MXML_ATTRIBUTE *attrib );
void mxml_attribute_unlink( MXML_ATTRIBUTE *attrib );
MXML_ATTRIBUTE *mxml_attribute_clone( MXML_ATTRIBUTE *attrib );

MXML_ATTRIBUTE *mxml_attribute_read( MXML_REFIL *data, MXML_DOCUMENT *doc, int style );

MXML_STATUS mxml_attribute_write( MXML_OUTPUT *out, MXML_ATTRIBUTE *attr, int style );


/* Operating on iterators */
MXML_ITERATOR *mxml_iterator_new( MXML_DOCUMENT *doc );
MXML_STATUS mxml_iterator_setup( MXML_ITERATOR *it, MXML_DOCUMENT *doc );

void mxml_iterator_set_top( MXML_ITERATOR *it, MXML_NODE *node );
MXML_NODE *mxml_iterator_top( MXML_ITERATOR *it );

void mxml_iterator_destroy( MXML_ITERATOR *it );
void mxml_iterator_copy( MXML_ITERATOR *dest, MXML_ITERATOR *src );
MXML_ITERATOR *mxml_iterator_clone( MXML_ITERATOR *src );

// Use this one to populate an empty tree
void mxml_iterator_add_below( MXML_ITERATOR *it, MXML_NODE *tag );

// don't use any of the followning on an empty tree
MXML_NODE *mxml_iterator_next( MXML_ITERATOR *it );
MXML_NODE *mxml_iterator_previous_brother( MXML_ITERATOR *it );
MXML_NODE *mxml_iterator_next_brother( MXML_ITERATOR *it );
MXML_NODE *mxml_iterator_ascend( MXML_ITERATOR *it );
MXML_NODE *mxml_iterator_descend( MXML_ITERATOR *it );

void mxml_iterator_insert_after( MXML_ITERATOR *it, MXML_NODE *tag  );
void mxml_iterator_insert_before( MXML_ITERATOR *it, MXML_NODE *tag );
void mxml_iterator_insert_below( MXML_ITERATOR *it, MXML_NODE *tag );

/*Expath enabled search routines */
MXML_NODE *mxml_iterator_find_first( MXML_ITERATOR *it, char *path );
MXML_NODE *mxml_iterator_find_next( MXML_ITERATOR *it );

/* Basic search routines */
MXML_NODE *mxml_iterator_scan_node( MXML_ITERATOR *it, char *name );
MXML_NODE *mxml_iterator_scan_attribute( MXML_ITERATOR *it, char *attrib );
MXML_NODE *mxml_iterator_scan_attribute_value( MXML_ITERATOR *it, char *attrib, char *val );

/* Refil routines */
MXML_REFIL *mxml_refil_new( MXML_REFIL_FUNC func, char *buf, int buflen,
   int bufsize );
MXML_STATUS mxml_refil_setup( MXML_REFIL *ref, MXML_REFIL_FUNC func,
   char *buf, int buflen, int bufsize );
void mxml_refil_destory( MXML_REFIL *ref );

int mxml_refil_getc( MXML_REFIL *ref );
#define mxml_refil_ungetc( ref, ch )  ref->sparechar = ch

void mxml_refill_from_stream_func( MXML_REFIL *ref );
void mxml_refill_from_buffer_func( MXML_REFIL *ref );
void mxml_refill_from_handle_func( MXML_REFIL *ref );

/* Output routines */
MXML_OUTPUT *mxml_output_new( MXML_OUTPUT_FUNC func, int node_count);
MXML_STATUS mxml_output_setup( MXML_OUTPUT *out, MXML_OUTPUT_FUNC func, int node_count);
void mxml_output_destroy( MXML_OUTPUT *out );
MXML_STATUS mxml_output_char( MXML_OUTPUT *out, int c );
MXML_STATUS mxml_output_string_len( MXML_OUTPUT *out, char *s, int len );
MXML_STATUS mxml_output_string( MXML_OUTPUT *out, char *s);
MXML_STATUS mxml_output_string_escape( MXML_OUTPUT *out, char *s );

void mxml_output_func_to_stream( MXML_OUTPUT *out, char *s, int len );
void mxml_output_func_to_handle( MXML_OUTPUT *out, char *s, int len );
void mxml_output_func_to_sgs( MXML_OUTPUT *out, char *s, int len );


/* Self growing string routines */
MXML_SGS *mxml_sgs_new();
void mxml_sgs_destroy( MXML_SGS *sgs );
MXML_STATUS mxml_sgs_append_char( MXML_SGS *sgs, char c );
MXML_STATUS mxml_sgs_append_string_len( MXML_SGS *sgs, char *s, int slen );
MXML_STATUS mxml_sgs_append_string( MXML_SGS *sgs, char *s );


/* Error description */
char *mxml_error_desc( MXML_ERROR_CODE code );

#include "defines.h"
#include "mem.h"
#include "treenode.h"
#include "trixplug.h"
#include "trixplug_wrapper.h"


extern struct trix_functions *ft;

/* Allocator and deletor functions are meant to be redeclared by includers */
#ifndef MXML_ALLOCATOR
   #define MXML_ALLOCATOR  malloc
#endif

#ifndef MXML_DELETOR
   #define MXML_DELETOR  free
#endif

#ifndef MXML_REALLOCATOR
   #define MXML_REALLOCATOR realloc
#endif


#ifdef __cplus__
}
#endif

#endif
