
#define MAX             512
#define CMD_COMPILE     1
#define CMD_PRECOMPILE  2

typedef struct s_label {
	char   *name;
	int     pos;
} label;

typedef struct s_func {
	char   *name;
	int     pos;
} func;

typedef struct s_patch {
	char   *name;
	int     pos;
	int     abs;
} patch;

typedef struct s__imp {
	char   *name;
	int     addr;
} imp;

typedef struct s_ref {
	void   *data;
	int     used;
	int     pos;
} ref;

typedef struct s_error {
	char   *text;
	int     line;
} error;

typedef struct s_warning {
	char   *text;
	int     line;
} warning;


typedef struct code_data {
	unsigned char    initialized;
	unsigned char   *data;
	char   *description;
	char   *name;
	int		use_shmem;
	int     current_line;
	int     arm32_flag_s;
	int     arm32_flag_b;
	int     arm32_flag_t;
	int     arm32_flag_h;
	int     arm32_flag_sh;
	int     arm32_flag_sb;
	int     arm32_flag_pu;
	int     arm32_field_cond;
    int     bytes;
	int     data_pos;
	int     end;
	int     last;
	int     bytepos;
	int     maxsize;
    int     dump;
    int     located;
    int     endianess;
	unsigned long start;
	unsigned char *base;
	unsigned long length;
	int           inject;
	unsigned long inj_pos;
	int     mode;
	int     arch_mode;
	func    functions[MAX];
	imp     imports[MAX];
	ref     dword_refs[MAX];
	label   labels[MAX];
	patch   b_patches[MAX];
	patch   d_patches[MAX];
	error   errors[MAX];
	error   warnings[MAX];
	void    (*cleanup) ( struct code_data *c );
	int     comments;
} code_t;

typedef struct
{
	unsigned char *string;
	unsigned int code;
} t_arm32_cond;



#define MODE_NORM 0
#define MODE_HEX  1
#define MODE_GSC  2
#define MODE_TRIX 3
#define MODE_NRX  4

#define ENDIAN_BE 0
#define ENDIAN_LE 1

#define MODE_THUMB 1
#define MODE_ARM32 2
#define MODE_ANY   3

struct s_comp {
	unsigned int mode;
	unsigned int modifiers;
	char   *opcode;
	int    (*function) ( struct code_data *, char *, int  );
	int     bytes;
	int params; // -1:    -2:     flag 0x80: case sensitive
	unsigned int p1;
	unsigned int p2;
	unsigned int p3;
	unsigned int p4;
	unsigned int p5;
	unsigned int p6;
	int     optype;
	char   *description;
};

typedef struct _locator {

    unsigned char *func_name;    //   draw_signal_strength          name for displaying in log window
    unsigned char *pattern;      //   b5 f0 20 00 21 00 22 00 ...   the bytes to search
    unsigned char *mask;         //   ff ff ff ff ff 00 ff 00 ...   bitmask for the pattern
    unsigned int   length;       //   20                            what do u think?!?!
    int            no_search_back;    // normally the engine searches back for a B5 XX (PUSH) 1 to disable this
} locator;

#define ASM_BL_1    (unsigned char)0xf0
#define ASM_BL_2    (unsigned char)0xf8
#define ASM_B       (unsigned char)0xe0
#define ASM_CMP     (unsigned char)0x28
#define ASM_PUSH    (unsigned char)0xb4
#define ASM_PUSHL   (unsigned char)0xb5
#define ASM_POP     (unsigned char)0xbc
#define ASM_POPL    (unsigned char)0xbd
#define ASM_BRANCH  (unsigned char)0xd0
#define ASM_LBRANCH (unsigned char)0xe0
#define ASM_MOV     (unsigned char)0x20
#define ASM_MOV_H   (unsigned char)0x44
#define ASM_MOV_R   (unsigned char)0x1c
#define ASM_ADDRH   (unsigned char)0xa0
#define ASM_ADDSP   (unsigned char)0xb0
#define ASM_LDR     (unsigned char)0x48
#define ASM_LDRH    (unsigned char)0x80
#define ASM_LS      (unsigned char)0x00
#define ASM_NOP_1   (unsigned char)0x46
#define ASM_NOP_2   (unsigned char)0xc0
#define ASM_CMP_MASK       (unsigned char)0xf8
#define ASM_BL_MASK        (unsigned char)0xf8
#define ASM_LS_MASK        (unsigned char)0xe0
#define ASM_ADDRH_MASK     (unsigned char)0xf0
#define ASM_BRANCH_MASK    (unsigned char)0xf0
#define ASM_LBRANCH_MASK   (unsigned char)0xf0
#define ASM_MOV_MASK       (unsigned char)0xf8
#define ASM_MOV_H_MASK     (unsigned char)0xfc
#define ASM_LDR_MASK       (unsigned char)0xf8
#define ASM_LDRH_MASK      (unsigned char)0xf0
#define ASM_LDRB_MASK      (unsigned char)0x78
#define ASM_LDRB           (unsigned char)0x78



