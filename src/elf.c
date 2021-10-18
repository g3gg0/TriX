#ifndef __TRIX_ELF_C__
#define __TRIX_ELF_C__

/*! \file elf.c
 */

#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "elf.h"
#include "fmt_elf.h"

#include "trixplug_funcs.h"
#include "mem.h"

ELF_PLUG_INIT;


unsigned int
elf_free_object ( t_elf_object *obj )
{
	t_elf_exports *exports = NULL;
	t_elf_imports *imports = NULL;

	exports = obj->exports;
	if ( exports )
	{
		while ( exports && exports->next )
		{
			exports = exports->next;
			free ( exports->prev->name );
			free ( exports->prev->position );
			free ( exports->prev );
		}
		free ( exports->name );
		free ( exports->position );
		free ( exports );
	}
	imports = obj->imports;
	if ( imports )
	{
		while ( imports->next )
		{
			imports = imports->next;
			free ( imports->prev->name );
			free ( imports->prev->position );
			free ( imports->prev->destination );
			free ( imports->prev );
		}
		free ( imports->name );
		free ( imports->position );
		free ( imports->destination );
		free ( imports );
	}
	free ( obj->name );
	free ( obj );

	return E_OK;
}


/*!
 * gets a specified segment (section) of the ELF object from the workspace.
 * \param ws workspace with .o file loaded into it
 * \param type type of section (see ELF section types)
 * \param num get the num'th section of that type (0=first..)
 * \return pointer to segment or NULL
 */
t_segment *
elf_get_tab ( t_workspace *ws, int type, int num )
{
	t_stage *stage = NULL;
	t_segment *segment = NULL;
	t_elf_sect_priv *priv = NULL;

	stage = stage_get_current ( ws->fileinfo->stages );
	if ( !stage )
		return NULL;

	segment = stage->segments;

	while ( segment )
	{
		priv = (t_elf_sect_priv*)segment->priv;
		if ( priv && (priv->header.sh_type == type || type == -1) )
		{
			if ( !num )
				return segment;
			else
				num--;
		}
		LIST_NEXT(segment);
	}
	return NULL;
}

/*!
 * gets a symtab from the workspace.
 * \param ws workspace with .o file loaded into it
 * \param num get the num'th section of that type (0=first..)
 * \return pointer to segment or NULL
 */
t_segment *
elf_get_symtab ( t_workspace *ws, int num )
{
	return elf_get_tab ( ws, SHT_SYMTAB, num );
}

/*!
 * gets a reloc from the workspace.
 * \param ws workspace with .o file loaded into it
 * \param num get the num'th section of that type (0=first..)
 * \return pointer to segment or NULL
 */
t_segment *
elf_get_reloc ( t_workspace *ws, int num )
{
	return elf_get_tab ( ws, SHT_REL, num );
}


/*!
 * gets a strtab from the workspace.
 * \param ws workspace with .o file loaded into it
 * \param num get the num'th section of that type (0=first..)
 * \return pointer to segment or NULL
 */
t_segment *
elf_get_strtab ( t_workspace *ws, int num )
{
	return elf_get_tab ( ws, SHT_STRTAB, num );
}


/*!
 * correct the given reloc header for current endianess.
 * \param source segment/stage (to know endianess)
 * \param header header to fix
 * \return pointer to segment or NULL
 */
unsigned int
elf_byteswap_rel ( t_priv *source, Elf32_Rel *header )
{
	header->r_offset = v_arch_w(source,header->r_offset);
	header->r_info = v_arch_w(source,header->r_info);

	return E_OK;
}

/*!
 * correct the given symbol header for current endianess.
 * \param source segment/stage (to know endianess)
 * \param header header to fix
 * \return pointer to segment or NULL
 */
unsigned int
elf_byteswap_sym ( t_priv *source, Elf32_Sym *header )
{
	header->st_name = v_arch_w(source,header->st_name);
	header->st_value = v_arch_w(source,header->st_value);
	header->st_size = v_arch_w(source,header->st_size);
	header->st_shndx = v_arch_h(source,header->st_shndx);

	return E_OK;
}


