#ifndef __TRIX_FMT_ELF_C__
#define __TRIX_FMT_ELF_C__

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

#include "mem.h"


unsigned int
fmt_elf_free ( t_stage * s )
{
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	return 0;
}

unsigned int fmt_elf_add_segment ( t_stage *source, t_stage *target, char *name, Elf32_Phdr header )
{
	t_segment *s = NULL;
	t_elf_sect_priv *priv = NULL;
	unsigned char *data = NULL;


	if ( header.p_memsz > 0x03FFFFFF )
    {
		DBG ( DEBUG_FMT_ELF,  "Segment <%s> too big!\n", name );
        return E_FAIL;
    }

	data = malloc ( header.p_memsz + 1 );
	if ( !data )
		return E_FAIL;

    memset ( data, 0xFF, header.p_memsz );
	v_memcpy_get_raw ( source, data, source->segments->start + header.p_offset, header.p_filesz );

	if ( !target->segments )
	{
		target->segments = segment_create ();
		s = target->segments;
	}
	else
		s = segment_add ( target->segments );

	s->name = strdup ( name );
	s->start = header.p_vaddr;

	s->length = header.p_memsz;
    s->end = header.p_vaddr + s->length;
    s->data = data;
	s->flags = FLAGS_FREE_NAME | target->flags;

	segment_map_in_mem ( s );

	s->priv = PRIV_MALLOC(t_elf_prog_priv);
	priv = s->priv;

	memcpy ( &priv->header, &header, sizeof ( Elf32_Phdr ) );

	return E_OK;
}

unsigned int
fmt_elf_add_section ( t_stage *source, t_stage *target, char *name, Elf32_Shdr header )
{
	t_segment *s = NULL;
	t_elf_sect_priv *priv = NULL;
	unsigned char *data = NULL;

//	DBG ( DEBUG_FMT_ELF,  "adding section: '%s'\n", name );

    if ( header.sh_size > 0x03FFFFFF )
    {
		DBG ( DEBUG_FMT_ELF,  "Section <%s> too big!\n", name );
        return E_FAIL;
    }
    
	data = malloc ( header.sh_size + 1 );
	if ( !data )
		return E_FAIL;

    // in case of a NOBITS section, just allocate 0-bytes
    if ( header.sh_type != SHT_NOBITS )
        v_memcpy_get_raw ( source, data, source->segments->start + header.sh_offset, header.sh_size );
    else
        memset ( data, 0x00, header.sh_size );

	if ( !target->segments )
	{
		target->segments = segment_create ();
		s = target->segments;
	}
	else
		s = segment_add ( target->segments );
	
	s->name = strdup ( name );
	s->start = header.sh_addr;
    
    s->end = header.sh_addr + header.sh_size;
	s->length = header.sh_size;
    s->data = data;
	s->flags = FLAGS_FREE_NAME | target->flags;

    // when this section has no data, dont map it into memory. but dont map if its neither allocated, executable nor writeable
	if ( ((header.sh_type == SHT_PROGBITS) || (header.sh_type == SHT_NOBITS)) && (header.sh_flags != 0) )
		segment_map_in_mem ( s );
    else
		segment_hide_in_mem ( s );
        
	s->priv = PRIV_MALLOC(t_elf_sect_priv);
	priv = s->priv;

	memcpy ( &priv->header, &header, sizeof ( Elf32_Shdr ) );

	return E_OK;
}


char *
fmt_elf_get_section_string ( t_stage *source, t_stage *target, unsigned int string )
{
	Elf32_Shdr header;
	unsigned int data_pos = 0;
	t_elf_priv *priv;

	if ( !target || !target->priv )
		return NULL;

	priv = target->priv;
	if ( !priv->hdr_str_offset )
		return NULL;

	v_memcpy_get_raw ( source, &header, source->segments->start + priv->hdr_str_offset, sizeof(Elf32_Shdr) );

	data_pos = v_arch_w ( source, header.sh_offset );
	if ( !data_pos || data_pos > source->segments->length )
		return NULL;

	return v_get_str_raw ( source, source->segments->start + data_pos + string );
}


unsigned int
fmt_elf_byteswap_ehdr ( t_stage *source, Elf32_Ehdr *header )
{
	header->e_type = v_arch_h(source,header->e_type);
	header->e_machine = v_arch_h(source,header->e_machine);
	header->e_version = v_arch_w(source,header->e_version);
	header->e_entry = v_arch_w(source,header->e_entry);
	header->e_phoff = v_arch_w(source,header->e_phoff);
	header->e_shoff = v_arch_w(source,header->e_shoff);
	header->e_flags = v_arch_w(source,header->e_flags);
	header->e_ehsize = v_arch_h(source,header->e_ehsize);
	header->e_phentsize = v_arch_h(source,header->e_phentsize);
	header->e_phnum = v_arch_h(source,header->e_phnum);
	header->e_shentsize = v_arch_h(source,header->e_shentsize);
	header->e_shnum = v_arch_h(source,header->e_shnum);
	header->e_shstrndx = v_arch_h(source,header->e_shstrndx);

	return E_OK;
}

