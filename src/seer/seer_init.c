#ifndef __TRIX_SEER_INIT_C__
#define __TRIX_SEER_INIT_C__

#include "seer_auto.h"



void scAdd_Internal_Header ( char *name, char *addr );
void scAdd_External_Symbol_Int ( char *name, void *addr );

#define scAddExtSymInt(a) scAdd_External_Symbol_Int(#a,&a);

unsigned int
seer_init_trix (  )
{
	scAdd_Internal_Header ( "trix", 
		"#include stdlib\n"
		"#define E_FAIL  0xFFFFFFFF\n"
		"#define E_OK    0\n"
		"export main;\n"
		"#define STRUCT_HEADER int struct_magic; char *struct_name; int struct_size; int struct_refs; int flags\n"
		"#define STRUCT_HEADER_LIST(type) STRUCT_HEADER; void *prev; void *next\n"
		"#define LIST_COUNT(x)  __util_list_count ( (t_list *)x )\n"\
		"#define LIST_END(x)    __util_list_get_last ( (t_list *)x )\n"\
		"#define LIST_FFWD(x)   while(x&&x->next) x=x->next\n"\
		"#define LIST_REWND(x)  while(x&&x->prev) x=x->prev\n"\
		"#define LIST_ADD(x,y)  __util_list_add ( (t_list *)x, (t_list *)y  )\n"\
		"#define LIST_NEW(x,y)  (y*)__util_list_add_new (  (t_list *)x, sizeof(y), #y )\n"\
		"#define LIST_PREV(x)   x=x->prev\n"\
		"#define LIST_NEXT(x)   x=x->next\n"\
		"#define LIST_FREE(x)   __util_list_free ( (t_list *)x ); x = NULL\n"\
		"#define LIST_MALLOC(x) (x*)__util_smalloc ( sizeof(x), #x )\n"\
		"#define PRIV_MALLOC(x) (x*)__util_smalloc ( sizeof(x), #x )\n"\
		"#define FLAGS_ENDIANESS    0x00030000\n"\
		"#define FLAGS_ENDIANESS_LE 0x00010000\n"\
		"#define FLAGS_ENDIANESS_BE 0x00020000\n"\
		"#define FLAGS_FREE_NAME    0x00040000\n"\
		"#define FLAGS_REPLACE      0x00080000\n"\
		"#define FLAGS_HIDE_IN_MEM  0x00100000\n"\
		"#define FLAGS_MAP_IN_MEM   0x00200000\n"\
		"#define FLAGS_MAP_ANY      0x00300000\n"\
		"#define FLAGS_SPARSE       0x00400000\n"\
		"#define FLAGS_SHADOW       0x00800000\n"\
		"import int say(char *,...);\n"
		"import int printf(char *,...);\n"
		"import int sprintf(char*,char *,...);\n"
		"import int scanf(char *,...);\n"
		"import int sscanf(char *,...);\n"
		"import int getchar();\n"
		"import int rand();\n"
		"import int srand(int seed);\n"
		"import int strlen(char *);\n"
		"import int strcpy(char *,char *);\n"
		"import int memcmp(char *,char *, int);\n"
		"import int memcpy(char *,char *, int);\n"
		"import int memmove(char *,char *, int);\n"
		"import int memset(char *, int, int);\n"
		"import int strcmp(char *,char *);\n"
		"import int strncmp(char *,char *, int);\n"
		"import int strtol(char *,char **,int);\n"
		"import int atoi(char *);\n"
		"import double atof(char *);\n"
		"import int time ( void *struct );\n"
		"import unsigned int strtoul(char *,char **,int);\n"
		"import double strtod(char *,char **);\n"
		"import char *strdup(char *);\n"
		"import void* malloc(int);\n"
		"import void* calloc(int,int);\n"
		"import void* realloc(void*,int);\n"
		"import char *str (char *);\n"
		"import void free(void *);\n"
		"import char *strcat(char *dest, const char *src);\n"
		"import int system ( char * );\n"
		"import int http_put ( char *filename, char *data, int length, int overwrite, char *type );\n"
		"import int http_parse_url ( char *url, char **pfilename );\n"

		"int int_strcpy(char *dest, char *src)\n"
		"{\n"
		"	if ( !dest || !src )\n"
		"		return 0;\n"
		"	while ( *src )\n"
		"		*(dest++) = *(src++);\n"
		"	*dest = 0;\n"
		"	return 1;\n"
		"}\n"
		"char *int_strcat(char *dest, char *src)\n"
		"{\n"
		"	if ( !dest || !src )\n"
		"		return 0;\n"
		"	while ( *dest )\n"
		"		dest++;\n"
		"	while ( *src )\n"
		"		*(dest++) = *(src++);\n"
		"	*dest = 0;\n"
		"	return 1;\n"
		"}\n"
		TRIX_AUTO_IMPORT
		);

	TRIX_AUTO_REGISTER;

	return 0;
}
#endif