#define  E_UNK_OPCODE     1
#define  E_MISS_SPACE     2
#define  E_WRONG_PARM     3
#define  E_WRONG_ALIGN    4
#define  E_WRONG_DALIGN   5
#define  E_INV_VAL        6
#define  E_INV_VAL_ALIGN  7
#define  E_SHORT_DISTANCE 8
#define  E_LONG_DISTANCE  9
#define  E_INV_REG       10
#define  E_UNK_REG       11
#define  C_ERR   0xFFFFFFFF

#define VER "3.4"


#define VERSION "// Armada "VER"\r\n// Development release compiled:\r\n//"


unsigned long get_highest_pos ( void );
int initCompiler ( void );
struct s_comp *GetOpcodes ( void );
char *compile      ( char  *text );
int add_func ( struct code_data *c, char *func, int pos );
int get_label ( struct code_data *c, char *label );
int add_dword ( struct code_data *c, char *data );
int add_import ( struct code_data *c, char *func, unsigned long addr );
unsigned long get_import ( struct code_data *c, char *func );
int add_dpatch ( struct code_data *c, char *name, int pos, int abs );
struct code_data *get_code ( void );
void free_code ( struct code_data *c );
int set_flash ( unsigned char* _base, unsigned long _length );
int asm_locate_functions ( code_t *c );
char *get_line ( char *text, unsigned int line );
int add_error ( struct code_data *c, char *m, int line );
int add_warning ( struct code_data *c, char *m, int line );


int   comp_code    ( struct code_data *c, char *t, int cmd );
int   comp_align   ( struct code_data *c, char *t, int cmd );
int   comp_gsc     ( struct code_data *c, char *t, int cmd );
int   comp_dmp     ( struct code_data *c, char *t, int cmd );
int   comp_loc     ( struct code_data *c, char *t, int cmd );
int   comp_ens     ( struct code_data *c, char *t, int cmd );
int   comp_nam     ( struct code_data *c, char *t, int cmd );
int   comp_des     ( struct code_data *c, char *t, int cmd );
int   comp_inj     ( struct code_data *c, char *t, int cmd );
int   comp_idd     ( struct code_data *c, char *t, int cmd );
int   comp_idd     ( struct code_data *c, char *t, int cmd );
int   comp_imp     ( struct code_data *c, char *t, int cmd );
int   comp_uni     ( struct code_data *c, char *t, int cmd );
int   comp_asc     ( struct code_data *c, char *t, int cmd );
int   comp_end     ( struct code_data *c, char *t, int cmd );
int   comp_org     ( struct code_data *c, char *t, int cmd );
int   comp_size    ( struct code_data *c, char *t, int cmd );
int   comp_hex     ( struct code_data *c, char *t, int cmd );
int   comp_trix    ( struct code_data *c, char *t, int cmd );
int   comp_nrx     ( struct code_data *c, char *t, int cmd );
int   comp_mov     ( struct code_data *c, char *t, int cmd );
int   comp_ldr     ( struct code_data *c, char *t, int cmd );
int   comp_cmp     ( struct code_data *c, char *t, int cmd );
int   comp_b       ( struct code_data *c, char *t, int cmd );
int   comp_bl      ( struct code_data *c, char *t, int cmd );
int   comp_blx     ( struct code_data *c, char *t, int cmd );
int   comp_subr    ( struct code_data *c, char *t, int cmd );
int   comp_addr    ( struct code_data *c, char *t, int cmd );
int   comp_subv    ( struct code_data *c, char *t, int cmd );
int   comp_addv    ( struct code_data *c, char *t, int cmd );
int   comp_lsl     ( struct code_data *c, char *t, int cmd );
int   comp_lsr     ( struct code_data *c, char *t, int cmd );
int   comp_asr     ( struct code_data *c, char *t, int cmd );
int   comp_nop     ( struct code_data *c, char *t, int cmd );

