#ifndef __TRIX_FMT_PE_H__
#define __TRIX_FMT_PE_H__


typedef struct s_pe_priv t_pe_priv;
struct s_pe_priv
{
	STRUCT_HEADER;
	t_dos_header hdr;
	t_image_nt_header32 hdr32;
};

typedef struct s_pe_sect_priv t_pe_sect_priv;
struct s_pe_sect_priv
{
	STRUCT_HEADER;
	unsigned int section_size;
	t_section_header header;
};

#endif /* __TRIX_FMT_PE_H__ */
