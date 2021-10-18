#ifndef __TRIX_XML_C__
#define __TRIX_XML_C__

/*! \file xml.c
 */



#ifdef WIN32
    #include <windows.h>
#endif

#include <stdio.h>


#include "defines.h"
#include "mem.h"


#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "treenode.h"

#include "trixplug.h"
#include "xml.h"
#include "mxml.h"

unsigned char *force_attribute = "";

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

OPT_BEGIN
//	OPTION( OPT_BOOL, "Bool Test Variable", "testbool", &testbool )
	OPTION( OPT_STR,  "Force Attribute for treenode type (empty to store within XML attribute)", "force_attribute", &force_attribute )
OPT_FINISH

#define XML_TITLE "XML"
#define XML_MAJOR 0
#define XML_MINOR 1

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( XML_TITLE )
	PLUGIN_VERSION ( XML_MAJOR, XML_MINOR )
	PLUGIN_INIT ( xml_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH





unsigned int xml_init ( )
{
    REGISTER_HEADER ( "trix_xml", XML_HEADERS );
    XML_SYMBOLS

	printf ( XML_TITLE " v%01i.%01i Plugin Loaded", XML_MAJOR, XML_MINOR  );
    return E_OK;
}


void xml_connector_create_parse_siblings( t_treenode *treenode_p, MXML_NODE *xml_node )
{

	MXML_NODE *xml_node_it = NULL;
	t_treenode *treenode_tmp = NULL;

	xml_node_it = xml_node;

	do
	{
		if ( xml_node_it->name )
		{
			// dont process PI types like stylesheets or directives
			if ( xml_node_it->type != MXML_TYPE_PI && xml_node_it->type != MXML_TYPE_DIRECTIVE )
			{
				treenode_tmp = treenode_addchild( treenode_p );
				treenode_set_name ( treenode_tmp, xml_node_it->name );

				if ( xml_node_it->data_length )
					treenode_set_content_data_raw ( treenode_tmp, xml_node_it->data );
				else
					treenode_set_content_data_raw ( treenode_tmp, "" );

				if ( xml_node_it->attributes && !strcmp ( xml_node_it->attributes->name, "type" ) )
					treenode_set_content_type_raw ( treenode_tmp, xml_node_it->attributes->value );
				else if  ( strlen ( force_attribute ))
					treenode_set_content_type_raw ( treenode_tmp, force_attribute );
				else
					treenode_set_content_type_raw ( treenode_tmp, "ASCII" );
				
				if ( xml_node_it->child )
					xml_connector_create_parse_siblings ( treenode_tmp, xml_node_it->child );
			}
		}
		else
		{
			if ( xml_node_it->child )
				xml_connector_create_parse_siblings ( treenode_p, xml_node_it->child );
		}
	

	} while  ( xml_node_it = xml_node_it->next );
	//iterator für brother's
	//wenn child, dann rekursiv (new it)

}

t_treenode *xml_connector_parse ( unsigned char *xml_data )
{
	t_treenode *treenode_p = NULL;
	MXML_DOCUMENT *xml_main = NULL;
	FILE *file = NULL;

	xml_main = mxml_buffer ( xml_data, 0 );

	if ( !xml_main )	//TODO check for errors!
	{
		printf ( "error parsing xml data %s\n" );
		return NULL;
	}

	treenode_p = treenode_addchild ( NULL );
	//treenode_set_name ( treenode_p, "first" );

	xml_connector_create_parse_siblings ( treenode_p, xml_main->root );

	mxml_document_destroy ( xml_main );

	
	return treenode_p;
}


t_treenode *xml_connector_parse_file ( unsigned char *filename )
{
	t_treenode *treenode_p = NULL;
	MXML_DOCUMENT *xml_main = NULL;
	FILE *file = NULL;

	xml_main = mxml_read ( filename, 0 );

	if ( !xml_main )	//TODO check for errors!
	{
		printf ( "error loading xml file %s\n", filename );
		return NULL;
	}

	treenode_p = treenode_addchild ( NULL );
	//treenode_set_name ( treenode_p, "first" );

	xml_connector_create_parse_siblings ( treenode_p, xml_main->root );

	mxml_document_destroy ( xml_main );

	return treenode_p;
}

void xml_connector_create_parse_children( t_treenode *treenode_p, MXML_NODE *xmlnode_p )
{

	t_treenode *treenode_child;
	MXML_NODE *xmlnode_child;
	MXML_ATTRIBUTE *attr;

	if ( !treenode_p || !xmlnode_p )
		return;

	treenode_child = treenode_p->children;

	if ( !treenode_child )
	{
		//mnode_child = mxmlNewText ( mnode_parent, 0, treenode_get_content_data ( treenode_p ) );
		//xmlnode_child = mxml_node_new ();
		//mxml_node_setup ( xmlnode_child );
		if ( treenode_get_content_data_raw ( treenode_p )  )
		{
			if ( !strlen ( force_attribute ) )
			{
				attr = mxml_attribute_new ();
				mxml_attribute_setup ( attr );
				attr->name = strdup ( "type" );
				attr->value = strdup ( treenode_get_content_type_raw ( treenode_p ) );
				xmlnode_p->attributes = attr;
			}
			xmlnode_p->data = strdup ( treenode_get_content_data_raw ( treenode_p ) );
			xmlnode_p->data_length = strlen ( xmlnode_p->data );
		}

		//mxml_node_add_below ( xmlnode_p, xmlnode_child );
		return;
	}

	while ( treenode_child )
	{
		if ( treenode_get_name ( treenode_child ) )
		{
			xmlnode_child = mxml_node_new ();
			mxml_node_setup ( xmlnode_child );
			xmlnode_child->name = strdup ( treenode_get_name ( treenode_child ) );

			mxml_node_add_below ( xmlnode_p, xmlnode_child );

			xml_connector_create_parse_children ( treenode_child, xmlnode_child );
		}
		else
			xml_connector_create_parse_children ( treenode_child, xmlnode_p );

		treenode_child = treenode_child->next;
	}
	
}

unsigned char *xml_connector_create ( void *treenode, char *stylesheet )
{
	MXML_DOCUMENT *xml_main = NULL;
	t_treenode *treenode_top = NULL;
	MXML_NODE *xmlnode_child;
	MXML_ATTRIBUTE *attr;
	MXML_SGS *buffer;


	if ( !treenode )
		return NULL;

	xml_main = mxml_document_new();

	if ( mxml_document_setup ( xml_main ) != MXML_STATUS_OK )
	{
		printf ( "could not setup xml document\n" );
		return NULL;
	}

	treenode_top = treenode_create ();
	treenode_set_name ( treenode_top, "TOP" );
	treenode_append_child ( treenode_top, treenode_clone ( (t_treenode*)treenode ) );

	xmlnode_child = mxml_node_new ();
	mxml_node_setup ( xmlnode_child );
	xmlnode_child->name = strdup ( "xml version=\"1.0\" encoding=\"UTF-8\"" );
	xmlnode_child->type = MXML_TYPE_PI;

	mxml_node_add_below ( xml_main->root, xmlnode_child );

	if ( stylesheet )
	{
		xmlnode_child = mxml_node_new ();
		mxml_node_setup ( xmlnode_child );
		xmlnode_child->name = malloc ( strlen ( stylesheet ) + 100 );
		sprintf ( xmlnode_child->name, "xml-stylesheet type=\"text/xsl\" href=\"%s\"", stylesheet );
		xmlnode_child->type = MXML_TYPE_PI;

		mxml_node_add_below ( xml_main->root, xmlnode_child );
	}

	xml_connector_create_parse_children ( treenode_top, xml_main->root );

	//mxml_node
	buffer = mxml_sgs_new();

	if ( mxml_sgs ( xml_main, buffer, MXML_STYLE_INDENT ) != MXML_STATUS_OK )
	{
		printf ( "error writing XML data\n" );
		return NULL;
	}

	mxml_document_destroy ( xml_main );
	treenode_release ( treenode_top );

	return buffer->buffer;
}


unsigned int xml_connector_create_file ( void *treenode, char *filename, char *stylesheet )
{
	MXML_DOCUMENT *xml_main = NULL;
	t_treenode *treenode_top = NULL;
	MXML_NODE *xmlnode_child;
	MXML_ATTRIBUTE *attr;

	if ( !treenode || !filename )
		return E_FAIL;

	xml_main = mxml_document_new();

	if ( mxml_document_setup ( xml_main ) != MXML_STATUS_OK )
	{
		printf ( "could not setup xml document\n" );
		return E_FAIL;
	}

	treenode_top = treenode_create ();
	treenode_set_name ( treenode_top, "TOP" );
	treenode_append_child ( treenode_top, treenode_clone ( (t_treenode*)treenode ) );


	xmlnode_child = mxml_node_new ();
	mxml_node_setup ( xmlnode_child );
	xmlnode_child->name = strdup ( "xml version=\"1.0\" encoding=\"UTF-8\"" );
	xmlnode_child->type = MXML_TYPE_PI;

	mxml_node_add_below ( xml_main->root, xmlnode_child );

	if ( stylesheet )
	{
		xmlnode_child = mxml_node_new ();
		mxml_node_setup ( xmlnode_child );
		xmlnode_child->name = malloc ( strlen ( stylesheet ) + 100 );
		sprintf ( xmlnode_child->name, "xml-stylesheet type=\"text/xsl\" href=\"%s\"", stylesheet );
		xmlnode_child->type = MXML_TYPE_PI;

		mxml_node_add_below ( xml_main->root, xmlnode_child );
	}

	xml_connector_create_parse_children ( treenode_top, xml_main->root );

	//mxml_node

	if ( mxml_write ( xml_main, filename, MXML_STYLE_INDENT ) != MXML_STATUS_OK )
	{
		printf ( "error writing XML data to file %s\n", filename );
		return E_FAIL;
	}

	mxml_document_destroy ( xml_main );
	treenode_release ( treenode_top );

	return E_OK;
}



#endif