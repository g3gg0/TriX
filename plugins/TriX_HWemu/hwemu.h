#ifndef __TRIX_HWEMU_H__
#define __TRIX_HWEMU_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );



#define HWEMU_HEADERS \
	"import unsigned int __hwemu_register ( t_workspace *ws, unsigned int address );\n" \


#define HWEMU_SYMBOLS \
	REGISTER_SYMBOL ( hwemu_register );\
	REGISTER_SYMBOL ( hwemu_handler );\

unsigned int hwemu_init ( );
unsigned int hwemu_register ( t_workspace *ws, unsigned int address );
unsigned int hwemu_handler ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );
unsigned int hwemu_default_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );



extern int system_type;
extern int upp_id;
extern int arch_type;

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



#define FAID_BASE         (hw_faid_bases[arch_type])
#define FLASH_BASE        (hw_flash_bases[arch_type])

#define LEAD3MMI_BASE     (hw_lead3mmi_bases[arch_type])
#define DSPPLL_BASE       (hw_dsppll_bases[arch_type])
#define MCUPLL_BASE       (hw_mcupll_bases[arch_type])
#define ACCPLL_BASE       (hw_accpll_bases[arch_type])
#define DSPMIF_BASE       (hw_dspmif_bases[arch_type])
#define EBUSC_BASE        (hw_ebusc_bases[arch_type])
#define FSIG_BASE         (hw_fsig_bases[arch_type])
#define TESTIF_BASE       (hw_testif_bases[arch_type])
#define CBUSC_BASE        (hw_cbusc_bases[arch_type])
#define STI_BASE          (hw_sti_bases[arch_type])
#define CTSI_BASE         (hw_ctsi_bases[arch_type])
#define PUP_BASE          (hw_pup_bases[arch_type])
#define UIF_BASE          (hw_uif_bases[arch_type])
#define ACCIF_BASE        (hw_accif_bases[arch_type])
#define SIMIF_BASE        (hw_simif_bases[arch_type])
#define MFI_BASE          (hw_mfi_bases[arch_type])
#define BRAIN_BASE        (hw_brain_bases[arch_type])
#define OSMODAT_BASE      (hw_osmodat_bases[arch_type])
#define OSMOCMD_BASE      (hw_osmocmd_bases[arch_type])

#define HWEMU_VERB_CBUS     0x00000001
#define HWEMU_VERB_CTSI     0x00000002
#define HWEMU_VERB_PUP      0x00000004
#define HWEMU_VERB_STI      0x00000008
#define HWEMU_VERB_SIMIF    0x00000010
#define HWEMU_VERB_TESTIF   0x00000020
#define HWEMU_VERB_EBUSC    0x00000040
#define HWEMU_VERB_UIF      0x00000080
#define HWEMU_VERB_ACCPLL   0x00000100
#define HWEMU_VERB_BRAIN    0x00000200
#define HWEMU_VERB_CBUSC    0x00000400
#define HWEMU_VERB_DSPMIF   0x00000800
#define HWEMU_VERB_DSPPLL   0x00001000
#define HWEMU_VERB_FLASH    0x00002000
#define HWEMU_VERB_FSIG     0x00004000
#define HWEMU_VERB_LEAD3MMI 0x00008000
#define HWEMU_VERB_MCUPLL   0x00010000
#define HWEMU_VERB_MFI      0x00020000
#define HWEMU_VERB_OSMO     0x00040000
#define HWEMU_VERB_ACCIF    0x00080000
#define HWEMU_VERB_SPI      0x00100000





#ifndef __TRIX_HWEMU_C__
extern int verbosity;
#endif

#endif

