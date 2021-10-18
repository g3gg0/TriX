/************************************************************************
 SeeR a C-scripting library v 0.95a
 http://przemekp.prv.pl/seer/        dev@przemekp.prv.pl

 File:pmem.c
 Desc:pmem struct - infinite buffor

 Copyright (c) 1998-99,2002, Przemyslaw Podsiadly
 All rights reserved.
 See seer.h for license information.
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "seer_internal.h"

#include "mem.h"

//#define SEER_MEMDEBUG

#ifdef SEER_MEMDEBUG
typedef struct hash_Node
{
    struct hash_Node *next;
    void *addr;
} hash_Node;
hash_Node *hash_table[4091];
int scAlloc_Inited = 0;

int
hash_tab ( unsigned int i )
{
    return i % 4091;
}

void
scAddToHash ( void *l )
{
    int i;
    hash_Node *nd, *ac;
    i = hash_tab ( ( unsigned int ) l );

    ac = malloc ( sizeof ( hash_Node ) );
    ac->addr = l;
    nd = hash_table[i];
/* {hash_Node* a=nd;
 while(a)
        {if (a->addr==l)
                deb0("Two at the same time!\n");
         a=a->next;
        }
 }*/
    ac->next = nd;
    hash_table[i] = ac;
}


void
scRemoveFromHash ( void *x )
{
    int i = hash_tab ( ( unsigned int ) x );
    if ( x == NULL )
        return;
    if ( !hash_table[i] )
        return;
    if ( hash_table[i]->addr == x )
    {
        hash_Node *p;
        p = hash_table[i];
        hash_table[i] = hash_table[i]->next;
        free ( p );
    }
    else
    {
        hash_Node *n, *p;
        n = hash_table[i];
        while ( n->next && n->next->addr != x )
            n = n->next;
        if ( n->next )
        {
            p = n->next;
            n->next = p->next;
            free ( p );
        }
        else
            deb1 ( "Suspitious remove of %d !!!!\n", ( int ) x );
    }
}
#endif

void *
scAlloc ( int size )
{
    void *l;
    l = calloc ( 1, size + 100 );
#ifdef SEER_MEMDEBUG
    scAddToHash ( l );
#endif
    return l;
}
void
scFree ( void *x )
{
#ifdef SEER_MEMDEBUG
    scRemoveFromHash ( x );
#endif
	free ( x );
}

void *
scRealloc ( void *mem, int size )
{
    void *l = realloc ( mem, size + 100 );
#ifdef SEER_MEMDEBUG
    scRemoveFromHash ( mem );
    scAddToHash ( l );
#endif
	return l;
}

void
scInitAlloc (  )
{
#ifdef SEER_MEMDEBUG
    int i;
    for ( i = 0; i < 4091; i++ )
        hash_table[i] = NULL;
#endif
}

char *
scStrdup ( char *x )
{
#ifdef SEER_MEMDEBUG
    char *y = scAlloc ( strlen ( x ) + 100 );
    strcpy ( y, x );
    return y;
#else
	return strdup ( x );
#endif
}

void
scCleanUp (  )
{
#ifdef SEER_MEMDEBUG
    int i;
    deb0 ( "Doing CleanUp" );
    for ( i = 0; i < 4091; i++ )
        if ( hash_table[i] )
        {
            hash_Node *x = hash_table[i], *p;
            while ( x )         //(x&x->next)
            {
                p = x->next;
                free ( x->addr );
//                deb0 ( "." );
//                deb1 ( "(%x)", x->addr );
                free ( x );
                x = p;
            }
            hash_table[i] = NULL;
        }
        else
        {                       //deb1("[%d]",i);
        }
    deb0 ( "\nDone.\n" );
#endif
}

void
scmemTestUp (  )
{
#ifdef SEER_MEMDEBUG
    int i;
    char o;
    deb0 ( "Doing TestUp" );
    for ( i = 0; i < 4091; i++ )
        if ( hash_table[i] )
        {
            hash_Node *x = hash_table[i], *p;
            while ( x )         //(x&x->next)
            {
                p = x->next;
                o = *( char * ) x->addr;
//      deb0(".");
                free ( x );
                x = p;
            }
            hash_table[i] = NULL;
        }
        else
        {                       //deb1("[%d]",i);
        }
    deb0 ( "\nDone.\n" );
#endif
}

int
new_pmem ( pmem * mem, int init_size )
{
    mem->mem = scAlloc ( init_size );
    mem->size = init_size;
    mem->pos = 0;
    return 1;                   //success
}

void
free_pmem ( pmem * mem )
{
    scFree ( mem->mem );
    mem->size = mem->pos = 0;
    mem->mem = NULL;
}

void
pop_pmem ( pmem * mem, int count )
{
    mem->pos -= count;
    if ( mem->pos < 0 )
    {
        mem->pos = 0;
        deb0 ( "pop_pmem: BUG results in mem->pos < 0\n" );
    }
}
void
push_pmem ( pmem * mem, int size, void *data )
{
#ifdef DEBUG
#ifdef _SHOW_MACHINE_CODE
    int i = mem->pos, k;
#endif
#endif
    if ( mem->pos + size + 16 >= mem->size )    //resize
    {                           /*
                                 * if (mem->size>1024) mem->size+=1024;
                                 * else
                                 * if (mem->size<2*size)
                                 * mem->size+=2*size;
                                 * else
                                 * mem->size*=2;
                                 */
        deb1 ( "REALLOC from %d ", mem->size );
        mem->size += size + 256 + mem->size / 8;
//         if (size<=1024) mem->size+=1024;
        mem->mem = scRealloc ( mem->mem, mem->size );
        deb1 ( "to %d!!\n", mem->size );    //getchar();
    }
    memcpy ( mem->mem + mem->pos, data, size );
    mem->pos += size;
#ifdef DEBUG
#ifdef _SHOW_MACHINE_CODE
    for ( ; i < mem->pos; i++ )
    {
        k = ( *( ( char * ) ( mem->mem + i ) ) ) & 0xFF;
        deb1 ( " %02X", k );
    }
#endif
#endif
}

void
pushint_pmem ( pmem * mem, int x )
{
    int l;
//1111 2222 3333 4444
// l=(x<<24)+((x>>8)<<24)+((x>>16)<<24)+(x>>24);
    l = x;
    push_pmem ( mem, 4, &l );
}

void
pushstr_pmem ( pmem * mem, char *s, int len )
{
    char c = 0;
    while ( len-- )
        push_pmem ( mem, 1, ( s++ ) );
    push_pmem ( mem, 1, &c );
}

void
align_pmem ( pmem * mem )
{
    char c = 0;
    while ( mem->pos % 4 )
        push_pmem ( mem, 1, &c );
}