int   comp_add     ( struct code_data *c, char *t, int cmd );
int   comp_sub     ( struct code_data *c, char *t, int cmd );

int   comp_beq     ( struct code_data *c, char *t, int cmd );
int   comp_bne     ( struct code_data *c, char *t, int cmd );
int   comp_bcs     ( struct code_data *c, char *t, int cmd );
int   comp_bcc     ( struct code_data *c, char *t, int cmd );
int   comp_bmi     ( struct code_data *c, char *t, int cmd );
int   comp_bpl     ( struct code_data *c, char *t, int cmd );
int   comp_bvs     ( struct code_data *c, char *t, int cmd );
int   comp_bvc     ( struct code_data *c, char *t, int cmd );
int   comp_bhi     ( struct code_data *c, char *t, int cmd );
int   comp_bls     ( struct code_data *c, char *t, int cmd );
int   comp_bge     ( struct code_data *c, char *t, int cmd );
int   comp_blt     ( struct code_data *c, char *t, int cmd );
int   comp_bgt     ( struct code_data *c, char *t, int cmd );
int   comp_ble     ( struct code_data *c, char *t, int cmd );

int   comp_and     ( struct code_data *c, char *t, int cmd );
int   comp_eor     ( struct code_data *c, char *t, int cmd );
int   comp_lslr    ( struct code_data *c, char *t, int cmd );
int   comp_lsrr    ( struct code_data *c, char *t, int cmd );
int   comp_asrr    ( struct code_data *c, char *t, int cmd );
int   comp_adc     ( struct code_data *c, char *t, int cmd );
int   comp_sbc     ( struct code_data *c, char *t, int cmd );
int   comp_ror     ( struct code_data *c, char *t, int cmd );
int   comp_tst     ( struct code_data *c, char *t, int cmd );
int   comp_neg     ( struct code_data *c, char *t, int cmd );
int   comp_cmpr    ( struct code_data *c, char *t, int cmd );
int   comp_cmn     ( struct code_data *c, char *t, int cmd );
int   comp_orr     ( struct code_data *c, char *t, int cmd );
int   comp_mul     ( struct code_data *c, char *t, int cmd );
int   comp_bic     ( struct code_data *c, char *t, int cmd );
int   comp_mvn     ( struct code_data *c, char *t, int cmd );

int   comp_ldrd    ( struct code_data *c, char *t, int cmd );

int   comp_ldr     ( struct code_data *c, char *t, int cmd );
int   comp_str     ( struct code_data *c, char *t, int cmd );
int   comp_ldrb    ( struct code_data *c, char *t, int cmd );
int   comp_strb    ( struct code_data *c, char *t, int cmd );

int   comp_strh    ( struct code_data *c, char *t, int cmd );
int   comp_strhr   ( struct code_data *c, char *t, int cmd );
int   comp_ldrh    ( struct code_data *c, char *t, int cmd );
int   comp_ldsb    ( struct code_data *c, char *t, int cmd );
int   comp_ldsh    ( struct code_data *c, char *t, int cmd );
int   comp_ldrhr   ( struct code_data *c, char *t, int cmd );

int   comp_ldrr    ( struct code_data *c, char *t, int cmd );
int   comp_strr    ( struct code_data *c, char *t, int cmd );
int   comp_ldrbr   ( struct code_data *c, char *t, int cmd );
int   comp_strbr   ( struct code_data *c, char *t, int cmd );

int   comp_db      ( struct code_data *c, char *t, int cmd );
int   comp_dw      ( struct code_data *c, char *t, int cmd );
int   comp_dd      ( struct code_data *c, char *t, int cmd );

int   comp_push    ( struct code_data *c, char *t, int cmd );
int   comp_pop     ( struct code_data *c, char *t, int cmd );

int   comp_ldrsp   ( struct code_data *c, char *t, int cmd );
int   comp_strsp   ( struct code_data *c, char *t, int cmd );

int   comp_addrh   ( struct code_data *c, char *t, int cmd );
int   comp_addhr   ( struct code_data *c, char *t, int cmd );
int   comp_addhh   ( struct code_data *c, char *t, int cmd );
int   comp_addrr   ( struct code_data *c, char *t, int cmd );
int   comp_cmprh   ( struct code_data *c, char *t, int cmd );
int   comp_cmphr   ( struct code_data *c, char *t, int cmd );
int   comp_cmphh   ( struct code_data *c, char *t, int cmd );
int   comp_movrh   ( struct code_data *c, char *t, int cmd );
int   comp_movhr   ( struct code_data *c, char *t, int cmd );
int   comp_movhh   ( struct code_data *c, char *t, int cmd );
int   comp_movrr   ( struct code_data *c, char *t, int cmd );
int   comp_bxr     ( struct code_data *c, char *t, int cmd );
int   comp_bxh     ( struct code_data *c, char *t, int cmd );
int   comp_blxr    ( struct code_data *c, char *t, int cmd );
int   comp_blxh    ( struct code_data *c, char *t, int cmd );


