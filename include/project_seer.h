
typedef struct s_project_files
{
	STRUCT_HEADER_LIST(t_project_files);
	char *name;
    char *filename;
	char *content;
	char *options;
} t_project_files;

typedef struct s_project
{
	char *projectname;		// a freely definable name for the project
    char *filename;			// the .txj project filename
	char *options;			// global option set
	int savemode;			// the file savemode (dry, partial, normal)
	int operationmode;		// the operation mode (simple, dct3, dct4)
	
	int num_files;				// number of loaded scripts
	t_project_files *files;	// loaded scripts
} t_project;
