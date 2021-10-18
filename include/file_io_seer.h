
typedef struct s_fileinfo {
	STRUCT_HEADER_LIST(t_fileinfo);
    char *filename;
    char *path;
    t_stage *stages;
    t_stage *stages_org;
    t_stage *stages_mod;
    void *priv;
    unsigned int options;
} t_fileinfo;

