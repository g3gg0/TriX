

/* seer headers for workspace.c */

#define WORKSPACE_AUTO_IMPORTS \
	"import void __workspace_skip_error ( int skip);\n"\
	"import unsigned int __workspace_set_modified ( t_workspace * ws);\n"\
	"import t_workspace *__workspace_startup ( char *filename);\n"\
	"import t_workspace *__workspace_create ( );\n"\
	"import t_workspace *__workspace_create_file ( );\n"\
	"import t_workspace *__workspace_create_file_from_buffer ( unsigned char *buffer, unsigned int length);\n"\
	"import unsigned int __workspace_add_file ( t_workspace * ws, t_fileinfo * fileinfo);\n"\
	"import unsigned int __workspace_memmap_add ( t_workspace * ws, t_stage * stage, t_segment * seg);\n"\
	"import unsigned int __workspace_release ( t_workspace * ws);\n"\
	"import unsigned int __workspace_memmap_release ( t_workspace * ws);\n"\
	"import unsigned int __workspace_memmap_sort ( t_workspace * ws);\n"\
	"import unsigned int __workspace_memmap_reverse ( t_workspace * ws);\n"\
	"import unsigned int __workspace_update_memmap ( t_workspace * ws);\n"\



#define WORKSPACE_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__workspace_skip_error</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">skip</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_set_modified</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">__workspace_startup</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">__workspace_create</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">__workspace_create_file</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">__workspace_create_file_from_buffer</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">buffer</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_add_file</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fileinfo</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_memmap_add</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">seg</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_memmap_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_memmap_sort</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_memmap_reverse</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__workspace_update_memmap</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for workspace.c */

#define WORKSPACE_AUTO_REGISTER \
	scAddExtSymInt ( workspace_skip_error );\
	scAddExtSymInt ( workspace_set_modified );\
	scAddExtSymInt ( workspace_startup );\
	scAddExtSymInt ( workspace_create );\
	scAddExtSymInt ( workspace_create_file );\
	scAddExtSymInt ( workspace_create_file_from_buffer );\
	scAddExtSymInt ( workspace_add_file );\
	scAddExtSymInt ( workspace_memmap_add );\
	scAddExtSymInt ( workspace_release );\
	scAddExtSymInt ( workspace_memmap_release );\
	scAddExtSymInt ( workspace_memmap_sort );\
	scAddExtSymInt ( workspace_memmap_reverse );\
	scAddExtSymInt ( workspace_update_memmap );\


/* seer function declaration for workspace.c */
#ifndef HEADER_SKIP_DECLARATION
void workspace_skip_error ();
void workspace_set_modified ();
void workspace_startup ();
void workspace_create ();
void workspace_create_file ();
void workspace_create_file_from_buffer ();
void workspace_add_file ();
void workspace_memmap_add ();
void workspace_release ();
void workspace_memmap_release ();
void workspace_memmap_sort ();
void workspace_memmap_reverse ();
void workspace_update_memmap ();

#endif
/* additional seer includes from workspace_seer.h */

#define WORKSPACE_AUTO_MISC_PRE \
	"\n"\
	"\n"\
	"\n"\
	"#define MEM_MAX  0x7FFFFFFF\n"\
	"#define MEM_AUTO  -2\n"\
	"\n"\
	"typedef struct s_memmap\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_memmap);\n"\
	"	unsigned int start;\n"\
	"	unsigned int end;\n"\
	"	unsigned int length;\n"\
	"	t_segment *segment;\n"\
	"	t_stage *stage;\n"\
	"} t_memmap;\n"\
	"\n"\
	"typedef struct s_workspace\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_workspace);\n"\
	"	t_memmap *memmap;\n"\
	"	t_fileinfo *fileinfo;\n"\
	"} t_workspace;\n"\
	"\n"\
	"\n"\


