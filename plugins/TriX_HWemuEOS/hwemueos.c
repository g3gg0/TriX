#ifndef __TRIX_HWEMUEOS_C__
#define __TRIX_HWEMUEOS_C__



#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"
#include "ui.h"

#include "mem.h"

#include "treenode.h"

#include "trixplug.h"


#include "hwemueos.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

#define PLUGIN_TITLE "HWemu for Canon EOS"
#define PLUGIN_MAJOR 0
#define PLUGIN_MINOR 1

int verbosity = 0xFFFFFFFF;
int tio_rxbyte = 0x100;
unsigned int term_box = E_FAIL;
unsigned int rom0_size = 0x00400000;
unsigned int rom1_size = 0x00400000;
unsigned int flash_state_machine = 1;


OPT_BEGIN
OPTION( OPT_HEX, "Verbosity flags", "verbosity", &verbosity )
OPTION( OPT_HEX, "Terminal I/O RX byte", "tio_rxbyte", &tio_rxbyte )
OPTION( OPT_HEX, "ROM0 Size", "rom0_size", &rom0_size )
OPTION( OPT_HEX, "ROM1 Size", "rom1_size", &rom1_size )
OPTION( OPT_HEX, "Use flash state machine", "flash_state_machine", &flash_state_machine )
OPT_FINISH

PLUGIN_INFO_BEGIN
PLUGIN_NAME ( PLUGIN_TITLE )
PLUGIN_VERSION ( PLUGIN_MAJOR, PLUGIN_MINOR )
PLUGIN_INIT ( hwemueos_init )
PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned char tempbuf[1024];

t_hwemueos_handler hwemueos_handlers[] =
{
    { "RAM Trace",    0x00000000, 0x10000000, hwemueos_handle_ram, 0 },
    { "FlashControl", 0xC0000000, 0xC0001FFF, hwemueos_handle_flashctrl, 0 },
    { "ROM0",         0xF8000000, 0xFFFFFFFF, hwemueos_handle_rom, 0 },
    { "ROM1",         0xF0000000, 0xF7FFFFFF, hwemueos_handle_rom, 1 },
    { "Interrupt",    0xC0201000, 0xC0201FFF, hwemueos_handle_intengine, 0 },
    { "Timers",       0xC0210000, 0xC0210FFF, hwemueos_handle_timers, 0 },
    { "RTC??",        0xC0242014, 0xC0242014, hwemueos_handle_unk, 0 },
    { "GPIO",         0xC0220000, 0xC0220FFF, hwemueos_handle_gpio, 0 },
    { "GPIO",         0xC0223000, 0xC0223FFF, hwemueos_handle_gpio, 3 },
    { "Basic1",       0xC0400000, 0xC0400FFF, hwemueos_handle_basic, 1 },
    { "Basic2",       0xC022F000, 0xC022FFFF, hwemueos_handle_basic, 2 },
    { "SDIO",         0xC0C10000, 0xC0C10FFF, hwemueos_handle_sdio, 0 },
    { "TIO",          0xC0800000, 0xC08000FF, hwemueos_handle_tio, 0 },
    { "SIO0",         0xC0820000, 0xC08200FF, hwemueos_handle_sio, 0 },
    { "SIO1",         0xC0820100, 0xC08201FF, hwemueos_handle_sio, 1 },
    { "SIO2",         0xC0820200, 0xC08202FF, hwemueos_handle_sio, 2 },
    { "SIO3",         0xC0820300, 0xC08203FF, hwemueos_handle_sio, 3 },
    { "DMA1",         0xC0A10000, 0xC0A100FF, hwemueos_handle_dma, 1 },
    { "DMA2",         0xC0A20000, 0xC0A200FF, hwemueos_handle_dma, 2 },
    { "DMA3",         0xC0A30000, 0xC0A300FF, hwemueos_handle_dma, 3 },
    { "DMA4",         0xC0A40000, 0xC0A400FF, hwemueos_handle_dma, 4 },
    { "CARTRIDGE",    0xC0F24000, 0xC0F24FFF, hwemueos_handle_cartridge, 0 },
};

unsigned int trigger_int_cur_id = 0;
unsigned int trigger_int_ctr = 0;
unsigned long long instr_ctr = 0;
unsigned int trigger_reload_cnt = 10000;

