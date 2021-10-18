
typedef struct s_segment {
	STRUCT_HEADER_LIST(t_segment);
    char *name;
    unsigned int start;
    unsigned int end;
    unsigned int length;
    unsigned char *data;
    void *priv;
} t_segment;

