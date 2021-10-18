#ifndef _COPROCESSOR_H_
#define _COPROCESSOR_H_

#ifdef __COPROCESSOR_DEFINE__

/* the ASM code will use R1 as temporary data register. this allows to catch 
   exceptions and return fake values by setting R1 to some magic value.
*/

#define MRC_DEFINE(name,a,b,c,d,e) \
u32 name () \
{ \
	u32 value; \
	asm volatile (  \
		"adr     r0, .ta_"#name";" \
		"bx	     r0;" \
		".code 32;" \
		".ta_"#name":;" \
		"mrc     "#a","#b",r1,"#c","#d","#e";" \
		"mov     %0, r1;" \
		"adr     r0, .tt_"#name"+1;" \
		"bx	     r0;" \
		".code 16;" \
		".tt_"#name":;" \
		: "=r"(value) : : "cc", "r0", "r1" \
		); \
	return value; \
} \


#define MCR_DEFINE(name,a,b,c,d,e) \
void name (u32 value) \
{ \
	asm volatile (  \
		"adr     r0, .ta_"#name";" \
		"bx	     r0;" \
		".code 32;" \
		".ta_"#name":;" \
		"mcr     "#a","#b",%0,"#c","#d","#e";" \
		"adr     r0, .tt_"#name"+1;" \
		"bx	     r0;" \
		".code 16;" \
		".tt_"#name":;" \
		: : "r"(value) : "cc", "r0", "r1" \
		); \
} \

#else

/* declare only */
#define MRC_DEFINE(name,a,b,c,d,e) \
u32 name ();


#define MCR_DEFINE(name,a,b,c,d,e) \
void name (u32 value);

#endif

MRC_DEFINE(read_idcode, p15, 0, c0, c0, 0);
MRC_DEFINE(read_cache_type, p15, 0, c0, c0, 1);

MRC_DEFINE(read_cache_ctrl, p15, 0, c1, c0, 0);
MCR_DEFINE(write_cache_ctrl, p15, 0, c1, c0, 0);



MRC_DEFINE(read_test_state_regs, p15, 0, c15, c0, 0);
MRC_DEFINE(read_C15CI, p15, 3, c15, c1, 0);
MRC_DEFINE(read_C15CD, p15, 3, c15, c2, 0);


MCR_DEFINE(write_dcache_invalidate, p15, 0, c7, c6, 0);
MCR_DEFINE(write_dcache_victim_lockdown, p15, 0, c9, c0, 0);
MCR_DEFINE(write_dcache_cam, p15, 2, c15, c6, 6);
MCR_DEFINE(write_dcache_ram, p15, 2, c15, c10, 6);
MCR_DEFINE(write_dcache_ram_read, p15, 2, c15, c10, 2);

MCR_DEFINE(write_icache_invalidate, p15, 0, c7, c5, 0);
MCR_DEFINE(write_icache_victim_lockdown, p15, 0, c9, c0, 1);
MCR_DEFINE(write_icache_cam, p15, 2, c15, c5, 6);
MCR_DEFINE(write_icache_ram, p15, 2, c15, c9, 6);
MCR_DEFINE(write_icache_ram_read, p15, 2, c15, c9, 2);
MCR_DEFINE(write_icache_cam_match_read, p15, 2, c15, c5, 5);
MCR_DEFINE(write_icache_prefetch, p15, 0, c7, c13, 1);

MCR_DEFINE(write_C15CI, p15, 3, c15, c1, 0);
MCR_DEFINE(write_C15CD, p15, 3, c15, c2, 0);
MCR_DEFINE(write_C15CID, p15, 3, c15, c3, 0);
MCR_DEFINE(write_cam_match_d_read, p15, 2, c15, c6, 5);


#define MVA_GET_TAG(address)  (((u32)address)>>8)
#define MVA_GET_SEG(address)  ((((u32)address)>>5) & 3)
#define MVA_GET_WORD(address) ((((u32)address)>>2) & 3)

#endif