unsigned int int_list[INT_ENTRIES];
unsigned int trigger_int_next_id = 0xFFFFFFFF;

unsigned int pc_trace_buf[PC_TRACE_ENTRIES];
unsigned int pc_trace_pos = 0;

unsigned int breakpoint = 0;

unsigned int hwemueos_init ( )
{
    REGISTER_HEADER ( "trix_hwemueos", HWEMUEOS_HEADERS );
    HWEMUEOS_SYMBOLS;

    printf ( PLUGIN_TITLE " v%01i.%01i Plugin Loaded", PLUGIN_MAJOR, PLUGIN_MINOR  );

    term_box = E_FAIL;
    hwemueos_reset();

    return E_OK;
}

unsigned int hwemueos_initdialog ( )
{
    if(term_box != E_FAIL)
    {
        ui_dlg_box_release(term_box);
    }

    term_box = ui_dlg_box_create("EOS Terminal");
    ui_dlg_box_size(term_box, 600, 500);
    ui_dlg_box_set_keypress_ptr(term_box, hwemueos_tio_get_rx_ptr());
    return E_OK;
}

unsigned int hwemueos_reset ( )
{
    for(trigger_int_cur_id = 0; trigger_int_cur_id < INT_ENTRIES; trigger_int_cur_id++)
    {
        int_list[trigger_int_cur_id] = 0;
    }

    for(pc_trace_pos = 0; pc_trace_pos < PC_TRACE_ENTRIES; pc_trace_pos++)
    {
        pc_trace_buf[pc_trace_pos] = 0;
    }

    trigger_int_next_id = 0xFFFFFFFF;
    trigger_int_cur_id = 0;
    trigger_int_ctr = 0;
    pc_trace_pos = 0;

    return E_OK;
}


int *hwemueos_tio_get_rx_ptr ()
{
    return &tio_rxbyte;
}

unsigned int hwemueos_register ( t_workspace *ws, unsigned int address )
{
    t_memmap *mm = NULL;

    if ( !ws || !ws->memmap )
        return E_FAIL;

    mm = ws->memmap;
    while ( mm )
    {
        if ( mm->start <= address && mm->end > address )
        {
            mm->segment->capture_plug = "hwemueos_handler";
        }

        LIST_NEXT(mm);
    }
    return E_OK;
}

unsigned int hwemueos_exec_cb (unsigned int pc)
{
    if(breakpoint)
    {
        breakpoint = 0;
        return 0x02;
    }

    /* was ctrl-c pressed? */
    if(tio_rxbyte == 0x03)
    {
        /* set as "already indicated" and break */
        tio_rxbyte = 0x100;
        return 0x02;
    }

    pc_trace_buf[pc_trace_pos++] = pc;
    pc_trace_pos %= PC_TRACE_ENTRIES;

    instr_ctr++;

    if(trigger_int_ctr)
    {
        trigger_int_ctr--;
        if(!trigger_int_ctr)
        {
            int pos = 0;

            /* disable this interrupt */
            int_list[trigger_int_next_id % INT_ENTRIES] = 0;
            trigger_int_cur_id = trigger_int_next_id;

            printf("[Int] Triggered interrupt 0x%02X\r\n", trigger_int_cur_id);

            trigger_int_next_id = 0xFFFFFFFF;
            /* if there is another enabled interrupt, set it up to fire */
            for(pos = 0; pos < INT_ENTRIES; pos++)
            {
                unsigned int next_id = (trigger_int_cur_id + pos) % INT_ENTRIES;

                if(int_list[next_id])
                {
                    trigger_int_next_id = next_id;
                    trigger_int_ctr = trigger_reload_cnt;
                    break;
                }
            }

            return ARMULATE_CB_IRQ;
        }
    }
    return ARMULATE_CB_NO_ACTION;
}

unsigned int hwemueos_print_trace ()
{
    unsigned int num = 0;

    for(num = 0; num < PC_TRACE_ENTRIES; num++)
    {
        unsigned int pos = (pc_trace_pos + num) % PC_TRACE_ENTRIES;

        printf("%i: 0x%08X\r\n", num, pc_trace_buf[pos]);
    }
}

unsigned int hwemueos_get_exec_callback (  )
{
    return &hwemueos_exec_cb;
}

