#ifndef __TRIX_FMT_EPOC_H__
#define __TRIX_FMT_EPOC_H__

typedef struct s_epoc_priv t_epoc_priv;
struct s_epoc_priv
{
	STRUCT_HEADER;
	E32ImageHeader original_header;
	E32ImageHeaderv2 original_header_v2;
	int version;

};



#endif
