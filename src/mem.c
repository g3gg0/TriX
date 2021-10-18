#ifndef __TRIX_MEM_C__
#define __TRIX_MEM_C__

#define NULL ((void*)0)

int max_heap = 200;
double heap_used = 0;

#include "defines.h"
#include "mutex.h"
#include "mem.h"
#include "options.h"

#include "trixplug_funcs.h"

MEM_PLUG_INIT;

t_mem_list *list[MEM_HASH_MAX] = { 0 };
t_mem_list *last[MEM_HASH_MAX] = { 0 };

t_mutex *mem_mutex = NULL;
unsigned int mem_state_mark = MEM_MARK_DEFAULT;
unsigned char mem_state_prefix[2] = { '\000', '\000' };

unsigned int
mem_init ( )
{
#ifdef WIN32
	//
	// prevent fragmentation caused by PPModd etc
	//
	// http://www.abstraction.net/content/articles/analyzing%20the%20heaps%20of%20a%20win32%20process.htm
	// http://msdn.microsoft.com/en-us/library/a6x53890(VS.71).aspx
	//
	_set_sbh_threshold ( 1015 );
#endif
	options_add_core_option ( OPT_INT, "util", max_heap, "maximum heap used by TriX and scripts (megabytes)" );
	return E_OK;
}

unsigned int
mem_get_hash ( unsigned int hash )
{
	unsigned int pos = (hash>>0);
	return pos%MEM_HASH_MAX;
}
void *
mem_allocate ( unsigned int bytes, const char *func, int line )
{
	return mem_allocate_type ( bytes, func, line, NULL );
}

t_mem_list *
mem_get_entry ( void *ptr )
{
	unsigned int start_hash = mem_get_hash ( (unsigned int)ptr );
	unsigned int end_hash = (start_hash+1) % MEM_HASH_MAX;
	unsigned int hash = start_hash;
	unsigned int address = (unsigned int)ptr - MEM_SEC_ZONE - sizeof (t_mem_list);
	t_mem_list *entry = NULL;

	MEM_LOCK_RD;

	if ( ptr )
	{

		// this part is now very tricky!
		// we check all entries, but first the most probable one.
		// due to the way we hash, we already have the *probable* list where our
		// memory entry sits. if not, just scan backwards
		//
		// that will usually cost maybe 2 or 3 loops to check if a ptr is valid

		while ( hash != MEM_HASH_MAX )
		{
			entry = list[hash];
			while ( entry )
			{
				if ( (address >= entry->address) && (address <= (entry->address + entry->bytes)) )
				{
					MEM_UNLOCK_RD;
					return entry;
				}
				entry = entry->next;
			}

			// processed last entry?
			if ( hash == end_hash )
				hash = MEM_HASH_MAX;			// signal end of loop
			else
			{
				// no, wrap over at entry 0 ?
				if ( hash )
					hash--;						// no wrap, just decrease
				else
					hash = MEM_HASH_MAX - 1;	// wrap over
			}
		}
	}

	MEM_UNLOCK_RD;
	return NULL;
}

unsigned int 
mem_delete_old_entry ( unsigned int address )
{
	unsigned int hash = 0;
	t_mem_list *entry = 0;

	MEM_LOCK_WR;
	while (	mem_get_entry ( address ) )
	{
		entry = mem_get_entry ( address );
		if ( entry )
		{
			// just remove that entry
			hash = mem_get_hash ( entry->address );

			if ( !entry->prev )
				list[hash] = entry->next;
			else
				entry->prev->next = entry->next;

			if ( !entry->next )
				last[hash] = entry->prev;
			else
				entry->next->prev = entry->prev;

			if ( !entry->freed )
			{
				printf ( "must not happen, memory not freed\n" );
				free ( entry->address );
			}
			free ( entry );
			heap_used -= sizeof ( t_mem_list );
		}
	}
	MEM_UNLOCK_WR;

	return E_OK;
}

