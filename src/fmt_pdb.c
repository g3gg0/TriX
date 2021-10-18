#ifndef __TRIX_FMT_PDB_C__
#define __TRIX_FMT_PDB_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "options.h"

#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"

#include "mem.h"

unsigned int fmt_pdb_enabled = 0;

#define dmDBNameLength    32 /* 31 chars + 1 null terminator */

typedef struct {       /* 78 bytes total */
	char	 name[ dmDBNameLength ];
	unsigned int	 attributes;
	unsigned short	 version;
	unsigned int	 create_time;
	unsigned int	 modify_time;
	unsigned int	 backup_time;
	unsigned int	 modificationNumber;
	unsigned int	 appInfoID;
	unsigned int	 sortInfoID;
	char	 type[4];
	char	 creator[4];
	unsigned int	 id_seed;
	unsigned int	 nextRecordList;
	unsigned short	 numRecords;
} t_pdb_priv;


typedef struct  {   /* 8 bytes total	*/
	STRUCT_HEADER;
	unsigned int	 offset;
/*	struct {
		int delete    : 1;
		int dirty     : 1;
		int busy      : 1;
		int secret    : 1;
		int category  : 4;
	}	 attributes;*/
	unsigned char attributes;

	char	 uniqueID[3];
} t_pdb_rec_header;


unsigned int
fmt_pdb_free ( t_stage * s )
{
    if ( !s )
        return E_OK;

    segment_release_all ( s->segments );
    s->segments = NULL;
	CHECK_AND_FREE ( s->priv );


    return E_OK;
}



int 
fmt_pdb_parse_records ( t_stage *source, t_stage *target )
{
	int pos = 0;
	t_segment *seg = NULL;
	unsigned int *lengths = NULL;
	unsigned int last_start = 0;
	t_pdb_priv *priv = NULL;

	if ( ! source || !target || !target->priv )
		return E_FAIL;
	priv = target->priv;

	if ( priv->numRecords < 0 || priv->numRecords > 0xF000 )
		return E_FAIL;

	lengths = malloc ( sizeof (int) * priv->numRecords );
	if ( !lengths )
		return E_FAIL;

	for ( pos=0; pos<priv->numRecords; pos++ )
	{
		if ( pos * 8 > GET_SIZE ( source ) )
		{
			free ( lengths );
			return E_FAIL;
		}
		if ( pos > 0 )
		{
			lengths[pos-1] = GET_WORD ( source, 78 + pos * 8 ) - last_start;
			if ( GET_WORD ( source, 78 + pos * 8 ) > GET_SIZE ( source ) )
			{
				free ( lengths );
				return E_FAIL;
			}
		}
		last_start = GET_WORD ( source, 78 + pos * 8 );
	}
	lengths[pos-1] = GET_SIZE ( source ) - last_start;

	for ( pos=0; pos<priv->numRecords; pos++ )
	{
		seg = LIST_NEW ( seg, t_segment );
		seg->priv = (t_seg_priv*)PRIV_MALLOC(t_pdb_rec_header);
		if ( !seg->priv )
		{
			free ( lengths );
			return E_FAIL;
		}
		seg->priv->struct_refs++;
		((t_pdb_rec_header*)seg->priv)->attributes = GET_BYTE ( source, 78 + pos * 8 + 4 );
		memcpy (((t_pdb_rec_header*)seg->priv)->uniqueID, GET_PTR ( source, 78 + pos * 8 + 5 ), 3 );
	    seg->name = "DATA";
		seg->start = 0;
		seg->end = lengths[pos];
		seg->data = malloc ( lengths[pos] );
		if ( !seg->data )
		{
			free ( lengths );
			return E_FAIL;
		}
		memcpy ( seg->data, GET_PTR(source,GET_WORD ( source, 78 + pos * 8 )), lengths[pos] );
		seg->length = lengths[pos];

		if ( !target->segments )
			target->segments = seg;
	}

	free ( lengths );

	return E_OK;
}

t_pdb_priv *
fmt_pdb_decode_header ( t_stage *source )
{
	int pos = 0;
	t_pdb_priv *priv = (t_seg_priv*)PRIV_MALLOC(t_pdb_priv);

	if ( GET_SIZE ( source ) < sizeof ( t_pdb_priv ) )
	{
		free ( priv );
		return NULL;
	}

	memcpy ( priv->name, GET_PTR ( source, 0 ), dmDBNameLength );
	priv->attributes = GET_HALF ( source, dmDBNameLength );
	priv->version = GET_HALF ( source, dmDBNameLength + 2 );
	priv->create_time = GET_WORD ( source, dmDBNameLength + 4 );
	priv->modify_time = GET_WORD ( source, dmDBNameLength + 8 );
	priv->backup_time = GET_WORD ( source, dmDBNameLength + 12 );
	priv->modificationNumber = GET_WORD ( source, dmDBNameLength + 16 );
	priv->appInfoID = GET_WORD ( source, dmDBNameLength + 20 );
	priv->sortInfoID = GET_WORD ( source, dmDBNameLength + 24 );
	memcpy ( priv->type, GET_PTR ( source, dmDBNameLength + 28), 4 );
	memcpy ( priv->creator, GET_PTR ( source, dmDBNameLength + 32), 4 );
	priv->id_seed = GET_WORD ( source, dmDBNameLength + 36 );
	priv->nextRecordList = GET_WORD ( source, dmDBNameLength + 40 );
	priv->numRecords = GET_HALF ( source, dmDBNameLength + 44 );

	// check for 0x00 as last char in header
	if ( priv->name[dmDBNameLength-1] != 0x00 )
	{
		free ( priv );
		return NULL;
	}
/*
	// check for ONE null-terminated string
	pos = strlen ( priv->name );
	while ( pos < dmDBNameLength )
	{
		if ( priv->name[pos] != 0x00 )
		{
			free ( priv );
			return NULL;
		}
		pos++;
	}
*/
	return priv;
}