unsigned int hwemueos_default_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int data = 0;
    unsigned int pc = options_get_int("plugins.ARMulate.pc");


    switch ( type & WIDTH_MASK )
    {
        case WIDTH_WORD:
            if ( type & MODE_WRITE )
                v_set_w ( ws, address, value );
            else
                data = v_get_w ( ws, address );
            break;

        case WIDTH_HALF:
            if ( type & MODE_WRITE )
                v_set_h ( ws, address, value );
            else
                data = v_get_h ( ws, address );
            break;

        case WIDTH_BYTE:
            if ( type & MODE_WRITE )
                v_set_b ( ws, address, value );
            else
                data = v_get_b ( ws, address );
            break;
    }

    /* do not log ram/flash access */
    if(((address & 0xF0000000) == 0) || ((address & 0xF0000000) == 0xF0000000) || ((address & 0xF0000000) == 0x40000000))
    {
        return data;
    }


    if ( type & MODE_WRITE )
    {
        if(verbosity & 1)
        {
            printf("Write: at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
        }
    }
    else
    {
        static int mod = 0;
        mod++;
        mod %= 2;

        if(mod)
        {
            data = ~data;
        }
        if(verbosity & 1)
        {
            printf("Read: at [0x%08X] [0x%08X] <- [0x%08X]\r\n", pc, data, address);
        }
    }
    return data;
}

t_hwemueos_handler *hwemueos_find_handler( unsigned int address)
{
    int pos = 0;
    for(pos = 0; pos < sizeof(hwemueos_handlers) / sizeof(hwemueos_handlers[0]); pos++)
    {
        if(hwemueos_handlers[pos].start <= address && hwemueos_handlers[pos].end >= address)
        {
            return &hwemueos_handlers[pos];
        }
    }

    return NULL;
}

unsigned int hwemueos_handler ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int data = 0;
    static unsigned int tmpval1 = 0;

    t_hwemueos_handler *handler = hwemueos_find_handler(address);

    if(handler)
    {
        return handler->handle(handler->parm, ws, address, type, value);
    }
    else
    {
        return hwemueos_default_handle ( ws, address, type, value );
    }
    return 0;
}

unsigned int hwemueos_trigger_int(unsigned int id, unsigned int delay)
{
    int_list[id % INT_ENTRIES] = 1;

    if(trigger_int_next_id == 0xFFFFFFFF)
    {
        /* trigger interrupt next time */
        trigger_int_next_id = id;
        trigger_int_ctr = delay + 1;
    }
}

unsigned int hwemueos_handle_intengine ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int pc = options_get_int("plugins.ARMulate.pc");
    
    switch(address & 0xFFF)
    {
        case 0x04:            
            if(type & MODE_WRITE)
            {
                printf("[Int] Wrote int reason [0x%08X] -> [0x%08X] PC: [0x%08X]\r\n", address, value, pc);
                return 0;
            }
            else
            {
                //printf("[Int] Requested int reason [0x%08X] <- [0x%08X] PC: [0x%08X]\r\n", trigger_int_cur_id << 2, address, pc);
                return trigger_int_cur_id << 2;
            }
            break;

        case 0x10:            
            if(type & MODE_WRITE)
            {
                printf("[Int] Enabled interrupt ID 0x%02X PC: [0x%08X]\r\n", value, pc);

                if(value != 0x0A && value != 0x2F && value != 0x74 && value != 0x75)
                {
                    return 0;
                }

                hwemueos_trigger_int(value, 0x5000);
                return 0;
            }
            else
            {
                return 0;
            }
            break;
    }

    if(type & MODE_WRITE)
    {
        printf("[Int] Write to Int space [0x%08X] -> [0x%08X] PC: [0x%08X]\r\n", value, address, pc);
    }
    else
    {
        return 0;
    }
    return 0;
}

unsigned int hwemueos_handle_timers_ ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int pc = options_get_int("plugins.ARMulate.pc");

    if(type & MODE_WRITE)
    {
        printf("[Timer?] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        return 0;
    }
    return 0;
}

unsigned int hwemueos_handle_timers ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int ret = 0;

    switch(address & 0xFF)
    {
        case 0x00:
            if(type & MODE_WRITE)
            {
            }
            else
            {
                ret = 0;
            }
            break;
    }

    if(type & MODE_WRITE)
    {
        unsigned int pc = options_get_int("plugins.ARMulate.pc");
        printf("[Timer] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        return ret;
    }
    return 0;
}