unsigned int fmt_elf_byteswap_phdr ( t_stage *source, Elf32_Phdr *header )
{
	header->p_type = v_arch_w(source,header->p_type);
	header->p_offset = v_arch_w(source,header->p_offset);
	header->p_vaddr = v_arch_w(source,header->p_vaddr);
	header->p_paddr = v_arch_w(source,header->p_paddr);
	header->p_filesz = v_arch_w(source,header->p_filesz);
	header->p_memsz = v_arch_w(source,header->p_memsz);
	header->p_flags = v_arch_w(source,header->p_flags);
	header->p_align = v_arch_w(source,header->p_align);

	return E_OK;
}

unsigned int
fmt_elf_byteswap_shdr ( t_stage *source, Elf32_Shdr *header )
{
	header->sh_name = v_arch_w(source,header->sh_name);
	header->sh_type = v_arch_w(source,header->sh_type);
	header->sh_flags = v_arch_w(source,header->sh_flags);
	header->sh_addr = v_arch_w(source,header->sh_addr);
	header->sh_offset = v_arch_w(source,header->sh_offset);
	header->sh_size = v_arch_w(source,header->sh_size);
	header->sh_link = v_arch_w(source,header->sh_link);
	header->sh_info = v_arch_w(source,header->sh_info);
	header->sh_addralign = v_arch_w(source,header->sh_addralign);
	header->sh_entsize = v_arch_w(source,header->sh_entsize);

	return E_OK;
}

static const char * get_segment_type ( unsigned int p_type )
{
	switch ( p_type )
    {
		case PT_NULL:
			return "NULL";
		case PT_LOAD:
			return "LOAD";
		case PT_DYNAMIC:
			return "DYNAMIC";
		case PT_INTERP:
			return "INTERP";
		case PT_NOTE:
			return "NOTE";
		case PT_SHLIB:
			return "SHLIB";
		case PT_PHDR:
			return "PHDR";
		case PT_TLS:
			return "TLS";

		default:
			return "UNK";
	}
}

unsigned int fmt_elf_parse_segments ( t_stage *source, t_stage *target, unsigned int start_offset, unsigned int headers, unsigned int header_size )
{
	unsigned int i = 0;
	Elf32_Phdr header;
	unsigned int offset = start_offset;
	char *name = NULL;

	if ( !source || !target )
		return E_FAIL;

	// TODO: add debug info

	while ( headers )
	{
		v_memcpy_get_raw ( source, &header, source->segments->start + offset, sizeof(Elf32_Phdr) );
		fmt_elf_byteswap_phdr ( source, &header );

		name = get_segment_type ( header.p_type );

		R ( fmt_elf_add_segment ( source, target, name, header ) );

		offset += header_size;
		headers--;
	}

	return E_OK;
}

unsigned int
fmt_elf_parse_sections ( t_stage *source, t_stage *target, unsigned int start_offset, unsigned int headers, unsigned int header_size )
{
	unsigned int i = 0;
	Elf32_Shdr header;
	unsigned int offset = start_offset;
	char *name = NULL;

	if ( !source || !target )
		return E_FAIL;

	while ( headers )
	{
		v_memcpy_get_raw ( source, &header, source->segments->start + offset, sizeof(Elf32_Shdr) );
		fmt_elf_byteswap_shdr ( source, &header );

		name = fmt_elf_get_section_string (source, target, header.sh_name );
		DBG ( DEBUG_FMT_ELF,  "Section  addr: 0x%X@%i  ", header.sh_addr, header.sh_addralign );
		DBG ( DEBUG_FMT_ELF,  "Type: %i  ", header.sh_type );
		switch ( header.sh_type )
		{
			case SHT_NULL:
				DBG ( DEBUG_FMT_ELF,  "NULL     ");
				break;
			case SHT_PROGBITS:
				DBG ( DEBUG_FMT_ELF,  "Progbits ");
				break;
			case SHT_NOTE:
				DBG ( DEBUG_FMT_ELF,  "Note     ");
				break;
			case SHT_STRTAB:
				DBG ( DEBUG_FMT_ELF,  "StrTab   ");
				break;
			case SHT_NOBITS:
				DBG ( DEBUG_FMT_ELF,  "Nobits   ");
				break;
			case SHT_SHLIB:
				DBG ( DEBUG_FMT_ELF,  "SHlib    ");
				break;
			case SHT_DYNAMIC:
				DBG ( DEBUG_FMT_ELF,  "Dynamic  ");
				break;
			case SHT_HASH:
				DBG ( DEBUG_FMT_ELF,  "Hash     ");
				break;
			case SHT_REL:
				DBG ( DEBUG_FMT_ELF,  "Reloc    ");
				break;
			case SHT_RELA:
				DBG ( DEBUG_FMT_ELF,  "RelocA   ");
				break;
			case SHT_SYMTAB:
				DBG ( DEBUG_FMT_ELF,  "Symtab   ");
				break;
			case SHT_DYNSYM:
				DBG ( DEBUG_FMT_ELF,  "DynSym   ");
				break;
			default:
				DBG ( DEBUG_FMT_ELF,  "Unknown  ");
				break;

		}
		DBG ( DEBUG_FMT_ELF,  "Name : '%s'\n", name );
		DBG ( DEBUG_FMT_ELF,  "Info : %i  ", header.sh_info );
		DBG ( DEBUG_FMT_ELF,  "Flags: %c%c%c  ", (header.sh_flags&SHF_WRITE)?'W':'-', (header.sh_flags&SHF_ALLOC)?'A':'-', (header.sh_flags&SHF_EXECINSTR)?'X':'-' );

		R(fmt_elf_add_section ( source, target, name, header ));

		offset += header_size;
		headers--;
	}

	return E_OK;
}

