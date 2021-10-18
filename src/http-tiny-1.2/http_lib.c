/*
 *  Http put/get mini lib
 *  written by L. Demailly
 *  (c) 1998 Laurent Demailly - http://www.demailly.com/~dl/
 *  (c) 1996 Observatoire de Paris - Meudon - France
 *  see LICENSE for terms, conditions and DISCLAIMER OF ALL WARRANTIES
 *
 * $Id: http_lib.c 3078 2008-09-21 02:31:36Z geggo $
 *
 * Description : Use http protocol, connects to server to echange data
 *
 * $Log$
 * Revision 1.8  2008-03-06 21:41:57  geggo
 * fixes for linux platform
 * many code cleanups
 *
 * Revision 1.7  2007/05/02 18:46:19  geggo
 * updated ppm uploading etc
 *
 * Revision 1.6  2006/04/30 02:12:08  geggo
 * updated RSA-key system
 *
 * Revision 1.5  2006/04/04 23:38:02  krisha
 * *** empty log message ***
 *
 * Revision 1.4  2006/04/03 00:53:31  geggo
 * added Valid wrapper for __v_valid
 * some netrix fixes ;)
 * phoenix file parser bug wit BT images solved
 *
 * Revision 1.3  2006/04/02 23:25:54  geggo
 * integrated http into file system
 *
 * Revision 1.2  2006/04/02 21:10:32  geggo
 * some bugfixes and compile warnings fixed
 *
 * Revision 1.1  2006/04/02 18:59:12  geggo
 * added NeTriX
 *
 * Revision 3.5  1998/09/23 06:19:15  dl
 * portability and http 1.x (1.1 and later) compatibility
 *
 * Revision 3.4  1998/09/23 05:44:27  dl
 * added support for HTTP/1.x answers
 *
 * Revision 3.3  1996/04/25 19:07:22  dl
 * using intermediate variable for htons (port) so it does not yell
 * on freebsd  (thx pp for report)
 *
 * Revision 3.2  1996/04/24  13:56:08  dl
 * added proxy support through http_proxy_server & http_proxy_port
 * some httpd *needs* cr+lf so provide them
 * simplification + cleanup
 *
 * Revision 3.1  1996/04/18  13:53:13  dl
 * http-tiny release 1.0
 *
 *
 */

static char *rcsid = "$Id: http_lib.c 3078 2008-09-21 02:31:36Z geggo $";

//#define VERBOSE

/* http_lib - Http data exchanges mini library.
 */
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
#include <winsock.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "mem.h"

#include "http_lib.h"

typedef enum
{
    CLOSE,                      /* Close the socket after the query (for put) */
    KEEP_OPEN                   /* Keep it open */
} querymode;


static int http_read_line ( int fd, char *buffer, int max );
static int http_read_buffer ( int fd, char *buffer, unsigned int length );
static http_retcode http_query ( char *command, char *url, char *additional_header, querymode mode, char *data, unsigned int length, unsigned int *pfd );

#ifdef WIN32
static WSADATA data;
#define close closesocket
#endif


/* pointer to a mallocated string containing server name or NULL */
char *http_server = NULL;
/* server port number */
int http_port = 80;
/* pointer to proxy server name or NULL */
char *http_proxy_server = NULL;
/* proxy server port number or 0 */
int http_proxy_port = 0;
/* user agent id string */
static char *http_user_agent = "NeTriX "__TRIX_VERSION__;

unsigned int *process_val = NULL;
unsigned int process_min = 0;
unsigned int process_max = 100;



http_retcode http_set_progress ( unsigned int min_percent, unsigned int max_percent, unsigned int *process )
{
	process_val = process;
	process_min = min_percent;
	process_max = max_percent;

	return E_OK;
}

/*
 * read a line from file descriptor
 * returns the number of bytes read. negative if a read error occured
 * before the end of line or the max.
 * cariage returns (CR) are ignored.
 */
static int
http_read_line ( fd, buffer, max )
     int fd;                    /* file descriptor to read from */
     char *buffer;              /* placeholder for data */
     int max;                   /* max number of bytes to read */
{                               /* not efficient on long lines (multiple unbuffered 1 char reads) */
    int n = 0;
    while ( n < max )
    {
        if ( recv ( fd, buffer, 1, 0 ) != 1 )
        {
            n = -n;
            break;
        }
        n++;
        if ( *buffer == '\015' )
            continue;           /* ignore CR */
        if ( *buffer == '\012' )
            break;              /* LF is the separator */
        buffer++;
    }
    *buffer = 0;
    return n;
}