unsigned int hwemueos_handle_gpio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int ret = 1;

    switch(parm)
    {
        case 0:
            if((address & 0xFFF) == 0x0DC)
            {
                /* abort situation for FROMUTIL on 600D */
                ret = 0;
            }
            if((address & 0xFFF) == 0x0B0)
            {
                /* FUNC SW OFF on 7D */
                ret = 0;
            }
            if((address & 0xFFF) == 0x024)
            {
                /* master woke up on 7D */
                ret = 0;
            }
            break;
        case 3:
            if((address & 0xFFF) == 0x01C)
            {
                /* 40D CF Detect -> set low, so there is no CF */
                ret = 0;
            }
            break;
    }

    if(type & MODE_WRITE)
    {
        unsigned int pc = options_get_int("plugins.ARMulate.pc");
        printf("[GPIO] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        return ret;
    }
    return 0;
}


unsigned int hwemueos_handle_cartridge ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    if(type & MODE_WRITE)
    {
        printf("[Cartridge] [0x%08X] -> [0x%08X]\r\n", value, address);
    }
    else
    {
        return 0;
    }
    return 0;
}


unsigned int hwemueos_handle_dma ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int ret = 0;
    unsigned int log = 1;
    static unsigned int srcAddr = 0;
    static unsigned int dstAddr = 0;
    static unsigned int count = 0;
    unsigned int interruptId[] = { 0x00, 0x2f, 0x74, 0x75, 0x76 };

    switch(address & 0xFF)
    {
        case 0x08:            
            if(type & MODE_WRITE)
            {
                if(value & 1)
                {
                    void (*dump_func)(unsigned int);

                    /* Start DMA */
                    printf("[DMA%i] Copy [0x%08X] -> [0x%08X], length [0x%08X]\r\n", parm, srcAddr, dstAddr, count);
                    
                    dump_func = trixplug_get_global_plugin_symbol("armulate_lr_dump");
                    if(dump_func)
                    {
                        printf("[DMA%i] Trace:\r\n", parm);
                        dump_func(8);
                    }
                    v_memcpy(ws, dstAddr, srcAddr, count);
                    hwemueos_trigger_int(interruptId[parm], 0);
                    log = 0;
                }
            }
            else
            {
                ret = 0;
            }
            break;

        case 0x18:
            if(type & MODE_WRITE)
            {
                srcAddr = value;
                log = 0;
            }
            else
            {
                return srcAddr;
            }
            break;

        case 0x1C:
            if(type & MODE_WRITE)
            {
                dstAddr = value;
                log = 0;
            }
            else
            {
                return dstAddr;
            }
            break;

        case 0x20:
            if(type & MODE_WRITE)
            {
                count = value;
                log = 0;
            }
            else
            {
                return count;
            }
            break;
    }
    
    if(log)
    {
        if(type & MODE_WRITE)
        {
            printf("[DMA%i] [0x%08X] -> [0x%08X]\r\n", parm, value, address);
        }
        else
        {
            printf("[DMA%i] [0x%08X] <- [0x%08X]\r\n", parm, ret, address);
        }
    }

    return 0;
}


unsigned int hwemueos_handle_tio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    switch(address & 0xFF)
    {
        case 0x00:            
            if(type & MODE_WRITE)
            {
                if((value == 0x08 || value == 0x0A || (value >= 0x20 && value <= 0x7F)))
                {
                    unsigned char buf[2];
                    sprintf(buf, "%c", value);
                    ui_dlg_box_msg(term_box | UI_OPT_DLG_CHAR, buf);
                }
            }
            else
            {
                return 0;
            }
            break;

        case 0x04:
            printf("[TIO] Read byte: 0x%02X\r\n", tio_rxbyte & 0xFF);
            return tio_rxbyte & 0xFF;
            break;

        case 0x14:
            if(type & MODE_WRITE)
            {
                if(value & 1)
                {
                    printf("[TIO] Reset RX indicator\r\n");
                    tio_rxbyte |= 0x100;
                }
            }
            else
            {
                if((tio_rxbyte & 0x100) == 0)
                {
                    printf("[TIO] Signalling RX indicator\r\n");
                    return 3;
                }
                return 2;
            }
            break;
    }

    return 0;
}

