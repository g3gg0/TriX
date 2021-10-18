/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2006 by Tomasz Malesinski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#else
#include <winsock.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#ifdef WIN32
#include <windows.h>
#endif

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "trixplug.h"
#include "arm.h"
#include "emu.h"
#include "armulate.h"

#include "mem.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

int gdb_port = 23946;
int gdb_socket = 0;


extern struct trix_functions *ft;
extern ARM7TDMI *emustate;
unsigned char *armulate_gdb_remote = "";
unsigned char *armulate_gdb_status = "(idle)";
unsigned int armulate_gdb_log_tx = 0;
unsigned int armulate_gdb_log_rx = 0;

extern breakpoint_t breakpoints[];


#ifdef WIN32
static WSADATA gdb_wsadata;
#define close closesocket
#endif


#define BUFMAX 1024

#define GDB_SIGNAL_ILL      4  // Illegal instruction       
#define GDB_SIGNAL_TRAP     5  // Trace Trap (Breakpoint and SingleStep)
#define GDB_SIGNAL_EMT      7  // Emulator Trap

/* errno values */
#define EPERM           1
#define ENOENT          2
#define EINTR           4
#define EBADF           9
#define EACCES         13
#define EFAULT         14
#define EBUSY          16
#define EEXIST         17
#define ENODEV         19
#define ENOTDIR        20
#define EISDIR         21
#define EINVAL         22
#define ENFILE         23
#define EMFILE         24
#define EFBIG          27
#define ENOSPC         28
#define ESPIPE         29
#define EROFS          30
#define ENAMETOOLONG   91
#define EUNKNOWN       9999

static char packet_buf[BUFMAX];
static char reply_buf[BUFMAX];

static const char hexchars[] = "0123456789abcdef";
static int gdb_exception_no, gdb_mem_access;
static unsigned long registers[17];

void gdb_api_breakpoint(void);
static void gdb_api_log(char *msg);

typedef unsigned int bool;

void gdb_loop(void);

int gdb_register_map[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    0xF01, // f0
    0xF02,
    0xF03,
    0xF11, // f1
    0xF12,
    0xF13,
    0xF21, // f2
    0xF22,
    0xF23,
    0xF31, // f3
    0xF32,
    0xF33,
    0xF41, // f4
    0xF42,
    0xF43,
    0xF51, // f5
    0xF52,
    0xF53,
    0xF61, // f6
    0xF62,
    0xF63,
    0xF71, // f7
    0xF72,
    0xF73,
    0xFFF, // fps
    16
};


int accepttimeout ( int s, struct sockaddr *addr, int *addrlen, int timeout )
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = 0;
    tv.tv_usec = timeout * 1000;

    // wait until timeout or data received
    n = select(s+1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return accept ( s, addr, addrlen );
}


int recvtimeout ( int s, char *buf, int len, int timeout )
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = 0;
    tv.tv_usec = timeout * 1000;

    // wait until timeout or data received
    n = select(s+1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return recv ( s, buf, len, 0 );
}


unsigned int gdb_listen ( )
{	
    struct sockaddr_in local;
    struct sockaddr_in remote;
    int remotelen = sizeof ( remote );
    int server_fd = INVALID_SOCKET;
    int client_fd = INVALID_SOCKET;

#ifdef WIN32
    if ( WSAStartup ( 0x101, &gdb_wsadata ) != 0 )
        return E_FAIL;
#endif

	local.sin_family = AF_INET; 
	local.sin_addr.s_addr = INADDR_ANY; 
	local.sin_port = htons ( (u_short)gdb_port );

	server_fd = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( server_fd == INVALID_SOCKET )
		options_set_string ( "plugins.IPShell.status", "(socket error)" );
	else if ( bind ( server_fd, (struct sockaddr*)&local, sizeof(local) ) !=0 )
		options_set_string ( "plugins.IPShell.status", "(port already used)" );
	else if ( listen ( server_fd, 10 ) !=0 )
		options_set_string ( "plugins.IPShell.status", "(listen error)" );
	else
	{
		options_set_string ( "plugins.IPShell.remote", "(none)" );

		while ( !util_script_is_aborted () )
		{
			client_fd = accepttimeout ( server_fd, (struct sockaddr*)&remote, &remotelen, 100 );
			if ( client_fd >= 0 )
			{
				close ( server_fd );

				options_set_string ( "plugins.ARMulate.armulate_gdb_status", "(remote connected)" );
				options_set_string ( "plugins.ARMulate.armulate_gdb_remote", inet_ntoa ( remote.sin_addr ) );
				gdb_socket = client_fd;

				gdb_loop();
				return E_OK;
			}
		}
	}
    return E_FAIL;
}