void *
mem_allocate_type ( unsigned int bytes, const char *func, int line, char *type )
{
#ifndef MEM_DEBUG
	return malloc ( bytes );
#else
	unsigned int hash = 0;
	t_mem_list *entry = 0;
	unsigned int address = 0;

	//HEAP_CHECK;
	if ( !bytes )
		return NULL;

	if ( ( heap_used + bytes + 2*MEM_SEC_ZONE + sizeof (t_mem_list) ) / 0x100000 >= max_heap )
	{
		// TODO - add support for realloc and non-seer mallocs
		
		if ( !strcmp ( func, "SEER_SCRIPT" ) )
		{
			printf ( "max_heap option value reached, won't allocate further memory\n" );
			return NULL;
		}
	}

	heap_used += bytes + 2*MEM_SEC_ZONE + sizeof(t_mem_list);
	address = malloc ( bytes + 2*MEM_SEC_ZONE + sizeof(t_mem_list) );

	if ( !address )
	{
		HEAP_CHECK;
		mem_check_all (  );
		return NULL;
	}

#ifndef MEM_KEEP_MALLOCED
//	mem_delete_old_entry ( address );
#endif

	entry = (t_mem_list *)address;

	hash = mem_get_hash ( address );

	entry->address = address;
	entry->bytes = bytes;
	if ( func )
		strncpy ( entry->caller, func, 32 );
	else
		strncpy ( entry->caller, "NULL", 32 );

	entry->type = type;
	entry->line = line;
	entry->state_mark = MEM_MARK_DEFAULT;
	entry->struct_magic = 0;
	entry->struct_name = "t_mem_list";
	entry->struct_refs = 0;
	entry->struct_size = sizeof(t_mem_list);
	entry->freed = 0;

	// for tagging mallocs for a radical free, like we will do for seer's sc* functions
	if ( func && func[0] && func[1] )
	{
		if ( (func[0] == mem_state_prefix[0] && func[1] == mem_state_prefix[1])) 
			entry->state_mark = mem_state_mark;
	}

	MEM_LOCK_WR;

	entry->next = NULL;
	entry->prev = last[hash];

	// insert into list

	if ( last[hash] )
		last[hash]->next = entry;
	else
		list[hash] = entry;

	last[hash] = entry;

	MEM_UNLOCK_WR;

	memset ( entry->address + sizeof(t_mem_list), MEM_MARK_BEFORE, MEM_SEC_ZONE );
	memset ( entry->address + sizeof(t_mem_list) + MEM_SEC_ZONE + entry->bytes, MEM_MARK_AFTER, MEM_SEC_ZONE );

	return (void*)(entry->address + sizeof(t_mem_list) + MEM_SEC_ZONE );
#endif
}




void *
mem_strduplicate ( char *text, const char *func, int line )
{
	unsigned char *buf = 0;

	if ( !text )
		return NULL;

	buf = mem_allocate_type ( strlen(text)+1, func, line, "char *" );
	if ( !buf )
		return NULL;

	strcpy ( buf, text );

	return (void*)buf;
}

void *
mem_calloc ( unsigned int count, unsigned int size, const char *func, int line )
{
	unsigned int bytes = count*size;
	unsigned char *buf = 0;

	buf = mem_allocate_type ( bytes, func, line, "cleared" );
	if ( !buf )
		return NULL;

	memset ( buf, 0x00, bytes );

	return (void*)buf;
}

