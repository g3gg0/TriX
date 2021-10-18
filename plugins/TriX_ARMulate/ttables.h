void setup_handle_tables_t (void)
{
	int i;

	for (i=0; i<0x400; i++) 
		opcode_handles_t [i] = tins_unknown;


	opcode_handles_t [0x100] = tins_and;
	opcode_handles_t [0x101] = tins_eor;
	opcode_handles_t [0x102] = tins_lsl;
	opcode_handles_t [0x103] = tins_lsr;
	opcode_handles_t [0x104] = tins_asr;
	opcode_handles_t [0x105] = tins_adc;
	opcode_handles_t [0x106] = tins_sbc;
	opcode_handles_t [0x107] = tins_ror;
	opcode_handles_t [0x108] = tins_tst;
	opcode_handles_t [0x109] = tins_neg;
	opcode_handles_t [0x10A] = tins_cmp;
	opcode_handles_t [0x10B] = tins_cmn;
	opcode_handles_t [0x10C] = tins_orr;
	opcode_handles_t [0x10D] = tins_mul;
	opcode_handles_t [0x10E] = tins_bic;
	opcode_handles_t [0x10F] = tins_mvn;

	opcode_handles_t [0x110|0x1] = tins_add_lo_hi;
	opcode_handles_t [0x110|0x2] = tins_add_lo_hi;
	opcode_handles_t [0x110|0x3] = tins_add_lo_hi;

	opcode_handles_t [0x114|0x1] = tins_cmp_lo_hi;
	opcode_handles_t [0x114|0x2] = tins_cmp_lo_hi;
	opcode_handles_t [0x114|0x3] = tins_cmp_lo_hi;

	opcode_handles_t [0x118|0x1] = tins_mov_lo_hi;
	opcode_handles_t [0x118|0x2] = tins_mov_hi_lo;
	opcode_handles_t [0x118|0x3] = tins_mov_hi_hi;

	opcode_handles_t [0x11C]     = tins_bx;
	opcode_handles_t [0x11C|0x1] = tins_bx;
	opcode_handles_t [0x11C|0x2] = tins_bx;
	opcode_handles_t [0x11C|0x3] = tins_bx;

	/* 7 bits fixed */
	for (i=0; i<0x8; i++) {
		opcode_handles_t [0x060|i] = tins_add_3;
		opcode_handles_t [0x068|i] = tins_sub_3;
		opcode_handles_t [0x070|i] = tins_add_short_imm;
		opcode_handles_t [0x078|i] = tins_sub_short_imm;
		opcode_handles_t [0x140|i] = tins_str;
		opcode_handles_t [0x148|i] = tins_strh;
		opcode_handles_t [0x150|i] = tins_strb;
		opcode_handles_t [0x158|i] = tins_ldrsb;
		opcode_handles_t [0x160|i] = tins_ldr;
		opcode_handles_t [0x168|i] = tins_ldrh;
		opcode_handles_t [0x170|i] = tins_ldrb;
		opcode_handles_t [0x178|i] = tins_ldrsh;
		opcode_handles_t [0x2D0|i] = tins_push;
		opcode_handles_t [0x2F0|i] = tins_pop;
	}	

	/* 5 bits fixed */
	for (i=0; i<=0x1F; i++) {
		opcode_handles_t [i] = tins_lsl_imm;
		opcode_handles_t [0x020|i] = tins_lsr_imm;
		opcode_handles_t [0x040|i] = tins_asr_imm;
		opcode_handles_t [0x080|i] = tins_mov_imm;
		opcode_handles_t [0x0A0|i] = tins_cmp_imm;
		opcode_handles_t [0x0C0|i] = tins_add_imm;
		opcode_handles_t [0x0E0|i] = tins_sub_imm;
		opcode_handles_t [0x120|i] = tins_pc_rel_ldr;
		opcode_handles_t [0x180|i] = tins_str_imm;
		opcode_handles_t [0x1A0|i] = tins_ldr_imm;
		opcode_handles_t [0x1C0|i] = tins_strb_imm;
		opcode_handles_t [0x1E0|i] = tins_ldrb_imm;
		opcode_handles_t [0x200|i] = tins_strh_imm;
		opcode_handles_t [0x220|i] = tins_ldrh_imm;
		opcode_handles_t [0x240|i] = tins_sp_rel_str;
		opcode_handles_t [0x260|i] = tins_sp_rel_ldr;
		opcode_handles_t [0x280|i] = tins_pc_add;
		opcode_handles_t [0x2A0|i] = tins_sp_add;
		opcode_handles_t [0x300|i] = tins_stmia;
		opcode_handles_t [0x320|i] = tins_ldmia;
		opcode_handles_t [0x380|i] = tins_b;
		opcode_handles_t [0x3A0|i] = tins_bl;
		opcode_handles_t [0x3C0|i] = tins_bl;
		opcode_handles_t [0x3E0|i] = tins_bl;
	}

	/* 4 bits fixed */
	for (i=0; i<0x3F; i++) {
		opcode_handles_t [0x340|i] = tins_bcond;
	}

	for (i=0; i<0x4; i++) 
	{
		opcode_handles_t [0x2C0|i] = tins_add_sp;
		opcode_handles_t [0x37C|i] = tins_swi;
	}


}
