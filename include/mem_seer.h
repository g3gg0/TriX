

typedef struct s_mem_list
{
	STRUCT_HEADER_LIST(t_mem_list);
	unsigned int bytes;
	unsigned int address;
	char *caller;
	char *type;
	int line;
	int freed;
} t_mem_list;