char *
elf_get_string ( t_workspace *ws, int string )
{
	int num = 0;
	t_segment *s = NULL;

	if ( !ws )
		return NULL;

	do
	{
		s = elf_get_strtab ( ws, num++ );
	} while ( s && strncmp ( s->name, ".strtab", 7 ) );

	if ( !s )
		return NULL;

	return v_get_str_raw ( s, string );
}

t_segment *
elf_get_section ( t_workspace *ws, int section )
{
	t_segment *seg = NULL;

	seg = elf_get_tab ( ws, -1, section );
	if ( !seg )
		return NULL;

	return seg;
}

unsigned int
fmt_elf_cify ( char *name )
{
	if ( !name )
		return E_FAIL;

	while ( *name )
	{
		if ( !isalnum(*name) && *name != '_' )
			*name = '_';
		name++;
	}

	return E_OK;
}



char *
elf_get_section_name ( t_workspace *ws, int section )
{
	t_segment *seg = NULL;

	seg = elf_get_tab ( ws, -1, section );
	if ( !seg )
		return NULL;

	return seg->name;
}

unsigned int
elf_parse_symtab ( t_workspace *ws, t_elf_object *obj, t_segment *tab )
{
	char buf[1024];
	Elf32_Sym symbol;
	t_elf_exports *exp = NULL;
	int data_pos = 0;
	int offset = 0;

	if ( !ws || !tab )
		return E_FAIL;


	while ( offset < tab->length )
	{
		v_memcpy_get_raw ( tab, &symbol, offset, sizeof ( Elf32_Sym ) );
		elf_byteswap_sym ( tab, &symbol );

		DBG ( DEBUG_ELF,  "bind: %02i  ", ELF32_ST_BIND(symbol.st_info) );
		switch ( ELF32_ST_BIND(symbol.st_info) )
		{
		case STB_LOCAL:
			DBG ( DEBUG_ELF,  "(Local)  " );
			break;
		case STB_GLOBAL:
			DBG ( DEBUG_ELF,  "(Global) " );
			break;
		case STB_WEAK:
			DBG ( DEBUG_ELF,  "(Weak)   " );
			break;
		case STB_LOPROC:
			DBG ( DEBUG_ELF,  "(Proc)   " );
			break;
		default:
			DBG ( DEBUG_ELF,  "(unkn)   " );
			break;
		}
		DBG ( DEBUG_ELF,  "section: %02i  ", symbol.st_shndx);
		DBG ( DEBUG_ELF,  "type: %02i  ", ELF32_ST_TYPE(symbol.st_info) );
		switch ( ELF32_ST_TYPE(symbol.st_info) )
		{
		case STT_NOTYPE:
			DBG ( DEBUG_ELF,  "(NoType) " );
			break;
		case STT_OBJECT:
			DBG ( DEBUG_ELF,  "(Object) " );
			break;
		case STT_FUNC:
			DBG ( DEBUG_ELF,  "(Func)   " );
			break;
		case STT_SECTION:
			DBG ( DEBUG_ELF,  "(Sect)   " );
			break;
		case STT_FILE:
			DBG ( DEBUG_ELF,  "(File)   " );
			obj->name = (char*)strdup ( elf_get_string ( ws, symbol.st_name ) );
			fmt_elf_cify ( obj->name );
			break;
		case STT_LOPROC:
			DBG ( DEBUG_ELF,  "(Export) " );
			break;
		default:
			DBG ( DEBUG_ELF,  "(unkn)   " );
			break;
		}
		DBG ( DEBUG_ELF,  "value: %04X  ", symbol.st_value);
		DBG ( DEBUG_ELF,  "Symbol: '%s'\n", elf_get_string ( ws, symbol.st_name ) );

		if ( obj )
		{
			/*
				Exported functions
			*/
			if ( ELF32_ST_BIND(symbol.st_info) == STB_GLOBAL &&
				 ELF32_ST_TYPE(symbol.st_info) == STT_LOPROC )
			{
				exp = LIST_NEW(obj->exports, t_elf_exports);
				if ( !obj->exports )
					obj->exports = exp;
				exp->name = (char*)strdup(elf_get_string ( ws, symbol.st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol.st_shndx ), symbol.st_value );
				exp->position = (char*)strdup(buf);
			}

			// ARM32 func
			if ( ELF32_ST_BIND(symbol.st_info) == STB_GLOBAL &&
				 ELF32_ST_TYPE(symbol.st_info) == STT_FUNC )
			{
				exp = LIST_NEW(obj->exports, t_elf_exports);
				if ( !obj->exports )
					obj->exports = exp;
				exp->name = (char*)strdup(elf_get_string ( ws, symbol.st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol.st_shndx ), symbol.st_value );
				exp->position = (char*)strdup(buf);
			}

			/*
				Exported variables
			*/
			if ( ELF32_ST_BIND(symbol.st_info) == STB_GLOBAL &&
				 ELF32_ST_TYPE(symbol.st_info) == STT_OBJECT )
			{
				exp = LIST_NEW(obj->exports, t_elf_exports);
				if ( !obj->exports )
					obj->exports = exp;
				exp->name = (char*)strdup(elf_get_string ( ws, symbol.st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol.st_shndx ), symbol.st_value );
				exp->position = (char*)strdup(buf);
			}
			if ( symbol.st_shndx == SHN_COMMON )
			{
				printf ( "ERROR: COMMON symbols like '%s' (uninitialized global variables) are not supported\n", elf_get_string ( ws, symbol.st_name ) );
				return E_FAIL;
			}
		}
		offset += sizeof ( Elf32_Sym );
	}

	return E_OK;
}