#define WORKSPACE_AUTO_MISC_POST \
	"\n"\
	"t_workspace *trix_workspace = NULL;\n"\
	"export trix_workspace;\n"\
	"\n"\
	"#define GetWorkspace(void) trix_workspace\n"\
	"\n"\
	"unsigned int WorkspaceUpdateMemmap ( t_workspace *ws ) { return __workspace_update_memmap ( ws ); }\n"\
	"unsigned int WorkspaceMemmapAdd ( t_workspace *ws, t_stage *stage, t_segment *seg ) { return __workspace_memmap_add ( ws, stage, seg );}\n"\
	"unsigned int WorkspaceAddFile ( t_workspace *ws, t_fileinfo *fileinfo ) { return __workspace_add_file ( ws, fileinfo );}\n"\
	"t_workspace *WorkspaceCreate ( ) { return __workspace_create ( );}\n"\
	"t_workspace *WorkspaceCreateFile ( ) { return __workspace_create_file ( );}\n"\
	"t_workspace *WorkspaceStartup ( char *filename ) { return __workspace_startup ( filename );}\n"\
	"unsigned int WorkspaceMemmapFree ( t_workspace *ws ) { return __workspace_memmap_release ( ws );}\n"\
	"unsigned int WorkspaceMemmapSort ( t_workspace *ws ) { return __workspace_memmap_sort ( ws );}\n"\
	"t_workspace *WorkspaceCreateFileFromBuffer ( unsigned char *buffer, int length ) { return __workspace_create_file_from_buffer ( buffer, length );}\n"\
	"\n"\
	"unsigned int SetWorkStage ( int filenum, int stagenum ) \n"\
	"{ \n"\
	"		int ret = E_OK; \n"\
	"		if ( !GetWorkspace() || !GetWorkspace()->fileinfo || !GetWorkspace()->fileinfo->stages  ) \n"\
	"			ret = E_FAIL; \n"\
	"		if ( ret == E_OK && __stage_get_modified ( GetWorkspace()->fileinfo->stages ) ) \n"\
	"			__file_sync ( GetWorkspace()->fileinfo ); \n"\
	"		if ( ret == E_OK && __stage_set_workspace ( __stage_find_by_num ( GetWorkspace()->fileinfo->stages, stagenum ) ) != E_OK) \n"\
	"			ret = E_FAIL; \n"\
	"		__workspace_update_memmap ( GetWorkspace() ); \n"\
	"		return ret; \n"\
	"}\n"\
	"\n"\
	"unsigned int GetWorkStageNum ( int filenum ) { return __stage_get_num ( __stage_get_workspace ( GetWorkspace()->fileinfo->stages ) ); }\n"\
	"unsigned int GetWorkStage ( int filenum ) { return __stage_get_workspace ( GetWorkspace()->fileinfo->stages ); }\n"\
	"unsigned int GetStageCount ( int filenum ) { return __stage_count ( __stage_get_current ( GetWorkspace()->fileinfo->stages ) ); }\n"\
	"\n"\