int   comp_addpcr  ( struct code_data *c, char *t, int cmd );
int   comp_addspr  ( struct code_data *c, char *t, int cmd );
int   comp_addsp   ( struct code_data *c, char *t, int cmd );
int   comp_subsp   ( struct code_data *c, char *t, int cmd );

int   comp_swi     ( struct code_data *c, char *t, int cmd );
int   comp_movpcrel( struct code_data *c, char *t, int cmd );

int   comp_stmia   ( struct code_data *c, char *t, int cmd );
int   comp_ldmia   ( struct code_data *c, char *t, int cmd );


int   comp32_bx       ( struct code_data *c, char *t, int cmd );
int   comp32_b        ( struct code_data *c, char *t, int cmd );
int   comp32_bl       ( struct code_data *c, char *t, int cmd );
int   comp32_mov      ( struct code_data *c, char *t, int cmd );
int   comp32_mov      ( struct code_data *c, char *t, int cmd );
int   comp32_mvn      ( struct code_data *c, char *t, int cmd );
int   comp32_cmp      ( struct code_data *c, char *t, int cmd );
int   comp32_cmn      ( struct code_data *c, char *t, int cmd );
int   comp32_teq      ( struct code_data *c, char *t, int cmd );
int   comp32_tst      ( struct code_data *c, char *t, int cmd );
int   comp32_and      ( struct code_data *c, char *t, int cmd );
int   comp32_eor      ( struct code_data *c, char *t, int cmd );
int   comp32_sub      ( struct code_data *c, char *t, int cmd );
int   comp32_rsb      ( struct code_data *c, char *t, int cmd );
int   comp32_add      ( struct code_data *c, char *t, int cmd );
int   comp32_adc      ( struct code_data *c, char *t, int cmd );
int   comp32_sbc      ( struct code_data *c, char *t, int cmd );
int   comp32_rsc      ( struct code_data *c, char *t, int cmd );
int   comp32_orr      ( struct code_data *c, char *t, int cmd );
int   comp32_bic      ( struct code_data *c, char *t, int cmd );

int   comp32_mrs      ( struct code_data *c, char *t, int cmd );
int   comp32_msr      ( struct code_data *c, char *t, int cmd );
int   comp32_mul      ( struct code_data *c, char *t, int cmd );
int   comp32_mla      ( struct code_data *c, char *t, int cmd );
int   comp32_umull      ( struct code_data *c, char *t, int cmd );
int   comp32_umlal      ( struct code_data *c, char *t, int cmd );
int   comp32_smull      ( struct code_data *c, char *t, int cmd );
int   comp32_smlal      ( struct code_data *c, char *t, int cmd );

int   comp32_ldr      ( struct code_data *c, char *t, int cmd );
int   comp32_str      ( struct code_data *c, char *t, int cmd );
int   comp32_ldrh     ( struct code_data *c, char *t, int cmd );
int   comp32_strh     ( struct code_data *c, char *t, int cmd );
int   comp32_ldm      ( struct code_data *c, char *t, int cmd );
int   comp32_stm      ( struct code_data *c, char *t, int cmd );

int   comp32_swp      ( struct code_data *c, char *t, int cmd );
int   comp32_swi      ( struct code_data *c, char *t, int cmd );
int   comp32_cdp      ( struct code_data *c, char *t, int cmd );

/*
.sub test1
.des own_test_function
push r1, r2 ,r3 ,r4
b @mark
mov r0,0
@mark:
pop r1, r2 ,r3 ,r4
.endsub

.hex 1
.org 12345
.sub test2
.des not_printed
push r1, r2 ,r3 ,r4
b @mark
mov r1,2
mov r1,r4
addsp r1, 10
@mark:
pop r1, r2 ,r3 ,r4
.endsub


.inj 0x32d00
.sub test3
.des another_test
push r1, r2 ,r3 ,r4
b @mark
mov r1,2
mov r1,r4
addsp r1, 10
bx r1
@mark:
pop r1, r2 ,r3 ,r4
.endsub


*/
