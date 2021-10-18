#ifndef __TRIX_CHUNK_VFNT_H__
#define __TRIX_CHUNK_VFNT_H__

/* these informations are taken from Microsoft's OpenType specification
	http://www.microsoft.com/typography/OTSPEC/default.htm
*/


// OpenType Required Tables
#define TTF_M_CMAP	'cmap'
#define TTF_M_HEAD	'head'
#define TTF_M_HHEA	'hhea'
#define TTF_M_HMTX	'hmtx'
#define TTF_M_MAXP	'maxp'
#define TTF_M_NAME	'name'
#define TTF_M_OS2		'OS/2'
#define TTF_M_POST	'post'

// Platform IDs
#define TTF_PLATFORM_ID_UNICODE		0
#define TTF_PLATFORM_ID_MACINTOSH	1
#define TTF_PLATFORM_ID_ISO         2
#define TTF_PLATFORM_ID_WINDOWS		3
#define TTF_PLATFORM_ID_CUSTOM		4

// Name IDs
#define TTF_NAME_ID_COPYRIGHT_NOTICE		0
#define TTF_NAME_ID_FONT_FAMILY_NAME		1
#define TTF_NAME_ID_FONT_SUBFAMILY_NAME	2
#define TTF_NAME_ID_UNIQUE_FONT_ID			3
#define TTF_NAME_ID_FULL_FONT_NAME			4
#define TTF_NAME_ID_VERSION_STRING			5
#define TTF_NAME_ID_POSTSCRIPT_NAME			6
#define TTF_NAME_ID_TRADEMARK					7
#define TTF_NAME_ID_MANUFACTURER_NAME		8
#define TTF_NAME_ID_DESIGNER					9
#define TTF_NAME_ID_DESCRIPTION				10
#define TTF_NAME_ID_URL_VENDOR				11
#define TTF_NAME_ID_URL_DESIGNER				12
#define TTF_NAME_ID_LICENSE_DESCRIPTION	13
#define TTF_NAME_ID_LICENSE_INFO_URL		14

typedef struct
{
	unsigned short	platformID;    // Platform ID.
	unsigned short	encodingID;    // Platform-specific encoding ID.
	unsigned short	languageID;    // Language ID.
	unsigned short	nameID;        // Name ID.
	unsigned short	length;        // String length (in bytes).
	unsigned short	offset;        // String offset from start of storage area (in bytes).
} t_ttf_name_rec;

typedef struct
{
	unsigned short	format;			// Format selector (=0).
	unsigned short	count;         // Number of name records.
	unsigned short	stringOffset;  // Offset to start of string storage (from start of table).
	t_ttf_name_rec	nameRecord;    // The name records where count is the number of records.
} t_ttf_name ;

#endif /* __TRIX_CHUNK_VFNT_H__ */
#ifndef __TRIX_CHUNK_VFNT_H__
#define __TRIX_CHUNK_VFNT_H__

/* these informations are taken from Microsoft's OpenType specification
	http://www.microsoft.com/typography/OTSPEC/default.htm
*/


// OpenType Required Tables
#define TTF_M_CMAP	'cmap'
#define TTF_M_HEAD	'head'
#define TTF_M_HHEA	'hhea'
#define TTF_M_HMTX	'hmtx'
#define TTF_M_MAXP	'maxp'
#define TTF_M_NAME	'name'
#define TTF_M_OS2		'OS/2'
#define TTF_M_POST	'post'

// Platform IDs
#define TTF_PLATFORM_ID_UNICODE		0
#define TTF_PLATFORM_ID_MACINTOSH	1
#define TTF_PLATFORM_ID_ISO         2
#define TTF_PLATFORM_ID_WINDOWS		3
#define TTF_PLATFORM_ID_CUSTOM		4

// Name IDs
#define TTF_NAME_ID_COPYRIGHT_NOTICE		0
#define TTF_NAME_ID_FONT_FAMILY_NAME		1
#define TTF_NAME_ID_FONT_SUBFAMILY_NAME	2
#define TTF_NAME_ID_UNIQUE_FONT_ID			3
#define TTF_NAME_ID_FULL_FONT_NAME			4
#define TTF_NAME_ID_VERSION_STRING			5
#define TTF_NAME_ID_POSTSCRIPT_NAME			6
#define TTF_NAME_ID_TRADEMARK					7
#define TTF_NAME_ID_MANUFACTURER_NAME		8
#define TTF_NAME_ID_DESIGNER					9
#define TTF_NAME_ID_DESCRIPTION				10
#define TTF_NAME_ID_URL_VENDOR				11
#define TTF_NAME_ID_URL_DESIGNER				12
#define TTF_NAME_ID_LICENSE_DESCRIPTION	13
#define TTF_NAME_ID_LICENSE_INFO_URL		14

typedef struct
{
	unsigned short	platformID;    // Platform ID.
	unsigned short	encodingID;    // Platform-specific encoding ID.
	unsigned short	languageID;    // Language ID.
	unsigned short	nameID;        // Name ID.
	unsigned short	length;        // String length (in bytes).
	unsigned short	offset;        // String offset from start of storage area (in bytes).
} t_ttf_name_rec;

typedef struct
{
	unsigned short	format;			// Format selector (=0).
	unsigned short	count;         // Number of name records.
	unsigned short	stringOffset;  // Offset to start of string storage (from start of table).
	t_ttf_name_rec	nameRecord;    // The name records where count is the number of records.
} t_ttf_name ;

#endif /* __TRIX_CHUNK_VFNT_H__ */
