#ifndef __TRIX_IPSHELL_C__
#define __TRIX_IPSHELL_C__

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

#include "IPShell.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

#define IPSHELL_VPRINTF_BUFFER_LENGTH 128*1024

int ipshell_port = 28822;
int ipshell_enabled = 0;
int ipshell_echo = 1;
int ipshell_listen = 1;

int ipshell_bytes_sent = 0;
int ipshell_bytes_received = 0;

int ipshell_socket = 0;
int ipshell_server = 0;

unsigned char *ipshell_remote = "";
unsigned char *ipshell_status = "(idle)";


#ifdef WIN32
static WSADATA ipshell_wsadata;
#define close closesocket
#endif



#define SOCKET_TRANSFER_SIZE 8192

OPT_BEGIN
	OPTION( OPT_STR,  "Status", "status", &ipshell_status )
	OPTION( OPT_STR,  "Remote", "remote", &ipshell_remote )
	OPTION( OPT_BOOL, "Enabled", "enabled", &ipshell_enabled )
	OPTION( OPT_BOOL, "Local echo", "echo", &ipshell_echo )
	OPTION( OPT_BOOL, "Listening mode (0=connect to remote, 1=wait for connection)", "listen", &ipshell_listen )
	OPTION( OPT_INT,  "TCP Port", "port", &ipshell_port )
	OPTION( OPT_INT,  "Bytes received", "bytes_sent", &ipshell_bytes_sent )
	OPTION( OPT_INT,  "Bytes received", "bytes_received", &ipshell_bytes_received )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "IPShell" )
	PLUGIN_INIT ( ipshell_init )
	PLUGIN_CLEANUP ( ipshell_cleanup )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int ipshell_init ( )
{
    REGISTER_HEADER ( "trix_ipshell", IPSHELL_HEADERS );
    IPSHELL_SYMBOLS

	printf ( "IPShell v0.1 Plugin Loaded" );
	ui_register_cb ( "ipshell_vprintf_cb", "ipshell_fgets_cb" );

	if ( ipshell_enabled )
	{
		ipshell_connect ( );

		ui_set_callback_state ( 0 );
		printf ( "IPShell: Remote host %s", ipshell_remote );
		ui_set_callback_state ( 1 );
	}

    return E_OK;
}

unsigned int ipshell_cleanup ( )
{
	ui_register_cb ( NULL, NULL );
	ipshell_disconnect ();
	printf ( "IPShell v0.1 Plugin Unloaded" );

	return E_OK;
}

unsigned int ipshell_disconnect ()
{
	if ( ipshell_socket )
		close ( ipshell_socket );
	if ( ipshell_server )
		close ( ipshell_server );
#ifdef WIN32
	WSACleanup();
#endif
	ipshell_bytes_sent = 0;
	ipshell_bytes_received = 0;
	ipshell_server = 0;
	ipshell_socket = 0;
	options_set_string ( "plugins.IPShell.status", "(disconnected)" );
	options_set_string ( "plugins.IPShell.remote", "(none)" );

    return E_OK;
}



int ipshell_vprintf_cb ( const char *format, va_list args )
{
	static unsigned char *buffer = NULL;
	static unsigned int length = IPSHELL_VPRINTF_BUFFER_LENGTH;
	int sent = 0;

	if ( !buffer )
		buffer = malloc ( length );

	if ( ipshell_echo || !ipshell_enabled )
	{
		ui_set_callback_state ( 0 );
		ui_vprintf_func ( format, args );
		ui_set_callback_state ( 1 );
	}

	if ( ipshell_enabled )
	{
		if ( ipshell_socket <= 0 )
		{
			if ( ipshell_connect () != E_OK )
				return 0;
		}

		vsnprintf ( buffer, length, format, args );

		ipshell_convert_newline ( &buffer, length );

		sent = send ( ipshell_socket, (const char*)buffer, strlen(buffer), 0 );

		if ( sent < 0 )
		{
			ipshell_disconnect ();
			return 0;
		}
		else
			ipshell_bytes_sent += sent;
	}
	return 0;
}

