

/* seer headers for treenode.c */

#define TREENODE_AUTO_IMPORTS \
	"import t_treenode *__treenode_create ( );\n"\
	"import t_treenode *__treenode_addchild ( t_treenode * parent);\n"\
	"import t_treenode *__treenode_append ( t_treenode * prev);\n"\
	"import unsigned int __treenode_append_child ( t_treenode * list, t_treenode * item);\n"\
	"import unsigned int __treenode_append_node ( t_treenode * list, t_treenode * item);\n"\
	"import t_treenode *__treenode_clone ( t_treenode * node);\n"\
	"import t_treenode *__treenode_insert_before ( t_treenode * target);\n"\
	"import unsigned int __treenode_insert_at ( t_treenode * target, t_treenode * node, int insert_pos);\n"\
	"import unsigned int __treenode_count ( t_treenode * node);\n"\
	"import unsigned int __treenode_position ( t_treenode * node);\n"\
	"import t_treenode *__treenode_prev ( t_treenode * node);\n"\
	"import t_treenode *__treenode_next ( t_treenode * node);\n"\
	"import t_treenode *__treenode_children ( t_treenode * node);\n"\
	"import t_treenode *__treenode_parent ( t_treenode * node);\n"\
	"import unsigned int __treenode_set_priv ( t_treenode * node, void *data);\n"\
	"import void *__treenode_get_priv ( t_treenode * node);\n"\
	"import unsigned int __treenode_set_name ( t_treenode * node, unsigned char *name);\n"\
	"import unsigned int __treenode_set_content_type_raw ( t_treenode * node, unsigned char *type);\n"\
	"import unsigned int __treenode_set_content_type ( t_treenode * node, int type);\n"\
	"import unsigned int __treenode_set_content_data_raw ( t_treenode * node, unsigned char *data);\n"\
	"import unsigned int __treenode_set_content_data ( t_treenode * node, unsigned char *data);\n"\
	"import unsigned int __treenode_set_content_data_binary ( t_treenode * node, unsigned char *data, int length);\n"\
	"import unsigned int __treenode_set_content_data_integer ( t_treenode * node, int data);\n"\
	"import unsigned int __treenode_set_content_data_hexval ( t_treenode * node, int data);\n"\
	"import unsigned int __treenode_set_content_data_float ( t_treenode * node, double data);\n"\
	"import unsigned int __treenode_set_attributes ( t_treenode * node, unsigned char *attributes);\n"\
	"import unsigned char *__treenode_get_name ( t_treenode * node);\n"\
	"import char *__treenode_get_content_type_raw ( t_treenode * node);\n"\
	"import unsigned int __treenode_get_content_type ( t_treenode * node);\n"\
	"import unsigned int __treenode_is_content_type ( t_treenode * node, int type);\n"\
	"import unsigned char *__treenode_get_content_data_raw ( t_treenode * node);\n"\
	"import unsigned char *__treenode_get_content_data ( t_treenode * node);\n"\
	"import unsigned char *__treenode_get_content_ascii ( t_treenode * node);\n"\
	"import unsigned int __treenode_get_content_data_integer ( t_treenode * node);\n"\
	"import unsigned int __treenode_get_content_data_hexval ( t_treenode * node);\n"\
	"import unsigned char *__treenode_get_content_data_binary ( t_treenode * node, int *length);\n"\
	"import unsigned char *__treenode_get_attributes ( t_treenode * node);\n"\
	"import t_treenode *__treenode_get_byname ( t_treenode * node, unsigned char *name);\n"\
	"import unsigned int __treenode_release ( t_treenode * node);\n"\
	"import unsigned int __treenode_unlink ( t_treenode * node);\n"\
	"import unsigned int __treenode_dump ( t_treenode * node);\n"\



#define TREENODE_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_create</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_addchild</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">parent</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_append</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">prev</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_append_child</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">list</font>,&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">item</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_append_node</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">list</font>,&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">item</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_clone</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_insert_before</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">target</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_insert_at</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">target</font>,&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">insert_pos</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_count</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_position</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_prev</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_next</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_children</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_parent</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_priv</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__treenode_get_priv</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_name</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_type_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_type</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_data_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_data</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_data_binary</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_data_integer</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_data_hexval</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_content_data_float</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">double</font>&nbsp;<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_set_attributes</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">attributes</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_name</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_content_type_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_get_content_type</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_is_content_type</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_content_data_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_content_data</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_content_ascii</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_get_content_data_integer</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_get_content_data_hexval</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_content_data_binary</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__treenode_get_attributes</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*<font color=\"#000000\">__treenode_get_byname</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_unlink</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__treenode_dump</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">node</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for treenode.c */