static int hex(char ch) 
{
    if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0';
    if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    return -1;
}

static void hex_byte(char *s, int byte) 
{
    s[0] = hexchars[(byte >> 4) & 0xf];
    s[1] = hexchars[byte & 0xf];
}

static void hex_word(char *s, unsigned long val) 
{
    int i;
    for (i = 0; i < 4; i++)
        hex_byte(s + i * 2, (val >> (i * 8)) & 0xff);
}

static void hex_string(char *d, char *s) 
{
    while (*s) {
        hex_byte(d, *s++);
        d += 2;
    }
    *d = 0;
}

static int get_hex_byte(char *s) 
{
    return (hex(s[0]) << 4) + hex(s[1]);
}

static unsigned long get_hex_word(char *s) 
{
    int i;
    unsigned long r = 0;
    for (i = 3; i >= 0; i--)
        r = (r << 8) + get_hex_byte(s + i * 2);
    return r;
}

static void reply_error(int n, char *reply) 
{
    reply[0] = 'E';
    hex_byte(reply + 1, n);
    reply[3] = 0;
}

static void reply_signal(int signal, char *reply) 
{
    reply[0] = 'S';

    hex_byte(reply + 1, signal);
    reply[3] = 0;
}

static void reply_ok(char *reply)
{
    strcpy(reply, "OK");
}

static int get_byte(void) {
    unsigned char b = 0;
	int ret = recvtimeout(gdb_socket, &b, 1, 100);
    return b;
}

static void put_byte(unsigned char ch) 
{	
	int sent = send ( gdb_socket, (const char*)&ch, 1, 0 );
}

static void serial_write(unsigned char *buf, int len) {
    int i;
    for (i = 0; i < len; i++)
	{
        put_byte(buf[i]);
	}
}

static void get_packet(char *buf, int len) {
    int count, checksum, escaped;
    int ch;

	while (!util_script_is_aborted())
	{
        do 
		{
            ch = get_byte();
        } while (ch != '$' && !util_script_is_aborted());

		if(util_script_is_aborted())
		{
			return;
		}

        checksum = 0;
        count = 0;
        escaped = 0;
        while (count < len) 
		{
            ch = get_byte();
            if (!escaped) 
			{
                if (ch == '$') 
				{
                    checksum = 0;
                    count = 0;
                } 
				else if (ch == '#')
				{
                    break;
				}
                else if (ch == 0x7d) 
				{
                    escaped = 1;
                    checksum += ch;
                } 
				else 
				{
                    checksum += ch;
                    buf[count] = ch;
                    count++;
                }
            } 
			else 
			{
                escaped = 0;
                checksum += ch;
                buf[count] = ch ^ 0x20;
                count++;
            }
        }
        buf[count] = 0;

        if (ch == '#') 
		{
            int rchksum;

            ch = get_byte();
            rchksum = hex(ch) << 4;
            ch = get_byte();
            rchksum += hex(ch);

            if ((checksum & 0xff) != rchksum)
			{
                put_byte('-');
			}
            else 
			{
                put_byte('+');
				if(armulate_gdb_log_rx)
				{
					/* hack: disable memory read command trace */
					if(armulate_gdb_log_rx != 2 || buf[0] != 'm')
					{
						printf("ARMulate GDB Stub: Command '%s' received\r\n", buf);
					}
				}
                return;
            }
        }
    }
}

