#ifndef __TRIX_TREENODE_C__
#define __TRIX_TREENODE_C__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "seer.h"
#include "workspace.h"
#include "util.h"

#include "treenode.h"
#include "trixplug_funcs.h"

#include "mem.h"

TREENODE_PLUG_INIT;

unsigned char *treenode_content_types[] = {
	"EMPTY",
	"ASCII",
	"UNICODE",
	"HEXCODED",
	"INTEGER",
	"HEXVAL",
	"FLOAT"
};

/*!
 * return a newly created treenode
 * \return pointer to treenode or NULL in case of failure
 */
t_treenode *treenode_create ( )
{
	t_treenode *node = LIST_MALLOC(t_treenode);

	if ( !node )
		return NULL;

	node->name = NULL;
	node->content_type = NULL;
	node->content_data = NULL;
	node->attributes = NULL;
	node->priv = NULL;

	node->parent = NULL;
	node->children = NULL;

	return node;
}

/*!
 * return a newly created treenode with given node as parent
 * \param parent the parent
 * \return pointer to treenode or NULL in case of failure
 */
t_treenode *treenode_addchild ( t_treenode *parent )
{
	t_treenode *node = NULL;

	node = treenode_create();
	treenode_append_child ( parent, node );
	return node;

}

/*!
 * return a newly created treenode with given node as previous list item
 * \param prev the list item to append the treenode
 * \return pointer to treenode or NULL in case of failure
 */
t_treenode *treenode_append ( t_treenode *prev )
{
	t_treenode *node = treenode_create();

	if ( prev )
	{
		// seek the last entry
		LIST_FFWD(prev);

		node->prev = prev;
		node->prev->next = node;

		node->parent = prev->parent;
	}

	return node;
}


/*!
 * insert treenode(s) at end of given node's children
 * \param prev the list item to append the treenode
 * \return E_OK or E_FAIL
 */
unsigned int treenode_append_child ( t_treenode *list, t_treenode *item )
{
	t_treenode *child = NULL;

	if ( !list || !item )
		return E_FAIL;

	child = list->children;
	if ( child )
	{
		// seek the last entry
		LIST_FFWD(child);

		item->prev = child;
		child->next = item;
	}
	else
	{
		// append as child
		item->prev = NULL;
		item->parent = list;
		list->children = item;
	}

	// update all children's parent pointers
	while ( item )
	{
		item->parent = list;
		item = treenode_next ( item );
	}
	return E_OK;
}

/*!
 * insert treenode(s) at end of given node as previous list item
 * \param prev the list item to append the treenode
 * \return E_OK or E_FAIL
 */
unsigned int treenode_append_node ( t_treenode *list, t_treenode *item )
{
	if ( !list || !item )
		return E_FAIL;

	// seek the last entry
	LIST_FFWD(list);

	item->prev = list;
	item->prev->next = item;

	while ( item )
	{
		item->parent = list->parent;
		item = treenode_next ( item );
	}

	return E_OK;
}

/*!
 * return a newly created treenode by cloning the given one
 * you will have to set parent to NULL to ensure independence from original tree
 * \param node the list item to get cloned
 * \return pointer to treenode or NULL in case of failure
 */
t_treenode *treenode_clone ( t_treenode *node )
{
	t_treenode *clone = treenode_create();
	t_treenode *child = NULL;
	t_treenode *cloned_child = NULL;

	if ( !node )
		return NULL;

	if ( node->attributes )
		clone->attributes = strdup ( node->attributes );
	if ( node->content_data )
		clone->content_data = strdup ( node->content_data );
	if ( node->content_type )
		clone->content_type = strdup ( node->content_type );
	if ( node->name )
		clone->name = strdup ( node->name );

	clone->flags = node->flags;
	clone->priv = node->priv;

	child = node->children;
	while ( child )
	{
		cloned_child = treenode_clone ( child );
		treenode_append_child ( clone, cloned_child );

		child = treenode_next ( child );
	}

	return clone;
}

/*!
 * return a newly created treenode with given node as target
 * \param target the list item where to insert (inserted before)
 * \return pointer to treenode or NULL in case of failure
 */
