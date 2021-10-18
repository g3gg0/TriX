
#ifndef __TRIX_TRIXPLUG_TREENODE_H__
#define __TRIX_TRIXPLUG_TREENODE_H__

/* trixplug struct */

struct treenode_funcs
{
	char *(*get_content_type_raw) ( t_treenode * node);
	t_treenode *(*addchild) ( t_treenode * parent);
	t_treenode *(*append) ( t_treenode * prev);
	t_treenode *(*children) ( t_treenode * node);
	t_treenode *(*clone) ( t_treenode * node);
	t_treenode *(*create) ( );
	t_treenode *(*get_byname) ( t_treenode * node, unsigned char *name);
	t_treenode *(*insert_before) ( t_treenode * target);
	t_treenode *(*next) ( t_treenode * node);
	t_treenode *(*parent) ( t_treenode * node);
	t_treenode *(*prev) ( t_treenode * node);
	unsigned char *(*get_attributes) ( t_treenode * node);
	unsigned char *(*get_content_ascii) ( t_treenode * node);
	unsigned char *(*get_content_data) ( t_treenode * node);
	unsigned char *(*get_content_data_binary) ( t_treenode * node, int *length);
	unsigned char *(*get_content_data_raw) ( t_treenode * node);
	unsigned char *(*get_name) ( t_treenode * node);
	unsigned int (*append_child) ( t_treenode * list, t_treenode * item);
	unsigned int (*append_node) ( t_treenode * list, t_treenode * item);
	unsigned int (*count) ( t_treenode * node);
	unsigned int (*dump) ( t_treenode * node);
	unsigned int (*get_content_data_hexval) ( t_treenode * node);
	unsigned int (*get_content_data_integer) ( t_treenode * node);
	unsigned int (*get_content_type) ( t_treenode * node);
	unsigned int (*insert_at) ( t_treenode * target, t_treenode * node, int insert_pos);
	unsigned int (*is_content_type) ( t_treenode * node, int type);
	unsigned int (*position) ( t_treenode * node);
	unsigned int (*release) ( t_treenode * node);
	unsigned int (*set_attributes) ( t_treenode * node, unsigned char *attributes);
	unsigned int (*set_content_data) ( t_treenode * node, unsigned char *data);
	unsigned int (*set_content_data_binary) ( t_treenode * node, unsigned char *data, int length);
	unsigned int (*set_content_data_float) ( t_treenode * node, double data);
	unsigned int (*set_content_data_hexval) ( t_treenode * node, int data);
	unsigned int (*set_content_data_integer) ( t_treenode * node, int data);
	unsigned int (*set_content_data_raw) ( t_treenode * node, unsigned char *data);
	unsigned int (*set_content_type) ( t_treenode * node, int type);
	unsigned int (*set_content_type_raw) ( t_treenode * node, unsigned char *type);
	unsigned int (*set_name) ( t_treenode * node, unsigned char *name);
	unsigned int (*set_priv) ( t_treenode * node, void *data);
	unsigned int (*unlink) ( t_treenode * node);
	void *(*get_priv) ( t_treenode * node);
};

/* trixplug struct initializer */

#define TREENODE_PLUG_INIT \
extern struct trix_functions *ft;\
struct treenode_funcs treenode_functions;\
unsigned int treenode_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->treenode = &treenode_functions;\
	ft->treenode->create = treenode_create;\
	ft->treenode->addchild = treenode_addchild;\
	ft->treenode->append = treenode_append;\
	ft->treenode->append_child = treenode_append_child;\
	ft->treenode->append_node = treenode_append_node;\
	ft->treenode->clone = treenode_clone;\
	ft->treenode->insert_before = treenode_insert_before;\
	ft->treenode->insert_at = treenode_insert_at;\
	ft->treenode->count = treenode_count;\
	ft->treenode->position = treenode_position;\
	ft->treenode->prev = treenode_prev;\
	ft->treenode->next = treenode_next;\
	ft->treenode->children = treenode_children;\
	ft->treenode->parent = treenode_parent;\
	ft->treenode->set_priv = treenode_set_priv;\
	ft->treenode->get_priv = treenode_get_priv;\
	ft->treenode->set_name = treenode_set_name;\
	ft->treenode->set_content_type_raw = treenode_set_content_type_raw;\
	ft->treenode->set_content_type = treenode_set_content_type;\
	ft->treenode->set_content_data_raw = treenode_set_content_data_raw;\
	ft->treenode->set_content_data = treenode_set_content_data;\
	ft->treenode->set_content_data_binary = treenode_set_content_data_binary;\
	ft->treenode->set_content_data_integer = treenode_set_content_data_integer;\
	ft->treenode->set_content_data_hexval = treenode_set_content_data_hexval;\
	ft->treenode->set_content_data_float = treenode_set_content_data_float;\
	ft->treenode->set_attributes = treenode_set_attributes;\
	ft->treenode->get_name = treenode_get_name;\
	ft->treenode->get_content_type_raw = treenode_get_content_type_raw;\
	ft->treenode->get_content_type = treenode_get_content_type;\
	ft->treenode->is_content_type = treenode_is_content_type;\
	ft->treenode->get_content_data_raw = treenode_get_content_data_raw;\
	ft->treenode->get_content_data = treenode_get_content_data;\
	ft->treenode->get_content_ascii = treenode_get_content_ascii;\
	ft->treenode->get_content_data_integer = treenode_get_content_data_integer;\
	ft->treenode->get_content_data_hexval = treenode_get_content_data_hexval;\
	ft->treenode->get_content_data_binary = treenode_get_content_data_binary;\
	ft->treenode->get_attributes = treenode_get_attributes;\
	ft->treenode->get_byname = treenode_get_byname;\
	ft->treenode->release = treenode_release;\
	ft->treenode->unlink = treenode_unlink;\
	ft->treenode->dump = treenode_dump;\
\
	return E_OK;\
}

#endif
