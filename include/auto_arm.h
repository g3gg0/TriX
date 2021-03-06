

/* seer headers for arm.c */

#define ARM_AUTO_IMPORTS \
	"import unsigned int __arm_init ( );\n"\
	"import unsigned int __arm_set ( t_workspace * ws, unsigned int type, unsigned int offset, unsigned int dest);\n"\
	"import unsigned int __arm_set_32 ( t_workspace * ws, unsigned int type, unsigned int offset, unsigned int dest, unsigned int cond_field);\n"\
	"import unsigned int __arm_get_bl ( t_workspace * ws, unsigned int offset);\n"\
	"import unsigned int __arm_get ( t_workspace * ws, unsigned int offset);\n"\
	"import unsigned int __arm_get_32 ( t_workspace *ws, unsigned int offset );\n"\
	"import unsigned int __arm_get_32_adv ( t_workspace *ws, unsigned int offset, unsigned int type );\n"\
	"import unsigned int __arm_get_adv ( t_workspace *ws, unsigned int offset, unsigned int type );\n"\
	"import unsigned int __arm_find ( t_workspace * ws, unsigned int type, unsigned int start, unsigned int end, unsigned int direction);\n"\
	"import unsigned int __arm_get_freespace ( t_workspace * ws, unsigned int size, unsigned int offset);\n"\
	"import unsigned int __arm_find_src_of_dest ( t_workspace * ws, int type, unsigned int dest, unsigned int start, unsigned int end, unsigned int direction);\n"\
	"import unsigned int __arm_create_func ( t_workspace * ws, char *routine, unsigned int offset);\n"\
	"import object *__arm_create_object_from_makefile ( const char *makefile_path, const char *object_file);\n"\
	"import unsigned int __arm_run_makefile ( const char *makefile_path);\n"\
	"import object *__arm_create_object ( const char *makefile_path, const char *object_file);\n"\



#define ARM_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_init</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_set</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">dest</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_set_32</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">dest</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">cond_field</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_get_bl</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_get</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_find</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">direction</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_get_freespace</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_find_src_of_dest</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">dest</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">direction</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_create_func</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">routine</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">object</font>&nbsp;*<font color=\"#000000\">__arm_create_object_from_makefile</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">makefile_path</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">object_file</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__arm_run_makefile</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">makefile_path</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">object</font>&nbsp;*<font color=\"#000000\">__arm_create_object</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">makefile_path</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">object_file</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for arm.c */

#define ARM_AUTO_REGISTER \
	scAddExtSymInt ( arm_init );\
	scAddExtSymInt ( arm_set );\
	scAddExtSymInt ( arm_set_32 );\
	scAddExtSymInt ( arm_get_bl );\
	scAddExtSymInt ( arm_get );\
	scAddExtSymInt ( arm_get_32 );\
	scAddExtSymInt ( arm_get_32_adv );\
	scAddExtSymInt ( arm_get_adv );\
	scAddExtSymInt ( arm_find );\
	scAddExtSymInt ( arm_get_freespace );\
	scAddExtSymInt ( arm_find_src_of_dest );\
	scAddExtSymInt ( arm_create_func );\
	scAddExtSymInt ( arm_create_object_from_makefile );\
	scAddExtSymInt ( arm_run_makefile );\
	scAddExtSymInt ( arm_create_object );\


/* seer function declaration for arm.c */
#ifndef HEADER_SKIP_DECLARATION
void arm_init ();
void arm_set ();
void arm_set_32 ();
void arm_get_bl ();
void arm_get ();
void arm_get_32 ();
void arm_get_32_adv ();
void arm_get_adv ();
void arm_find ();
void arm_get_freespace ();
void arm_find_src_of_dest ();
void arm_create_func ();
void arm_create_object_from_makefile ();
void arm_run_makefile ();
void arm_create_object ();

#endif
/* additional seer includes from arm_seer.h */

#define ARM_AUTO_MISC_PRE \
	"\n"\
	"\n"\
	"\n"\