static void put_packet(char *buf) {
    int i, checksum;
    int ch;
    char tmp[3];

	if(armulate_gdb_log_tx)
	{
		printf("ARMulate GDB Stub: Response '%s'\r\n", buf);
	}
    do {
        put_byte('$');

        checksum = 0;
        for (i = 0; buf[i]; i++)
            checksum += buf[i];

        serial_write(buf, i);

        tmp[0] = '#';
        hex_byte(tmp + 1, checksum & 0xff);
        serial_write(tmp, 3);

        ch = get_byte();

    } while (ch != '+' && ch != 0);
}

static unsigned long get_general_reg(int n)
{
	if(n == 15)
	{
		return armulate_get_pc();
	}
	else
	{
		return armulate_get_reg(n);
	}
}

static void set_general_reg(int n, unsigned long v)
{
	if(n == 15)
	{
		armulate_set_pc(v);
	}
	else
	{
		armulate_set_reg(n, v);
	}
}

static unsigned long get_cpsr(void)
{
    return armulate_get_cpsr();
}

static void set_cpsr(unsigned long v)
{
    armulate_set_cpsr(v);
}

static void g_reply(char *buf)
{
    int i;
    char *p;

    p = buf;
    for (i = 0; i < sizeof(gdb_register_map) / sizeof(gdb_register_map[0]); i++)
    {
        if(gdb_register_map[i] < 16)
        {
            hex_word(p, get_general_reg(gdb_register_map[i]));
        }
        else if(gdb_register_map[i] == 16)
        {
            hex_word(p, get_cpsr());
        }
        else
        {
            hex_word(p, 0);
        }
        p += 8;
    }

    p[0] = 0;
}

static void cmd_get_register(char *args, char *reply) 
{
    int r = 0;
    int reg = 0;

    if (sscanf(args, "%x", &r) != 1)
    {
        reply_error(0, reply);
        return;
    }

    reg = gdb_register_map[r];

    if (reg < 16)
    {
        hex_word(reply, get_general_reg(reg));
    }
    else if (reg == 16)
    {
        hex_word(reply, get_cpsr());
    }
    else
    {
        hex_word(reply, 0);
    }
    reply[8] = 0;
}

static void cmd_set_register(char *args, char *reply)
{
    int r = 0;
    int p = 0;
    int reg = 0;
    unsigned long v = 0;

    p = -1;
    sscanf(args, "%x=%n", &r, &p);
    if (p == -1)
    {
        reply_error(0, reply);
        return;
    }

    v = get_hex_word(args + p);
    
    reg = gdb_register_map[r];

    if (reg < 16)
    {
        set_general_reg(r, v);
    }
    else if (reg == 16)
    {
        set_cpsr(v);
    }
    reply_ok(reply);
}

static void cmd_set_registers(char *args, char *reply)
{
    char *p;
    int i, len;

    len = strlen(args);

    p = args;
    
    for (i = 0; i < sizeof(gdb_register_map) / sizeof(gdb_register_map[0]); i++)
    {
        if(strlen(p) < 8)
        {
            break;
        }
        if(gdb_register_map[i] < 16)
        {
            set_general_reg(gdb_register_map[i], get_hex_word(p));
        }
        else if(gdb_register_map[i] == 16)
        {
            set_cpsr(get_hex_word(p));
        }
        p += 8;
    }

    reply_ok(reply);
}