t_treenode *treenode_insert_before ( t_treenode *target )
{
	t_treenode *node = NULL;

	if ( target )
	{
		node = treenode_create();

		if ( target->prev )
		{
			node->prev = target->prev;
			target->prev->next = node;
		}
		else if ( target->parent && target->parent->children == target )
		{
			target->parent->children = node;
		}
		else
		{
			// standalone node, no chance yet to insert before 
			// TODO: appending one and copying data...
			treenode_release ( node );
			return NULL;
		}

		node->next = target;
		target->prev = node;

		node->parent = target->parent;
	}

	return node;
}


/*!
 * insert treenode at specific position of given target list
 * \param target the list item where to insert
 * \param node item to insert
 * \param pos position starting from 0
 * \return E_OK or E_FAIL
 */
unsigned int treenode_insert_at ( t_treenode *target, t_treenode *node, int insert_pos )
{
	int last = 0;
	int pos = 0;
	t_treenode *list = NULL;

	if ( !target || !node )
		return E_FAIL;

	// will insert as first child
	if ( !insert_pos )
	{
		node->next = target->children;
		node->prev = NULL;

		if ( target->children )
			target->children->prev = node;

		node->parent = target;
		target->children = node;

		return E_OK;
	}

	// insert elsewhere
	list = treenode_children ( target );

	while ( pos < insert_pos )
	{
		if ( treenode_next ( list ) )
			list = treenode_next ( list );
		else
			last = 1;

		pos++;
	}

	// must never happen!!
	if ( !list )
		return E_FAIL;

	// will insert as last child
	if ( last )
	{
		node->prev = list;
		node->next = NULL;
		node->parent = target;
		list->next = node;

		return E_OK;
	}

	// now just the insertion between first and last position

	// must never happen!!!
	if ( !list->prev )
		return E_FAIL;

	node->prev = list->prev;
	node->next = list;
	node->parent = target;

	list->prev->next = node;
	list->prev = node;

	return E_OK;
}


/*!
 * get the number of nodes in the list
 * \param node the node to count
 * \return number of nodes
 */
unsigned int treenode_count ( t_treenode *node )
{
	int count = 0;

	while ( node )
	{
		count++;
		LIST_NEXT ( node );
	}

	return count;
}


/*!
 * get the position of the node in the parent's list starting from 0
 * \param node the node to count
 * \return number of nodes
 */
unsigned int treenode_position ( t_treenode *node )
{
	t_treenode *list = NULL;
	int pos = 0;
	
	if ( !node )
		return 0;

	list = treenode_children ( treenode_parent ( node ) );

	while ( list )
	{
		if ( list == node )
			return pos;
		pos++;
		LIST_NEXT ( list );
	}

	// must not happen!!

	return 0;
}

/*!
 * get the previous node in the list
 * \param node 
 * \return node
 */
t_treenode *treenode_prev ( t_treenode *node )
{
	if ( !node )
		return NULL;
	return node->prev;
}

/*!
 * get the next node in the list
 * \param node 
 * \return node
 */
t_treenode *treenode_next ( t_treenode *node )
{
	if ( !node )
		return NULL;
	return node->next;
}

/*!
 * get the first child node in the list
 * \param node 
 * \return node
 */
t_treenode *treenode_children ( t_treenode *node )
{
	if ( !node )
		return NULL;
	return node->children;
}

/*!
 * get the parent node 
 * \param node 
 * \return node
 */
t_treenode *treenode_parent ( t_treenode *node )
{
	if ( !node )
		return NULL;
	return node->parent;
}

/*!
 * set the node privdata
 * \param node 
 * \param data pricdata
 * \return status
 */
unsigned int treenode_set_priv ( t_treenode *node, void *data )
{
	if ( !node )
		return E_FAIL;
	node->priv = data;
	
	return E_OK;
}

/*!
 * get the node privdata 
 * \param node 
 * \return privdata
 */
void *treenode_get_priv ( t_treenode *node )
{
	if ( !node )
		return NULL;
	return node->priv;
}

/*!
 * set the node name
 * \param node 
 * \param name name of the node
 * \return status
 */
