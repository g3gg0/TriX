

void setup_hwdt_handles (u32 base, void *handle)
{	
	opcode_handles[base] = handle;
	opcode_handles[base|0x20] = handle;
	opcode_handles[base|0x80] = handle;
	opcode_handles[base|0x80|0x20] = handle;
	opcode_handles[base|0x100] = handle;
	opcode_handles[base|0x100|0x20] = handle;
	opcode_handles[base|0x100|0x80] = handle;
	opcode_handles[base|0x100|0x80|0x20] = handle;
}

void setup_hwdt_handles2 (u32 base, void *handle, void *handle2)
{	
	opcode_handles[base] = handle;
	opcode_handles[base|0x20] = handle;
	opcode_handles[base|0x80] = handle;
	opcode_handles[base|0x80|0x20] = handle;
	opcode_handles[base|0x100] = handle2;
	opcode_handles[base|0x100|0x20] = handle2;
	opcode_handles[base|0x100|0x80] = handle2;
	opcode_handles[base|0x100|0x80|0x20] = handle2;
}

void setup_sdt_handles2 (u32 base, void *handle, void *handle2, void *handle3, void *handle4)
{
	int i;

	for (i=0; i<0x10; i++) 
	{
		opcode_handles [base|i]					= handle;
		opcode_handles [base|0x20|i]			= handle;
		opcode_handles [base|0x80|i]			= handle2;
		opcode_handles [base|0x80|0x20|i]		= handle2;
		opcode_handles [base|0x100|i]			= handle3;
		opcode_handles [base|0x100|0x80|i]		= handle4;
		opcode_handles [base|0x100|0x20|i]		= handle3;
		opcode_handles [base|0x100|0x80|0x20|i]	= handle4;
	}
}

void setup_dp_handle (u32 base, void *ins, void *ins_reg, void *ins_imm)
{
	int i;

	for (i=0; i<8; i++)
	{
		opcode_handles[base|(i<<1)]           = ins;		
		opcode_handles[base|0x200|(i<<1)]     = ins_imm;
		opcode_handles[base|0x200|(i<<1)|0x1] = ins_imm;
		opcode_handles[base|((i&3)<<1)|0x1]   = ins_reg;	
	}
}