char *ipshell_fgets_cb ( char *buffer, int length )
{
	int recvd = 0;

	if ( !buffer )
		return NULL;

	buffer[0] = '\000';

	if ( ipshell_enabled )
	{
		if ( ipshell_socket <= 0 )
		{
			if ( ipshell_connect () != E_OK )
				return fgets ( buffer, length, stdin );;
		}

		while ( recvd <= 0 && !util_script_is_aborted () )
		{
			recvd = recvtimeout ( ipshell_socket, (char*) buffer, length, 500 );

			if ( recvd == -1 )
			{
				ipshell_disconnect ();
				return NULL;
			}
		}
		ipshell_bytes_received += recvd;
		buffer[recvd] = '\000';
	}
	else 
		return NULL;

	if ( ipshell_echo || !ipshell_enabled )
	{
		ui_set_callback_state ( 0 );
		printf ( buffer );
		ui_set_callback_state ( 1 );
	}
	return buffer;
}


int ipshell_convert_newline ( unsigned char **string, unsigned int length )
{
	unsigned int pos = 0;
	unsigned char *buffer;

	if ( !string || !(*string) )
		return E_FAIL;

	buffer = *string;

	while ( buffer[pos] != '\000' && pos < (length - 2) )
	{
		if ( buffer[pos] == '\n' && pos > 0 && buffer[pos-1] != '\r' )
		{
			memmove ( &buffer[pos+1], &buffer[pos], (length - 1) - pos );
			buffer[pos] = '\r';
		}
		pos++;
	}

	return E_OK;
}


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

unsigned int ipshell_connect ( )
{	
    struct sockaddr_in local;
    struct sockaddr_in remote;
    int remotelen = sizeof ( remote );
    int server_fd = INVALID_SOCKET;
    int client_fd = INVALID_SOCKET;

#ifdef WIN32
    if ( WSAStartup ( 0x101, &ipshell_wsadata ) != 0 )
        return E_FAIL;
#endif

	if ( ipshell_listen )
	{
		local.sin_family = AF_INET; 
		local.sin_addr.s_addr = INADDR_ANY; 
		local.sin_port = htons ( (u_short)ipshell_port );

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
				client_fd = accepttimeout ( server_fd, (struct sockaddr*)&remote, &remotelen, 500 );
				if ( client_fd >= 0 )
				{
					close ( server_fd );

					options_set_string ( "plugins.IPShell.status", "(remote connected)" );
					options_set_string ( "plugins.IPShell.remote", inet_ntoa ( remote.sin_addr ) );
					ipshell_socket = client_fd;
					return E_OK;
				}
			}
		}
	}
	else
	{
		struct hostent *hp;
		unsigned long addr = 0;

		if ( inet_addr ( (const char*)ipshell_remote ) == INADDR_NONE )
		{
			hp = gethostbyname ( (const char*)ipshell_remote );
		}
		else
		{
			addr = inet_addr ( (const char*)ipshell_remote );
			hp = gethostbyaddr( (char*)&addr, sizeof(addr), AF_INET);
		}

		if ( hp )
		{
			remote.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
			remote.sin_family = AF_INET;
			remote.sin_port = htons ( ipshell_port );

			//the socket function creates our SOCKET
			client_fd = socket ( AF_INET, SOCK_STREAM, 0 );

			//If the socket() function fails we exit
			if ( client_fd == INVALID_SOCKET )
			{
				options_set_string ( "plugins.IPShell.status", "(socket error)" );
				return E_FAIL;
			}

			if ( !connect ( client_fd, (struct sockaddr*)&remote, sizeof(remote) ) )
			{
				options_set_string ( "plugins.IPShell.status", "(remote connected)" );
				options_set_string ( "plugins.IPShell.remote", inet_ntoa ( remote.sin_addr ) );
				ipshell_socket = client_fd;
				return E_OK;
			}

			options_set_string ( "plugins.IPShell.status", "(host refused connection)" );
			//close the client socket
			close (client_fd);
		}

	}
    return E_FAIL;
}


#endif