unsigned int
elf_dump_symtabs ( t_workspace *ws )
{
	int num = 0;
	t_segment *s = NULL;

	do
	{
		s = elf_get_symtab ( ws, num );
		if ( s )
		{
			DBG ( DEBUG_ELF, "Symtab\n" );
			elf_parse_symtab ( ws, NULL, s );
		}
		num++;
	} while ( s );

	return E_OK;
}



Elf32_Sym *
elf_get_symbol ( t_workspace *ws, int symbol_id )
{
	static Elf32_Sym symbol;
	int data_pos = 0;
	int offset = 0;
	int num = 0;
	int bind = 0;
	int type = 0;
	t_segment *sym = NULL;


	if ( !ws )
		return NULL;

	sym = elf_get_symtab ( ws, 0 );

	if ( !sym )
		return NULL;

	while ( offset < sym->length )
	{
		v_memcpy_get_raw ( sym, &symbol, offset, sizeof ( Elf32_Sym ) );
		elf_byteswap_sym ( sym, &symbol );
		if ( !symbol_id )
			return &symbol;
		else
			symbol_id--;

		offset += sizeof ( Elf32_Sym );
	}
	return NULL;
}


char *
elf_get_symbol_name ( t_workspace *ws, int symbol_id )
{
	Elf32_Sym *symbol = NULL;

	symbol = elf_get_symbol ( ws, symbol_id );
	if ( symbol )
		return elf_get_string ( ws, symbol->st_name );

	return NULL;
}


