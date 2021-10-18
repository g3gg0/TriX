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

unsigned int
fmt_elf_add_section ( t_stage *source, t_stage *target, char *name, Elf32_Shdr header )
{
	t_segment *s = NULL;
	t_elf_sect_priv *priv = NULL;
	unsigned char *data = NULL;

//	DBG ( DEBUG_FMT_ELF,  "adding section: '%s'\n", name );

	data = malloc ( header.sh_size + 1);
	if ( !data )
		return E_FAIL;

	v_memcpy_get_raw ( source, data, header.sh_offset, header.sh_size );

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

	v_memcpy_get_raw ( source, &header, priv->hdr_str_offset, sizeof(Elf32_Shdr) );

	data_pos = v_arch_w ( source, header.sh_offset );
	if ( !data_pos || data_pos > source->length )
		return NULL;

	return v_get_str_raw ( source, data_pos + string );
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

unsigned int
fmt_elf_parse_sections ( t_stage *source, t_stage *target, unsigned int start_offset, unsigned int headers, unsigned int header_size )
{
	unsigned int i = 0;
	Elf32_Shdr header;
	unsigned int offset = start_offset;
	char *name = NULL;
	t_elf_priv *priv;

	if ( !source || !target )
		return E_FAIL;

	priv = target->priv;
	while ( headers )
	{
		v_memcpy_get_raw ( source, &header, offset, sizeof(Elf32_Shdr) );
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
		DBG ( DEBUG_FMT_ELF,  "info: %i  ", header.sh_info );
		DBG ( DEBUG_FMT_ELF,  "Flags: %c%c%c  ", header.sh_flags&SHF_WRITE?'W':'-',header.sh_flags&SHF_ALLOC?'A':'-',header.sh_flags&SHF_EXECINSTR?'X':'-' );
		DBG ( DEBUG_FMT_ELF,  "name: '%s'\n", name );

		R(fmt_elf_add_section ( source, target, name, header ));

		offset += header_size;
		headers--;
	}

	return E_OK;
}

unsigned int
fmt_elf_parse ( t_stage * source, t_stage * target )
{
	unsigned int i = 0;
	Elf32_Ehdr header;
	t_elf_priv *priv;

	v_memcpy_get_raw ( source, &header, 0, sizeof (Elf32_Ehdr)  );

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
	switch ( NULL,header.e_ident[EI_CLASS] )
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

	
	//	DBG ( DEBUG_FMT_ELF,  "Program Headers start at: 0x%08X  count: %i  each: %i bytes\n", v_arch_w(NULL, header.e_phoff), v_arch_h(NULL, header.e_phnum), v_arch_h(NULL, header.e_phentsize) );
//	DBG ( DEBUG_FMT_ELF,  "Section Headers start at: 0x%08X  count: %i  each: %i bytes\n", v_arch_w(NULL, header.e_shoff), v_arch_h(NULL, header.e_shnum), v_arch_h(NULL, header.e_shentsize) );
//	DBG ( DEBUG_FMT_ELF,  "Table %i holds strings\n", v_arch_h(NULL,header.e_shstrndx) );
//	DBG ( DEBUG_FMT_ELF,  "Entry point at:           0x%08X\n", v_arch_w(NULL, header.e_entry) );
//	basecode = v_get_h_raw (t, mcu_flash_start + auto_data[0][0]) ^ auto_data[0][1];

	if ( !header.e_shoff || !header.e_shnum )
	{
		DBG ( DEBUG_FMT_ELF,  "No sections in object\n" );
		return E_FAIL;
	}

	target->priv = PRIV_MALLOC(t_elf_priv);
	priv = target->priv;

	priv->hdr_str_offset = header.e_shoff+header.e_shstrndx*header.e_shentsize;
	memcpy ( &priv->header, &header, sizeof ( Elf32_Ehdr ) );


	fmt_elf_parse_sections ( source, target, header.e_shoff, header.e_shnum, header.e_shentsize );
	

	return E_OK;
}

unsigned int
fmt_elf_decode ( t_stage * source, t_stage * target )
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

	R(fmt_elf_parse (  source,  target ));

    target->name = "PARSED";
    target->length = 0;
    target->parser = "ELF";
    target->type = "ELF";


	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}


unsigned int
fmt_elf_encode ( t_stage * source, t_stage * target )
{
	char * data = NULL;

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    if ( !target )
        target = source->next;

	return E_FAIL;

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

//	data = fmt_elf_create ( source, &length );

	if ( !data )
    {
		
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    target->segments = segment_create (  );
    target->segments->name = "DATA";
    target->segments->start = 0;
    target->segments->end = 0;
    target->segments->length = 0;
    target->segments->data = data;

    target->name = "RAW";
    target->length = 0;
    target->parser = "ELF";
    target->type = "ELF";

	
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


unsigned int
fmt_elf_init (  )
{
    fmt_register_handler ( &elf_handler );
    return E_OK;
}

#endif
