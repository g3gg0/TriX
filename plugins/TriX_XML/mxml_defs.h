/*
   Mini XML lib
   Header files

   Author: Giancarlo Niccolai <gian@niccolai.ws>

   MXML standard definitions

   $Id: mxml_defs.h 2762 2008-03-06 21:42:00Z geggo $
*/

#ifndef MXML_DEFS_H
#define MXML_DEFS_H

/* Standard definitions */
#define MXML_LINE_TERMINATOR      '\n'
#define MXML_SOFT_LINE_TERMINATOR '\r'
#define MXML_PATH_SEPARATOR       '/'
#define MXML_EOF                  -256

#define MXML_FILE_BLOCK_SIZE  2048
#define MXML_MAX_NAME_LEN     128
#define MXML_MAX_ATTRIB_LEN   256
#define MXML_ALLOC_BLOCK      128
#define MXML_MAX_DEPTH        64

/* Styles */
#define MXML_STYLE_INDENT        0x0001
#define MXML_STYLE_TAB           0x0002
#define MXML_STYLE_THREESPACES   0x0004
#define MXML_STYLE_NOESCAPE      0x0008

/* Status vaules */

typedef enum
{
   MXML_STATUS_ERROR=0,
   MXML_STATUS_OK=1,
   MXML_STATUS_MORE,
   MXML_STATUS_DONE,
   MXML_STATUS_UNDEFINED,
   MXML_STATUS_MALFORMED
} MXML_STATUS;

/* Error codes */

typedef enum
{
   MXML_ERROR_NONE = 0,
   MXML_ERROR_IO = 1,
   MXML_ERROR_NOMEM,

   MXML_ERROR_OUTCHAR,
   MXML_ERROR_INVNODE,
   MXML_ERROR_INVATT,
   MXML_ERROR_MALFATT,
   MXML_ERROR_INVCHAR,
   MXML_ERROR_NAMETOOLONG,
   MXML_ERROR_ATTRIBTOOLONG,
   MXML_ERROR_VALATTOOLONG,
   MXML_ERROR_UNCLOSED,
   MXML_ERROR_UNCLOSEDENTITY,
   MXML_ERROR_WRONGENTITY
} MXML_ERROR_CODE;

/* Iterator modes */

typedef enum
{
   MXML_ITERATOR_MODE_FORWARD=0,
   MXML_ITERATOR_MODE_BACKWARD
} MXML_ITERATOR_MODE;

/* Node types */

typedef enum
{
   MXML_TYPE_TAG=0,
   MXML_TYPE_COMMENT,
   MXML_TYPE_PI,
   MXML_TYPE_DIRECTIVE,
   MXML_TYPE_DATA,
   MXML_TYPE_DOCUMENT   // used for document level root node
} MXML_NODE_TYPE;

/* Refil function */
struct tag_mxml_refil;
struct tag_mxml_output;

typedef void (*MXML_REFIL_FUNC)( struct tag_mxml_refil *ref );
typedef void (*MXML_OUTPUT_FUNC)( struct tag_mxml_output *out, char *data, int len );

/*************************************************
   Structures holding the XML data
**************************************************/

/* Attribute */
typedef struct tag_mxml_attribute
{
   char *name;
   char *value;
   struct tag_mxml_attribute *next;
} MXML_ATTRIBUTE;

/* Node */
typedef struct tag_mxml_node
{
   char *name;
   char *data;
   int data_length;
   MXML_NODE_TYPE type;
   struct tag_mxml_attribute *attributes;
   struct tag_mxml_node *next;
   struct tag_mxml_node *prev;
   struct tag_mxml_node *child;
   struct tag_mxml_node *last_child;
   struct tag_mxml_node *parent;
} MXML_NODE;

/* Index for faster node retrival */

typedef struct tag_mxml_index
{
   int length;
   int allocated;
   MXML_NODE *data;
} MXML_INDEX;

/* XML document */

typedef struct tag_mxml_document
{
   char *name;
   MXML_NODE *root;
   MXML_STATUS status;
   MXML_INDEX *index;
   MXML_ERROR_CODE error;
   //this is mainly for stats & progress
   int node_count;
   int iLine;
} MXML_DOCUMENT;

/* Iterator */

typedef struct tag_mxml_iterator
{
   MXML_DOCUMENT *document;
   MXML_NODE *node;
   MXML_NODE *root_node;
   int level;
   MXML_ITERATOR_MODE mode;
} MXML_ITERATOR;

/* Refiller */

typedef struct tag_mxml_refil
{
   // status variables
   MXML_STATUS status;
   MXML_ERROR_CODE error;

   // buffer for reading data
   unsigned char *buffer;
   int bufsize;  // size of the whole buffer
   int buflen;   // valid characters in the current buffer
   int bufpos;   // current position

   // lenght of the stream for implementing progress indicators
   long int streampos;
   long int streamlen;

   // callback funcs
   MXML_REFIL_FUNC refil_func;

   // ungetc implementation
   int sparechar;

   // data available for callback functions
   void *data;

} MXML_REFIL;


typedef struct tag_mxml_output
{
   // status variables
   MXML_STATUS status;
   MXML_ERROR_CODE error;

   // output operation
   MXML_OUTPUT_FUNC  output_func;

   // data to implement progress indicators
   int node_count;
   int node_done;

   // data available for callback functions
   void *data;

} MXML_OUTPUT;


typedef struct tag_mxml_self_growing_string
{
   char *buffer;
   int allocated;
   int length;
} MXML_SGS;

typedef struct 
{
	unsigned char *buffer;
	unsigned int pos;
	unsigned int length;
} t_buf_refill;
#endif
