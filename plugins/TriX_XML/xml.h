#ifndef __TRIX_XML_H__
#define __TRIX_XML_H__

#include "trixplug.h"

//#include "mxml.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );

#define XML_HEADERS \
	"import t_treenode * __xml_connector_parse ( unsigned char *xml_data );\n"\
	"import t_treenode * __xml_connector_parse_file ( unsigned char *filename );;\n"\
	"import unsigned int __xml_connector_create_file ( void *treenode, char *filename, char *stylesheet );\n"\
	"import unsigned char * __xml_connector_create ( void *treenode, char *stylesheet );\n"\


#define XML_SYMBOLS \
	REGISTER_SYMBOL ( xml_connector_parse );\
	REGISTER_SYMBOL ( xml_connector_parse_file );\
	REGISTER_SYMBOL ( xml_connector_create_file );\
	REGISTER_SYMBOL ( xml_connector_create );\
	

t_treenode *xml_connector_parse ( unsigned char *xml_data );
t_treenode *xml_connector_parse_file ( unsigned char *filename );
unsigned char * xml_connector_create ( void *treenode, char *stylesheet );
unsigned int xml_connector_create_file ( void *node, char *filename, char *stylesheet );
unsigned int xml_init ( );

#endif