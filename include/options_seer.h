

#define OPT_BOOL   0x0001
#define OPT_INT    0x0002
#define OPT_HEX    0x0003
#define OPT_STR    0x0004
#define OPT_LAST   0x0005
#define OPT_INTCB  0x0006
#define OPT_STRCB  0x0007
#define OPT_END    { 0, NULL, NULL, NULL }
#define OPT_BEGIN  t_options script_options[] = {
#define OPT_FINISH OPT_END }; export script_options;
#define OPTION(t,d,n,g)  { t, d, n, g },

typedef struct s_options
{
	int type;
	char *description;
	char *name;
	void *target;
} t_options;