unsigned int
mem_check ( t_mem_list *entry, unsigned int final )
{
	static char *buf_1 = 0;
	static char *buf_2 = 0;

	if ( !buf_1 || !buf_2 )
	{
		buf_1 = malloc ( MEM_SEC_ZONE );
		buf_2 = malloc ( MEM_SEC_ZONE );
		memset ( buf_1, MEM_MARK_BEFORE, MEM_SEC_ZONE );
		memset ( buf_2, MEM_MARK_AFTER, MEM_SEC_ZONE );
	}
#ifdef MEM_KEEP_MALLOCED
	if ( !final && entry->freed )
	{
		return 0;
	}
	if ( memcmp ( entry->address + sizeof(t_mem_list), buf_1, MEM_SEC_ZONE ) )
	{
		return 0;
	}
	if ( memcmp ( entry->address + sizeof(t_mem_list) + entry->bytes + MEM_SEC_ZONE, buf_2, MEM_SEC_ZONE ) )
	{
		return 0;
	}
#else
	if ( !entry->freed )
	{
		if ( memcmp ( entry->address + sizeof(t_mem_list), buf_1, MEM_SEC_ZONE ) )
		{
			util_dump_bytes ( entry->address, 16, entry->bytes+2*MEM_SEC_ZONE + 2 + sizeof(t_mem_list) );
			return 0;
		}
		if ( memcmp ( entry->address + sizeof(t_mem_list) + MEM_SEC_ZONE + entry->bytes, buf_2, MEM_SEC_ZONE ) )
		{
			util_dump_bytes ( entry->address, 16, entry->bytes+2*MEM_SEC_ZONE + 2 + sizeof(t_mem_list) );
			return 0;
		}
	}
#endif
	return 1;
}


unsigned int
mem_check_overwrite ( void * ptr )
{
#ifndef MEM_DEBUG
   return 1;
#else
	unsigned int address = (unsigned int)ptr - MEM_SEC_ZONE - sizeof(t_mem_list);
	t_mem_list *entry = (t_mem_list *)address;

	if ( !mem_check ( address, 1 ) )
	{
		printf ( "%s(%i) type <%s>: %i bytes block, DATA CORRUPTED!\n", entry->caller, entry->line, entry->type, entry->bytes );
		return 0;
	}

	return 1;
#endif
}

unsigned int
mem_check_overwrites (  )
{
	unsigned int hash = 0;
	unsigned int ret = 1;
	unsigned int checked = 0;
	unsigned int bytes = 0;
	unsigned int unfreedbytes = 0;
	unsigned int unfreedbytes_seer = 0;
	unsigned int unfreedblocks = 0;
	unsigned int unfreedblocks_seer = 0;
	t_mem_list *entry = NULL;

	MEM_LOCK_RD;
	while ( hash < MEM_HASH_MAX )
	{
		entry = list[hash];
		while ( entry )
		{
			if ( !mem_check ( entry, 1 ) )
			{
				printf ( "%s(%i) type <%s>: %i bytes block, DATA CORRUPTED!\n", entry->caller, entry->line, entry->type, entry->bytes );
				ret = 0;
			}
			entry = entry->next;
		}
		hash++;
	}

	MEM_UNLOCK_RD;
	return ret;
}


unsigned int
mem_check_all (  )
{
	unsigned int hash = 0;
	unsigned int ret = 1;
	unsigned int checked = 0;
	unsigned int bytes = 0;
	unsigned int unfreedbytes = 0;
	unsigned int unfreedbytes_seer = 0;
	unsigned int unfreedblocks = 0;
	unsigned int unfreedblocks_seer = 0;
	t_mem_list *entry = NULL;

	MEM_LOCK_RD;
	while ( hash < MEM_HASH_MAX )
	{
		entry = list[hash];
		while ( entry )
		{
			bytes += entry->bytes;
			if ( !entry->freed )
			{
				if ( !strncmp ( entry->caller, "sc", 2 ) )
				{
					unfreedbytes_seer += entry->bytes;
					unfreedblocks_seer++;
				}
				else
				{
					unfreedbytes += entry->bytes;
					unfreedblocks++;
					printf ( "UnFree'd:   Bytes: %8i  Caller: <%s:%i> Type: <%s>\n", entry->bytes, entry->caller, entry->line, (entry->type)?entry->type:"unk" );
				}
			}

			checked++;
			if ( !mem_check ( entry, 1 ) )
			{
				printf ( "%s(%i) type <%s>: %i bytes block, DATA CORRUPTED!\n", entry->caller, entry->line, entry->type, entry->bytes );
				ret = 0;
			}
			entry = entry->next;
		}
		hash++;
	}
	MEM_UNLOCK_RD;
	printf ( "Memory Check Done,  %i entries checked.\n", checked );
	printf ( "  allocated bytes:  %i bytes\n", bytes );
	printf ( "  HEAP used      :  %1.0f bytes\n", heap_used );
	printf ( "  unfree'd  bytes:  %i bytes in %i blocks (core)\n", unfreedbytes, unfreedblocks );
	printf ( "  unfree'd  bytes:  %i bytes in %i blocks (seer)\n", unfreedbytes_seer, unfreedblocks_seer );

	return ret;
}