/*
 * read data from file descriptor
 * retries reading until the number of bytes requested is read.
 * returns the number of bytes read. negative if a read error (EOF) occured
 * before the requested length.
 */
static int
http_read_buffer ( fd, buffer, length )
     int fd;                    /* file descriptor to read from */
     char *buffer;              /* placeholder for data */
     unsigned int length;                /* number of bytes to read */
{
    int n, r;
    for ( n = 0; n < length; n += r )
    {
        r = recv ( fd, buffer, length - n, 0);
        if ( r <= 0 )
            return n;
        buffer += r;

		// update process variable
		if ( process_val )
			*process_val = process_min + ((process_max-process_min)*n)/length;
    }
    return n;
}


#ifndef OSK

#endif

/* beware that filename+type+rest of header must not exceed MAXBUF */
/* so we limit filename to 256 and type to 64 chars in put & get */
#define MAXBUF 512

/*
 * Pseudo general http query
 *
 * send a command and additional headers to the http server.
 * optionally through the proxy (if http_proxy_server and http_proxy_port are
 * set).
 *
 * Limitations: the url is truncated to first 256 chars and
 * the server name to 128 in case of proxy request.
 */
static http_retcode
http_query ( command, url, additional_header, mode, data, length, pfd )
     char *command;             /* command to send  */
     char *url;                 /* url / filename queried  */
	 char *additional_header;   /* additional header */
     querymode mode;            /* type of query */
     char *data;                /* Data to send after header. If NULL, not data is sent */
     unsigned int length;                /* size of data */
     unsigned int *pfd;                  /* pointer to variable where to set file descriptor value */
{
#ifdef WIN32
	SOCKET s;
#else
    int s;
#endif
    struct hostent *hp;
    struct sockaddr_in server;
    char header[MAXBUF];
    int hlg;
    http_retcode ret = 0;
    int proxy = ( http_proxy_server != NULL && http_proxy_port != 0 );
    int port = proxy ? http_proxy_port : http_port;

    if ( pfd )
        *pfd = -1;

    /*
     * get host info by name :
     */
	if ( proxy )
		hp = gethostbyname ( http_proxy_server );
	else
		hp = gethostbyname ( http_server );

    if ( hp )
    {
        memset ( ( char * ) &server, 0, sizeof ( server ) );
        memmove ( ( char * ) &server.sin_addr, hp->h_addr, hp->h_length );
        server.sin_family = hp->h_addrtype;
        server.sin_port = ( unsigned short ) htons ( port );
    }
    else
        return E_FAIL;

    /*
     * create socket
     */
	s = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( s < 0 )
        return ERRSOCK;
    setsockopt ( s, SOL_SOCKET, SO_KEEPALIVE, 0, 0 );

    /*
     * connect to server
     */
    if ( connect ( s, &server, sizeof ( server ) ) < 0 )
        ret = E_FAIL;
    else
    {
        if ( pfd )
            *pfd = s;

        /*
         * create header
         */
        if ( proxy )
        {
            sprintf ( header, "%s http://%.128s:%d/%.256s HTTP/1.0\015\012User-Agent: %s\015\012%s\015\012", command, http_server, http_port, url, http_user_agent, additional_header );
        }
        else
        {
			sprintf ( header, "%s /%.256s HTTP/1.0\015\012Host: %s:%i\015\012User-Agent: %s\015\012%s\015\012", command, url, http_server, http_port, http_user_agent, additional_header );
        }

        hlg = strlen ( header );

        /*
         * send header
         */

        if ( send ( s, header, hlg, 0 ) != hlg )
            ret = E_FAIL;

        /*
         * send data
         */
        if ( ret != E_FAIL && length && data )
		{
			int data_sent = 0;
			int retval = 0;
			int data_blocksize = 8192;

			while ( data_sent < length && ret != E_FAIL )
			{
				if ( data_blocksize > (length - data_sent) )
					data_blocksize = length - data_sent;

				retval = send ( s, &data[data_sent], data_blocksize, 0 );

				if ( retval != data_blocksize )
					ret = E_FAIL;
				else
					data_sent += retval;

				// update process variable
				if ( process_val )
					*process_val = process_min + ((process_max-process_min)*data_sent)/length;

			}
		}

		if ( ret != E_FAIL )
        {
            /*
             * read result & check
             */
            ret = http_read_line ( s, header, MAXBUF - 1 );
#ifdef VERBOSE
            fputs ( header, stderr );
            putc ( '\n', stderr );
#endif
            if ( ret <= 0 )
                ret = E_FAIL;
            else if ( sscanf ( header, "HTTP/1.%*d %03d", ( int * ) &ret ) != 1 )
                ret = E_FAIL;
            else if ( mode == KEEP_OPEN )
                return ret;
        }
    }
    /*
     * close socket
     */
    close ( s );
    return E_OK;
}