unsigned int
elf_parse_reloc ( t_workspace *ws, t_elf_object *obj, t_segment *rel )
{
	char buf[1024];
	Elf32_Rel reloc;
	Elf32_Sym *symbol;
	int data_pos = 0;
	int offset = 0;
	int type = 0;
	int bind = 0;
	int section = 0;
	int relative = 0;
	t_elf_imports *imp = NULL;

	if ( !ws || !rel )
		return E_FAIL;

	while ( offset < rel->length )
	{
		v_memcpy_get_raw ( rel, &reloc, offset, sizeof ( Elf32_Rel ) );
		elf_byteswap_rel ( rel, &reloc );

		symbol = elf_get_symbol ( ws, ELF32_R_SYM(reloc.r_info));
		if ( !symbol )
			return E_FAIL;
		type = ELF32_ST_TYPE(symbol->st_info);
		bind = ELF32_ST_BIND(symbol->st_info);

		switch ( bind )
		{
		case STB_LOCAL:
			DBG ( DEBUG_ELF,  "Reloc: internal  " );
			DBG ( DEBUG_ELF,  "section: %02i  ", symbol->st_shndx );
			DBG ( DEBUG_ELF,  "type: %02i  ", type );
			DBG ( DEBUG_ELF,  "offset: 0x%04X  ", reloc.r_offset );
			DBG ( DEBUG_ELF,  "data: 0x%08X ", v_get_w_raw(elf_get_section(ws, ((t_elf_sect_priv*)rel->priv)->header.sh_info), reloc.r_offset) );
			DBG ( DEBUG_ELF,  "name: '%s'\n", elf_get_symbol_name ( ws, ELF32_R_SYM(reloc.r_info)) );
			break;
		case STB_GLOBAL:
			DBG ( DEBUG_ELF,  "Reloc: external  " );
			DBG ( DEBUG_ELF,  "section: %02i  ", symbol->st_shndx );
			DBG ( DEBUG_ELF,  "type: %02i  ", type );
			DBG ( DEBUG_ELF,  "offset: 0x%04X  ", reloc.r_offset );
			DBG ( DEBUG_ELF,  "data: 0x%08X ", v_get_w_raw(elf_get_section(ws, ((t_elf_sect_priv*)rel->priv)->header.sh_info), reloc.r_offset) );
			DBG ( DEBUG_ELF,  "name: '%s'\n", elf_get_symbol_name ( ws, ELF32_R_SYM(reloc.r_info)) );
			break;
		case STB_WEAK:
			break;
		default:
			return E_FAIL;
			break;
		}

		if ( obj )
		{

			/*
				external variables or calls
			*/
			if ( bind == STB_GLOBAL && type == STT_NOTYPE && !symbol->st_shndx )
			{
				section = ((t_elf_sect_priv*)rel->priv)->header.sh_info;
				imp = LIST_NEW(obj->imports, t_elf_imports);
				if ( !obj->imports )
					obj->imports = imp;
				imp->name = (char*)strdup(elf_get_string ( ws, symbol->st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, section ), reloc.r_offset );
				imp->position = (char*)strdup(buf);

				if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xF7FFFFFE) == 0xF7FFFFFE )
				{
					if ( symbol->st_value )
						sprintf ( buf, "%s+0x%02X", elf_get_string ( ws, symbol->st_name ), symbol->st_value );
					else
						sprintf ( buf, "%s", elf_get_string ( ws, symbol->st_name ) );
					imp->type = "CALL";
				}
				else if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xEBFFFFFE) == 0xEBFFFFFE )
				{
					if ( symbol->st_value )
						sprintf ( buf, "%s+0x%02X", elf_get_string ( ws, symbol->st_name ), symbol->st_value );
					else
						sprintf ( buf, "%s", elf_get_string ( ws, symbol->st_name ) );
					imp->type = "ARM_CALL";
				}
				else
				{
					if ( symbol->st_value + v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) )
						sprintf ( buf, "%s+0x%02X", elf_get_string ( ws, symbol->st_name ), symbol->st_value + v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) );
					else
						sprintf ( buf, "%s", elf_get_string ( ws, symbol->st_name )  );
					imp->type = "PTR";
				}
				imp->destination =  (char*)strdup(buf);