static unsigned int fmt_elf_parse ( t_stage *source, t_stage *target )
{
	unsigned int i = 0;
	Elf32_Ehdr header;
	t_elf_priv *priv;

	v_memcpy_get_raw ( source, &header, source->segments->start, sizeof (Elf32_Ehdr) );

//	DBG ( DEBUG_FMT_ELF,  "ident: '%s'\n", header.e_ident );
	if ( !(header.e_ident[EI_MAG0] == ELFMAG0 &&
	       header.e_ident[EI_MAG1] == ELFMAG1 &&
	       header.e_ident[EI_MAG2] == ELFMAG2 &&
	       header.e_ident[EI_MAG3] == ELFMAG3) )
	{
		DBG ( DEBUG_FMT_ELF,  "Unknown file\n" );
		return E_FAIL;
	}

	DBG ( DEBUG_FMT_ELF,  "ELF File,  " );
	switch ( header.e_ident[EI_CLASS] )
	{
		case ELFCLASS32:
			DBG ( DEBUG_FMT_ELF,  "32bit  " );
			break;
		case ELFCLASS64:
			DBG ( DEBUG_FMT_ELF,  "64bit  " );
			break;
		case ELFCLASSNONE:
		default:
			DBG ( DEBUG_FMT_ELF,  "invalid\n" );
			return E_FAIL;
			break;

	}

	switch ( header.e_ident[EI_DATA] )
	{
		case ELFDATA2LSB:
			DBG ( DEBUG_FMT_ELF,  "Little Endian\n" );
			source->flags &= ~FLAGS_ENDIANESS;
			source->flags |= FLAGS_ENDIANESS_LE;
			target->flags &= ~FLAGS_ENDIANESS;
			target->flags |= FLAGS_ENDIANESS_LE;
			break;
		case ELFDATA2MSB:
			DBG ( DEBUG_FMT_ELF,  "Big Endian\n" );
			source->flags &= ~FLAGS_ENDIANESS;
			source->flags |= FLAGS_ENDIANESS_BE;
			target->flags &= ~FLAGS_ENDIANESS;
			target->flags |= FLAGS_ENDIANESS_BE;
			break;
		case ELFDATANONE:
		default:
			DBG ( DEBUG_FMT_ELF,  "invalid\n" );
			return E_FAIL;
			break;
	}

	// now we know the endianess... prepare the rest of the struct
	fmt_elf_byteswap_ehdr ( source, &header );

	DBG ( DEBUG_FMT_ELF,  "Program Headers start at: 0x%08X  count: %i  each: %i bytes\n", v_arch_w(NULL, header.e_phoff), v_arch_h(NULL, header.e_phnum), v_arch_h(NULL, header.e_phentsize) );
	DBG ( DEBUG_FMT_ELF,  "Section Headers start at: 0x%08X  count: %i  each: %i bytes\n", v_arch_w(NULL, header.e_shoff), v_arch_h(NULL, header.e_shnum), v_arch_h(NULL, header.e_shentsize) );
	DBG ( DEBUG_FMT_ELF,  "Table %i holds strings\n", v_arch_h(NULL,header.e_shstrndx) );
	DBG ( DEBUG_FMT_ELF,  "Entry point at:           0x%08X\n", v_arch_w(NULL, header.e_entry) );

	if ( ( !header.e_shoff || !header.e_shnum ) && ( !header.e_phnum ) )
	{
		DBG ( DEBUG_FMT_ELF,  "No segments and sections in object\n" );
		return E_FAIL;
	}

   	target->priv = PRIV_MALLOC(t_elf_priv);
	priv = target->priv;
	memcpy ( &priv->header, &header, sizeof ( Elf32_Ehdr ) );
	priv->hdr_str_offset = source->segments->start + header.e_shoff+header.e_shstrndx*header.e_shentsize;

	fmt_elf_parse_segments ( source, target, header.e_phoff, header.e_phnum, header.e_phentsize );
	fmt_elf_parse_sections ( source, target, header.e_shoff, header.e_shnum, header.e_shentsize );

	return E_OK;
}