static void cmd_get_memory(char *args, char *reply)
{
    unsigned long addr, len, i;    
    unsigned int old_verb = options_get_int("plugins.HWemu_for_Canon_EOS.verbosity");

    if (sscanf(args, "%lx,%lx", &addr, &len) != 2)
    {
        reply_error(0, reply);
        return;
    }

    if (len > (BUFMAX - 16) / 2)
    {
        reply_error(1, reply);
        return;
    }
  
    options_set_int("plugins.HWemu_for_Canon_EOS.verbosity", 0);
    gdb_mem_access = 1;
    for (i = 0; i < len; i++)
	{
		if(!v_valid(emustate->memint, addr + i))
		{
			reply_error(EACCES, reply);
			return;
		}
        hex_byte(reply + i * 2, read_byte(addr + i, arm));
	}
    gdb_mem_access = 0;
    options_set_int("plugins.HWemu_for_Canon_EOS.verbosity", old_verb);

    reply[len * 2] = 0;
}

static void cmd_put_memory(char *args, char *reply)
{
    unsigned long addr, len, i;
    int pos;
    unsigned int old_verb = options_get_int("plugins.HWemu_for_Canon_EOS.verbosity");

    pos = -1;
    sscanf(args, "%lx,%lx:%n", &addr, &len, &pos);
    if (pos == -1)
    {
        reply_error(0, reply);
        return;
    }

    options_set_int("plugins.HWemu_for_Canon_EOS.verbosity", 0);
    gdb_mem_access = 1;
    for (i = 0; i < len; i++)
    {
        write_byte(addr + i, get_hex_byte(args + pos + i * 2), arm);
    }
    gdb_mem_access = 0;
    options_set_int("plugins.HWemu_for_Canon_EOS.verbosity", old_verb);

    reply_ok(reply);
}

static void cmd_put_memory_binary(char *args, char *reply)
{
    unsigned long addr, len, i;
    int pos;
    unsigned int old_verb = options_get_int("plugins.HWemu_for_Canon_EOS.verbosity");

    pos = -1;
    sscanf(args, "%lx,%lx:%n", &addr, &len, &pos);
    if (pos == -1)
    {
        reply_error(0, reply);
        return;
    }

    options_set_int("plugins.HWemu_for_Canon_EOS.verbosity", 0);
    gdb_mem_access = 1;
    for (i = 0; i < len; i++)
    {
        write_byte(addr + i, args[pos + i], arm);
    }
    gdb_mem_access = 0;
    options_set_int("plugins.HWemu_for_Canon_EOS.verbosity", old_verb);

    reply_ok(reply);
}

unsigned int add_bp(unsigned long type, unsigned long addr, unsigned long length)
{
	int pos = 0;

    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		if ((breakpoints[pos].flags != 0) && (breakpoints[pos].address & ~1) == addr )
		{
			printf("ARMulate gdb stub: Added BP at 0x%08X (already set)\r\n", addr);
			return 1;
		}
	}

	/* not set yet, add */
    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		if(breakpoints[pos].flags == 0)
		{
			printf("ARMulate gdb stub: Added BP at 0x%08X\r\n", addr);
			breakpoints[pos].address = addr;
			breakpoints[pos].flags = type;
			return 1;
		}
	}
	return 0;
}

unsigned int del_bp(unsigned long type, unsigned long addr, unsigned long length)
{
	int pos = 0;

    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		if ((breakpoints[pos].flags != 0) && (breakpoints[pos].address & ~1) == addr )
		{
			printf("ARMulate gdb stub: Removed BP at 0x%08X\r\n", addr);
			breakpoints[pos].flags = 0;
			breakpoints[pos].address = 0;
		}
	}
	return 1;
}

static void cmd_del_breakpoint(char *args, char *reply)
{
	unsigned int type = ARMULATE_BP_EXEC;
    unsigned int gdb_type;
    unsigned long addr;
    unsigned long length;

	if (sscanf(args, "%i,%lx,%lx", &gdb_type, &addr, &length) == 3)
	{
		switch(gdb_type)
		{
			case 0:
			case 1:
				type = ARMULATE_BP_EXEC;
				break;
			case 2:
				type = ARMULATE_BP_WRITE;
				break;
			case 3:
				type = ARMULATE_BP_READ;
				break;
			case 4:
				type = ARMULATE_BP_READ | ARMULATE_BP_WRITE;
				break;
		}
		if(del_bp(type,addr,length))
		{
			reply_ok(reply);
			return;
		}
	}

	reply_error(0xFF, reply);
}