#define ARM_AUTO_MISC_POST \
	"\n"\
	"unsigned int ARMFind  ( unsigned int type, int start, int end, unsigned int direction ) { return __arm_find( GetWorkspace(), type, start, end, direction ); }\n"\
	"unsigned int ARMGet   ( unsigned int x ) { return __arm_get ( GetWorkspace(), x ); }\n"\
	"unsigned int ARMSet   ( unsigned int x, int y, int z) { return __arm_set(GetWorkspace(),x,y,z); }\n"\
	"unsigned int ARMGet32 ( unsigned int x ) { return __arm_get_32 ( GetWorkspace(), x ); }\n"\
	"unsigned int ARMSet32 ( unsigned int x, int y, int z, int cond) { return __arm_set_32(GetWorkspace(),x,y,z,cond); }\n"\
	"unsigned int ARMFindSrcOfDest(int a, int b, int c, int d, unsigned int e) { __arm_find_src_of_dest(GetWorkspace(),a,b,c,d,e); }\n"\
	"\n"\
    "#define ARM_DEST_VAL  0x8000                       \n"\
    "#define ARM_REG_RD    0x4000                       \n"\
	"\n"\
    "#define THUMB_OPCODE  0x1000                       \n"\
    "#define ARM_OPCODE    0x2000                       \n"\
    "#define ARM_THUMB_MASK (THUMB_OPCODE | ARM_OPCODE) \n"\
    "                                                   \n"\
    "// THUMB opcodes                                   \n"\
    "#define THUMB_BL  (THUMB_OPCODE | 0)               \n"\
    "#define THUMB_B   (THUMB_OPCODE | 1)               \n"\
    "#define THUMB_BEQ (THUMB_OPCODE | 2)               \n"\
    "#define THUMB_BNE (THUMB_OPCODE | 3)               \n"\
    "#define THUMB_BCS (THUMB_OPCODE | 4)               \n"\
    "#define THUMB_BCC (THUMB_OPCODE | 5)               \n"\
    "#define THUMB_BMI (THUMB_OPCODE | 6)               \n"\
    "#define THUMB_BPL (THUMB_OPCODE | 7)               \n"\
    "#define THUMB_BVS (THUMB_OPCODE | 8)               \n"\
    "#define THUMB_BVC (THUMB_OPCODE | 9)               \n"\
    "#define THUMB_BHI (THUMB_OPCODE | 10)              \n"\
    "#define THUMB_BLS (THUMB_OPCODE | 11)              \n"\
    "#define THUMB_BGE (THUMB_OPCODE | 12)              \n"\
    "#define THUMB_BLT (THUMB_OPCODE | 13)              \n"\
    "#define THUMB_BGT (THUMB_OPCODE | 14)              \n"\
    "#define THUMB_BLE (THUMB_OPCODE | 15)              \n"\
    "#define THUMB_LDR (THUMB_OPCODE | 16)              \n"\
    "#define THUMB_ADR (THUMB_OPCODE | 17)              \n"\
    "#define THUMB_BLX (THUMB_OPCODE | 18)              \n"\
    "                                                   \n"\
    "// ARM opcodes                                     \n"\
    "#define ARM_B    (ARM_OPCODE | 19)                 \n"\
    "#define ARM_BL   (ARM_OPCODE | 20)                 \n"\
    "#define ARM_BLX  (ARM_OPCODE | 21)                 \n"\
    "#define ARM_BLX1 (ARM_OPCODE | 22)                 \n"\
    "#define ARM_BLX2 (ARM_OPCODE | 23)                 \n"\
    "#define ARM_LDR  (ARM_OPCODE | 24)                 \n"\
    "#define ARM_ADR  (ARM_OPCODE | 25)                 \n"\
    "#define ARM_MOV  (ARM_OPCODE | 26)                 \n"\
	"\n"\
	"#define BL  0\n"\
	"#define B   1\n"\
	"#define BEQ 2\n"\
	"#define BNE 3\n"\
	"#define BCS 4\n"\
	"#define BCC 5\n"\
	"#define BMI 6\n"\
	"#define BPL 7\n"\
	"#define BVS 8\n"\
	"#define BVC 9\n"\
	"#define BHI 10\n"\
	"#define BLS 11\n"\
	"#define BGE 12\n"\
	"#define BLT 13\n"\
	"#define BGT 14\n"\
	"#define BLE 15\n"\
	"#define LDR 16\n"\
	"#define ADR 17\n"\
	"#define BLX 18\n"\
	"\n"\
	"#define ADDRESS 0xFF\n"\


#define ARM_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define ARM_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">ARMFind</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">direction</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__arm_find</font>(&nbsp;<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#000000\">direction</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">ARMGet</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__arm_get</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">ARMSet</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">y</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">z</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__arm_set</font>(<font color=\"#000000\">GetWorkspace</font>(),<font color=\"#000000\">x</font>,<font color=\"#000000\">y</font>,<font color=\"#000000\">z</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">ARMFindSrcOfDest</font>(<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">a</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">b</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">c</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">d</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">e</font>)&nbsp;{&nbsp;<font color=\"#000000\">__arm_find_src_of_dest</font>(<font color=\"#000000\">GetWorkspace</font>(),<font color=\"#000000\">a</font>,<font color=\"#000000\">b</font>,<font color=\"#000000\">c</font>,<font color=\"#000000\">d</font>,<font color=\"#000000\">e</font>);&nbsp;}<br>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;BL&nbsp;&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;B&nbsp;&nbsp;&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BEQ&nbsp;2<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BNE&nbsp;3<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BCS&nbsp;4<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BCC&nbsp;5<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BMI&nbsp;6<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BPL&nbsp;7<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BVS&nbsp;8<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BVC&nbsp;9<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BHI&nbsp;10<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BLS&nbsp;11<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BGE&nbsp;12<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BLT&nbsp;13<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BGT&nbsp;14<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BLE&nbsp;15<br></font>"\
	"<font color=\"#008000\">#define&nbsp;LDR&nbsp;16<br></font>"\
	"<font color=\"#008000\">#define&nbsp;ADR&nbsp;17<br></font>"\
	"<font color=\"#008000\">#define&nbsp;BLX&nbsp;18<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;ADDRESS&nbsp;0xFF<br></font>"\
	"		</font>"\