unsigned int
mem_set_state_mark ( unsigned int mark, unsigned char *prefix )
{
	if ( !prefix )
		return E_FAIL;

	mem_state_mark = mark;
	mem_state_prefix[0] = prefix[0];
	mem_state_prefix[1] = prefix[1];

	return E_OK;
}

unsigned int
mem_get_state_mark ( unsigned int *mark, unsigned char *prefix )
{
	if ( !prefix || !mark )
		return E_FAIL;

	*mark = mem_state_mark;
	prefix[0] = mem_state_prefix[0];
	prefix[1] = mem_state_prefix[1];

	return E_OK;
}

unsigned int
mem_release_tagged (  )
{
	unsigned int hash = 0;
	unsigned int ret = 1;
	unsigned int free_block = 0;
	unsigned int checked = 0;
	unsigned int bytes = 0;
	unsigned int freedbytes_seer = 0;
	unsigned int freedblocks_seer = 0;
	t_mem_list *entry = NULL;

	MEM_LOCK_RD;
	while ( hash < MEM_HASH_MAX )
	{
		free_block = 0;
		entry = list[hash];
		while ( entry )
		{
			if ( !mem_check ( entry, 1 ) )
			{
				printf ( "%s(%i) type <%s>: %i bytes block, DATA CORRUPTED!\n", entry->caller, entry->line, entry->type, entry->bytes );
				ret = 0;
			}

			if ( !entry->freed )
			{
				if ( entry->caller && entry->caller[0] && entry->caller[1] )
				{
					if ( (entry->caller[0] == mem_state_prefix[0] && entry->caller[1] == mem_state_prefix[1])
							&& entry->state_mark == mem_state_mark
						)
					{
						freedbytes_seer += entry->bytes;
						freedblocks_seer++;
						free_block = 1;
					}
				}
			}

			if ( free_block )
			{
				MEM_UNLOCK_RD;
				mem_release ( entry->address + sizeof(t_mem_list) + MEM_SEC_ZONE , __FUNCTION__, 0 );
				MEM_LOCK_RD;
				entry = NULL;
			}
			else
				entry = entry->next;
		}
		if ( !free_block )
			hash++;
	}
	MEM_UNLOCK_RD;
	printf ( "    free'd  bytes:  %i bytes in %i blocks (seer)\n", freedbytes_seer, freedblocks_seer );

	return ret;
}

