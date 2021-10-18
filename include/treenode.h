#ifndef __TRIX_TREENODE_H__
#define __TRIX_TREENODE_H__

typedef struct s_treenode t_treenode;
struct s_treenode
{
	STRUCT_HEADER_LIST(t_treenode);

	char *name;          // "PPM", "TONE", "FONT", ....
	char *content_type;  // "ascii", "unicode", "hexcoded", "integer", "empty"
	char *content_data;  // "This is a String"
	char *attributes;    // "someparam=value,anotherparam=somevalue"

	void *priv;  // when linking other items to this tree

	t_treenode *parent;
	t_treenode *children;
};

#define TREENODE_TYPE_EMPTY_STR     "EMPTY"
#define TREENODE_TYPE_ASCII_STR     "ASCII"
#define TREENODE_TYPE_UNICODE_STR   "UNICODE"
#define TREENODE_TYPE_HEXCODED_STR  "HEXCODED"
#define TREENODE_TYPE_INTEGER_STR   "INTEGER"
#define TREENODE_TYPE_HEXVAL_STR    "HEXVAL"
#define TREENODE_TYPE_FLOAT_STR     "FLOAT"

#define TREENODE_TYPE_EMPTY     0
#define TREENODE_TYPE_ASCII     1
#define TREENODE_TYPE_UNICODE   2
#define TREENODE_TYPE_HEXCODED  3
#define TREENODE_TYPE_INTEGER   4
#define TREENODE_TYPE_HEXVAL    5
#define TREENODE_TYPE_FLOAT     6

#define TREENODE_MAX_TYPES      6

#include "trixplug_treenode.h"


t_treenode *treenode_create ( );
t_treenode *treenode_addchild ( t_treenode *parent );
t_treenode *treenode_append ( t_treenode *pre );
unsigned int treenode_count ( t_treenode *node );
t_treenode *treenode_prev ( t_treenode *node );
t_treenode *treenode_next ( t_treenode *node );
t_treenode *treenode_children ( t_treenode *node );
t_treenode *treenode_parent ( t_treenode *node );

unsigned int treenode_set_name ( t_treenode *node, unsigned char *name );
unsigned int treenode_set_content_type ( t_treenode *node, int type );
unsigned int treenode_set_content_data ( t_treenode *node, unsigned char *data );
unsigned int treenode_set_content_data_hexval ( t_treenode *node, int data );
unsigned int treenode_set_content_data_integer ( t_treenode *node, int data );
unsigned int treenode_set_content_data_float ( t_treenode *node, double data );
unsigned int treenode_set_content_data_binary ( t_treenode *node, unsigned char *data, int length );
unsigned int treenode_set_attributes ( t_treenode *node, unsigned char *attributes );

unsigned int treenode_is_content_type ( t_treenode *node, int type );
unsigned char *treenode_get_name ( t_treenode *node );
unsigned int treenode_get_content_type ( t_treenode *node );
unsigned int treenode_is_content_type ( t_treenode *node, int type );
unsigned int treenode_get_content_data_hexval ( t_treenode *node );
unsigned char *treenode_get_content_data ( t_treenode *node );
unsigned char *treenode_get_content_ascii ( t_treenode *node );
unsigned int treenode_get_content_data_integer ( t_treenode *node );
unsigned char *treenode_get_content_data_binary ( t_treenode *node, int *length );
unsigned char *treenode_get_attributes ( t_treenode *node );
t_treenode *treenode_get_byname ( t_treenode *node, unsigned char *name );
unsigned int treenode_release ( t_treenode *node );
unsigned int treenode_unlink ( t_treenode *node );
unsigned int treenode_position ( t_treenode *node );
unsigned int treenode_insert_at ( t_treenode *target, t_treenode *node, int insert_pos );
t_treenode *treenode_clone ( t_treenode *node );
unsigned int treenode_append_node ( t_treenode *list, t_treenode *item );
unsigned int treenode_append_child ( t_treenode *list, t_treenode *item );
unsigned int treenode_dump ( t_treenode *node );
unsigned int treenode_set_content_type_raw ( t_treenode *node, unsigned char *type );
char *treenode_get_content_type_raw ( t_treenode *node );
unsigned int treenode_set_content_data_raw ( t_treenode *node, unsigned char *data );
unsigned char *treenode_get_content_data_raw ( t_treenode *node );
t_treenode *treenode_insert_before ( t_treenode *target );
unsigned int treenode_set_priv ( t_treenode *node, void *data );
void *treenode_get_priv ( t_treenode *node );

#endif