#define TREENODE_AUTO_REGISTER \
	scAddExtSymInt ( treenode_create );\
	scAddExtSymInt ( treenode_addchild );\
	scAddExtSymInt ( treenode_append );\
	scAddExtSymInt ( treenode_append_child );\
	scAddExtSymInt ( treenode_append_node );\
	scAddExtSymInt ( treenode_clone );\
	scAddExtSymInt ( treenode_insert_before );\
	scAddExtSymInt ( treenode_insert_at );\
	scAddExtSymInt ( treenode_count );\
	scAddExtSymInt ( treenode_position );\
	scAddExtSymInt ( treenode_prev );\
	scAddExtSymInt ( treenode_next );\
	scAddExtSymInt ( treenode_children );\
	scAddExtSymInt ( treenode_parent );\
	scAddExtSymInt ( treenode_set_priv );\
	scAddExtSymInt ( treenode_get_priv );\
	scAddExtSymInt ( treenode_set_name );\
	scAddExtSymInt ( treenode_set_content_type_raw );\
	scAddExtSymInt ( treenode_set_content_type );\
	scAddExtSymInt ( treenode_set_content_data_raw );\
	scAddExtSymInt ( treenode_set_content_data );\
	scAddExtSymInt ( treenode_set_content_data_binary );\
	scAddExtSymInt ( treenode_set_content_data_integer );\
	scAddExtSymInt ( treenode_set_content_data_hexval );\
	scAddExtSymInt ( treenode_set_content_data_float );\
	scAddExtSymInt ( treenode_set_attributes );\
	scAddExtSymInt ( treenode_get_name );\
	scAddExtSymInt ( treenode_get_content_type_raw );\
	scAddExtSymInt ( treenode_get_content_type );\
	scAddExtSymInt ( treenode_is_content_type );\
	scAddExtSymInt ( treenode_get_content_data_raw );\
	scAddExtSymInt ( treenode_get_content_data );\
	scAddExtSymInt ( treenode_get_content_ascii );\
	scAddExtSymInt ( treenode_get_content_data_integer );\
	scAddExtSymInt ( treenode_get_content_data_hexval );\
	scAddExtSymInt ( treenode_get_content_data_binary );\
	scAddExtSymInt ( treenode_get_attributes );\
	scAddExtSymInt ( treenode_get_byname );\
	scAddExtSymInt ( treenode_release );\
	scAddExtSymInt ( treenode_unlink );\
	scAddExtSymInt ( treenode_dump );\


/* seer function declaration for treenode.c */
#ifndef HEADER_SKIP_DECLARATION
void treenode_create ();
void treenode_addchild ();
void treenode_append ();
void treenode_append_child ();
void treenode_append_node ();
void treenode_clone ();
void treenode_insert_before ();
void treenode_insert_at ();
void treenode_count ();
void treenode_position ();
void treenode_prev ();
void treenode_next ();
void treenode_children ();
void treenode_parent ();
void treenode_set_priv ();
void treenode_get_priv ();
void treenode_set_name ();
void treenode_set_content_type_raw ();
void treenode_set_content_type ();
void treenode_set_content_data_raw ();
void treenode_set_content_data ();
void treenode_set_content_data_binary ();
void treenode_set_content_data_integer ();
void treenode_set_content_data_hexval ();
void treenode_set_content_data_float ();
void treenode_set_attributes ();
void treenode_get_name ();
void treenode_get_content_type_raw ();
void treenode_get_content_type ();
void treenode_is_content_type ();
void treenode_get_content_data_raw ();
void treenode_get_content_data ();
void treenode_get_content_ascii ();
void treenode_get_content_data_integer ();
void treenode_get_content_data_hexval ();
void treenode_get_content_data_binary ();
void treenode_get_attributes ();
void treenode_get_byname ();
void treenode_release ();
void treenode_unlink ();
void treenode_dump ();

#endif
/* additional seer includes from treenode_seer.h */

#define TREENODE_AUTO_MISC_PRE \
	"\n"\
	"typedef void t_treenode;\n"\
	"\n"\
	"\n"\


#define TREENODE_AUTO_MISC_POST \
	"\n"\


#define TREENODE_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">t_treenode</font>;<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define TREENODE_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"		</font>"\


