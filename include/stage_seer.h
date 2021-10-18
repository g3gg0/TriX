
typedef struct s_stage_info {
	STRUCT_HEADER_LIST(t_stage_info);
} t_stage_info;

typedef struct s_stage {
	STRUCT_HEADER_LIST(t_stage);
    char *name;
    char *type;
    char *parser;
    unsigned int length;
    t_segment *segments;
    void *priv;
} t_stage;

#define S_MODIFIED    0x0001
#define S_WORKSPACE   0x0002
#define S_SYNCBASE    0x0004
#define S_SAVESTAGE   0x0008