/*
 * Put data on the server
 *
 * This function sends data to the http data server.
 * The data will be stored under the ressource name filename.
 * returns a negative error code or a positive code from the server
 *
 * limitations: filename is truncated to first 256 characters
 *              and type to 64.
 */
http_retcode
http_put ( filename, data, length, overwrite, type )
     char *filename;            /* name of the ressource to create */
     char *data;                /* pointer to the data to send   */
     unsigned int length;                /* length of the data to send  */
     unsigned int overwrite;             /* flag to request to overwrite the ressource if it
                                 * was already existing */
     char *type;                /* type of the data, if NULL default type is used */
{
    char header[MAXBUF];
    if ( type )
        sprintf ( header, "Content-length: %d\015\012Content-type: %.64s\015\012%s", length, type, overwrite ? "Control: overwrite=1\015\012" : "" );
    else
        sprintf ( header, "Content-length: %d\015\012%s", length, overwrite ? "Control: overwrite=1\015\012" : "" );
    return http_query ( "PUT", filename, header, CLOSE, data, length, NULL );
}


/*
 * Get data from the server
 *
 * This function gets data from the http data server.
 * The data is read from the ressource named filename.
 * Address of new new allocated memory block is filled in pdata
 * whose length is returned via plength.
 *
 * returns a negative error code or a positive code from the server
 *
 *
 * limitations: filename is truncated to first 256 characters
 */
http_retcode
http_get ( filename, pdata, plength, typebuf )
     char *filename;            /* name of the ressource to read */
     char **pdata;              /* address of a pointer variable which will be set
                                 * to point toward allocated memory containing read data. */
     unsigned int *plength;              /* address of integer variable which will be set to
                                 * length of the read data */
     char *typebuf;             /* allocated buffer where the read data type is returned.
                                 * If NULL, the type is not returned */

{
    http_retcode ret;

    char header[MAXBUF];
    char *pc;
    int fd;
    int n, length = -1;

    if ( !pdata )
        return E_FAIL;
    else
        *pdata = NULL;
    if ( plength )
        *plength = 0;
    if ( typebuf )
        *typebuf = '\0';

	ret = http_query ( "GET", filename, "Accept: */*\r\n", KEEP_OPEN, NULL, 0, &fd );
    if ( ret == 200 )
    {
		ret = E_OK;

        while ( 1 )
        {
            n = http_read_line ( fd, header, MAXBUF - 1 );
#ifdef VERBOSE
            fputs ( header, stderr );
            putc ( '\n', stderr );
#endif
            if ( n <= 0 )
            {
                close ( fd );
                return ERRRDHD;
            }
            /*
             * empty line ? (=> end of header)
             */
            if ( n > 0 && ( *header ) == '\0' )
                break;
            /*
             * try to parse some keywords :
             */
            /*
             * convert to lower case 'till a : is found or end of string
             */
            for ( pc = header; ( *pc != ':' && *pc ); pc++ )
                *pc = tolower ( *pc );
            sscanf ( header, "content-length: %d", &length );
            if ( typebuf )
                sscanf ( header, "content-type: %s", typebuf );
        }
        if ( length <= 0 && plength )
			*plength = 1024*1024;
		else if ( plength )
            *plength = length;

		if ( !( *pdata = malloc ( *plength ) ) )
        {
            close ( fd );
            return E_FAIL;
        }
        n = http_read_buffer ( fd, *pdata, *plength );
        close ( fd );
		*plength = n;
		// to end any returned data with \x00
		*pdata = realloc ( *pdata, *plength+1 );
		(*pdata)[n] = 0;

        if ( length >= 0 && n != length )
            ret = E_FAIL;
    }
    else if ( ret != E_FAIL )
        close ( fd );


    return ret;
}