unsigned int hwemueos_handle_sio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    static unsigned int last_sio_data = 0;
    static unsigned int last_sio_setup1 = 0;
    static unsigned int last_sio_setup2 = 0;
    static unsigned int last_sio_setup3 = 0;
    unsigned int pc = options_get_int("plugins.ARMulate.pc");

    switch(address & 0xFF)
    {
        case 0x04:
            if((type & MODE_WRITE) && (value & 1))
            {
                void (*dump_func)(unsigned int);
                printf("[SIO%i] Transmit: 0x%08X, setup 0x%08X 0x%08X 0x%08X PC: 0x%08X\r\n", parm, last_sio_data, last_sio_setup1, last_sio_setup2, last_sio_setup3, pc );
                
                dump_func = trixplug_get_global_plugin_symbol("armulate_lr_dump");
                if(dump_func)
                {
                    printf("[SIO%i] Trace:\r\n", parm);
                    dump_func(8);
                }
                return 0;
            }
            else
            {
                return 0;
            }
            break;

        case 0x0C:
            if(type & MODE_WRITE)
            {
                last_sio_setup1 = value;
                return 0;
            }
            else
            {
                return last_sio_setup1;
            }

        case 0x10:
            if(type & MODE_WRITE)
            {
                last_sio_setup3 = value;
                return 0;
            }
            else
            {
                return last_sio_setup3;
            }

        case 0x14:
            if(type & MODE_WRITE)
            {
                last_sio_setup1 = value;
                return 0;
            }
            else
            {
                return last_sio_setup1;
            }

        case 0x18:
            if(type & MODE_WRITE)
            {
                last_sio_data = value;
                return 0;
            }
            else
            {
                return last_sio_data;
            }

        case 0x1C:
            if(type & MODE_WRITE)
            {
                printf("[SIO%i] Write access to RX register\r\n", parm);
                return 0;
            }
            else
            {
                return last_sio_data;
            }
    }

    return 0;
}


unsigned int hwemueos_handle_unk ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int pc = options_get_int("plugins.ARMulate.pc");
    unsigned int ret = 0;
    static unsigned val = 0;

    switch(parm)
    {
        case 0:
            if(type & MODE_WRITE)
            {
                ret = 0;
            }
            else
            {
                ret = val;
                val++;
            }
            break;
    }

    if(type & MODE_WRITE)
    {
        printf("[Basic] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        printf("[Basic] at [0x%08X] [0x%08X] <- [0x%08X]\r\n", pc, ret, address);
    }
    return ret;
}


unsigned int hwemueos_handle_sdio ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int pc = options_get_int("plugins.ARMulate.pc");
    unsigned int ret = 0;
    static unsigned val = 0;

    switch(address & 0xFFF)
    {
        case 0x10:
            /* code is waiting for bit0 getting high, bit1 is an error flag */
            ret = 3;
            break;
        case 0x34:
            /* code is waiting for transfer status */
            ret = 0xFFFFFF;
            break;
    }

    if(type & MODE_WRITE)
    {
        printf("[Basic] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        printf("[Basic] at [0x%08X] [0x%08X] <- [0x%08X]\r\n", pc, ret, address);
    }
    return ret;
}

unsigned int hwemueos_handle_basic ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int ret = 0;
    unsigned int pc = options_get_int("plugins.ARMulate.pc");
    static int unk = 0;

    switch(parm)
    {
        case 1:
            switch(address & 0xFFF)
            {
                case 0xA4:
                    if(type & MODE_WRITE)
                    {
                    }
                    else
                    {
                        ret = 1;
                    }
                    break;
            }
            break;

        case 2:
            switch(address & 0xFFF)
            {
                case 0x198:
                    if(type & MODE_WRITE)
                    {
                    }
                    else
                    {
                        ret = unk;
                        unk++;
                        unk %= 2;
                    }
                    break;
                case 0x100:
                    if(type & MODE_WRITE)
                    {
                    }
                    else
                    {
                        ret = unk;
                        unk++;
                        unk %= 2;
                    }
                    break;
            }
            break;

        case 3:
            switch(address & 0xFFF)
            {
                case 0x10:
                    if(type & MODE_WRITE)
                    {
                    }
                    else
                    {
                        ret = unk;
                        unk++;
                        unk %= 0x10;
                    }
                    break;

                case 0x34:
                case 0x38:
                    if(type & MODE_WRITE)
                    {
                    }
                    else
                    {
                        ret = 0;//0xFFFFFFFF;
                    }
                    break;
            }
            break;

    }
    if(type & MODE_WRITE)
    {
        printf("[Basic] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
    }
    else
    {
        printf("[Basic] at [0x%08X] [0x%08X] <- [0x%08X]\r\n", pc, ret, address);
    }

    return ret;
}


