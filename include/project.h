#ifndef __TRIX_PROJECT_H__
#define __TRIX_PROJECT_H__


typedef struct s_project_files t_project_files;
struct s_project_files
{
	STRUCT_HEADER_LIST(t_project_files);
	char *title;
    char *filename;
	char *content;
	char *settings;
	t_options *options;

	char *code;
	int state;
};

typedef struct s_project t_project;
struct s_project
{
	char *projectname;		// a freely definable name for the project
    char *filename;			// the .txj project filename
	char *settings;			// global option set
	int savemode;			// the file savemode (dry, partial, normal)
	int operationmode;		// the operation mode (simple, dct3, dct4)

	int simple_in;
	char *simple_input;
	int simple_out;
	char *simple_output;

	int dct3_in;
	char *dct3_mcu_input;
	int dct3_seperate_ppm;
	char *dct3_mcu_output;
	int dct3_ppm_custom;
	int dct3_ppm_address;

	int dct3_out;
	char *dct3_ppm_input;
	char *dct3_ppm_output;

	int dct4_in;
	char *dct4_mcu_input;
	char *dct4_mcu_output;

	int dct4_out;
	char *dct4_ppm_input;
	char *dct4_ppm_output;

	int num_files;			// number of loaded scripts
	t_project_files *files;	// loaded scripts
};

#define PROJ_FILE_OK		0
#define PROJ_FILE_SIGNED	1
#define PROJ_FILE_FAILED	2


#define PROJ_ASSIGN_STR(field, str) \
	{\
		if (project)\
		{\
			if ( project->field )\
				free ( project->field );\
			project->field = (char*)strdup ( str.toAscii().data () );\
		}\
	}

#define PROJ_ASSIGN_BOOL(field, boolean) \
	{\
		if (project)\
			project->field = boolean?1:0;\
	}

#define PROJ_ASSIGN_INT(field, value) \
	{\
		if (project)\
			project->field = value;\
	}

#include "trixplug_project.h"

unsigned int project_save ( t_project *p, char *file );
t_project *project_load ( char *file );
unsigned int project_del_file ( t_project *p, unsigned int pos );
unsigned int project_add_file ( t_project *p, char *file );
unsigned int project_init ( );
t_project_files *project_get_file ( t_project *p, unsigned int pos );

#endif
