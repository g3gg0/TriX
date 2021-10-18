#ifndef __TRIX_PPMODD_NET_C__
#define __TRIX_PPMODD_NET_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#include "http_lib.h"
#include "md5.h"

#include "mem.h"




unsigned char* ppmodd_net_login ( unsigned char *username, unsigned char *password )
{
	int pos = 0;
	unsigned int length = 8192;
	unsigned char *buffer = NULL;
	unsigned char *buffer2 = NULL;
	unsigned char *ret_buffer = NULL;
	unsigned char *type_buffer = NULL;
	unsigned char *login_id = NULL;
	MD5_DIGEST digest;

	if ( !username || !password )
		return NULL;

	// allocate buffers
	buffer = (unsigned char*)malloc ( 8192 );
	buffer2 = (unsigned char*)malloc ( 8192 );
	type_buffer = (unsigned char*)malloc ( 8192 );

	if ( !buffer || !buffer2 || !type_buffer )
		return NULL;

	http_parse_url ( "http://ppmodd.g3gg0.de/", NULL );

	// get inital auth token
	sprintf ( (char*)buffer, "auth?%s", username );
	http_get ( (char*)buffer, (char**)&ret_buffer, &length, (char*)type_buffer );
	if ( length != 32 )
	{
		free ( buffer );
		free ( buffer2 );
		free ( type_buffer );
		return NULL;
	}
	ret_buffer[32] = '\000';

	// build "username:passwd:token" md5 hash
	sprintf ( (char*)buffer, "%s:%s:%s", username, password, ret_buffer );
	md5_digest ( (const char*)buffer, strlen ( (const char*)buffer ), digest );

	// build "username:hash" login response
	sprintf ( (char*)buffer, "login?%s:", username );
	for ( pos=0; pos < 16; pos++ )
	{
		sprintf ( (char*)buffer2, "%02x", digest[pos] );
		strcat ( (char*)buffer, (const char*)buffer2 );
	}

	// send that and get session id
	CHECK_AND_FREE ( ret_buffer );
	http_get ( (char*)buffer, (char**)&ret_buffer, &length, (char*)type_buffer );
	if ( length != 32 )
	{
		free ( buffer );
		free ( buffer2 );
		free ( type_buffer );
		return NULL;
	}

	login_id = malloc ( 33 );
	memcpy ( login_id, ret_buffer, 32 );
	login_id[32] = '\000';

	free ( buffer );
	free ( buffer2 );
	free ( ret_buffer );
	free ( type_buffer );

	return login_id;
}

#endif