unsigned int treenode_set_name ( t_treenode *node, unsigned char *name )
{
	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->name);

	node->name = strdup ( name );

	return E_OK;
}

/*!
 * set the node content type raw
 * \param node 
 * \param name content type of the node
 * \return status
 */
unsigned int treenode_set_content_type_raw ( t_treenode *node, unsigned char *type )
{
	if ( !node || !type )
		return E_FAIL;

	CHECK_AND_FREE(node->content_type);

	node->content_type = strdup ( type );

	return E_OK;
}

/*!
 * set the node content type
 * \param node 
 * \param name content type of the node
 * \return status
 */
unsigned int treenode_set_content_type ( t_treenode *node, int type )
{
	if ( !node || type < 0 || type > TREENODE_MAX_TYPES )
		return E_FAIL;

	CHECK_AND_FREE(node->content_type);

	node->content_type = strdup ( treenode_content_types[type] );

	return E_OK;
}

/*!
 * set the node content data raw
 * \param node 
 * \param data content data of the node
 * \return status
 */
unsigned int treenode_set_content_data_raw ( t_treenode *node, unsigned char *data )
{
	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->content_data);

	node->content_data = strdup ( data );

	return E_OK;
}


/*!
 * set the node content data
 * \param node 
 * \param data content data of the node
 * \return status
 */
unsigned int treenode_set_content_data ( t_treenode *node, unsigned char *data )
{
	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->content_data);

	node->content_data = strdup ( data );
	treenode_set_content_type ( node, TREENODE_TYPE_ASCII );

	return E_OK;
}


/*!
 * set the node content data as binary
 * \param node 
 * \param data binary content data of the node
 * \param length length of the binary buffer
 * \return status
 */
unsigned int treenode_set_content_data_binary ( t_treenode *node, unsigned char *data, int length )
{
	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->content_data);

	node->content_data = util_hexpack ( data, length );
	treenode_set_content_type ( node, TREENODE_TYPE_HEXCODED );

	return E_OK;
}

/*!
 * set the node content data as integer
 * \param node 
 * \param data integer value
 * \return status
 */
unsigned int treenode_set_content_data_integer ( t_treenode *node, int data )
{
	unsigned char buffer[64];

	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->content_data);

	sprintf ( buffer, "%i", data );
	node->content_data = strdup ( buffer );
	treenode_set_content_type ( node, TREENODE_TYPE_INTEGER );

	return E_OK;
}

/*!
 * set the node content data as hexval
 * \param node 
 * \param data integer value
 * \return status
 */
unsigned int treenode_set_content_data_hexval ( t_treenode *node, int data )
{
	unsigned char buffer[64];

	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->content_data);

	sprintf ( buffer, "%08X", data );
	node->content_data = strdup ( buffer );
	treenode_set_content_type ( node, TREENODE_TYPE_HEXVAL );

	return E_OK;
}


/*!
 * set the node content data as double
 * \param node 
 * \param data binary content data of the node
 * \return status
 */
unsigned int treenode_set_content_data_float ( t_treenode *node, double data )
{
	unsigned char buffer[64];

	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->content_data);

	sprintf ( buffer, "%d", data );
	node->content_data = strdup ( buffer );
	treenode_set_content_type ( node, TREENODE_TYPE_FLOAT );

	return E_OK;
}


/*!
 * set the node attributes
 * \param node 
 * \param name attributes of the node
 * \return status
 */
unsigned int treenode_set_attributes ( t_treenode *node, unsigned char *attributes )
{
	if ( !node )
		return E_FAIL;

	CHECK_AND_FREE(node->attributes);

	node->attributes = strdup ( attributes );

	return E_OK;
}




/*!
 * get the node name
 * \param node 
 * \return name
 */
unsigned char *treenode_get_name ( t_treenode *node )
{
	if ( !node )
		return NULL;

	return node->name;
}

/*!
 * get the node content type raw
 * \param node 
 * \return content type
 */
char *treenode_get_content_type_raw ( t_treenode *node )
{
	if ( !node || !node->content_type )
		return TREENODE_TYPE_EMPTY_STR;

	return node->content_type;
}

/*!
 * get the node content type
 * \param node 
 * \return content type
 */