//				imp->destination = (char*)strdup ( elf_get_string ( ws, symbol->st_name ) );
			}

			/*
				internal calls
			*/
			if ( type == STT_LOPROC && symbol->st_shndx )
			{
				section = ((t_elf_sect_priv*)rel->priv)->header.sh_info;
				imp = LIST_NEW(obj->imports, t_elf_imports);
				if ( !obj->imports )
					obj->imports = imp;
				imp->name = (char*)strdup(elf_get_string ( ws, symbol->st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, section ), reloc.r_offset );
				imp->position = (char*)strdup(buf);

				if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xF7FFFFFE) == 0xF7FFFFFE )
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value );
					imp->type = "CALL";
				}
				else if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xFFFEF7FF) == 0xFFFEF7FF )
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value );
					imp->type = "CALL";
				}
				else
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value + v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) );
					imp->type = "PTR";
				}
				imp->destination =  (char*)strdup(buf);

			}

			// ARM32 call
			if ( type == STT_FUNC && symbol->st_shndx )
			{
				section = ((t_elf_sect_priv*)rel->priv)->header.sh_info;
				imp = LIST_NEW(obj->imports, t_elf_imports);
				if ( !obj->imports )
					obj->imports = imp;
				imp->name = (char*)strdup(elf_get_string ( ws, symbol->st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, section ), reloc.r_offset );
				imp->position = (char*)strdup(buf);

				if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0x0A000000) == 0x0A000000 )
				{
					imp->type = "ARM_CALL";
					// ARM branch
					relative = arm_convert_to_int ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0x00FFFFFF), 24 );
					relative += 2;
					relative *= 4;

					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value + relative );
				}
				else
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value + v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) );
					imp->type = "UNK";
				}
				imp->destination =  (char*)strdup(buf);

			}

			/*
				internal variables
			*/
			if ( type == STT_OBJECT && symbol->st_shndx )
			{
				section = ((t_elf_sect_priv*)rel->priv)->header.sh_info;
				imp = LIST_NEW(obj->imports, t_elf_imports);
				if ( !obj->imports )
					obj->imports = imp;
				imp->name = (char*)strdup(elf_get_string ( ws, symbol->st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, section ), reloc.r_offset );
				imp->position = (char*)strdup(buf);

				if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xF7FFFFFE) == 0xF7FFFFFE )
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value );
					imp->type = "CALL";
				}
				else if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xFFFEF7FF) == 0xFFFEF7FF )
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value );
					imp->type = "CALL";
				}
				else
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value + v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) );
					imp->type = "PTR";
				}
				imp->destination =  (char*)strdup(buf);
			}

			/*
				internal anonymous variables
			*/
			if ( type == STT_SECTION && symbol->st_shndx )
			{
				section = ((t_elf_sect_priv*)rel->priv)->header.sh_info;
				imp = LIST_NEW(obj->imports, t_elf_imports);
				if ( !obj->imports )
					obj->imports = imp;
				imp->name = (char*)strdup(elf_get_string ( ws, symbol->st_name ));
				sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, section ), reloc.r_offset );
				imp->position = (char*)strdup(buf);

				if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xF7FFFFFE) == 0xF7FFFFFE )
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value );
					imp->type = "CALL";
				}
				else if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0xFFFEF7FF) == 0xFFFEF7FF )
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value );
					imp->type = "CALL";
				}
				else if ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0x0A000000) == 0x0A000000 )
				{
					// ARM branch
					relative = arm_convert_to_int ( (v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) & 0x00FFFFFF), 24 );
					relative += 2;
					relative *= 4;

					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value + relative);
					imp->type = "ARM_BRANCH";
				}
				else
				{
					sprintf ( buf, "%s+0x%02X", elf_get_section_name ( ws, symbol->st_shndx ), symbol->st_value + v_get_w_raw(elf_get_section(ws, section), reloc.r_offset) );
					imp->type = "PTR";
				}
				imp->destination =  (char*)strdup(buf);
			}

		}
		offset += sizeof ( Elf32_Rel );
	}

	return E_OK;
}



unsigned int
elf_dump_relocs ( t_workspace *ws )
{
	int num = 0;
	t_segment *s = NULL;

	do
	{
		s = elf_get_reloc ( ws, num );
		if ( s )
			elf_parse_reloc ( ws, NULL, s );
		num++;
	} while ( s );

	return E_OK;
}