unsigned int fmt_elf_decode ( t_stage *source, t_stage *target )
{

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

    if ( !source || !target || !source->segments || !source->segments->data )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

	if ( source->length < EI_NIDENT )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

	if ( target->segments )
		segment_release ( target->segments );

	if ( fmt_elf_parse (  source,  target ) != E_OK )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

    target->name = "PARSED";
    target->length = 0;
    target->parser = "ELF";
    target->type = "ELF";

	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

static unsigned int fmt_elf_create ( t_stage *source, t_stage *target )
{
	t_elf_priv *epriv;
	t_elf_prog_priv *ppriv;
	t_segment *ts = NULL;
	char *data = NULL;
	unsigned int id, length;
	unsigned int phnum, shnum;

	epriv = (t_elf_priv *)source->priv;
	phnum = epriv->header.e_phnum;
	shnum = epriv->header.e_shnum;

	// EXEC with no sections is supported for now
	if ( epriv->header.e_type != ET_EXEC || shnum )
		return E_FAIL;

	// only program segments should be here
	if ( segment_count ( source->segments ) != phnum )
		return E_FAIL;

	id = 0;
	length = sizeof ( Elf32_Ehdr );
	ts = segment_find_by_num ( source->segments, id );
	while ( ts )
	{
		ppriv = (t_elf_prog_priv *)ts->priv;

		// minimum supported alignment is 0x1000 bytes
		if ( ppriv->header.p_align > 0x1000 )
            length = ALIGN_BY ( length, ppriv->header.p_align );
		else
            length = ALIGN_BY ( length, 0x1000 );

		while ( ( ts->length >= ppriv->header.p_filesz + 4 ) && ( v_get_w_raw ( ts, ts->start + ts->length - 4) == 0xFFFFFFFF ) )
			ts->length -= 4;

		ppriv->header.p_filesz = ts->length;
		ts->end = ts->start + ts->length;

		length += ts->length;

		ts = segment_find_by_num ( source->segments, ++id );
	}

	length = ALIGN_WORD ( length );
	data = malloc ( length );
	if ( !data )
		return E_FAIL;

	memset ( data, 0, length );

	if ( target->segments )
		segment_release ( target->segments );

	target->segments = segment_create (  );
	target->segments->name = "DATA";
	target->segments->start = 0;
	target->segments->end = length;
	target->segments->length = length;
	target->segments->data = data;


	// TODO: endianess
   	memcpy ( data, &epriv->header, sizeof ( Elf32_Ehdr ) );

	id = 0;
	length = sizeof ( Elf32_Ehdr );
	ts = segment_find_by_num ( source->segments, id );
	while ( ts )
	{
		ppriv = (t_elf_prog_priv *)ts->priv;

		if ( ppriv->header.p_align > 0x1000 )
            length = ALIGN_BY ( length, ppriv->header.p_align );
		else
            length = ALIGN_BY ( length, 0x1000 );

		// TODO: endianess, p_filesz etc
		memcpy ( data + epriv->header.e_phoff + id * epriv->header.e_phentsize, &ppriv->header, sizeof ( Elf32_Phdr ) );

		memcpy ( data + length, ts->data, ts->length );

		length += ts->length;

		ts = segment_find_by_num ( source->segments, ++id );
	}

	return E_OK;
}

unsigned int fmt_elf_encode ( t_stage *source, t_stage *target )
{
	char * data = NULL;
	unsigned int length;


	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

	if ( !target )
        target = source->next;

    if ( !source || !target )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) < 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

	if ( fmt_elf_create ( source, target ) != E_OK )
	{
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
		return E_FAIL;
	}

	target->length = target->segments->length;

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "ELF";
	source->type = "ELF";


	HEAP_CHECK;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

t_fmt_handler elf_handler = {
    "ELF",
    "FORMAT",
    "fmt_elf_decode",
    fmt_elf_decode,
    "fmt_elf_encode",
    fmt_elf_encode,
    "fmt_elf_free",
    fmt_elf_free,
	NULL,
	NULL
};

unsigned int fmt_elf_init ( )
{
	fmt_register_handler ( &elf_handler );
	return E_OK;
}

#endif