void *
mem_reallocate ( void *ptr, unsigned int bytes, const char *func, int line )
{
#ifndef MEM_DEBUG
	return realloc ( ptr, bytes );
#else
	unsigned int hash = 0;
	unsigned int address = (unsigned int)ptr - MEM_SEC_ZONE - sizeof(t_mem_list);
	t_mem_list *entry = NULL;

	//printf ( "%s(%i): realloc(%i)\n", func, line, bytes );
	if ( !ptr )
		return mem_allocate ( bytes, func, line );

	if ( heap_used / 0x100000 >= max_heap )
	{
		// TODO - add support for non-seer mallocs
		
		if ( !strcmp ( func, "SEER_SCRIPT" ) )
		{
			printf ( "max_heap option value reached, won't allocate further memory\n" );
			return NULL;
		}
	}

	hash = mem_get_hash ( address );

	MEM_LOCK_WR;
	entry = list[hash];

	while ( entry )
	{
#ifdef MEM_KEEP_MALLOCED
		if ( entry->address == address )
#else
		if ( entry->address == address && !entry->freed )
#endif
		{
			if ( !mem_check ( entry, 0 ) )
			{
				if ( entry->freed )
					printf ( "%s(%i): realloc(%i)     DATA ALREADY FREE'D!\n", func, line, bytes );
				else
					printf ( "%s(%i): realloc(%i)     DATA CORRUPTED!\n", func, line, bytes );
			}

			// unlink it from list
			if ( entry->prev )
				entry->prev->next = entry->next;
			else
				list[hash] = entry->next;

			if ( entry->next )
				entry->next->prev = entry->prev;
			else
				last[hash] = entry->prev;


			heap_used -= entry->bytes;
			heap_used += bytes;
			// now its unlinked cleanly
			address = realloc ( entry, bytes + 2*MEM_SEC_ZONE + sizeof(t_mem_list) );
			if ( !address )
			{
				MEM_UNLOCK_WR;
				return NULL;
			}

#ifndef MEM_KEEP_MALLOCED
//			mem_delete_old_entry ( entry );
#endif
			entry = (t_mem_list *) address;

			hash = mem_get_hash ( address );


			entry->bytes = bytes;
			entry->address = address;
			if ( func )
				strncpy ( entry->caller, func, 32 );
			else
				strncpy ( entry->caller, "NULL", 32 );
			entry->line = line;

			entry->next = NULL;
			entry->prev = last[hash];

			if ( last[hash] )
				last[hash]->next = entry;
			else
				list[hash] = entry;

			last[hash] = entry;

			memset ( entry->address + sizeof(t_mem_list) + MEM_SEC_ZONE + entry->bytes, MEM_MARK_AFTER, MEM_SEC_ZONE );

			MEM_UNLOCK_WR;
			return entry->address + sizeof(t_mem_list) + MEM_SEC_ZONE;
		}
		entry = entry->next;
	}
	MEM_UNLOCK_WR;
	printf ( "%s(%i): FAILED !! realloc(%i)\n", func, line, bytes );

	return 0;
#endif
}

void
mem_release ( void *ptr, const char *func, int line )
{
#ifndef MEM_DEBUG
	free ( ptr );
	return;
#else
	unsigned int hash = 0;
	unsigned int address = (unsigned int)ptr - MEM_SEC_ZONE - sizeof(t_mem_list);
	t_mem_list *entry = NULL;

	hash = mem_get_hash ( address );

	MEM_LOCK_WR;
	entry = list[hash];

	while ( entry )
	{
		if ( entry->address == address )
		{
			if ( !mem_check ( entry, 0 ) )
			{
				if ( entry->freed )
					printf ( "%s(%i): free()     DATA ALREADY FREE'D AT %s:%i!\n", func, line, entry->caller, entry->line );
				else
					printf ( "%s(%i): free()     DATA CORRUPTED!\n", func, line );
			}

//TODO
			// if we free the memory _NOW_
			// a) a later memory access to it will cause a crash (bad for debugging)
			// b) we get that address assigned later again, which will cause confusion
			// so... leave it? free it? or realloc(1) it?
			// for now 0xFF the area to track bugs
			//
			// FIXED: MEM_KEEP_MALLOCED now let you switch between a "very safe" mode
			//        when defined and a normal "somewhat safe" mode if not defined
			//

#ifndef MEM_KEEP_MALLOCED

			// just remove that entry
			if ( entry->prev )
				entry->prev->next = entry->next;
			else
				list[hash] = entry->next;

			if ( entry->next )
				entry->next->prev = entry->prev;
			else
				last[hash] = entry->prev;

			heap_used -= (entry->bytes + 2*MEM_SEC_ZONE + sizeof ( t_mem_list ));

			// wipe memory
//			memset ( entry, 0x00, (entry->bytes + 2*MEM_SEC_ZONE + sizeof ( t_mem_list ) ));
			free ( entry );
#else
			memset ( ptr, 0xFF, entry->bytes );
			strncpy ( ptr, "FREE", entry->bytes );
			if ( func )
				strncpy ( entry->caller, func, 32 );
			else
				strncpy ( entry->caller, "NULL", 32 );
			entry->line = line;
			entry->freed = 1;
#endif
			MEM_UNLOCK_WR;
			return;
		}
		entry = entry->next;
	}
	MEM_UNLOCK_WR;

	printf ( "%s(%i): FAILED !! free()\n", func, line );

	return;
#endif
}