unsigned int
elf_section_used ( t_workspace *ws, t_elf_object *obj, t_segment *seg )
{
	t_elf_imports *imp = NULL;
	t_elf_exports *exp = NULL;

	if ( !ws || !obj || !seg || !seg->name || !strlen (seg->name) )
		return 0;

	exp = obj->exports;
	while ( exp )
	{
		if ( !strncmp ( exp->position, seg->name, strlen(seg->name) ) )
			return 1;
		LIST_NEXT(exp);
	}
	imp = obj->imports;
	while ( imp )
	{
		if ( !strncmp ( imp->position, seg->name, strlen(seg->name) ) )
			return 1;
		if ( !strncmp ( imp->destination, seg->name, strlen(seg->name) ) )
			return 1;
		LIST_NEXT(imp);
	}
	return 0;
}

/*!
 * creates an object from *.o loaded into a workspace\n
 * you have to free the object and all strings (util_free_object)
 * \param ws workspace with .o file loaded into it
 * \return pointer to object or NULL
 */
object *
elf_create_object ( t_workspace *ws )
{
    int num = 0;
    int i = 0;
    int allocated = 0;
    int pos = 0;
    t_segment *s = NULL;
    t_elf_object *obj = NULL;
    t_elf_imports *imp = NULL;
	t_elf_exports *exp = NULL;
    object *o = NULL;

    if ( !ws || !ws->fileinfo || !ws->fileinfo->stages )
		return NULL;

    if ( !stage_get_current ( ws->fileinfo->stages ) )
		return NULL;

    if ( ((t_elf_priv*)(stage_get_current ( ws->fileinfo->stages )->priv))->header.e_machine != ELFARCH_ARM )
        return NULL;


    obj = LIST_MALLOC(t_elf_object);
    obj->exports = NULL;
    obj->imports = NULL;
    obj->name = NULL;

    num = 0;
	s = elf_get_symtab ( ws, num );
	while ( s )
	{
		elf_parse_symtab ( ws, obj, s );
		num++;
		s = elf_get_symtab ( ws, num );
	};

	num = 0;
	s = elf_get_reloc ( ws, num );
	while ( s )
	{
		elf_parse_reloc ( ws, obj, s );
		num++;
		s = elf_get_reloc ( ws, num );
	};

	//printf ( "object %s = \n{\n", obj->name );
	allocated += 3;
	o = (object*)realloc ( o, sizeof ( char* ) * allocated );
	o[allocated-3] = (char*)strdup ( "name" );
	o[allocated-2] = (char*)strdup ( obj->name );
	o[allocated-1] = (char*)strdup ( "" );

	num = 0;
	s = elf_get_section ( ws, num );
	while ( s )
	{
		if ( elf_section_used ( ws, obj, s ) )
		{
		    allocated += 3;
		    o = (object*)realloc ( o, sizeof ( char* ) * allocated );
		    o[allocated-3] = (char*)strdup ( s->name );
		    o[allocated-2] = (char*)malloc ( s->length );
		    memcpy ( o[allocated-2], s->data, s->length );

//			pos = 0;
//			i = 0;
//			while ( pos < s->length )
//			{
////				if ( s->data[pos] >= 0x20 && s->data[pos] <= 0x7F )
////				{
//				    o[allocated-2] = (char*)realloc ( o[allocated-2], i+1 );
//				    o[allocated-2][i] = s->data[pos++];
//					//printf ( "%c", s->data[pos++] );
////				}
////				else
////				{
////
////					printf ( "\\x%02X", s->data[pos++] );
////				}
//			}
            o[allocated-1] = (char*)malloc ( 30 );
			sprintf ( o[allocated-1], "%i", s->length );
		}
		num++;
		s = elf_get_section ( ws, num );
	}

	exp = obj->exports;
	while ( exp )
	{
        allocated += 3;
        o = (object*)realloc ( o, sizeof ( char* ) * allocated );
        o[allocated-3] = (char*)strdup ( "!SYM_EXP" );
        o[allocated-2] = (char*)strdup ( exp->name );
        o[allocated-1] = (char*)strdup ( exp->position );

		//printf ( "\t\"!SYM_EXP\",\t\"%s\",\t\"%s\",\n", exp->name, exp->position );
		LIST_NEXT(exp);
	}
	imp = obj->imports;
	while ( imp )
	{
	    allocated += 3;
        o = (object*)realloc ( o, sizeof ( char* ) * allocated );
        o[allocated-3] = (char *) malloc ( 5 + strlen ( imp->type ) + 1 );
        sprintf ( o[allocated-3], "!SYM_%s", imp->type );
        o[allocated-2] = (char*)strdup ( imp->destination );
        o[allocated-1] = (char*)strdup ( imp->position );

//		printf ( "\t\"!SYM_%s\",\t\"%s\",\t\"%s\",", imp->type, imp->destination, imp->position );
//		if ( !strcmp ( imp->destination, imp->name ) || strlen ( imp->name ) == 0 )
//			printf ( "\n" );
//		else
//			printf ( "\t/* %s */\n", imp->name );
		LIST_NEXT(imp);
	}
//	printf ( "\tNULL\n};\n" );

    allocated++;
    o = (object*)realloc ( o, sizeof ( char* ) * allocated );
    o[allocated-1] = NULL;

	elf_free_object ( obj );

	return o;

}

