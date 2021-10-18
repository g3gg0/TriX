


#define MEM_MAX  0x7FFFFFFF
#define MEM_AUTO  -2

typedef struct s_memmap
{
	STRUCT_HEADER_LIST(t_memmap);
	unsigned int start;
	unsigned int end;
	unsigned int length;
	t_segment *segment;
	t_stage *stage;
} t_memmap;

typedef struct s_workspace
{
	STRUCT_HEADER_LIST(t_workspace);
	t_memmap *memmap;
	t_fileinfo *fileinfo;
} t_workspace;

// INSERT_DECLARATIONS

t_workspace *trix_workspace = NULL;
export trix_workspace;

#define GetWorkspace(void) trix_workspace

unsigned int WorkspaceUpdateMemmap ( t_workspace *ws ) { return __workspace_update_memmap ( ws ); }
unsigned int WorkspaceMemmapAdd ( t_workspace *ws, t_stage *stage, t_segment *seg ) { return __workspace_memmap_add ( ws, stage, seg );}
unsigned int WorkspaceAddFile ( t_workspace *ws, t_fileinfo *fileinfo ) { return __workspace_add_file ( ws, fileinfo );}
t_workspace *WorkspaceCreate ( ) { return __workspace_create ( );}
t_workspace *WorkspaceCreateFile ( ) { return __workspace_create_file ( );}
t_workspace *WorkspaceStartup ( char *filename ) { return __workspace_startup ( filename );}
unsigned int WorkspaceMemmapFree ( t_workspace *ws ) { return __workspace_memmap_release ( ws );}
unsigned int WorkspaceMemmapSort ( t_workspace *ws ) { return __workspace_memmap_sort ( ws );}
t_workspace *WorkspaceCreateFileFromBuffer ( unsigned char *buffer, int length ) { return __workspace_create_file_from_buffer ( buffer, length );}

unsigned int SetWorkStage ( int filenum, int stagenum ) 
{ 
		int ret = E_OK; 
		if ( !GetWorkspace() || !GetWorkspace()->fileinfo || !GetWorkspace()->fileinfo->stages  ) 
			ret = E_FAIL; 
		if ( ret == E_OK && __stage_get_modified ( GetWorkspace()->fileinfo->stages ) ) 
			__file_sync ( GetWorkspace()->fileinfo ); 
		if ( ret == E_OK && __stage_set_workspace ( __stage_find_by_num ( GetWorkspace()->fileinfo->stages, stagenum ) ) != E_OK) 
			ret = E_FAIL; 
		__workspace_update_memmap ( GetWorkspace() ); 
		return ret; 
}

unsigned int GetWorkStageNum ( int filenum ) { return __stage_get_num ( __stage_get_workspace ( GetWorkspace()->fileinfo->stages ) ); }
unsigned int GetWorkStage ( int filenum ) { return __stage_get_workspace ( GetWorkspace()->fileinfo->stages ); }
unsigned int GetStageCount ( int filenum ) { return __stage_count ( __stage_get_current ( GetWorkspace()->fileinfo->stages ) ); }