unsigned int
mem_is_valid ( void *ptr )
{
#ifndef MEM_DEBUG
	return 1;
#else
	unsigned int start_hash = mem_get_hash ( ptr );
	unsigned int end_hash = (start_hash+1) % MEM_HASH_MAX;
	unsigned int hash = start_hash;
	unsigned int address = (unsigned int)ptr - MEM_SEC_ZONE - sizeof(t_mem_list);
	t_mem_list *entry = NULL;

	unsigned int loops = 0;

	MEM_LOCK_RD;
	if ( ptr )
	{
		// this part is now very tricky!
		// we check all entries, but first the most probable one.
		// due to the way we hash, we already have the *probable* list where our
		// memory entry sits. if not, just scan backwards
		//
		// that will usually cost maybe 2 or 3 loops to check if a ptr is valid

		while ( hash != MEM_HASH_MAX )
		{
			entry = list[hash];
			while ( entry )
			{
				if ( (address >= entry->address) && (address <= entry->address+entry->bytes) )
				{
					if ( !mem_check ( entry, 0 ) )
					{
						if ( entry->freed )
							printf ( "mem_is_valid()     DATA ALREADY FREE'D AT %s:%i!\n", entry->caller, entry->line );
						else
							printf ( "mem_is_valid()     DATA CORRUPTED!\n" );

						MEM_UNLOCK_RD;
						return 0;
					}

					MEM_UNLOCK_RD;
					return 1;
				}
				entry = entry->next;
				loops++;
			}

			// processed last entry?
			if ( hash == end_hash )
				hash = MEM_HASH_MAX;			// signal end of loop
			else
			{
				// no, wrap over at entry 0 ?
				if ( hash )
					hash--;						// no wrap, just decrease
				else
					hash = MEM_HASH_MAX - 1;	// wrap over
			}
		}
	}
//	printf ( "FAILED !! mem_is_valid ( 0x%08X )\n", ptr );

	MEM_UNLOCK_RD;
	return 0;
#endif
}

void *
mem_reallocate_seer ( void *ptr, unsigned int bytes )
{
#ifdef MEM_DEBUG
	return mem_reallocate ( ptr, bytes, "SEER_SCRIPT", 0 );
#else
	return realloc ( ptr, bytes );
#endif
}

void *
mem_allocate_seer ( unsigned int bytes )
{
#ifdef MEM_DEBUG
	return mem_allocate_type ( bytes, "SEER_SCRIPT", 0, "internal" );
#else
	return malloc ( bytes );
#endif
}

void *
mem_calloc_seer ( unsigned int count, unsigned int size )
{
#ifdef MEM_DEBUG
	return mem_calloc ( count, size, "SEER_SCRIPT", 0 );
#else
	return calloc ( count, size );
#endif
}

void *
mem_strduplicate_seer ( char *text )
{
#ifdef MEM_DEBUG
	return mem_strduplicate ( text, "SEER_SCRIPT", 0 );
#else
	return strdup ( text );
#endif
}

void
mem_release_seer ( char *buf )
{
#ifdef MEM_DEBUG
	mem_release ( buf, "SEER_SCRIPT", 0 );
#else
	free ( buf );
#endif
}

void *
mem_allocate_passthru ( unsigned int bytes )
{
	void *ptr = malloc ( bytes );

	if ( !ptr )
	{
		HEAP_CHECK;
		mem_check_all (  );
		return NULL;
	}

	return ptr;
}
void
mem_release_passthru ( char *buf )
{
	free ( buf );
}

#endif