unsigned int treenode_get_content_type ( t_treenode *node )
{
	if ( !node || !node->content_type )
		return TREENODE_TYPE_EMPTY;

	if ( !strcmp ( node->content_type, TREENODE_TYPE_EMPTY_STR ) )
		return TREENODE_TYPE_EMPTY;
	if ( !strcmp ( node->content_type, TREENODE_TYPE_ASCII_STR ) )
		return TREENODE_TYPE_ASCII;
	if ( !strcmp ( node->content_type, TREENODE_TYPE_UNICODE_STR ) )
		return TREENODE_TYPE_UNICODE;
	if ( !strcmp ( node->content_type, TREENODE_TYPE_HEXCODED_STR ) )
		return TREENODE_TYPE_HEXCODED;
	if ( !strcmp ( node->content_type, TREENODE_TYPE_INTEGER_STR ) )
		return TREENODE_TYPE_INTEGER;
	if ( !strcmp ( node->content_type, TREENODE_TYPE_HEXVAL_STR ) )
		return TREENODE_TYPE_HEXVAL;
	if ( !strcmp ( node->content_type, TREENODE_TYPE_FLOAT_STR ) )
		return TREENODE_TYPE_FLOAT;


	return TREENODE_TYPE_EMPTY;
}

/*!
 * check content type
 * \param node 
 * \param type the type to compare to
 * \return 1 if true, 0 if not
 */
unsigned int treenode_is_content_type ( t_treenode *node, int type )
{
	if ( treenode_get_content_type ( node ) == type )
		return 1;

	return 0;
}


/*!
 * get the node content raw
 * \param node 
 * \return content
 */
unsigned char *treenode_get_content_data_raw ( t_treenode *node )
{
	if ( !node )
		return NULL;

	return node->content_data;
}

/*!
 * get the node content
 * \param node 
 * \return content
 */
unsigned char *treenode_get_content_data ( t_treenode *node )
{
	if ( !node )
		return NULL;

	return node->content_data;
}

/*!
 * get the node content in ascii
 * \param node 
 * \return content
 */
unsigned char *treenode_get_content_ascii ( t_treenode *node )
{
	if ( !node )
		return NULL;

	return node->content_data;
}

/*!
 * get the node content as integer
 * \param node 
 * \return content
 */
unsigned int treenode_get_content_data_integer ( t_treenode *node )
{
	int value = 0;

	if ( !node || !node->content_data )
		return E_FAIL;

	if ( !treenode_is_content_type ( node, TREENODE_TYPE_INTEGER ) )
		return E_FAIL;

	if ( sscanf ( node->content_data, "%i", &value ) != 1 )
		return E_FAIL;

	return value;
}

/*!
 * get the node content as integer
 * \param node 
 * \return content
 */
unsigned int treenode_get_content_data_hexval ( t_treenode *node )
{
	unsigned int value = 0;
	int err = 0;

	if ( !node || !node->content_data )
		return E_FAIL;

	if ( !treenode_is_content_type ( node, TREENODE_TYPE_HEXVAL ) )
		return E_FAIL;

	value = util_hex_get ( node->content_data, &err );

	if ( err )
		return E_FAIL;

	return value;
}

/*!
 * get the node content as binary data
 * \param node 
 * \param length pointer to the returned length 
 * \return content
 */
unsigned char *treenode_get_content_data_binary ( t_treenode *node, int *length )
{
	unsigned char *buffer = NULL;

	if ( !node || !length || !node->content_data )
		return NULL;

	if ( !treenode_is_content_type ( node, TREENODE_TYPE_HEXCODED ) )
		return NULL;

	buffer = util_hexunpack ( node->content_data, length );

	return buffer;
}


/*!
 * get the node attributes
 * \param node 
 * \return attributes
 */
unsigned char *treenode_get_attributes ( t_treenode *node )
{
	if ( !node )
		return NULL;

	return node->attributes;
}



/*!
 * get the node specified by name
 * \param node the node tree to search in
 * \param name the name to search for
 * \return node identified by that name
 */