static void cmd_add_breakpoint(char *args, char *reply)
{
	unsigned int type = ARMULATE_BP_EXEC;
    unsigned int gdb_type;
    unsigned long addr;
    unsigned long length;

	if (sscanf(args, "%i,%lx,%lx", &gdb_type, &addr, &length) == 3)
	{
		switch(gdb_type)
		{
			case 0:
			case 1:
				type = ARMULATE_BP_EXEC;
				break;
			case 2:
				type = ARMULATE_BP_WRITE;
				break;
			case 3:
				type = ARMULATE_BP_READ;
				break;
			case 4:
				type = ARMULATE_BP_READ | ARMULATE_BP_WRITE;
				break;
		}
		if(add_bp(type,addr,length))
		{
			reply_ok(reply);
			return;
		}
	}

	reply_error(0xFF, reply);
}

static void cmd_old_breakpoint(char *args, char *reply)
{
    unsigned char mode;
    unsigned long addr;

	if (sscanf(args, "%lx,%c", &addr, &mode) == 2)
	{
		switch(mode)
		{
			case 'S':
				if(add_bp(ARMULATE_BP_EXEC,addr,1))
				{
					reply_ok(reply);
					return;
				}
				break;

			case 'C':
				if(del_bp(ARMULATE_BP_EXEC,addr,1))
				{
					reply_ok(reply);
					return;
				}
				break;
		}
	}

	reply_error(0xFF, reply);
}

static void parse_continue_args(char *args)
{
    int sig;
    unsigned long addr;

    if (sscanf(args, "%x;%lx", &sig, &addr) == 2)
    {
        set_general_reg(15, addr);
    }
    else if (sscanf(args, "%lx", &addr) == 1)
    {
        set_general_reg(15, addr);
    }
}

static void cmd_go_signal(char *args, char *reply)
{
	unsigned int status = 0;

	status = armulate_run_until(0);

	printf("ARMulate gdb stub: GoSignal stopped at 0x%08X\r\n", armulate_get_pc());

	if(status == ARMULATE_ABORT_UNKNOWN_INSTR)
	{
		printf("ARMulate gdb stub: GoSignal stopped due to code %i, sending signal %i\r\n", status, GDB_SIGNAL_ILL);
		reply_signal(GDB_SIGNAL_ILL,reply);
		return;
	}

	//printf("ARMulate gdb stub: GoSignal stopped due to code %i, sending %i\r\n", status, GDB_SIGNAL_TRAP);
	reply_signal(GDB_SIGNAL_TRAP,reply);
	return;
}


static void cmd_go(char *args, char *reply)
{
	unsigned int status = 0;

    parse_continue_args(args);
	status = armulate_run_until(0);

	printf("ARMulate gdb stub: Go stopped at 0x%08X\r\n", armulate_get_pc());

	if(status == ARMULATE_ABORT_UNKNOWN_INSTR)
	{
		printf("ARMulate gdb stub: Go stopped due to code %s, sending signal %i\r\n", armulate_reason(status), GDB_SIGNAL_ILL);
		reply_signal(GDB_SIGNAL_ILL,reply);
		return;
	}

	//printf("ARMulate gdb stub: Go stopped due to code %s, sending %i\r\n", armulate_reason(status), GDB_SIGNAL_TRAP);
	reply_signal(GDB_SIGNAL_TRAP,reply);
	return;
}

static void cmd_step(char *args, char *reply)
{
	unsigned int status = 0;

    parse_continue_args(args);
	status = armulate_exec_step();

	if(status == ARMULATE_ABORT_UNKNOWN_INSTR)
	{
		printf("ARMulate gdb stub: Step stopped due to code %s, sending signal %i\r\n", armulate_reason(status), GDB_SIGNAL_ILL);
		reply_signal(GDB_SIGNAL_ILL,reply);
		return;
	}

	//printf("ARMulate gdb stub: Step stopped due to code %s, sending %i\r\n", armulate_reason(status), GDB_SIGNAL_TRAP);
	reply_signal(GDB_SIGNAL_TRAP,reply);

	return;
}

