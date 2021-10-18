#ifndef _HW_DEFINES_H_
#define _HW_DEFINES_H_


#define INT_VECTOR_RESET      0
#define INT_VECTOR_UNDEF      1
#define INT_VECTOR_SWI        2
#define INT_VECTOR_ABORT_INST 3
#define INT_VECTOR_ABORT_DATA 4
#define INT_VECTOR_IRQ        5
#define INT_VECTOR_FIQ        6

extern unsigned int HW_DEVICE_TYPE;

#define DEV_TYPE_ASIC2    2
#define DEV_TYPE_ASIC11  11
#define DEV_TYPE_TIKU     7

#define TIKU_DEVICE (HW_DEVICE_TYPE==DEV_TYPE_TIKU)
#define ASIC2_DEVICE (HW_DEVICE_TYPE==DEV_TYPE_ASIC2)
#define ASIC11_DEVICE (HW_DEVICE_TYPE==DEV_TYPE_ASIC11)

#define DEV_TYPE_BASE_OFFSET (TIKU_DEVICE?0:1)

extern int hw_faid_bases[];
extern int hw_flash_bases[];

extern int hw_lead3mmi_bases[];
extern int hw_dsppll_bases[];
extern int hw_mcupll_bases[];
extern int hw_accpll_bases[];
extern int hw_dspmif_bases[];
extern int hw_ebusc_bases[]; 
extern int hw_fsig_bases[];
extern int hw_testif_bases[];
extern int hw_cbusc_bases[];
extern int hw_sti_bases[];
extern int hw_ctsi_bases[];
extern int hw_pup_bases[];
extern int hw_uif_bases[];
extern int hw_accif_bases[];
extern int hw_simif_bases[];
extern int hw_mfi_bases[];
extern int hw_brain_bases[];
extern int hw_osmodat_bases[];
extern int hw_osmocmd_bases[];

extern int hw_ram_bases[];
extern int hw_intvec_bases[];

extern int hw_endianess_little[];


#define FAID_BASE         (hw_faid_bases[DEV_TYPE_BASE_OFFSET])
#define FLASH_BASE        (hw_flash_bases[DEV_TYPE_BASE_OFFSET])

#define LEAD3MMI_BASE     (hw_lead3mmi_bases[DEV_TYPE_BASE_OFFSET])
#define DSPPLL_BASE       (hw_dsppll_bases[DEV_TYPE_BASE_OFFSET])
#define MCUPLL_BASE       (hw_mcupll_bases[DEV_TYPE_BASE_OFFSET])
#define ACCPLL_BASE       (hw_accpll_bases[DEV_TYPE_BASE_OFFSET])
#define DSPMIF_BASE       (hw_dspmif_bases[DEV_TYPE_BASE_OFFSET])
#define EBUSC_BASE        (hw_ebusc_bases[DEV_TYPE_BASE_OFFSET])
#define FSIG_BASE         (hw_fsig_bases[DEV_TYPE_BASE_OFFSET])
#define TESTIF_BASE       (hw_testif_bases[DEV_TYPE_BASE_OFFSET])
#define CBUSC_BASE        (hw_cbusc_bases[DEV_TYPE_BASE_OFFSET])
#define STI_BASE          (hw_sti_bases[DEV_TYPE_BASE_OFFSET])
#define CTSI_BASE         (hw_ctsi_bases[DEV_TYPE_BASE_OFFSET])
#define PUP_BASE          (hw_pup_bases[DEV_TYPE_BASE_OFFSET])
#define UIF_BASE          (hw_uif_bases[DEV_TYPE_BASE_OFFSET])
#define ACCIF_BASE        (hw_accif_bases[DEV_TYPE_BASE_OFFSET])
#define SIMIF_BASE        (hw_simif_bases[DEV_TYPE_BASE_OFFSET])
#define MFI_BASE          (hw_mfi_bases[DEV_TYPE_BASE_OFFSET])
#define BRAIN_BASE        (hw_brain_bases[DEV_TYPE_BASE_OFFSET])
#define OSMODAT_BASE      (hw_osmodat_bases[DEV_TYPE_BASE_OFFSET])
#define OSMOCMD_BASE      (hw_osmocmd_bases[DEV_TYPE_BASE_OFFSET])

#define RAM_BASE          (hw_ram_bases[DEV_TYPE_BASE_OFFSET])
#define INT_VECTOR_BASE   (hw_intvec_bases[DEV_TYPE_BASE_OFFSET])

#define IRQ_STACK         (RAM_BASE+0x0800)
#define OWN_STACK         (RAM_BASE+0x1000)
#define OWN_BUFFER        (RAM_BASE+0x2000)


#define LITTLE_ENDIAN     hw_endianess_little[DEV_TYPE_BASE_OFFSET]


#define CBUS_CLK_SET      (CTSI_BASE + 0x0034)
#define CBUS_CLK_RESET    (CTSI_BASE + 0x0036)

#define CBUS_BITCOUNT     (UIF_BASE + 0x0020)
#define CBUS_CTRL         (UIF_BASE + 0x0022)
#define CBUS_DATA_HIGH    (UIF_BASE + 0x0024)
#define CBUS_DATA_LOW     (UIF_BASE + 0x0026)
#define CBUS_CLK_SPEED    (UIF_BASE + 0x0030)

#define CBUS_CTRL_READY   (0x0400)
#define CBUS_CTRL_WRITE   (0x0200)
#define CBUS_CTRL_READ    (0x0100)
#define CBUS_BITS_WRITE   (0xE0)
#define CBUS_INIT         (0x40)

#define CLK_SPEED         (0x66)

#define CBUSC_INIT        (CBUSC_BASE + 0x0000)
#define CBUSC_FAID        (CBUSC_BASE + 0x001A)
#define CBUSC_BYTE        (CBUSC_BASE + 0x0036)

#define UEM_ADDRESS       (1)

// evil macros to convert defined constants to strings
#define STR(x) #x
#define ADDR_STR(x) STR(x)

#endif