#define WORKSPACE_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<br>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;MEM_MAX&nbsp;&nbsp;0x7FFFFFFF<br></font>"\
	"<font color=\"#008000\">#define&nbsp;MEM_AUTO&nbsp;&nbsp;-2<br></font>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_memmap</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_memmap</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">segment</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">stage</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_memmap</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_workspace</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_workspace</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_memmap</font>&nbsp;*<font color=\"#000000\">memmap</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*<font color=\"#000000\">fileinfo</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_workspace</font>;<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define WORKSPACE_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">trix_workspace</font>&nbsp;=&nbsp;<font color=\"#000000\">NULL</font>;<br>"\
	"<font color=\"#000000\">export</font>&nbsp;<font color=\"#000000\">trix_workspace</font>;<br>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;GetWorkspace(void)&nbsp;trix_workspace<br></font>"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">WorkspaceUpdateMemmap</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">ws</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_update_memmap</font>&nbsp;(&nbsp;<font color=\"#000000\">ws</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">WorkspaceMemmapAdd</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">seg</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_memmap_add</font>&nbsp;(&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#000000\">seg</font>&nbsp;);}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">WorkspaceAddFile</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*<font color=\"#000000\">fileinfo</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_add_file</font>&nbsp;(&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">fileinfo</font>&nbsp;);}<br>"\
	"<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">WorkspaceCreate</font>&nbsp;(&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_create</font>&nbsp;(&nbsp;);}<br>"\
	"<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">WorkspaceCreateFile</font>&nbsp;(&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_create_file</font>&nbsp;(&nbsp;);}<br>"\
	"<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">WorkspaceStartup</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_startup</font>&nbsp;(&nbsp;<font color=\"#000000\">filename</font>&nbsp;);}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">WorkspaceMemmapFree</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">ws</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_memmap_release</font>&nbsp;(&nbsp;<font color=\"#000000\">ws</font>&nbsp;);}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">WorkspaceMemmapSort</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">ws</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_memmap_sort</font>&nbsp;(&nbsp;<font color=\"#000000\">ws</font>&nbsp;);}<br>"\
	"<font color=\"#000000\">t_workspace</font>&nbsp;*<font color=\"#000000\">WorkspaceCreateFileFromBuffer</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">buffer</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__workspace_create_file_from_buffer</font>&nbsp;(&nbsp;<font color=\"#000000\">buffer</font>,&nbsp;<font color=\"#000000\">length</font>&nbsp;);}<br>"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SetWorkStage</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">filenum</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">stagenum</font>&nbsp;)&nbsp;<br>"\
	"{&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">ret</font>&nbsp;=&nbsp;<font color=\"#000000\">E_OK</font>;&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>if</b></font>&nbsp;(&nbsp;!<font color=\"#000000\">GetWorkspace</font>()&nbsp;||&nbsp;!<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>&nbsp;||&nbsp;!<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>-&gt;<font color=\"#000000\">stages</font>&nbsp;&nbsp;)&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">ret</font>&nbsp;=&nbsp;<font color=\"#000000\">E_FAIL</font>;&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>if</b></font>&nbsp;(&nbsp;<font color=\"#000000\">ret</font>&nbsp;==&nbsp;<font color=\"#000000\">E_OK</font>&nbsp;&amp;&amp;&nbsp;<font color=\"#000000\">__stage_get_modified</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>-&gt;<font color=\"#000000\">stages</font>&nbsp;)&nbsp;)&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">__file_sync</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>&nbsp;);&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>if</b></font>&nbsp;(&nbsp;<font color=\"#000000\">ret</font>&nbsp;==&nbsp;<font color=\"#000000\">E_OK</font>&nbsp;&amp;&amp;&nbsp;<font color=\"#000000\">__stage_set_workspace</font>&nbsp;(&nbsp;<font color=\"#000000\">__stage_find_by_num</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>-&gt;<font color=\"#000000\">stages</font>,&nbsp;<font color=\"#000000\">stagenum</font>&nbsp;)&nbsp;)&nbsp;!=&nbsp;<font color=\"#000000\">E_OK</font>)&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">ret</font>&nbsp;=&nbsp;<font color=\"#000000\">E_FAIL</font>;&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">__workspace_update_memmap</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()&nbsp;);&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">ret</font>;&nbsp;<br>"\
	"}<br>"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetWorkStageNum</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">filenum</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__stage_get_num</font>&nbsp;(&nbsp;<font color=\"#000000\">__stage_get_workspace</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>-&gt;<font color=\"#000000\">stages</font>&nbsp;)&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetWorkStage</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">filenum</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__stage_get_workspace</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>-&gt;<font color=\"#000000\">stages</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetStageCount</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">filenum</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__stage_count</font>&nbsp;(&nbsp;<font color=\"#000000\">__stage_get_current</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>()-&gt;<font color=\"#000000\">fileinfo</font>-&gt;<font color=\"#000000\">stages</font>&nbsp;)&nbsp;);&nbsp;}<br>"\
	"<br>"\
	"		</font>"\