static void remote_cmd(char *cmd, char *reply)
{
    int i, err;
    i = 0;
    err = 0;

    while ((cmd[i] >= 'a' && cmd[i] <= 'z') || cmd[i] == '_')
    {
        i++;
    }

    if (!strncmp(cmd, "reboot", i))
    {
        reply_ok(reply);
        put_packet(reply);
    }
    else if (!strncmp(cmd, "power_off", i))
    {
        reply_ok(reply);
        put_packet(reply);
        while (1);
    }
    else
    {
        hex_string(reply, "Unrecognized command\n");
    }

    if (err)
        reply_error(err, reply);
}

static void cmd_query(char *args, char *reply)
{
    if (!strncmp(args, "Rcmd,", 5))
    {
        unsigned i = 0;
        char *s = args + 5;
        char cmd[200];

        while (isxdigit(s[0]) && isxdigit(s[1]) && i < sizeof(cmd) - 1)
        {
            cmd[i++] = get_hex_byte(s);
            s += 2;
        }
        cmd[i] = 0;
        remote_cmd(cmd, reply);
    }
    else
    {
        reply[0] = 0;
    }
}

void gdb_loop(void)
{
    int no_reply;
	int running = 1;

    gdb_mem_access = 0;

	while (running && !util_script_is_aborted()) 
	{
        get_packet(packet_buf, sizeof(packet_buf) - 1);
            
        no_reply = 0;
        switch (packet_buf[0]) 
		{
            case '?':
                reply_signal(gdb_exception_no, reply_buf);
                break;

            case 'D':
				running = 0;
                reply_ok(reply_buf);
                break;

            case 'H':
                reply_ok(reply_buf);
                break;

            case 'p':
                cmd_get_register(packet_buf + 1, reply_buf);
                break;

            case 'P':
                cmd_set_register(packet_buf + 1, reply_buf);
                break;

            case 'g':
                g_reply(reply_buf);
                break;

            case 'k':
				printf("ARMulate gdb stub: Host requested 'kill'\r\n", packet_buf);
				running = 0;
                break;

            case 'G':
                cmd_set_registers(packet_buf + 1, reply_buf);
                break;

            case 'm':
                cmd_get_memory(packet_buf + 1, reply_buf);
                break;

            case 'M':
                cmd_put_memory(packet_buf + 1, reply_buf);
                break;

            case 'X':
                cmd_put_memory_binary(packet_buf + 1, reply_buf);
                break;

            case 'z':
                cmd_del_breakpoint(packet_buf + 1, reply_buf);
                break;

            case 'Z':
                cmd_add_breakpoint(packet_buf + 1, reply_buf);
                break;

            case 'B':
                cmd_old_breakpoint(packet_buf + 1, reply_buf);
                break;

            case 'q':
                cmd_query(packet_buf + 1, reply_buf);
                break;

            case 'c':
                cmd_go(packet_buf + 1, reply_buf);
                break;

            case 'C':
                cmd_go_signal(packet_buf + 1, reply_buf);
                break;

            case 's':
                cmd_step(packet_buf + 1, reply_buf);
                break;

            default:
				printf("ARMulate gdb stub: Unhandled command: '%s'\r\n", packet_buf);
                reply_buf[0] = 0;
        }

        if (!no_reply)
		{
            put_packet(reply_buf);
		}
    }
}

static void gdb_api_log(char *msg)
{
    int i;

    reply_buf[0] = 'O';
    i = 1;
    while (*msg && i + 2 <= BUFMAX - 1)
    {
        hex_byte(reply_buf + i, *msg++);
        i += 2;
    }
    reply_buf[i] = 0;
    put_packet(reply_buf);
}