unsigned int
fmt_pdb_decode ( t_stage * source, t_stage * target )
{
    char *decoded = NULL;
    int length = 0;
	t_pdb_priv *priv = NULL;

	if ( !fmt_pdb_enabled )
	{
	    DBG ( DEBUG_FMT, " -> %s ( ) disabled !!\n", __FUNCTION__ );
        return E_FAIL;
	}
	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

	if ( !source || !target || !source->segments || !source->segments->data  )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) != 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed (need one segment)!!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

	priv = fmt_pdb_decode_header ( source );
	if ( priv == NULL )
    {
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    target->name = "DECODED";
    target->length = length;
    target->parser = "PDB";
    target->type = "PDB";
    target->flags = S_MODIFIED;
	target->priv = priv;

	if ( fmt_pdb_parse_records ( source, target ) != E_OK )
    {
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}



unsigned int
fmt_pdb_encode_data ( t_stage * source, t_stage * target )
{
	unsigned char *buffer = NULL;
	unsigned int buffer_length = 78 + 2;
	t_pdb_priv *priv = NULL;
	t_segment *seg = NULL;
	unsigned int startpos = 0;
	int pos = 0;

	if ( !source || !source->priv )
		return NULL;
	priv = (t_pdb_priv *)source->priv;

	seg = source->segments;
	while ( seg )
	{
		buffer_length += seg->length + 8;
		pos++;
		LIST_NEXT(seg);
	}
	priv->numRecords = pos;

	buffer = malloc ( buffer_length );
	memset ( buffer, 0x00, buffer_length );

    target->segments = segment_create (  );
    target->segments->name = "DATA";
    target->segments->start = 0;
    target->segments->end = buffer_length;
    target->segments->length = buffer_length;
    target->segments->data = buffer;

	startpos = 78 + 8 * priv->numRecords + 2;
	seg = source->segments;
	pos = 0;
	while ( seg )
	{
		t_pdb_rec_header *recpriv = seg->priv;

		SET_WORD ( target, 78 + pos * 8, startpos );
		SET_BYTE ( target, 78 + pos * 8 + 4, recpriv->attributes );
		memcpy (GET_PTR ( target, 78 + pos * 8 + 5 ), recpriv->uniqueID, 3 );
		memcpy (GET_PTR ( target, startpos ), seg->data, seg->length );
		
		startpos += seg->length;
		pos++;
		LIST_NEXT(seg);
	}

	memcpy ( GET_PTR ( target, 0 ), priv->name, dmDBNameLength );
	SET_HALF ( target, dmDBNameLength, priv->attributes );
	SET_HALF ( target, dmDBNameLength + 2, priv->version );
	SET_WORD ( target, dmDBNameLength + 4, priv->create_time );
	SET_WORD ( target, dmDBNameLength + 8, priv->modify_time );
	SET_WORD ( target, dmDBNameLength + 12, priv->backup_time );
	SET_WORD ( target, dmDBNameLength + 16, priv->modificationNumber );
	SET_WORD ( target, dmDBNameLength + 20, priv->appInfoID );
	SET_WORD ( target, dmDBNameLength + 24, priv->sortInfoID );
	memcpy ( GET_PTR ( target, dmDBNameLength + 28), priv->type, 4 );
	memcpy ( GET_PTR ( target, dmDBNameLength + 32), priv->creator,  4 );
	SET_WORD ( target, dmDBNameLength + 36, priv->id_seed );
	SET_WORD ( target, dmDBNameLength + 40, priv->nextRecordList );
	SET_HALF ( target, dmDBNameLength + 44, priv->numRecords );

	return E_OK;
}


unsigned int
fmt_pdb_encode ( t_stage * source, t_stage * target )
{
	unsigned char *buffer = NULL;
	unsigned int length = 0;

	if ( !fmt_pdb_enabled )
	{
	    DBG ( DEBUG_FMT, " -> %s ( ) disabled !!\n", __FUNCTION__ );
        return E_FAIL;
	}

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    if ( !target )
        target = source->next;

	if ( !source || !target || !source->segments )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) < 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

	if ( fmt_pdb_encode_data ( source, target ) != E_OK )
        return E_FAIL;

	target->name = "RAW";
    target->length = length;
    target->parser = "PDB";
    target->type = "PDB";

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;

}

t_fmt_handler pdb_handler = {
    "PDB",
    "PARSER",
	"fmt_pdb_decode",
    fmt_pdb_decode,
	"fmt_pdb_encode",
	fmt_pdb_encode,
	"fmt_pdb_free",
    fmt_pdb_free,
	NULL,
	NULL
};

unsigned int
fmt_pdb_init (  )
{
	fmt_register_handler ( &pdb_handler );

	options_add_core_option ( OPT_BOOL, "fmt.pdb", fmt_pdb_enabled, "Enable PDB. Disabled by default since there is no safe detection of the format" );
	return E_OK;
}

#endif