/*!
 * dumps an object from *.o loaded into a workspace
 * \param ws workspace with .o file loaded into it
 * \return E_FAIL (not dumped ) or E_OK (dumped)
 */
unsigned int
elf_dump_object ( t_workspace *ws )
{
	unsigned int num = 0;
	unsigned int pos = 0;
	t_segment *s = NULL;
	t_stage *stage = NULL;
	t_elf_object *obj = NULL;
	t_elf_imports *imp = NULL;
	t_elf_exports *exp = NULL;
	t_elf_priv *priv = NULL;

	if ( !ws || !ws->fileinfo || !ws->fileinfo->stages )
		return E_FAIL;

	stage = stage_get_current ( ws->fileinfo->stages );
	if ( !stage )
		return E_FAIL;
	priv = (t_elf_priv*)stage->priv;

	if ( priv->header.e_machine != ELFARCH_ARM )
		return E_FAIL;


	obj = LIST_MALLOC(t_elf_object);
	obj->exports = NULL;
	obj->imports = NULL;
	obj->name = NULL;

	num = 0;
	s = elf_get_symtab ( ws, num );
	while ( s )
	{
		R(elf_parse_symtab ( ws, obj, s ));
		num++;
		s = elf_get_symtab ( ws, num );
	};


	num = 0;
	s = elf_get_reloc ( ws, num );
	while ( s )
	{
		R(elf_parse_reloc ( ws, obj, s ));
		num++;
		s = elf_get_reloc ( ws, num );
	};


	printf ( "object %s = \n{\n", obj->name );

	num = 0;
	s = elf_get_section ( ws, num );
	while ( s )
	{
		if ( elf_section_used ( ws, obj, s ) )
		{
			printf ( "\t\"%s\",\t\"", s->name );
			pos = 0;
			while ( pos < s->length )
			{
				if ( s->data[pos] >= 0x20 && s->data[pos] <= 0x7F )
					printf ( "%c", s->data[pos++] );
				else
					printf ( "\\x%02X", s->data[pos++] );
			}
			printf ( "\",\t\"%i\",\n", s->length );
		}
		num++;
		s = elf_get_section ( ws, num );
	}

	exp = obj->exports;
	while ( exp )
	{
		printf ( "\t\"!SYM_EXP\",\t\"%s\",\t\"%s\",\n", exp->name, exp->position );
		LIST_NEXT(exp);
	}
	imp = obj->imports;
	while ( imp )
	{
		printf ( "\t\"!SYM_%s\",\t\"%s\",\t\"%s\",", imp->type, imp->destination, imp->position );
		if ( !strcmp ( imp->destination, imp->name ) || strlen ( imp->name ) == 0 )
			printf ( "\n" );
		else
			printf ( "\t/* %s */\n", imp->name );
		LIST_NEXT(imp);
	}
	printf ( "\tNULL\n};\n" );

	elf_free_object ( obj );
	return E_OK;
}




#endif