/*
 * Request the header
 *
 * This function outputs the header of thehttp data server.
 * The header is from the ressource named filename.
 * The length and type of data is eventually returned (like for http_get(3))
 *
 * returns a negative error code or a positive code from the server
 *
 * limitations: filename is truncated to first 256 characters
 */
http_retcode
http_head ( filename, plength, typebuf )
     char *filename;            /* name of the ressource to read */
     unsigned int *plength;              /* address of integer variable which will be set to
                                 * length of the data */
     char *typebuf;             /* allocated buffer where the data type is returned.
                                 * If NULL, the type is not returned */
{
/* mostly copied from http_get : */
    http_retcode ret;

    char header[MAXBUF];
    char *pc;
    int fd;
    int n, length = -1;

    if ( plength )
        *plength = 0;
    if ( typebuf )
        *typebuf = '\0';

    ret = http_query ( "HEAD", filename, "", KEEP_OPEN, NULL, 0, &fd );
    if ( ret == 200 )
    {
        while ( 1 )
        {
            n = http_read_line ( fd, header, MAXBUF - 1 );
#ifdef VERBOSE
            fputs ( header, stderr );
            putc ( '\n', stderr );
#endif
            if ( n <= 0 )
            {
                close ( fd );
                return ERRRDHD;
            }
            /*
             * empty line ? (=> end of header)
             */
            if ( n > 0 && ( *header ) == '\0' )
                break;
            /*
             * try to parse some keywords :
             */
            /*
             * convert to lower case 'till a : is found or end of string
             */
            for ( pc = header; ( *pc != ':' && *pc ); pc++ )
                *pc = tolower ( *pc );
            sscanf ( header, "content-length: %d", &length );
            if ( typebuf )
                sscanf ( header, "content-type: %s", typebuf );
        }
        if ( plength )
            *plength = length;
        close ( fd );

    }
    else if ( ret >= 0 )
        close ( fd );

    return ret;
}



/*
 * Delete data on the server
 *
 * This function request a DELETE on the http data server.
 *
 * returns a negative error code or a positive code from the server
 *
 * limitations: filename is truncated to first 256 characters
 */

http_retcode
http_delete ( filename )
     char *filename;            /* name of the ressource to create */
{
    return http_query ( "DELETE", filename, "", CLOSE, NULL, 0, NULL );
}



/* parses an url : setting the http_server and http_port global variables
 * and returning the filename to pass to http_get/put/...
 * returns a negative error code or 0 if sucessfully parsed.
 */
int
http_parse_url ( url, pfilename )
    /*
     * writeable copy of an url
     */
     char *url;
    /*
     * address of a pointer that will be filled with allocated filename
     * * the pointer must be equal to NULL before calling or it will be
     * * automatically freed (free(3))
     */
     char **pfilename;
{
    char *pc = NULL;

	if ( !url )
		return E_FAIL;

    http_port = 80;
    if ( http_server )
    {
        free ( http_server );
        http_server = NULL;
    }

    if ( pfilename && *pfilename )
    {
        free ( *pfilename );
        *pfilename = NULL;
    }

    if ( strncasecmp ( "http://", url, 7 ) )
    {
#ifdef VERBOSE
        fprintf ( stderr, "invalid url (must start with 'http://')\n" );
#endif
        return ERRURLH;
    }
    url += 7;
    http_server = strdup ( url );

	pc = http_server;
	while ( pc && *pc && *pc != ':' && *pc != '/' )
		pc++;

	if ( *pc )
	{
		*pc = 0;
		pc++;
	}

    if ( *pc == ':' )
    {
        if ( sscanf ( pc, "%d", &http_port ) != 1 )
        {
#ifdef VERBOSE
            fprintf ( stderr, "invalid port in url\n" );
#endif
            return E_FAIL;
        }
		while ( pc && *pc && *pc != '/' )
			pc++;

        if ( *pc )
            pc++;
    }

	if ( pfilename )
		*pfilename = strdup ( *pc ? pc : "" );

#ifdef VERBOSE
    fprintf ( stderr, "host=(%s), port=%d, filename=(%s)\n", http_server, http_port, *pfilename );
#endif
    return E_OK;
}

int
http_init (  )
{
#ifdef WIN32
        /*
         *
         * * windows only: prepare sockets usage,
         *
         * * terminate in case of failure
         */
	WSAStartup ( MAKEWORD ( 1, 1 ), &data );

#endif  /*
 */
	return E_OK;
}