#define FLASH_STATE_READ      0
#define FLASH_STATE_UNLOCK_2  1
#define FLASH_STATE_UNLOCKED  2
#define FLASH_STATE_ERASE_1   3
#define FLASH_STATE_ERASE_2   4
#define FLASH_STATE_ERASE_3   5
#define FLASH_STATE_PROGRAM   6
#define FLASH_STATE_UNLOCK_BYPASS   7
#define FLASH_STATE_UNLOCK_BYPASS_RESET 8
#define FLASH_STATE_UNLOCK_BYPASS_ERASE 9
#define FLASH_STATE_BLOCK_ERASE_BUSY 10

unsigned int flash_get_blocksize(unsigned int rom, unsigned int size, unsigned int word_offset)
{
    switch(size)
    {
        /* 32mbit flash x16 */
        case 0x00400000:
            if((word_offset < 0x8000) || (word_offset > 0x1F0000))
            {
                /* 4 kwords */
                return 4 * 1024 * 2;
            }
            else
            {
                /* 32 kwords */
                return 32 * 1024 * 2;
            }
            break;

        default:
            return 0;
    }
}

unsigned int hwemueos_handle_rom ( unsigned int rom, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int pc = options_get_int("plugins.ARMulate.pc");
    unsigned int ret = 0;
    unsigned int real_address = 0;
    unsigned int byte_offset = 0;
    unsigned int word_offset = 0;
    unsigned int base = 0;
    unsigned int size = 0;
    unsigned int fail = 0;

    static int block_erase_counter = 0;
    static int state[2] = { FLASH_STATE_READ, FLASH_STATE_READ };

    switch(rom)
    {
        case 0:
            base = 0xF8000000;
            size = rom0_size;
            break;
        case 1:
            base = 0xF0000000;
            size = rom1_size;
            break;
    }

    /* the offset relative from flash chip start */
    byte_offset = (address - base) & (size - 1);
    word_offset = byte_offset >> 1;

    /* the address of the flash data in memory space (TriX) */
    real_address = base + byte_offset;


    if(!flash_state_machine)
    {
        return hwemueos_default_handle ( ws, real_address, type, value );
    }

    if(type & MODE_WRITE)
    {
        switch(state[rom])
        {
            case FLASH_STATE_READ:
                if(value == 0xF0)
                {
                    state[rom] = FLASH_STATE_READ;
                } 
                else if(word_offset == 0x555 && value == 0xAA)
                {
                    state[rom] = FLASH_STATE_UNLOCK_2;
                }
                else if(value == 0xA0)
                {
                    //printf("[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS PROGRAM\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_PROGRAM;
                }
                else if(value == 0x80)
                {
                    state[rom] = FLASH_STATE_UNLOCK_BYPASS_ERASE;
                }
                else if(value == 0x90)
                {
                    state[rom] = FLASH_STATE_UNLOCK_BYPASS_RESET;
                }
                else if(value == 0x98)
                {
                    printf("[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS CFI unhandled\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_READ;
                }
                else
                {
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCK_BYPASS:
                printf("[ROM%i:%i] at [0x%04X]       2nd UNLOCK BYPASS [0x%08X] -> [0x%08X] unhandled\r\n", rom, state[rom], pc, value, word_offset);
                state[rom] = FLASH_STATE_READ;
                break;


            case FLASH_STATE_UNLOCK_BYPASS_RESET:
                if(value == 0x00)
                {
                    printf("[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS RESET\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_READ;
                }
                else
                {
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCK_2:
                if(word_offset == 0x2AA && value == 0x55)
                {
                    state[rom] = FLASH_STATE_UNLOCKED;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCKED:
                if(value == 0x90)
                {
                    printf("[ROM%i:%i] at [0x%04X] [0x%08X] -> [0x%08X] in autoselect unhandled\r\n", rom, state[rom], pc, value, word_offset);
                    state[rom] = FLASH_STATE_READ;
                }
                else if(word_offset == 0x555 && value == 0xA0)
                {
                    //printf("[ROM%i:%i] at [0x%04X] Command: PROGRAM\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_PROGRAM;
                }
                else if(word_offset == 0x555 && value == 0x20)
                {
                    printf("[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS\r\n", rom, state[rom], pc);
                    state[rom] = FLASH_STATE_READ;
                }
                else if(word_offset == 0x555 && value == 0x80)
                {
                    state[rom] = FLASH_STATE_ERASE_1;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_ERASE_1:
                if(word_offset == 0x555 && value == 0xAA)
                {
                    state[rom] = FLASH_STATE_ERASE_2;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_ERASE_2:
                if(word_offset == 0x2AA && value == 0x55)
                {
                    state[rom] = FLASH_STATE_ERASE_3;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_UNLOCK_BYPASS_ERASE:
                if(value == 0x30)
                {
                    int pos = 0;
                    int block_size = flash_get_blocksize(rom, size, word_offset);

                    printf("[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS BLOCK ERASE [0x%08X]\r\n", rom, state[rom], pc, real_address);
                    for(pos = 0; pos < block_size; pos += 2)
                    {
                        v_set_w ( ws, real_address + pos, 0xFFFF );
                    }
                    block_erase_counter = 0;
                    state[rom] = FLASH_STATE_BLOCK_ERASE_BUSY;
                }
                else if(value == 0x10)
                {
                    int pos = 0;

                    printf("[ROM%i:%i] at [0x%04X] Command: UNLOCK BYPASS CHIP ERASE\r\n", rom, state[rom], pc);
                    for(pos = 0; pos < size; pos += 2)
                    {
                        v_set_w ( ws, base + pos, 0xFFFF );
                    }
                    state[rom] = FLASH_STATE_READ;
                }
                else
                {
                    fail = 1;
                }
                break;

            case FLASH_STATE_ERASE_3:
                if(word_offset == 0x555 && value == 0x10)
                {
                    int pos = 0;
                    printf("[ROM%i:%i] at [0x%04X] Command: CHIP ERASE\r\n", rom, state[rom], pc);
                    for(pos = 0; pos < size; pos += 2)
                    {
                        v_set_w ( ws, base + pos, 0xFFFF );
                    }
                    state[rom] = FLASH_STATE_READ;
                }
                else if(value == 0x30)
                {
                    int pos = 0;
                    int block_size = flash_get_blocksize(rom, size, word_offset);

                    printf("[ROM%i:%i] at [0x%04X] Command: BLOCK ERASE [0x%08X]\r\n", rom, state[rom], pc, real_address);
                    for(pos = 0; pos < block_size; pos += 2)
                    {
                        v_set_w ( ws, real_address + pos, 0xFFFF );
                    }
                    block_erase_counter = 0;
                    state[rom] = FLASH_STATE_BLOCK_ERASE_BUSY;
                }
                else
                {
                    state[rom] = FLASH_STATE_READ;
                    fail = 1;
                }
                break;

            case FLASH_STATE_PROGRAM:
                printf("[ROM%i:%i] at [0x%04X] Command: PROGRAM [0x%04X] -> [0x%08X]\r\n", rom, state[rom], pc, value, real_address);
                v_set_w ( ws, real_address, value );
                state[rom] = FLASH_STATE_READ;
                break;
        }
        if(fail)
        {
            printf("[ROM%i:%i] at [0x%04X] [0x%08X] -> [0x%08X]\r\n", rom, state[rom], pc, value, word_offset);
        }
    }
    else
    {

        switch(state[rom])
        {
            case FLASH_STATE_READ:
                ret = hwemueos_default_handle ( ws, real_address, type, value );
                break;

            case FLASH_STATE_BLOCK_ERASE_BUSY:
                if(block_erase_counter < 0x10)
                {
                    block_erase_counter++;
                    ret = ((block_erase_counter&1)<<6) | ((block_erase_counter&1)<<2);
                }
                else
                {
                    ret = 0x80;
                    state[rom] = FLASH_STATE_READ;
                }
                break;

            default:
                printf("[ROM%i:%i] at [0x%04X] read in unknown state [0x%08X] <- [0x%08X]\r\n", rom, state[rom], pc, ret, word_offset);
                break;
        }
    }

    return ret;
}


unsigned int hwemueos_handle_flashctrl ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    unsigned int pc = options_get_int("plugins.ARMulate.pc");
    unsigned int ret = 0;

    switch(address & 0x1FF)
    {
        case 0x10:
            if(type & MODE_WRITE)
            {
                if(((value | (value >> 16)) & 0xFFFF) == 0xD9C5)
                {
                    printf("[FlashIF] at [0x%08X]: 'Write enable' enabled\r\n", pc);
                }
                else if(value == 0x0)
                {
                    printf("[FlashIF] at [0x%08X]: 'Write enable' disabled\r\n", pc);
                }
                else
                {
                    printf("[FlashIF] at [0x%08X]: unknown command\r\n", pc);
                }
            }
            else
            {
                ret = 1;
            }
            break;
    }
    return ret;
}


unsigned int taskListBase = 0x103A68;
unsigned int taskListEnd = 0x1058EC;

unsigned int hwemueos_task_regdump(t_workspace *ws, unsigned int addr)
{
    int reg = 0;

    printf("    CPSR: 0x%08X ", v_get_w(ws, addr));
    addr+=4;

    for(reg = 0; reg < 16; reg++)
    {
        if(reg != 13)
        {
            printf("R%i: 0x%08X ", reg, v_get_w(ws, addr));
            addr+=4;
        }
    }

    printf("\r\n");
}

unsigned int hwemueos_handle_ram ( unsigned int parm, t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
    /*
    if(address == 0x96800)
    {
        unsigned int pc = options_get_int("plugins.ARMulate.pc");
        if(type & MODE_WRITE)
        {
            printf("[Bug] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
            breakpoint = 1;
        }
        else
        {
            printf("[Bug] at [0x%08X] [0x%08X] <- [0x%08X]\r\n", pc, v_get_w(ws, address), address);
        }
    }
    if(address == 0x103ABC + 64)
    {
        unsigned int pc = options_get_int("plugins.ARMulate.pc");
        if(type & MODE_WRITE)
        {
            printf("[Bug] at [0x%08X] [0x%08X] -> [0x%08X]\r\n", pc, value, address);
            breakpoint = 1;
        }
        else
        {
            printf("[Bug] at [0x%08X] [0x%08X] <- [0x%08X]\r\n", pc, v_get_w(ws, address), address);
        }
    }
    if(address == 0x1A2C)
    {
        if(type & MODE_WRITE)
        {
            unsigned int taskPtr = v_get_w(ws, address);

            if(value != taskPtr)
            {
                taskPtr = value;
                printf("Task ID 0x%08X '%s' was activated.\r\n", v_get_w(ws, taskPtr + 0x40), v_get_ptr(ws, v_get_w(ws, taskPtr + 0x24)));
            }

            v_set_w(ws, address, value);
            return E_OK;
        }
    }

    if(address >= taskListBase && address < taskListEnd)
    {
        unsigned int taskNum = (address - taskListBase) / 0x54;
        unsigned int field = ((address - taskListBase) % 0x54);
        unsigned int pc = options_get_int("plugins.ARMulate.pc");

        unsigned int taskId = v_get_w(ws, taskListBase + (taskNum * 0x54) + 0x40);
        unsigned int taskName = v_get_w(ws, taskListBase + (taskNum * 0x54) + 0x24);

        if(taskName != 0)
        {
            if(field == 0x49)
            {
                if(type & MODE_WRITE)
                {
                    printf ( "Task ID 0x%08X '%s' state CHANGE %i -> %i PC: 0x%08X\r\n", taskId, v_get_ptr(ws, taskName), v_get_b(ws, address), value, pc);
                }
            }
            if(field == 0x50)
            {
                if(type & MODE_WRITE)
                {
                    printf ( "Task ID 0x%08X '%s' context save. stack WRITE 0x%08X PC: 0x%08X\r\n", taskId, v_get_ptr(ws, taskName), value, pc);
                    //hwemueos_task_regdump(ws, value);
                }
                else
                {
                    printf ( "Task ID 0x%08X '%s' context read. stack READ  0x%08X PC: 0x%08X\r\n", taskId, v_get_ptr(ws, taskName), v_get_w(ws, address), pc);
                    //hwemueos_task_regdump(ws, v_get_w(ws, address));
                }
            }
        }
    }
    */
    return hwemueos_default_handle ( ws, address, type, value );
}
#endif
