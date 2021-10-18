

/* seer headers for stage.c */

#define STAGE_AUTO_IMPORTS \
	"import unsigned int __stage_merge ( t_stage * s, unsigned char *buffer, unsigned int length);\n"\
	"import unsigned int __stage_get_start ( t_stage * s);\n"\
	"import unsigned int __stage_get_end ( t_stage * s);\n"\
	"import unsigned int __stage_has_unmapped ( t_stage * s);\n"\
	"import unsigned int __stage_has_overlaps ( t_stage * s);\n"\
	"import unsigned int __stage_replace ( t_stage * t, t_stage * s);\n"\
	"import t_stage *__stage_duplicate ( t_stage * s);\n"\
	"import t_stage *__stage_add ( t_stage * s);\n"\
	"import t_stage *__stage_create ( );\n"\
	"import t_stage *__stage_find_by_num ( t_stage * s, unsigned int num);\n"\
	"import t_stage *__stage_get_last ( t_stage * s);\n"\
	"import unsigned int __stage_restore_info ( t_stage * s, t_stage_info * i);\n"\
	"import unsigned int __stage_release_info ( t_stage_info * info);\n"\
	"import t_stage_info *__stage_save_info ( t_stage * s);\n"\
	"import unsigned int __stage_count ( t_stage * s);\n"\
	"import unsigned int __stage_get_num ( t_stage * s);\n"\
	"import unsigned int __stage_get_segments ( t_stage * s);\n"\
	"import unsigned int __stage_dump ( t_stage * s);\n"\
	"import t_stage *__stage_get ( t_stage * s, unsigned int flag);\n"\
	"import unsigned int __stage_clear_flags ( t_stage * s, unsigned int flag);\n"\
	"import t_stage *__stage_get_modified ( t_stage * s);\n"\
	"import t_stage *__stage_get_savestage ( t_stage * s);\n"\
	"import t_stage *__stage_get_workspace ( t_stage * s);\n"\
	"import unsigned int __stage_set_modified ( t_stage * s);\n"\
	"import unsigned int __stage_set_savestage ( t_stage * s);\n"\
	"import unsigned int __stage_set_workspace ( t_stage * s);\n"\
	"import t_stage *__stage_get_current ( t_stage * s);\n"\
	"import unsigned int __stage_release ( t_stage * s);\n"\
	"import unsigned int __stage_release_all ( t_stage * s);\n"\



#define STAGE_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_merge</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">buffer</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_get_start</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_get_end</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_has_unmapped</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_has_overlaps</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_replace</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">t</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_duplicate</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_add</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_create</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_find_by_num</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_get_last</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_restore_info</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#000000\">t_stage_info</font>&nbsp;*&nbsp;<font color=\"#000000\">i</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_release_info</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage_info</font>&nbsp;*&nbsp;<font color=\"#000000\">info</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage_info</font>&nbsp;*<font color=\"#000000\">__stage_save_info</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_count</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_get_num</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_get_segments</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_dump</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_get</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">flag</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_clear_flags</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">flag</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_get_modified</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_get_savestage</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_get_workspace</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_set_modified</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_set_savestage</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_set_workspace</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">__stage_get_current</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__stage_release_all</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for stage.c */

#define STAGE_AUTO_REGISTER \
	scAddExtSymInt ( stage_merge );\
	scAddExtSymInt ( stage_get_start );\
	scAddExtSymInt ( stage_get_end );\
	scAddExtSymInt ( stage_has_unmapped );\
	scAddExtSymInt ( stage_has_overlaps );\
	scAddExtSymInt ( stage_replace );\
	scAddExtSymInt ( stage_duplicate );\
	scAddExtSymInt ( stage_add );\
	scAddExtSymInt ( stage_create );\
	scAddExtSymInt ( stage_find_by_num );\
	scAddExtSymInt ( stage_get_last );\
	scAddExtSymInt ( stage_restore_info );\
	scAddExtSymInt ( stage_release_info );\
	scAddExtSymInt ( stage_save_info );\
	scAddExtSymInt ( stage_count );\
	scAddExtSymInt ( stage_get_num );\
	scAddExtSymInt ( stage_get_segments );\
	scAddExtSymInt ( stage_dump );\
	scAddExtSymInt ( stage_get );\
	scAddExtSymInt ( stage_clear_flags );\
	scAddExtSymInt ( stage_get_modified );\
	scAddExtSymInt ( stage_get_savestage );\
	scAddExtSymInt ( stage_get_workspace );\
	scAddExtSymInt ( stage_set_modified );\
	scAddExtSymInt ( stage_set_savestage );\
	scAddExtSymInt ( stage_set_workspace );\
	scAddExtSymInt ( stage_get_current );\
	scAddExtSymInt ( stage_release );\
	scAddExtSymInt ( stage_release_all );\


/* seer function declaration for stage.c */
#ifndef HEADER_SKIP_DECLARATION
void stage_merge ();
void stage_get_start ();
void stage_get_end ();
void stage_has_unmapped ();
void stage_has_overlaps ();
void stage_replace ();
void stage_duplicate ();
void stage_add ();
void stage_create ();
void stage_find_by_num ();
void stage_get_last ();
void stage_restore_info ();
void stage_release_info ();
void stage_save_info ();
void stage_count ();
void stage_get_num ();
void stage_get_segments ();
void stage_dump ();
void stage_get ();
void stage_clear_flags ();
void stage_get_modified ();
void stage_get_savestage ();
void stage_get_workspace ();
void stage_set_modified ();
void stage_set_savestage ();
void stage_set_workspace ();
void stage_get_current ();
void stage_release ();
void stage_release_all ();

#endif
/* additional seer includes from stage_seer.h */

#define STAGE_AUTO_MISC_PRE \
	"\n"\
	"typedef struct s_stage_info {\n"\
	"	STRUCT_HEADER_LIST(t_stage_info);\n"\
	"} t_stage_info;\n"\
	"\n"\
	"typedef struct s_stage {\n"\
	"	STRUCT_HEADER_LIST(t_stage);\n"\
	"    char *name;\n"\
	"    char *type;\n"\
	"    char *parser;\n"\
	"    unsigned int length;\n"\
	"    t_segment *segments;\n"\
	"    void *priv;\n"\
	"} t_stage;\n"\
	"\n"\
	"#define S_MODIFIED    0x0001\n"\
	"#define S_WORKSPACE   0x0002\n"\
	"#define S_SYNCBASE    0x0004\n"\
	"#define S_SAVESTAGE   0x0008\n"\
	"\n"\
	"\n"\


#define STAGE_AUTO_MISC_POST \


#define STAGE_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_stage_info</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_stage_info</font>);<br>"\
	"}&nbsp;<font color=\"#000000\">t_stage_info</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_stage</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_stage</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">parser</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">segments</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">priv</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_stage</font>;<br>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;S_MODIFIED&nbsp;&nbsp;&nbsp;&nbsp;0x0001<br></font>"\
	"<font color=\"#008000\">#define&nbsp;S_WORKSPACE&nbsp;&nbsp;&nbsp;0x0002<br></font>"\
	"<font color=\"#008000\">#define&nbsp;S_SYNCBASE&nbsp;&nbsp;&nbsp;&nbsp;0x0004<br></font>"\
	"<font color=\"#008000\">#define&nbsp;S_SAVESTAGE&nbsp;&nbsp;&nbsp;0x0008<br></font>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define STAGE_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


