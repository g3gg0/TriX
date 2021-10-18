
#define LOC_UNIQUE    1
#define LOC_FORWARD   0
#define LOC_BACKWARD  2

#define DEBUG_FMT       0x0001
#define DEBUG_SEER      0x0002
#define DEBUG_ARM       0x0004
#define DEBUG_NOKIA     0x0008
#define DEBUG_LIST      0x0010
#define DEBUG_UNKNOWN   0x0020


typedef char* t_function2;
typedef char* func;
typedef char* object;

typedef struct s_list
{
	STRUCT_HEADER_LIST(t_list);
} t_list;

typedef struct s_priv
{
	STRUCT_HEADER;
} t_priv;

typedef struct s_locator
{
    unsigned char *func_name;
    unsigned char *pattern;
    unsigned char *mask;
    unsigned int length;
    unsigned int options;
} t_locator;

typedef struct s_function_block
{
    char *name;
    unsigned char *data;
    unsigned int length;
} t_function_block;

typedef struct s_function_relative_data
{
    unsigned int offset;
    int type;
    char *dest;
} t_function_relative_data;

typedef struct s_function
{
    char *name;
    t_function_block *blocks;
    t_function_relative_data *rel_data;
} t_function;

// INSERT_DECLARATIONS

unsigned int FindPattern(t_locator *loc, unsigned int start, unsigned int end, int options) { return __util_find_pattern(GetWorkspace(), loc, start, end, options); }
unsigned int QuickFindPattern(char *pattern, char *mask, unsigned int size, unsigned int start, unsigned int end, int options) { return __util_quick_find_pattern(GetWorkspace(),pattern, mask, size, start, end, options); }
unsigned int SimpleFindPattern(char *pattern, char *mask, unsigned int size, unsigned int options) { return __util_simple_find_pattern(GetWorkspace(), pattern, mask, size, options); }
unsigned int Valid(int x) { return __v_valid (GetWorkspace(), x); }
unsigned int GetSize() { return __v_get_size (GetWorkspace()); }
void CacheDisable() { __util_cache_disable(); }
void CacheEnable() { __util_cache_enable(); }
void DumpBytes(unsigned char *data, int width, unsigned int bytes) { __util_dump_bytes(data, width, bytes); }
unsigned int GetByte(unsigned int x) { return __v_get_b (GetWorkspace(), x); }
unsigned int GetHalf(unsigned int x) { return __v_get_h (GetWorkspace(), x); }
unsigned int GetWord(unsigned int x) { return __v_get_w (GetWorkspace(), x); }
unsigned int SetByte(unsigned int x,unsigned int y) { return __v_set_b (GetWorkspace(), x, y); }
unsigned int SetHalf(unsigned int x,unsigned int y) { return __v_set_h (GetWorkspace(), x, y); }
unsigned int SetWord(unsigned int x,unsigned int y) { return __v_set_w (GetWorkspace(), x, y); }
unsigned int MemCpy(unsigned int x,unsigned int y,unsigned int z) { return __v_memcpy (GetWorkspace(),x,y,z); }
unsigned int MemCpyGet(unsigned char *x,unsigned int y,unsigned int z) { return __v_memcpy_get (GetWorkspace(),x,y,z); }
unsigned int MemCpyPut(unsigned int x,unsigned char *y,unsigned int z) { return __v_memcpy_put (GetWorkspace(),x,y,z); }
unsigned char *GetPtr(unsigned int x) { return __v_get_ptr (GetWorkspace(), x); }
unsigned char *GetStr(unsigned int x) { return __v_get_str (GetWorkspace(), x); }
unsigned int GetStrLen(unsigned int x) { return __v_get_strlen (GetWorkspace(), x); }
unsigned int SH(unsigned int x) { return __util_swap_half (x); }
unsigned int SW(unsigned int x) { return __util_swap_word (x); }
void SetDebugLevel ( int debuglevel ) { __util_set_debuglevel ( debuglevel ); }
void SetErrorLevel ( int errorlevel ) { __util_set_errorlevel ( errorlevel ); }
unsigned int Str2Int ( char *str ) { return __util_str2int ( str ); }

void DrawBytes ( unsigned int data, int width, unsigned int bytes )
{
	unsigned int pos = 0;
	int col = 0;
	unsigned char byte = 0;

	say ( "-------- print bytes at 0x%08X -------\n", data );

	while ( pos < bytes )
	{
		say ( "%08X: ", pos );
		col = 0;
		while ( col < width  )
		{
			if ( col+pos < bytes )
				say ( "%02X ", GetByte (data+pos+col) );
			else
				say ( "   " );
			col++;
		}

		col = 0;
		while ( col < width )
		{
			if ( col+pos < bytes )
			{
				byte = GetByte (data+pos+col);
				if ( byte < 0x80 && byte > 0x20 )
					say ( "%c", byte );
				else
					say ( "." );
			}
			else
				say ( " " );

			col++;
		}
		say ( "\n" );
		pos += width;
	}

	say ( "\n" );
	say ( "---------- printed all bytes ---------\n" );
	return;
}