void setup_handle_tables (void)
{
	int i, n;

	for (i=0; i<0x1000; i++)
	{
		opcode_handles[i] = unknown_opcode;
		opcode_handles_special[i] = unknown_opcode;
	}

	setup_dp_handle (0x000, ins_and, ins_and_reg, ins_and_imm);
	setup_dp_handle (0x080, ins_add, ins_add_reg, ins_add_imm);
	setup_dp_handle (0x040, ins_sub, ins_sub_reg, ins_sub_imm);
	setup_dp_handle (0x150, ins_cmp, ins_cmp_reg, ins_cmp_imm);
	setup_dp_handle (0x020, ins_eor, ins_eor_reg, ins_eor_imm);
	setup_dp_handle (0x060, ins_rsb, ins_rsb_reg, ins_rsb_imm);
	setup_dp_handle (0x0A0, ins_adc, ins_adc_reg, ins_adc_imm);
	setup_dp_handle (0x0C0, ins_sbc, ins_sbc_reg, ins_sbc_imm);
	setup_dp_handle (0x0E0, ins_rsc, ins_rsc_reg, ins_rsc_imm);
	setup_dp_handle (0x110, ins_tst, ins_tst_reg, ins_tst_imm);
	setup_dp_handle (0x130, ins_teq, ins_teq_reg, ins_teq_imm);
	setup_dp_handle (0x170, ins_cmn, ins_cmn_reg, ins_cmn_imm);
	setup_dp_handle (0x180, ins_orr, ins_orr_reg, ins_orr_imm);
	setup_dp_handle (0x1A0, ins_mov, ins_mov_reg, ins_mov_imm);
	setup_dp_handle (0x1C0, ins_bic, ins_bic_reg, ins_bic_imm);
	setup_dp_handle (0x1E0, ins_mvn, ins_mvn_reg, ins_mvn_imm);
	setup_dp_handle (0x010, ins_ands, ins_ands_reg, ins_ands_imm);
	setup_dp_handle (0x090, ins_adds, ins_adds_reg, ins_adds_imm);
	setup_dp_handle (0x050, ins_subs, ins_subs_reg, ins_subs_imm);
	setup_dp_handle (0x030, ins_eors, ins_eors_reg, ins_eors_imm);
	setup_dp_handle (0x070, ins_rsbs, ins_rsbs_reg, ins_rsbs_imm);
	setup_dp_handle (0x0B0, ins_adcs, ins_adcs_reg, ins_adcs_imm);
	setup_dp_handle (0x0D0, ins_sbcs, ins_sbcs_reg, ins_sbcs_imm);
	setup_dp_handle (0x0F0, ins_rscs, ins_rscs_reg, ins_rscs_imm);
	setup_dp_handle (0x190, ins_orrs, ins_orrs_reg, ins_orrs_imm);
	setup_dp_handle (0x1B0, ins_movs, ins_movs_reg, ins_movs_imm);
	setup_dp_handle (0x1D0, ins_bics, ins_bics_reg, ins_bics_imm);
	setup_dp_handle (0x1F0, ins_mvns, ins_mvns_reg, ins_mvns_imm);

	opcode_handles[0x100] = ins_mrs_cpsr;
	opcode_handles[0x140] = ins_mrs_spsr;
	opcode_handles[0x120] = ins_msr_cpsr;
	opcode_handles[0x160] = ins_msr_spsr;

	for (i=0; i<0x10; i++) 
	{
		//opcode_handles[0x300|i] = ins_tst_imm;
		//opcode_handles[0x340|i] = ins_cmp_imm;
		opcode_handles[0x320|i] = ins_msr_cpsr_imm;
		opcode_handles[0x360|i] = ins_msr_spsr_imm;

		/* STM(2) */
		opcode_handles[0x840|i] = ins_stm;
		opcode_handles[0x940|i] = ins_stm;
		opcode_handles[0x8C0|i] = ins_stm;
		opcode_handles[0x9C0|i] = ins_stm;
		setup_hwdt_handles ( 0x800|i, ins_stm ); /* STM(1) */
		setup_hwdt_handles ( 0x840|i, ins_stm ); /* STM(2) */
		setup_hwdt_handles ( 0x810|i, ins_ldm ); /* LDM(1) */
		setup_hwdt_handles ( 0x850|i, ins_ldm ); /* LDM(2)/LDM(3) */

		for (n=0; n<0x8; n++)
		{
			opcode_handles[0xE00|(i<<4)|(n<<1)] = ins_cdp;
		}
	}

	setup_sdt_handles2 (0x600, ins_str_post_down, ins_str_post_up, ins_str_pre_down, ins_str_pre_up);
	setup_sdt_handles2 (0x640, ins_strb_post_down, ins_strb_post_up, ins_strb_pre_down, ins_strb_pre_up);
	setup_sdt_handles2 (0x400, ins_str_imm_post_down, ins_str_imm_post_up, ins_str_imm_pre_down, ins_str_imm_pre_up);
	setup_sdt_handles2 (0x440, ins_strb_imm_post_down, ins_strb_imm_post_up, ins_strb_imm_pre_down, ins_strb_imm_pre_up);
	setup_sdt_handles2 (0x610, ins_ldr_post_down, ins_ldr_post_up, ins_ldr_pre_down, ins_ldr_pre_up);
	setup_sdt_handles2 (0x650, ins_ldrb_post_down, ins_ldrb_post_up, ins_ldrb_pre_down, ins_ldrb_pre_up);
	setup_sdt_handles2 (0x410, ins_ldr_imm_post_down, ins_ldr_imm_post_up, ins_ldr_imm_pre_down, ins_ldr_imm_pre_up);
	setup_sdt_handles2 (0x450, ins_ldrb_imm_post_down, ins_ldrb_imm_post_up, ins_ldrb_imm_pre_down, ins_ldrb_imm_pre_up);

	setup_hwdt_handles ( 0x05D, ins_ldrb ); // imm
	setup_hwdt_handles ( 0x01D, ins_ldrb ); // reg

	setup_hwdt_handles ( 0x05F, ins_ldrh ); // imm 
	setup_hwdt_handles ( 0x01F, ins_ldrh ); // reg

	setup_hwdt_handles ( 0x05B, ins_ldrh ); // imm
	setup_hwdt_handles ( 0x01B, ins_ldrh ); // reg

	setup_hwdt_handles ( 0x04B, ins_strh ); // imm
	setup_hwdt_handles ( 0x00B, ins_strh ); // reg

	opcode_handles[0x009] = ins_mul;
	opcode_handles[0x019] = ins_muls;
	opcode_handles[0x029] = ins_mla;
	opcode_handles[0x039] = ins_mlas;

	opcode_handles[0x089] = ins_umull;
	opcode_handles[0x099] = ins_umulls;
	opcode_handles[0x0C9] = ins_smull;
	opcode_handles[0x0E9] = ins_smulls;

	opcode_handles[0x0A9] = ins_umlal;
	opcode_handles[0x0B9] = ins_umlals;
	opcode_handles[0x0E9] = ins_smlal;
	opcode_handles[0x0F9] = ins_smlals;

	opcode_handles[0x109] = ins_swp;
	opcode_handles[0x149] = ins_swpb;
	
	opcode_handles[0x121] = ins_bx;
	opcode_handles[0x123] = ins_blx;

	opcode_handles[0x161] = ins_clz;


	for (i=0; i<0x100; i++)
	{
		opcode_handles[0xF00|i] = ins_swi;
	}

	for (i=0; i<0x80; i++) 
	{
		opcode_handles[0xA00|i] = ins_bpl;
		opcode_handles[0xB00|i] = ins_blpl;
		opcode_handles[0xA80|i] = ins_bmi;
		opcode_handles[0xB80|i] = ins_blmi;
		opcode_handles_special[0xA00|i] = ins_bpl;
		opcode_handles_special[0xB00|i] = ins_blpl;
		opcode_handles_special[0xA80|i] = ins_bmi;
		opcode_handles_special[0xB80|i] = ins_blmi;
	}

	for (i=0; i<0x8; i++) 
	{
		for (n=0; n<0x8; n++) 
		{
			opcode_handles[0xE11|(i<<5)|(n<<1)] = ins_mrc;
			opcode_handles[0xE01|(i<<5)|(n<<1)] = ins_mcr;
		}
	}

	// enhanced DSP instructions
	for (i=0; i<0x10; i++) 
	{
		opcode_handles[0x00D | (i<<5)] = ins_ldrd;
		opcode_handles[0x00F | (i<<5)] = ins_strd;
	}
}

void setup_string_tables(void)
{	
	int i;

	for (i=0; i<0x20; i++)
		cpu_mode_strings [i] = "BadCpuMode";

	cpu_mode_strings [0x10] = "User";
	cpu_mode_strings [0x11] = "FIQ";
	cpu_mode_strings [0x12] = "IRQ";
	cpu_mode_strings [0x13] = "Supervisor";
	cpu_mode_strings [0x17] = "Abort";
	cpu_mode_strings [0x1B] = "Undefined";
	cpu_mode_strings [0x1F] = "System";
}