t_treenode *treenode_get_byname ( t_treenode *node, unsigned char *name )
{
	t_treenode *ret = NULL;

	if ( !node || !name )
		return NULL;

	while ( node )
	{
		if ( treenode_get_name ( node ) && !strcmp ( treenode_get_name ( node ), name ) )
			return node;

		if ( treenode_children ( node ) )
		{
			ret = treenode_get_byname ( treenode_children ( node ), name );
			if ( ret )
				return ret;
		}
		node = treenode_next ( node );
	}

	return NULL;
}


/*!
 * release the node tree
 * \param node the node tree to free
 * \return E_OK or E_FAIL
 */
unsigned int treenode_release ( t_treenode *node )
{
	if ( !node  )
		return E_FAIL;

	while ( treenode_children ( node ) )
		treenode_release ( treenode_children ( node ) );

	// if has parent AND we are the first child, set the next in list as first child
	if ( node->parent && node->parent->children == node )
		node->parent->children = node->next;

	// if has next in list, update chains
	if ( node->next )
		node->next->prev = node->prev;

	// if has prev in list, update chains
	if ( node->prev )
		node->prev->next = node->next;

	CHECK_AND_FREE ( node->content_data );
	CHECK_AND_FREE ( node->content_type );
	CHECK_AND_FREE ( node->attributes );
	CHECK_AND_FREE ( node->name );

	free ( node );

	return E_OK;
}


/*!
 * unlink the node tree from its parent
 * \param node the node tree to unlink
 * \return E_OK or E_FAIL
 */
unsigned int treenode_unlink ( t_treenode *node )
{
	if ( !node  )
		return E_FAIL;

	// if has parent AND we are the first child, set the next in list as first child
	if ( node->parent && node->parent->children == node )
			node->parent->children = node->next;
	// if has next in list, update chains
	if ( node->next )
		node->next->prev = node->prev;
	// if has prev in list, update chains
	if ( node->prev )
		node->prev->next = node->next;

	return E_OK;
}

#define PRINT_INDENT pos=indent; while (pos--) printf ( " " );
#define FPRINT_INDENT pos=indent; while (pos--) fprintf ( file, " " );


unsigned int treenode_dump ( t_treenode *node )
{
	int pos = 0;
	static int indent = 0;
	int as_xml = 1;
	static FILE *file = NULL;

	
	if ( !file )
		file = fopen ( "c:\\t.xml", "wb" );

	indent++;
	while ( node )
	{
		if ( !as_xml )
		{
			PRINT_INDENT;
			printf ( "Name       : '%s'\n", treenode_get_name(node) );
			PRINT_INDENT;
			printf ( "Type       : '%s'\n", treenode_content_types[treenode_get_content_type(node)] );
			PRINT_INDENT;
			if ( treenode_is_content_type ( node, TREENODE_TYPE_ASCII ) )
				printf ( "Content    : '%s'\n", treenode_get_content_data ( node ) );
			else if ( treenode_is_content_type ( node, TREENODE_TYPE_INTEGER ) )
				printf ( "Content    : %i\n", treenode_get_content_data_integer ( node ) );
			else if ( treenode_is_content_type ( node, TREENODE_TYPE_HEXVAL ) )
				printf ( "Content    : 0x%08X\n", treenode_get_content_data_hexval ( node ) );
			else
				printf ( "Content    : \n" );
			PRINT_INDENT;
			printf ( "Content RAW: '%s'\n", treenode_get_content_data ( node ) );

			PRINT_INDENT;
			printf ( "-------------------------------------------\n", treenode_get_content_data ( node ) );

			if ( treenode_children ( node ) )
				treenode_dump ( treenode_children ( node ) );
		}
		else
		{
			FPRINT_INDENT;
			fprintf ( file, "<%s>", treenode_get_name(node) );

			if ( !treenode_children ( node ) )
				fprintf ( file, "%s", treenode_get_content_data ( node ) );
			else
			{
				fprintf ( file, "\n" );
				treenode_dump ( treenode_children ( node ) );
				FPRINT_INDENT;
			}
			fprintf ( file, "</%s>\n", treenode_get_name(node) );
		}

		node = treenode_next ( node );
	}

	indent--;

	if ( !indent )
	{
		fclose ( file );
		file = NULL;
	}
	
	return E_OK;
}



#endif
