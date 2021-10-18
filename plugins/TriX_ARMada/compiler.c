

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "code.h"
#include "compiler.h"

char path[512];

unsigned char* string_uppercase ( unsigned char *string )
{
	unsigned char *ret = string;

    while ( *string )
    {
		if ( *string >= 'a' && *string <= 'z' )
			*string -= 0x20;
		string++;
    }

	return ret;
}


int set_path ( char* p )
{
	return (strcpy ( path, p )?1:0);
}

void * asm_get_return_addr ( void * base, void * offset )
{
    int found = 0;
    unsigned char * cur = (unsigned char *)((unsigned int) base + (unsigned int) offset);

    while ( !found )
    {
	    if ( cur[0] == ASM_POPL )
	        found = 1;
	    else
	    {
	        cur++;
	        cur++;
	    }
    }

    cur = (unsigned char *)((unsigned int) cur - (unsigned int) base);

    return cur ;
}



void * asm_find_function ( void * base, unsigned int length, locator loc, int msg )
{
    int done = 0;
    int found = 0;
    int cont = 0;
	int multiple = 0;
    unsigned int pos = 0;
    unsigned char *ret = NULL;
    unsigned char *cur = base;

	if ( msg & 0x80 )
		multiple = 1;

	msg = msg & 0x7F;

    while ( !done )
    {

//	message ( "%p: is: %02X should: %02X\n", (void*)(unsigned int)cur + pos, cur[pos], loc.pattern[pos] );
	if ( ( (unsigned int)cur - (unsigned int)base + pos + 1  < length ) && 
	     ((cur[pos] & loc.mask[pos]) == ( loc.pattern[pos] & loc.mask[pos]) && !cont)
	   )
	{
	    if ( loc.length == pos + 1 )
	    {
			if ( found  )
			{
				if ( !multiple )
				{
//					if ( msg ) 
//						message ( "  located at: 0x%p   DUP!!\n", (void*)((unsigned int)cur - (unsigned int)base ));
					ret = NULL;
				}
				else
				{
//					if ( 0 && msg ) 
//						message ( "         and: 0x%p\n", (void*)((unsigned int)cur - (unsigned int)base ));
				}
			}
			else
			{
//				if ( msg ) 
//					message ( "  located at: 0x%p\n", (void*)((unsigned int)cur - (unsigned int)base ));
    			found = 1;
    			ret = cur;		
			}
			cont = 1;
			}
		else
		{
			pos++;
	    }
	}
	else
	{
	    cont = 0;
	    if ( (unsigned int)cur - (unsigned int)base + pos + 1  < length )
	    {
			cur = (unsigned char *) (unsigned int) cur + 1;
			pos = 0;
	    }
	    else
	    {
			if ( !found )
			{
				if ( !multiple )
                {
//                    if ( msg )
//					message ( "  FATAL: not found - sorry\n" );
                }
//				else if ( msg )
//					message ( "  no more matches\n" );

			}
			else
			{
//				if ( !ret )
//					message ( "  FATAL: too many matches, please let g3gg0 refine the search!\n" );
			}
			done = 1;
	    }
	}
    }
    
    if ( ret && ret[0] != ASM_PUSHL && !loc.no_search_back )
    {
//		if ( msg )
//			message ( "  [searching back to entry point]\n" );
		while ( (unsigned int)ret > (unsigned int)base + 1 && ret[0] != ASM_PUSHL )
		{
			ret--;
			ret--;
		}
		if  ( ret[0] != ASM_PUSHL )
		{
//			message ( "  could not find entry point!\n" );
			ret = NULL;
		}
		else
		{
			ret = (unsigned char *)((unsigned int) ret - (unsigned int) base);
//    		if ( msg )
//				message ( "  - func_start: %p\n", ret );
		}
    }
    else if ( ret )
    {
        ret = (unsigned char *)((unsigned int) ret - (unsigned int) base);
//        if ( msg && !loc.no_search_back )
//				message ( "  - func_start: %p\n", ret );
    }
    /*
     *       just to show some confusing infos ;P
     *       
     */ 
    
    if ( ret && !loc.no_search_back )
	{
		if (!asm_get_return_addr ( base, ret ) )
		{
//			message ( "could not find function's end :(\n" );
			return NULL;
		}
	}
	    
//    if ( msg ) 
///		message ( "----------------------------------\n" );
    
    return ret;
}


int asm_locate_functions ( code_t *c )
{
	int ret = 0;
	int num = 0;
	int lin = 0;
	int done = 0;
	char file[540];
	char buf[819201];
	char pattern[8192];
	char mask[8192];
	char *line = NULL;
	char *temp = NULL;
	unsigned char byte;
	unsigned long addr = 0;
	unsigned int i = 0;
	locator loc[1024];
	FILE *f = NULL;

	loc[0].func_name = NULL;
	memset ( buf, 0, 819201 );

	sprintf ( file, "%s\\patterns.txt", path );
	f = fopen ( file, "r" );

	if ( f )
	{
		if ( fread  ( buf, 819200, 1, f ) < 0 )
			done = 1;
		while ( !done && num < 1024 )
		{
			loc[num].func_name = NULL;
			line = get_line( buf, lin );
			if ( !line )
				done = 1;
			else
			{
				if ( line[0] != '#' )
				{
					loc[num].func_name = malloc ( 1024 );
					ret = sscanf ( line, "%s %s %s %d", loc[num].func_name, pattern, mask, &(loc[num].no_search_back) );
					if ( ret != 4 )
					{
						free (loc[num].func_name);
						loc[num].func_name = NULL;
					}
					else
					{
						if ( strlen ( pattern )  !=  strlen ( mask) )
						{
							free ( loc[num].func_name );
							loc[num].func_name = NULL;
							add_error ( c, "PATTERNS.TXT mask != pattern", lin + 1 );
						}
						else
						{
							loc[num].func_name = realloc ( loc[num].func_name, strlen ( loc[num].func_name ) + 200 );
							loc[num].length    = strlen ( pattern ) / 4;
							loc[num].pattern   = malloc ( loc[num].length + 10 );
							loc[num].mask      = malloc ( loc[num].length + 10 );
							for ( i=0; i<loc[num].length; i++ )
							{
								temp =  (char*)((unsigned long)pattern + (i*4+2));
								sscanf ( temp, "%02x", &byte );
								loc[num].pattern[i] = byte;
								temp =  (char*)((unsigned long)mask + (i*4+2));
								sscanf ( temp, "%02x", &byte );
								loc[num].mask[i] = byte;
							}
							num++;
						}
					}
				}
			}
			free ( line );
			lin++;
		}
		fclose ( f );
	}
	else
	{
		add_error ( c, "PATTERNS.TXT not in current Directory", 0 );
	}

	num = 0;
	while ( loc[num].func_name )
	{
		addr = (unsigned long)asm_find_function ( c->base, c->length, loc[num], 0 );
		if ( addr )
			add_import ( c, loc[num].func_name, addr );
		free ( loc[num].func_name );
		free ( loc[num].pattern );
		free ( loc[num].mask );
		num++;
	}
	return 0;
}


int _httoi( char *value, int *ret )
{
  struct CHexMap
  {
    char chr;
    int value;
  };
  const int HexMapL = 16;
  struct CHexMap HexMap[16] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };
  char *mstr = NULL;
  char *s = NULL;
  int result = 1;
  int found = 0;
  int i = 0;

  if ( !value )
	  return -1;

  mstr = string_uppercase(strdup(value));
  s = mstr;

  *ret = 0;

  if (*s == '0' && *(s + 1) == 'X') 
	  s += 2;
  while (*s != '\0')
  {
    for (i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
		*ret <<= 4;
        *ret |= HexMap[i].value;
        found = 1;
        break;
      }
    }
    if (!found) 
		break;
    s++;
	found = 0;
  }
  if ( *s != ' ' && *s != '\000' )
	  result = -1;

  free(mstr);

  return result;
}


char *getparm ( char *t, unsigned int p )
{
	unsigned int start   = 0;
	unsigned int end     = 0;
	unsigned int pos     = 0;
	unsigned int cur     = 1;
	unsigned int retsize = 0;
	unsigned int length  = 0;
	int string = 0;
	static char *ret = NULL;

	if ( !t )
		return NULL;

	length = strlen (t);

	while ( cur <= p )
	{
		while ( t[pos] && (t[pos] == ' ' ||t[pos] == '[' || t[pos] == '{' ) && pos < length )
			pos++;

		if ( pos >= length )
			return NULL;

		start = pos;
		while ( t[pos] && ((t[pos] != ' ' && t[pos] != ',' && t[pos] != '[' && t[pos] != ']' && t[pos] != '{'  && t[pos] != '}' ) || string) )
		{
			if ( t[pos] == '"' || t[pos] == '\'' )
				string = string ^ 1;
			if ( string && (t[pos] == '\\' && t[pos+1] == '"') )
				pos++; // skip '\'
			pos++;
		}

		end = pos;

		if ( end < start )
			return NULL;

		if ( cur == p )
		{
			if ( t[start] == '"' || t[start] == '\'' )
				start++;
			if ( end > start && (t[end-1] == '"' || t[end-1] == '\'') )
				end--;
			if ( !ret )
			{
				retsize = end - start + 1;
				ret = malloc ( retsize + 200 );
			}
			if ( end - start > retsize )
			{
				retsize = end - start + 1;
				ret = realloc ( ret, retsize + 200 );
			}
				
			memcpy ( ret, (void*)((unsigned long)t + start), end - start );
			ret[end-start] = '\000';

			pos = 0;
			while ( ret[pos] )
			{
				if ( ret[pos] == '\\' )
					memcpy ( &ret[pos], &ret[pos+1], strlen ( &ret[pos+1] ) + 1 );
				pos++;
			}

			return ret;
		}
		pos++;
		cur++;

	}

	return NULL;
}

int getparms ( char *t )
{
	int pos = 0;
	char *ret = NULL;

	if ( !t )
		return 0;

	while ( ret = getparm ( t, pos + 1 ) )
		pos++;

	return pos;

}


unsigned int pt32 ( char *p )
{
	int val;

	if ( !p )
		return PARM_UNK;

	if ( !strcmp ( p, "PC" ) )
		return PARM_REG32;
	if ( !strcmp ( p, "LR" ) )
		return PARM_REG32;
	if ( !strcmp ( p, "CPSR" ) )
		return PARM_REG_CPSR;
	if ( !strcmp ( p, "CPSR_FLG" ) )
		return PARM_REG_CPSRF;
	if ( !strcmp ( p, "SPSR" ) )
		return PARM_REG_SPSR;
	if ( !strcmp ( p, "SPSR_FLG" ) )
		return PARM_REG_SPSRF;

	if ( p[0] == 'R' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 16 )
			return PARM_REG32;

		return PARM_UNK;
	}

	if ( p[0] == 'C' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 16 )
			return PARM_CREG;

		return PARM_UNK;
	}

	if ( p[0] == 'P' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 16 )
			return PARM_CPID;

		return PARM_UNK;
	}

	if ( 
		(
		 (  ( p[0] >= '0' && p[0] <= '9') || 
			( p[0] >= 'A' && p[0] <= 'F') || 
			( p[0] >= 'a' && p[0] <= 'f') 
		 ) && _httoi ( p, &val ) != -1 )
	   )
	   return PARM_VAL;

	if (  (p[0] == '.' || p[0] == '#') && 
		 (  ( p[1] >= '0' && p[1] <= '9')  
		 ) && sscanf (  p+1, "%i", &val ) == 1 )
		return PARM_VAL;

	return PARM_UNK;
}


unsigned int pt ( char *p )
{
	int val;

	if ( !p )
		return PARM_UNK;

	if ( p[0] == 'R' && p[1] >= '0' && p[1] <= '7' && p[2] == '\000' )
		return PARM_REG;
	if ( p[0] == 'H' && p[1] >= '0' && p[1] <= '7' && p[2] == '\000' )
		return PARM_REG_HI;

	if ( !strcmp ( p, "LR" ) )
		return PARM_REG_HI;
	if ( !strcmp ( p, "SP" ) )
		return PARM_REG_HI;
	if ( !strcmp ( p, "PC" ) )
		return PARM_REG_HI;

	if ( !strcmp ( p, "CPSR" ) )
		return PARM_REG_CPSR;
	if ( !strcmp ( p, "CPSR_FLG" ) )
		return PARM_REG_CPSRF;
	if ( !strcmp ( p, "SPSR" ) )
		return PARM_REG_SPSR;
	if ( !strcmp ( p, "SPSR_FLG" ) )
		return PARM_REG_SPSRF;

	if ( p[0] == 'R' && 
		( 
		  ( p[1] >= '8' && p[1] <= '9' && p[2] == '\000' ) ||
		  ( p[1] == '1' && p[2] >= '0' && p[2] <= '5' && p[3] == '\000' )
		)
	   )
		return PARM_REG_HI;

	if ( 
		(
		 (  ( p[0] >= '0' && p[0] <= '9') || 
			( p[0] >= 'A' && p[0] <= 'F') || 
			( p[0] >= 'a' && p[0] <= 'f') 
		 ) && _httoi ( p, &val ) != -1 )
	   )
	   return PARM_VAL;

	if (  (p[0] == '.' || p[0] == '#') && 
		 (  ( p[1] >= '0' && p[1] <= '9')  
		 ) && sscanf (  p+1, "%i", &val ) == 1 )
		return PARM_VAL;

	return PARM_UNK;
}

unsigned int pt_true ( char *p, unsigned int parm_type )
{
	int val;

	if ( !p )
	{
		if ( parm_type == PARM_UNK )
			return 1;
		return 0;
	}

	if ( p[0] == 'R' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 16 )
			return parm_type & (1<<val);
	}

	if ( p[0] == 'C' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 16 )
			return parm_type & PARM_CREG;
	}
	if ( p[0] == 'P' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 16 )
			return parm_type & PARM_CPID;
	}


	if ( p[0] == 'H' )
	{
		if ( (sscanf ( p+1, "%i", &val ) == 1) && val < 8 )
			return parm_type & (1<<(8+val));
	}

	if ( !strcmp ( p, "LR" ) )
		return parm_type & PARM_REG_LR;
	if ( !strcmp ( p, "SP" ) )
		return parm_type & PARM_REG_SP;
	if ( !strcmp ( p, "PC" ) )
		return parm_type & PARM_REG_PC;

	if ( !strcmp ( p, "CPSR" ) )
		return parm_type & PARM_REG_CPSR;
	if ( !strcmp ( p, "CPSR_FLG" ) )
		return parm_type & PARM_REG_CPSRF;
	if ( !strcmp ( p, "SPSR" ) )
		return parm_type & PARM_REG_SPSR;
	if ( !strcmp ( p, "SPSR_FLG" ) )
		return parm_type & PARM_REG_SPSRF;

	if ( 
		(
		 (  ( p[0] >= '0' && p[0] <= '9') || 
			( p[0] >= 'A' && p[0] <= 'F') || 
			( p[0] >= 'a' && p[0] <= 'f') 
		 ) && _httoi ( p, &val ) != -1 
		)
	   )
	   return PARM_VAL & parm_type;

	if (  (p[0] == '.' || p[0] == '#') && 
		 (  ( p[1] >= '0' && p[1] <= '9')  
		 ) && sscanf (  p+1, "%i", &val ) == 1 
	   )
		return PARM_VAL & parm_type;

	if ( parm_type == PARM_UNK )
		return 1;

	return 0;
}

unsigned short reg ( char *p )
{
	unsigned short ret = REG_UNK;
	int r = 0;

	if ( !p )
		return REG_UNK;

	if ( !strcmp ( "SP", p ) )
		return REG_R13;

	if ( !strcmp ( "LR", p ) )
		return REG_R14;

	if ( !strcmp ( "PC", p ) )
		return REG_R15;

	if ( p[0] != 'R' && p[0] != 'H' )
		return REG_UNK;

	if ( p[0] == 'R' && p[1] >= '0' && p[1] <= '7' && p[2] == '\000' )
	{
		r = atoi ( (char*)((unsigned long)p + 1) );
		switch ( r )
		{
			case 0:
				ret = REG_R0;
				break;
			case 1:
				ret = REG_R1;
				break;
			case 2:
				ret = REG_R2;
				break;
			case 3:
				ret = REG_R3;
				break;
			case 4:
				ret = REG_R4;
				break;
			case 5:
				ret = REG_R5;
				break;
			case 6:
				ret = REG_R6;
				break;
			case 7:
				ret = REG_R7;
				break;
			default:
				ret = REG_UNK;
				break;
		}
	}
	else if ( 
		  ( p[1] >= '8' && p[1] <= '9' && p[2] == '\000' ) ||
		  ( p[1] == '1' && p[2] >= '0' && p[2] < '6' && p[3] == '\000' )
		)
	{
		r = atoi ( (char*)((unsigned long)p + 1) );
		switch ( r )
		{
			case 8:
				ret = REG_R8;
				break;
			case 9:
				ret = REG_R9;
				break;
			case 10:
				ret = REG_R10;
				break;
			case 11:
				ret = REG_R11;
				break;
			case 12:
				ret = REG_R12;
				break;
			case 13:
				ret = REG_R13;
				break;
			case 14:
				ret = REG_R14;
				break;
			case 15:
				ret = REG_R15;
				break;
			default:
				ret = REG_UNK;
				break;
		}

	}
	else if ( p[0] == 'H' && p[1] >= '0' && p[1] <= '7' && p[2] == '\000' )
	{
		r = atoi ( (char*)((unsigned long)p + 1) );
		switch ( r )
		{
			case 0:
				ret = REG_R8;
				break;
			case 1:
				ret = REG_R9;
				break;
			case 2:
				ret = REG_R10;
				break;
			case 3:
				ret = REG_R11;
				break;
			case 4:
				ret = REG_R12;
				break;
			case 5:
				ret = REG_R13;
				break;
			case 6:
				ret = REG_R14;
				break;
			case 7:
				ret = REG_R15;
				break;
			default:
				ret = REG_UNK;
				break;
		}
	}



	return ret;
}


unsigned short reg32 ( char *p )
{
	unsigned short ret = REG32_UNK;
	int r = 0;

	if ( !p )
		return REG32_UNK;

	if ( !strcmp ( "SP", p ) )
		return REG32_R13;

	if ( !strcmp ( "LR", p ) )
		return REG32_R14;

	if ( !strcmp ( "PC", p ) )
		return REG32_R15;

	if ( p[0] != 'R' && p[0] != 'C' && p[0] != 'P' )
		return REG32_UNK;

	if ( p[1] >= '0' && p[1] <= '9' && ( p[2] == '\000' | (p[2] >= '0' && p[2] <= '9') ) )
	{
		ret = atoi ( (char*)((unsigned long)p + 1) );
		if ( ret > 15 )
			ret = REG32_UNK;
	}

	return ret;
}


unsigned long val ( char *p )
{
	unsigned long ret = 0;
	int val;

	if ( !p )
		return 0;

	if ( p[0] == '#' || p[0] == '.' )
	{
		p++;
		if ( sscanf (  p, "%i", &val ) != 1 )
			return 0;
		ret = val;
//		ret = atoi( p );
	}
	else 
	{
		_httoi( p, &val );
		ret = val;
	}

	return ret;
}

int set_endianess ( struct code_data *c, int length )
{
	int i = 0;
	unsigned char buf[4];


	if ( c->endianess == ENDIAN_LE )
	{
		memcpy ( buf, &c->data[c->bytes], length );

		for ( i=0; i<length; i++ )
			c->data[c->bytes+i] = buf[length-i-1];
	}
	if ( c->endianess == ENDIAN_BE )
	{
		// leave it as it is ;)
	}

	return 0;
}

int comp_type1 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rs   = 0;
	unsigned int   ri   = 0;
	unsigned short word = 0;

	ALIGN_2;


	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG || 
		 pt ( p3 ) != PARM_VAL  )
    {
	    free ( p1 );
	    free ( p2 );
	    free ( p3 );
		return -E_WRONG_PARM;
    }
	rd  = reg ( p1 ) & 0x07;
	rs  = reg ( p2 ) & 0x07;
	ri  = val ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

    if ( (ri & 0x1F) != ri )
		return -E_INV_VAL;

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE1 | type | (rs<<TYPE1_S_O) | (ri<<TYPE1_I_O) | rd;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );
	c->bytes += 2;

	return 2;
}

int comp_type2 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rs   = 0;
	unsigned int   ro   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG || 
		 (pt ( p3 ) != PARM_VAL && pt ( p3 ) != PARM_REG ) )
    {
	    free ( p1 );
	    free ( p2 );
	    free ( p3 );
        return -E_WRONG_PARM;
    }

	rd  = reg ( p1 );
	rs  = reg ( p2 );
	if ( pt ( p3 ) == PARM_VAL )
		ro = val ( p3 );
	else
		ro = reg ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

    if ( (ro & 0x07) != ro )
		return -E_INV_VAL;


	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE2 | type | (rs<<TYPE2_S_O) | (ro<<TYPE2_O_O) | rd;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type3 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	unsigned long  v   = 0;
	unsigned int   r   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_VAL )
    {
	    free ( p1 );
	    free ( p2 );
        return -E_WRONG_PARM;
    }

	r   = reg ( p1 );
	v   = val ( p2 );

	free ( p1 );
	free ( p2 );

	c->data = realloc ( c->data, c->bytes + 200 );

    if ( (v & 0xFF) != v )
		return -E_INV_VAL;

	word = TYPE3 | type | (r<<TYPE3_R_O) | (unsigned short)v;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type4 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	unsigned int   r1  = 0;
	unsigned int   r2  = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG )
    {
        free ( p1 );
        free ( p2 );
        return -E_WRONG_PARM;
    }

	r1  = reg ( p1 );
	r2  = reg ( p2 );

	free ( p1 );
	free ( p2 );

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE4 | type | (r2<<TYPE4_S_O) | r1;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

// High Register Branch/Exchange

int comp_type5 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	unsigned int   r1  = 0;
	unsigned int   r2  = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
/*
	if ( pt ( p1 ) != PARM_REG || pt ( p2 ) != PARM_REG )
    {
        free ( p1 );
        free ( p2 );
        return -E_WRONG_PARM;
    }
*/
	r1  = reg ( p1 );
	r2  = reg ( p2 );

	if ( type == TYPE5_BXR || type == TYPE5_BXH ||
        type == TYPE5_BLXR || type == TYPE5_BLXH )
	{
		//  target address is in Rs
		r2 = r1;
		r1 = 0;
	}

	free ( p1 );
	free ( p2 );

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE5 | type | (r2<<TYPE5_S_O) | r1;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

//
// PC relative load
//
//  LDR R0, label1
//
int comp_type6 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	unsigned int   rd   = 0;
	unsigned long  pos  = 0;
	unsigned short v    = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));

    if ( pt ( p1 ) != PARM_REG )
    {
	    free ( p1 );
	    free ( p2 );
		return -E_INV_REG;
    }

	rd  = reg ( p1 );
	pos = get_label ( c, p2 );
	free ( p1 );

	if ( pos == C_ERR )
	{
		pos = get_import ( c, p2 + 1 );
		free ( p2 );
		if ( pos == C_ERR )
			return -E_WRONG_PARM;
		pos -= c->start;
	}
	else
		free ( p2 );

	if ( (c->start + pos) & 3 )
		return -E_WRONG_DALIGN;

	if ( c->bytes & 1 )
		pos--;

	v = (unsigned int)( (c->start + pos ) / 4 ) - ( ( c->start + c->bytes ) / 4 ) - 1;

    if ( (v & 0xFF) != v )
		return -E_INV_VAL;

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE6 | (rd<<TYPE6_D_O) | v;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type7 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rb   = 0;
	unsigned int   ro   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG || 
		 pt ( p3 ) != PARM_REG  )
    {
        free ( p1 );
        free ( p2 );
        free ( p3 );
		return -E_INV_REG;
    }

	rd  = reg ( p1 );
	rb  = reg ( p2 );
	ro  = reg ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE7 | type | (rb<<TYPE7_B_O) | (ro<<TYPE7_O_O) | rd;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type8 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rb   = 0;
	unsigned int   ro   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG || 
		 pt ( p3 ) != PARM_REG  )
    {
        free ( p1 );
        free ( p2 );
        free ( p3 );
		return -E_INV_REG;
    }

	rd  = reg ( p1 );
	rb  = reg ( p2 );
	ro  = reg ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE8 | type | (rb<<TYPE8_B_O) | (ro<<TYPE8_O_O) | rd;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type9 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rb   = 0;
	unsigned int   o    = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG || 
		 pt ( p3 ) != PARM_VAL  )
    {
	    free ( p1 );
	    free ( p2 );
	    free ( p3 );
		return -E_INV_REG;
    }

	rd  = reg ( p1 );
	rb  = reg ( p2 );
	o   = val ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

	if ( type == TYPE9_STR || type == TYPE9_LDR )
	{
		if ( (c->start + o) & 3 )
			return -E_INV_VAL_ALIGN;

		o = o>>2;
	}
    if ( (o & 0x1F) != o )
		return -E_INV_VAL;

    c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE9 | type | (rb<<TYPE9_B_O) | (o<<TYPE9_O_O)  | rd;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type10 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rb   = 0;
	unsigned int   o    = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG || 
		 pt ( p3 ) != PARM_VAL  )
    {
	    free ( p1 );
	    free ( p2 );
	    free ( p3 );
		return -E_INV_REG;
    }

	rd  = reg ( p1 );
	rb  = reg ( p2 );
	o   = val ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

	if ( (c->start + o) & 1 )
		return -E_INV_VAL_ALIGN;

	o = o>>1;
    if ( (o & 0x1F) != o )
		return -E_INV_VAL;

    c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE10 | type | (rb<<TYPE10_B_O) | (o<<TYPE10_O_O)  | rd;
	c->data[c->bytes]   = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type11 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rb   = 0;
	unsigned int   o    = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG_HI || 
		 pt ( p3 ) != PARM_VAL  )
    {
	    free ( p1 );
	    free ( p2 );
	    free ( p3 );
		return -E_INV_REG;
    }

	rd  = reg ( p1 );
	rb  = reg ( p2 );
	o   = val ( p3 );

	free ( p1 );
	free ( p2 );
	free ( p3 );

	if ( rb != REG_R13 )
		return -E_INV_REG;

	if ( (c->start + o) & 3 )
		return -E_INV_VAL_ALIGN;

	o = o>>2;
    if ( (o & 0xFF) != o )
		return -E_INV_VAL;

    c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE11 | type | o | rd << TYPE11_D_O;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type12 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned long  v   = 0;
	unsigned int   r   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));
	p3 = strdup(getparm ( t, 3 ));

	if ( pt ( p1 ) != PARM_REG || pt ( p2 ) != PARM_REG_HI || pt ( p3 ) != PARM_VAL )
    {
	    free ( p1 );
	    free ( p2 );
	    free ( p3 );
        return -E_WRONG_PARM;
    }

	r   = reg ( p1 );
	v   = val ( p3 );

	free ( p1 );
    free ( p2 );
	free ( p3 );

	c->data = realloc ( c->data, c->bytes + 200 );

	if ( (c->start + v) & 3 )
		return -E_INV_VAL_ALIGN;

	v = v >> 2;

    if ( (v & 0xFF) != v )
		return -E_INV_VAL;

	word = TYPE12 | type | (r<<TYPE12_R_O) | (unsigned short)v;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type12_mark ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	char *p2 = NULL;
	unsigned long  v   = 0;
	unsigned int   r   = 0;
	unsigned short word = 0;
	unsigned long  pos = 0;
    int rel = 0;

	ALIGN_2

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));

	if ( pt ( p1 ) != PARM_REG )
    {
	    free ( p1 );
	    free ( p2 );
        return -E_WRONG_PARM;
    }

	r   = reg ( p1 );
	pos = get_label ( c, p2 );
	free ( p1 );

	if ( pos == C_ERR )
	{
		pos = get_import ( c, p2 + 1 );
		free ( p2 );
		if ( pos == C_ERR )
			return -E_WRONG_PARM;
		pos -= c->start;
	}
	else
		free ( p2 );


	// dword (32 bit) alignment of the target data
	if ( (c->start + pos) & 3 )
		return -E_INV_VAL_ALIGN;

	// PC is always up to 4 bytes ahead the current opcode (prefetch) 
	// and data must be BEHIND the instruction
	if ( (pos >> 2) <= ((unsigned int)c->bytes >> 2) )
		return -E_SHORT_DISTANCE;

	// distance is maximal 0x0100 dwords (be aware of the already advanced PC)
	if ( ((c->start + pos) >> 2) - ((c->start + c->bytes) >> 2) > 0x0100 )
		return -E_LONG_DISTANCE;

	// minus 1 because of the advanced PC
	rel = ((int)((c->start + pos) >> 2) - (int)((c->start + c->bytes) >> 2)) - 1;
	v = (unsigned int)rel;

	c->data = realloc ( c->data, c->bytes + 200 );

    if ( (v & 0xFF) != v )
		return -E_INV_VAL;

	word = TYPE12 | type | (r<<TYPE12_R_O) | (unsigned short)v;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type13 ( struct code_data *c, char *t, unsigned short type )
{
	char *p1 = NULL;
	unsigned long  v   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 2 ));

	if ( pt ( p1 ) != PARM_VAL )
    {
	    free ( p1 );
        return -E_WRONG_PARM;
    }

	v   = val ( p1 );

	if ( (c->start + v) & 3 )
		return -E_INV_VAL_ALIGN;

	v = v >> 2;

	free ( p1 );

	c->data = realloc ( c->data, c->bytes + 200 );

    if ( (v & 0x7F) != v )
		return -E_INV_VAL;

	word = TYPE13 | type | (unsigned short) v;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}




//
// PUSH/POP Registers
//
//  PUSH { R0, LR }
//
int comp_type14 ( struct code_data *c, char *t, unsigned short type )
{
	char *p  = NULL;
	unsigned short word = 0;
	unsigned int   i = 1;
	unsigned int   regs = 0;

	ALIGN_2;

	word = TYPE14 | type;

	regs = getparms ( t );

	while ( i <= regs )
	{
		p = getparm ( t, i );
		if ( p )
		{
			if ( pt ( p ) == PARM_REG )
			{
				switch ( reg ( p ) )
				{
					case REG_R0:
						word |= TYPE14_R0;
						break;
					case REG_R1:
						word |= TYPE14_R1;
						break;
					case REG_R2:
						word |= TYPE14_R2;
						break;
					case REG_R3:
						word |= TYPE14_R3;
						break;
					case REG_R4:
						word |= TYPE14_R4;
						break;
					case REG_R5:
						word |= TYPE14_R5;
						break;
					case REG_R6:
						word |= TYPE14_R6;
						break;
					case REG_R7:
						word |= TYPE14_R7;
						break;
					default:
						break;
				}
			}
			else if ( pt ( p ) == PARM_REG_HI )
			{
				switch ( reg ( p ) )
				{
					case REG_R14:
						if ( type != TYPE14_PUSH )
							add_warning ( c, "Using POP with LR, thats not correct. Use PC instead.", c->current_line );
						word |= TYPE14_LR;
						break;
					case REG_R15:
						if ( type != TYPE14_POP )
							add_warning ( c, "Using PUSH with PC, thats not correct. Use LR instead.", c->current_line );
						word |= TYPE14_PC;
						break;
					default:
						return -E_INV_REG;
						break;
				}
			}
			else
				return -E_UNK_REG;

			i++;
		}
	} 


	c->data = realloc ( c->data, c->bytes + 200 );

	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

// STMIA/LDMIA
int comp_type15 ( struct code_data *c, char *t, unsigned short type )
{
	char *p  = NULL;
	unsigned short word = 0;
	unsigned int   i = 1;

	ALIGN_2;

	word = TYPE15 | type;
	p = getparm ( t, i++ );
	if ( pt ( p ) != PARM_REG )
		return -E_WRONG_PARM;
	word |= reg ( p ) << TYPE15_R_O;

	do 
	{
		p = getparm ( t, i );
		if ( pt ( p ) == PARM_REG )
		{
			switch ( reg ( p ) )
			{
				case REG_R0:
					word |= TYPE15_R0;
					break;
				case REG_R1:
					word |= TYPE15_R1;
					break;
				case REG_R2:
					word |= TYPE15_R2;
					break;
				case REG_R3:
					word |= TYPE15_R3;
					break;
				case REG_R4:
					word |= TYPE15_R4;
					break;
				case REG_R5:
					word |= TYPE15_R5;
					break;
				case REG_R6:
					word |= TYPE15_R6;
					break;
				case REG_R7:
					word |= TYPE15_R7;
					break;
				default:
					return -E_INV_REG;
					break;
			}
		}
		else
			return -E_UNK_REG;

		i++;
	} while (reg ( p ) != REG_UNK );


	c->data = realloc ( c->data, c->bytes + 200 );

	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

//
// Conditional branch
//
//   BEQ @label1
//
int comp_type16 ( struct code_data *c, char *t, unsigned short type )
{
	char *p  = NULL;
	unsigned long  pos = 0;
	unsigned short v   = 0;
	unsigned short word = 0;
    int rel = 0;

	ALIGN_2;

	p = getparm ( t, 1 );

	pos = get_label ( c, p );
	if ( pos == C_ERR )
	{
		pos = get_import ( c, p + 1 );
		if ( pos == C_ERR )
			return -E_WRONG_PARM;
		pos -= c->start;
	}
	

	rel = ((int)pos - (int)c->bytes - 4) / 2;
	v = (unsigned int)rel;

    if ( 
        (
          rel > 0 && 
          ((v & 0x7F) != v)
        )||
        (
          rel < 0 && 
          (((v ^ 0xFFFF) & 0x7F) != (v ^ 0xFFFF) )
        )
       )
		return -E_INV_VAL;

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE16 | type | (v & 0xFF);
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int comp_type17 ( struct code_data *c, char *t, unsigned short type )
{
	char *p  = NULL;
	unsigned long  pos = 0;
	unsigned short v   = 0;
	unsigned short word = 0;
    int rel = 0;

	ALIGN_2;

	p = getparm ( t, 1 );

	pos = val ( p );

    if ( pos == C_ERR )
		return -E_WRONG_PARM;


    if ( pos > 0xFF )
		return -E_INV_VAL;

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE17 | type | (unsigned char)(pos & 0xFF);
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}


int comp_type18 ( struct code_data *c, char *t )
{
	char *p  = NULL;
	unsigned long  pos = 0;
	unsigned short v   = 0;
	unsigned short word = 0;
    int rel = 0;

	ALIGN_2;

	p = getparm ( t, 1 );
	pos = get_label ( c, p );

	if ( pos == C_ERR )
	{
		pos = get_import ( c, p + 1 );
		if ( pos == C_ERR )
			return -E_WRONG_PARM;
		pos -= c->start;
	}

    rel = ((int)pos - (int)c->bytes - 4) / 2;
	v = (unsigned int)rel;

    if ( 
        (
          rel > 0 && 
          ((v & 0x7FF) != v)
        )||
        (
          rel < 0 && 
          (((v ^ 0xFFFF) & 0x7FF) != (v ^ 0xFFFF) )
        )
       )
		return -E_INV_VAL;

	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE18 | (v & 0x7FF);
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;
}

int   comp_inj     ( struct code_data *c, char *t, int cmd )
{
	unsigned long addr = 0;
	int val;

	_httoi (getparm ( t, 1 ), &val);
	addr       = val;
	c->inj_pos = addr;
	c->inject  = 1;

	return 0;
}

int   comp_nam     ( struct code_data *c, char *t, int cmd )
{
	if ( c->name )
		free ( c->name );

//	if ( !strcmp (c->name, "func_new" ) )
		c->name    = strlwr(strdup ( t ));
//	else
//	{
//		c->end = 1;
//		return -1024;
//	}

	return 0;
}

int   comp_end     ( struct code_data *c, char *t, int cmd )
{
	c->end = 1;
	return 0;
}

int   comp_uni     ( struct code_data *c, char *t, int cmd )
{
	int i   = 0;
	int pos = 0;
	int len = 0;
	char *text = NULL;
	char *uni  = NULL;

	if ( !getparms ( t ) )
		return -E_WRONG_PARM;

	len  = strlen ( getparm ( t, 1 )) * 2;
	i = 2;
	while ( getparm ( t, i++ ) != PARM_UNK )
		len++;

	if ( cmd == CMD_PRECOMPILE )
		return len;

	text = malloc (   len + 10 );
	uni  = malloc (   len + 10 );
	strcpy ( text, getparm ( t, 1 ) );

	for ( i=0; i<strlen ( getparm ( t, 1 )); i++ )
	{
      if ( c->endianess ) // LE
      {
		   uni[2*i]   = text[i];
		   uni[2*i+1] = '\000';
      }
      else
      {
		   uni[2*i]   = '\000';
		   uni[2*i+1] = text[i];
      }
	}

	c->data = realloc ( c->data, c->bytes + len + 200 );
	
	for ( i=0; i<2*strlen ( getparm ( t, 1 )); i++ )
	{
		c->data[c->bytes] = uni[i];
		c->bytes++;
	}
	pos = 2;
	while ( i++ < len )
	{
		if ( pt ( getparm ( t, pos ) ) != PARM_VAL )
		{
			free ( text );
			return -E_INV_VAL;
		}
		c->data[c->bytes] = val(getparm ( t, pos ));
		c->bytes++;
		pos++;
	}
/*
	if ( len & 0x03 )
		add_warning ( c, "\r\n      IMPORTANT: \r\n      You are trying to place data at a NON-ALIGNED position.\r\n      This causes unpredictable behaviour when using ADR and LDR opcodes.\r\n      Please use DWORD (32-bit) aligned addresses!!\r\n      (if you're just patching some bytes, it should work okay)", c->current_line );
*/
	free ( text );
	free ( uni );
	return 0;
}

int   comp_asc     ( struct code_data *c, char *t, int cmd )
{
	int i   = 0;
	int pos = 0;
	int len = 0;
	char *text = NULL;

	if ( !getparms ( t ) )
		return -E_WRONG_PARM;

	len  = strlen ( getparm ( t, 1 ));
	i = 2;
	while ( getparm ( t, i++ ) != PARM_UNK )
		len++;

	if ( cmd == CMD_PRECOMPILE )
		return len;

	text = malloc ( len + 10 );
	strcpy ( text, getparm ( t, 1 ) );


	c->data = realloc ( c->data, c->bytes + len + 200 );
	
	for ( i=0; i<strlen ( getparm ( t, 1 )); i++ )
	{
		c->data[c->bytes] = text[i];
		c->bytes++;
	}
	pos = 2;
	while ( i++ < len )
	{
		if ( pt ( getparm ( t, pos ) ) != PARM_VAL )
		{
			free ( text );
			return -E_INV_VAL;
		}
		c->data[c->bytes] = val(getparm ( t, pos ));
		c->bytes++;
		pos++;
	}
/*
	if ( len & 0x03 )
		add_warning ( c, "\r\n      IMPORTANT: \r\n      You are trying to place data at a NON-ALIGNED position.\r\n      This causes unpredictable behaviour when using ADR and LDR opcodes.\r\n      Please use DWORD (32-bit) aligned addresses!!\r\n      (if you're just patching some bytes, it should work okay)", c->current_line );
*/

	free ( text );
	return len;
}


int   comp_des     ( struct code_data *c, char *t, int cmd )
{
	if ( c->description )
		free ( c->description );
	c->description    = strlwr( strdup ( t ) );

	return 0;
}


int   comp_ens     ( struct code_data *c, char *t, int cmd )
{
	char *text = strdup ( t );

	string_uppercase ( text );
	c->endianess = -1;
	if ( !strcmp ( text, "BIG_ENDIAN" ) )
		c->endianess = ENDIAN_BE;
	if ( !strcmp ( text, "LITTLE_ENDIAN" ) )
		c->endianess = ENDIAN_LE;
	if ( !strcmp ( text, "BE" ) )
		c->endianess = ENDIAN_BE;
	if ( !strcmp ( text, "LE" ) )
		c->endianess = ENDIAN_LE;
	free ( text );

	if ( c->endianess == -1 )
	{
		add_warning ( c, "\r\n      NOTE: \r\n      Invalid endianess, defaulting to BIG_ENDIAN\r\n", c->current_line );
		c->endianess = ENDIAN_BE;
	}
	return 0;
}

int   comp_dmp     ( struct code_data *c, char *t, int cmd )
{
	c->dump           = 1;

	return 0;
}


int   comp_imp     ( struct code_data *c, char *t, int cmd )
{
	char *        func = NULL;
	unsigned long addr = 0;
	int           val  = 0;

	func     = strdup (getparm ( t, 1 ));
	_httoi (getparm ( t, 2 ), &val);
	addr     = val;
	add_import ( c, func, addr );
	free ( func );
	return 0;
}

int   comp_idd     ( struct code_data *c, char *t, int cmd )
{
	char *        name = NULL;
	unsigned long addr = 0;
	int           abs  = 0;
	int           val  = 0;


	name     = strdup (getparm ( t, 1 ));
	_httoi (getparm ( t, 2 ), &val);
	abs      = val;
	add_dpatch ( c, name, c->bytes, abs );
	free ( name );
	return 0;
}
/*
int   comp_lnk     ( struct code_data *c, char *t, int cmd )
{
	char *        func = NULL;
	unsigned long addr = 0;

	func     = strdup (getparm ( t, 1 ));

	return 0;
}
*/

int   comp_code     ( struct code_data *c, char *t, int cmd )
{
	char *addr = NULL;
	int   i = 0;
	int   val = 0;
	int   len = 0;
	int   bitcount = 0;

	if ( getparms ( t ) != 1 )
		return -E_UNK_OPCODE;

	addr     = getparm ( t, 1 );
	val      = atoi ( addr );

	switch ( val )
	{
		case 16:
			c->arch_mode = MODE_THUMB;
			break;

		case 32:
			c->arch_mode = MODE_ARM32;
			break;

		default:
			add_error ( c, "Invalid mode. Use '.code 16' or '.code 32'.", c->current_line );
			return -E_UNK_OPCODE;
			break;
	}

	return 0;
}


int   comp_align     ( struct code_data *c, char *t, int cmd )
{
	char *addr = NULL;
	int   i = 0;
	int   val = 0;
	int   len = 0;
	int   bitcount = 0;

	if ( getparms ( t ) != 1 )
		return -E_UNK_OPCODE;

	addr     = getparm ( t, 1 );
	_httoi ( addr, &val );

	while ( val )
	{
		if ( val & 1 )
			bitcount++;
		val >>= 1;
	}
	
	_httoi ( addr, &val );
	if ( bitcount != 1 || val < 2 )
	{
		add_error ( c, "You try to align to an invalid boundary", c->current_line );
		return -E_UNK_OPCODE;
	}


	// get the length
	len = val - ((c->start + c->bytepos ) & (val-1));

	if ( val == len )
		len = 0;

	if ( cmd == CMD_PRECOMPILE )
		return len;


	// insert alignment data
	c->data = realloc ( c->data, c->bytes + len );
	
	i = len;
	while ( i-- )
	{
		c->data[c->bytes] = 0x00;
		c->bytes++;
	}

	return len;
}

int   comp_size     ( struct code_data *c, char *t, int cmd )
{
	char *addr = NULL;
	int   val = 0;

	addr     = getparm ( t, 1 );
	_httoi ( addr, &val );
	c->maxsize = val;


	return 0;
}


int   comp_org     ( struct code_data *c, char *t, int cmd )
{
	char *addr = NULL;
	int   val = 0;

	addr     = getparm ( t, 1 );
	_httoi ( addr, &val );
	c->start = val;

	add_import_global ( c->name, c->start );

	if ( val & 0x03 && cmd == CMD_PRECOMPILE  )
		add_warning ( c, "\r\n      IMPORTANT: \r\n      You are trying to place data at a NON-ALIGNED position.\r\n      This causes unpredictable behaviour when using ADR and LDR opcodes.\r\n      Please use DWORD (32-bit) aligned addresses!!\r\n      (if you're just patching some bytes, it should work okay)", c->current_line );

	return 0;
}

int   comp_hex     ( struct code_data *c, char *t, int cmd )
{
	c->mode = MODE_HEX;
	return 0;
}

int   comp_trix     ( struct code_data *c, char *t, int cmd )
{
	c->mode = MODE_TRIX;
	return 0;
}

int   comp_nrx     ( struct code_data *c, char *t, int cmd )
{
	c->mode = MODE_NRX;
	return 0;
}

int   comp_gsc     ( struct code_data *c, char *t, int cmd )
{
	c->mode = MODE_GSC;
	return 0;
}

int   comp_loc     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;
	
	if ( !c->located )
	{
		c->located = 1;
		asm_locate_functions ( c );
	}

	return ret;
}



int   comp_mov     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type3 ( c, t, TYPE3_MOV );

	return ret;
}

int   comp_add     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type3 ( c, t, TYPE3_ADD );

	return ret;
}

int   comp_cmp     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type3 ( c, t, TYPE3_CMP );

	return ret;
}

int   comp_sub     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type3 ( c, t, TYPE3_SUB );

	return ret;
}


int   comp_beq     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BEQ );

	return ret;
}

int   comp_bne     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BNE );

	return ret;
}

int   comp_bcs     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BCS );

	return ret;
}

int   comp_bcc     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BCC );

	return ret;
}

int   comp_bmi     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BMI );

	return ret;
}

int   comp_bpl     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BPL );

	return ret;
}

int   comp_bvs     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BVS );

	return ret;
}

int   comp_bvc     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BVC );

	return ret;
}

int   comp_bhi     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BHI );

	return ret;
}

int   comp_bls     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BLS );

	return ret;
}

int   comp_bge     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BGE );

	return ret;
}

int   comp_blt     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BLT );

	return ret;
}

int   comp_bgt     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BGT );

	return ret;
}

int   comp_ble     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type16 ( c, t, TYPE16_BLE );

	return ret;
}


int   comp_b       ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type18 ( c, t );

	return ret;
}
int   comp_blx     ( struct code_data *c, char *t, int cmd )
{
	return comp_bl_blx (c, t, cmd, TYPE19_BLX);
}
int   comp_bl      ( struct code_data *c, char *t, int cmd )
{
	return comp_bl_blx (c, t, cmd, TYPE19_BL);
}

int   comp_bl_blx  ( struct code_data *c, char *t, int cmd, int type )
{
	unsigned long src = 0;
	unsigned long dst = 0;
	unsigned short word = 0;
	char *addr = NULL;
	int val = 0;
	char *tmp = NULL;
	int ret_code = 0;
	int resolved = 0;

	c->data = realloc ( c->data, c->bytes + 200 + 200 );
	addr = getparm ( t, 1 );

	if ( addr[0] == '@' )
	{
		dst = get_label ( c, addr );
		if ( dst != C_ERR )
		{
			asm_inject_bl(c, c->bytes, dst, type);  
		}
		else
		{
			dst = get_import ( c, addr + 1 );
			if ( dst != C_ERR )  // if not found, use import name which is absolute
			{
				src = c->bytes + c->start;
				ret_code = asm_inject_bl(c, src, dst, type);  
			}
			else // not found, add to BL list
			{
				add_func ( c, addr, c->bytes );
				ret_code = asm_inject_bl(c, src, src + 4, type);  
			}
		}
		resolved = 1;
	}
	else if ( addr[0] == '$' )    // direct address jump
	{
		src = c->bytes + c->start;
		_httoi ( addr + 1, &val );
		dst = val;
		ret_code = asm_inject_bl(c, src, dst, type);  
		resolved = 1;
	}
	else if ( addr[0] == '_' )    // just use import name which is absolute
	{
		src = c->bytes + c->start;
		dst = get_import ( c, addr + 1 );
		if ( dst != C_ERR )
		{
			ret_code = asm_inject_bl(c, src, dst, type); 
			resolved = 1;
		}
	}

	if ( !resolved )
	{
		tmp = malloc ( strlen ( addr ) + 200 );
		sprintf ( tmp, "The symbol '%s' could not be resolved\r\n", addr );
		add_error ( c, tmp, c->current_line );
		free ( tmp );
		return -E_WRONG_PARM;
	}
	

	return ret_code;
}

int asm_inject_bl ( struct code_data *c, unsigned int src, unsigned int jmp_addr, int type )
{
	int delta = (int)jmp_addr - (int)(src + 4);
	unsigned int word = 0;

	if(delta > 0x400000 || delta < -0x400000)
	{
		add_error ( c, "BL destination is out of range (ARM THUMB BL does not allow more than 0x400000 byte range)", c->current_line );
		return -E_WRONG_PARM;
	}
	
	word = (type | TYPE19_HI);
	word |= (delta >> 12) & 0x7FF;

	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );
	c->bytes += 2;
		
	word = (type | TYPE19_LO);
	word |= (delta >> 1) & 0x7FF;

	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );
	c->bytes += 2;
}

int   comp_subr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type2 ( c, t, TYPE2_SUBR );

	return ret;
}

int   comp_addr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type2 ( c, t, TYPE2_ADDR );

	return ret;
}

int   comp_subv     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type2 ( c, t, TYPE2_SUBV );

	return ret;
}

int   comp_addv     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type2 ( c, t, TYPE2_ADDV );

	return ret;
}

int   comp_lsl     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type1 ( c, t, TYPE1_LSL );

	return ret;
}

int   comp_lsr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type1 ( c, t, TYPE1_LSR );

	return ret;
}
int   comp_asr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type1 ( c, t, TYPE1_ASR );

	return ret;
}


int   comp_nop     ( struct code_data *c, char *t, int cmd )
{
	unsigned short word = 0;

	c->data = realloc ( c->data, c->bytes + 200);

	word = TYPE_NOP;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;
	return 2;
}

int   comp_and     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_AND );

	return ret;
}
int   comp_eor     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_EOR );

	return ret;
}
int   comp_lslr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_LSL );

	return ret;
}
int   comp_lsrr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_LSR );

	return ret;
}
int   comp_asrr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_ASR );

	return ret;
}
int   comp_adc     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_ADC );

	return ret;
}
int   comp_sbc     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_SBC );

	return ret;
}
int   comp_ror     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_ROR );

	return ret;
}
int   comp_tst     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_NEG );

	return ret;
}
int   comp_neg     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_NEG );

	return ret;
}
int   comp_cmpr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_CMP );

	return ret;
}
int   comp_cmn     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_CMN );

	return ret;
}
int   comp_orr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_ORR );

	return ret;
}
int   comp_mul     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_MUL );

	return ret;
}
int   comp_bic     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_BIC );

	return ret;
}
int   comp_mvn     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type4 ( c, t, TYPE4_MVN );

	return ret;
}

int   comp_ldrd    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type6( c, t, 0 );

	return ret;
}


int   comp_ldr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type7( c, t, TYPE7_LDR );

	return ret;
}

int   comp_str     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type7 ( c, t, TYPE7_STR );

	return ret;
}

int   comp_ldrb    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type7 ( c, t, TYPE7_LDRB );

	return ret;
}

int   comp_strb    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type7 ( c, t, TYPE7_STRB );

	return ret;
}

int   comp_ldrh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type8 ( c, t, TYPE8_LDRH );

	return ret;
}

int   comp_ldsb    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type8 ( c, t, TYPE8_LDSB );

	return ret;
}

int   comp_ldsh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type8 ( c, t, TYPE8_LDSH );

	return ret;
}

int   comp_strh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type8 ( c, t, TYPE8_STRH );

	return ret;
}


int   comp_ldrr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type9 ( c, t, TYPE9_LDR );

	return ret;
}

int   comp_strr     ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type9 ( c, t, TYPE9_STR );

	return ret;
}

int   comp_ldrbr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type9 ( c, t, TYPE9_LDRB );

	return ret;
}

int   comp_strbr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type9 ( c, t, TYPE9_STRB );

	return ret;
}

int   comp_ldrhr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type10 ( c, t, TYPE10_LDRH );

	return ret;
}

int   comp_strhr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type10 ( c, t, TYPE10_STRH );

	return ret;
}


int   comp_push    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type14 ( c, t, TYPE14_PUSH );

	return ret;
}

int   comp_pop    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type14 ( c, t, TYPE14_POP );

	return ret;
}




int   comp_db      ( struct code_data *c, char *t, int cmd )
{
	int pos = 0;
	int count = 0;
	char *p = NULL;
	unsigned char  r    = '\000';

	count = getparms ( t );

	if ( cmd == CMD_PRECOMPILE )
		return count;

	while ( ++pos <= count )
	{
		p = getparm ( t, pos );

		if ( pt ( p ) != PARM_VAL )
			return -E_WRONG_PARM;

		r   = (unsigned char) (val ( p ) & 0xFF);

		c->data = realloc ( c->data, c->bytes + 200 );
		c->data[c->bytes] = r;
		c->bytes += 1;
	}
	pos--;
	if ( pos > 1 && pos & 1 )
		add_warning ( c, "You are sure you want misaligned code here? add a 00 to the db's ;)", c->current_line );

	return pos;
}

int   comp_dw      ( struct code_data *c, char *t, int cmd )
{
	int pos = 0;
	int count = 0;
	char *p = NULL;
	unsigned short  r    = 0;

	ALIGN_2;

	count = getparms ( t );

	if ( cmd == CMD_PRECOMPILE )
		return count*2;

	while ( ++pos <= count )
	{
		p = getparm ( t, pos );

		if ( pt ( p ) != PARM_VAL )
			return -E_WRONG_PARM;

		r   = (unsigned short)(val ( p ) & 0xFFFF);


		c->data = realloc ( c->data, c->bytes + 200 );
		c->data[c->bytes]   = (r >> 8 ) & 0xFF;
		c->data[c->bytes+1] = r & 0xFF;

		set_endianess ( c, 2 );

		c->bytes += 2;
	}
	pos--;

	return pos*2;
}

int   comp_dd      ( struct code_data *c, char *t, int cmd )
{
	int pos = 0;
	int count = 0;
	char *p = NULL;
	unsigned int  r    = 0;

	ALIGN_4;

	count = getparms ( t );

	if ( cmd == CMD_PRECOMPILE )
		return count*4;

	while ( ++pos <= count )
	{
		p = getparm ( t, pos );

		if ( pt ( p ) != PARM_VAL )
			return -E_WRONG_PARM;

		r   = val ( p ) & 0xFFFFFFFF;


		c->data = realloc ( c->data, c->bytes + 200 );

		c->data[c->bytes]   = (r >> 24) & 0xFF;
		c->data[c->bytes+1] = (r >> 16) & 0xFF;
		c->data[c->bytes+2] = (r >> 8 ) & 0xFF;
		c->data[c->bytes+3] = r & 0xFF;

		set_endianess ( c, 4 );

		c->bytes += 4;
	}
	pos--;

	return pos*4;
}


int   comp_addrh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_ADDRH );

	return ret;
}
int   comp_addhr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_ADDHR );

	return ret;
}
int   comp_addhh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_ADDHH );

	return ret;
}
int   comp_addrr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_ADDRR );

	return ret;
}
int   comp_cmprh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_CMPRH );

	return ret;
}
int   comp_cmphr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_CMPHR );

	return ret;
}
int   comp_cmphh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_CMPHH );

	return ret;
}
int   comp_movrh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_MOVRH );

	return ret;
}
int   comp_movhr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_MOVHR );

	return ret;
}
int   comp_movhh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_MOVHH );

	return ret;
}
int   comp_movrr    ( struct code_data *c, char *t, int cmd )
{
/*	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_MOVRR );

	return ret;*/
	unsigned short type = TYPE2_ADDV;
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	unsigned int   rd   = 0;
	unsigned int   rs   = 0;
	unsigned int   ro   = 0;
	unsigned short word = 0;

	ALIGN_2;

	p1 = strdup(getparm ( t, 1 ));
	p2 = strdup(getparm ( t, 2 ));

	if ( pt ( p1 ) != PARM_REG || 
		 pt ( p2 ) != PARM_REG  )
    {
	    free ( p1 );
	    free ( p2 );
        return -E_WRONG_PARM;
    }

	rd  = reg ( p1 );
	rs  = reg ( p2 );
	ro  = 0;

	free ( p1 );
	free ( p2 );

    if ( (ro & 0x07) != ro )
		return -E_INV_VAL;


	c->data = realloc ( c->data, c->bytes + 200 );

	word = TYPE2 | type | (rs<<TYPE2_S_O) | (ro<<TYPE2_O_O) | rd;
	c->data[c->bytes] = (word & 0xFF00) >> 8;
	c->data[c->bytes+1] = (word & 0x00FF);

	set_endianess ( c, 2 );

	c->bytes += 2;

	return 2;

}
int   comp_bxr    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_BXR );

	return ret;
}
int   comp_bxh    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_BXH );

	return ret;
}
int   comp_blxr   ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_BLXR );

	return ret;
}
int   comp_blxh   ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type5 ( c, t, TYPE5_BLXH );

	return ret;
}


int   comp_ldrsp    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type11 ( c, t, TYPE11_LDR );

	return ret;
}

int   comp_strsp    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type11 ( c, t, TYPE11_STR );

	return ret;
}


int   comp_addpcr   ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type12 ( c, t, TYPE12_ADDPC );

	return ret;
}
int   comp_movpcrel   ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type12_mark ( c, t, TYPE12_ADDPC );

	return ret;
}

int   comp_addspr   ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type12 ( c, t, TYPE12_ADDSP );

	return ret;
}

int   comp_addsp    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type13 ( c, t, TYPE13_ADDSP );

	return ret;
}
int   comp_subsp    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type13 ( c, t, TYPE13_SUBSP );

	return ret;
}



int   comp_stmia    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type15 ( c, t, TYPE15_STMIA );

	return ret;
}

int   comp_ldmia    ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type15 ( c, t, TYPE15_LDMIA );

	return ret;
}


int   comp_swi      ( struct code_data *c, char *t, int cmd )
{
	int ret = 0;

	ret = comp_type17 ( c, t, 0 );

	return ret;
}


//
//   ARM32 goes here...
//
//
//
//
//



t_arm32_cond arm32_shift_types[] = {
	{ "LSL", 0x00 },
	{ "LSR", 0x01 },
	{ "ASR", 0x02 },
	{ "ROR", 0x03 },
	{ NULL, 0 }
};


int get_shift_type ( unsigned char *string )
{
	int pos = 0;

	if ( !string || strlen ( string ) == 0 )
		return 0xFE;

	while ( arm32_shift_types[pos].string )
	{
		if ( !strcmp ( string, arm32_shift_types[pos].string ) )
			return arm32_shift_types[pos].code;
		pos++;
	}

	return -E_WRONG_PARM;
}


int get_shift_code ( unsigned char *parms, int parm_num )
{
	unsigned int shift_code = 0;
	unsigned int shift_type = 0;
	char *p1 = NULL;
	char *p2 = NULL;

	p1 = strdup(getparm ( parms, parm_num ));
	shift_type = get_shift_type ( p1 );
	free ( p1 );
	switch ( shift_type )
	{
		case 0:
		case 1:
		case 2:
		case 3:
			shift_code = shift_type << 1;
			p2 = strdup(getparm ( parms, parm_num + 1 ));
			if ( pt32 ( p2 ) == PARM_REG32 )
				shift_code |= reg32 ( p2 ) << 4 | 1;
			else if ( pt32 ( p2 ) == PARM_VAL )
			{
				if ( val ( p2 ) > 31 )
					return -E_WRONG_PARM;

				shift_code |= val ( p2 ) << 3;
			}
			else
				return -E_WRONG_PARM;

			free ( p2 );

			break;

		case 0xFE:
			shift_code = 0;
			break;

		default:
			return -E_WRONG_PARM;
	}

	return shift_code;
}

int get_operand ( unsigned char *parms, int parm_num )
{
	int operand_code = 0;
	int shift_code = 0;
	int shift_type = 0;
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	int rm = 0;

	p1 = strdup(getparm ( parms, parm_num ));
	
	// MOV R0, R1 LSR 2
	// MOV R0, R1 LSR R2
	if ( pt32 ( p1 ) == PARM_REG32  )
	{
		rm = reg32 ( p1 );
		free ( p1 );

		shift_code = get_shift_code ( parms, parm_num+1 );
		if ( shift_code < 0 )
			return shift_code;

		operand_code = (shift_code << 4) | rm | TYPE32_OP_REG;
	}
	else if ( pt32 ( p1 ) == PARM_VAL  )
	{
		int shifts = 0;
		unsigned int value = val ( p1 );

		free ( p1 );

		while ( value > 0xFF && shifts <= 15 )
		{
			ROL(value);
			ROL(value);
			shifts++;
		}

		if ( shifts > 15 )
			return -E_INV_VAL;

		operand_code = (shifts << 8) | value | TYPE32_OP_IMM;
	}
	else
		operand_code = -E_INV_VAL;

	return operand_code;
}



int   comp32_b      ( struct code_data *c, char *t, int cmd )
{
	return comp32_bl_b ( c, t, cmd, 0 );
}

int   comp32_bl      ( struct code_data *c, char *t, int cmd )
{
	return comp32_bl_b ( c, t, cmd, 1 );
}

int   comp32_bl_b      ( struct code_data *c, char *t, int cmd, int link_bit )
{
	unsigned long src = 0;
	unsigned long dst = 0;
	unsigned int dword = TYPE32_B | COND(c);
	char *addr = NULL;
	int val = 0;
	char *tmp = NULL;
	int ok = 0;

	ALIGN_4;

	if ( link_bit )
		dword |= TYPE32_B_L;

	addr = getparm ( t, 1 );

	if ( addr[0] == '@' )
	{
		dst = get_label ( c, addr );
		if ( dst != C_ERR )
			dword |= ((dst-(c->bytes+8))/4) & TYPE32_B_O_MASK;
		else
		{
			dst = get_import ( c, addr + 1 );
			if ( dst != C_ERR )  // if not found, use import name which is absolute
				dword |= (((dst - (c->start + c->bytes + 8) ) /4 ) & TYPE32_B_O_MASK);
			else // not found, add to B list
				add_func ( c, addr, c->bytes );
		}
		ok = 1;
	}
	else if ( addr[0] == '$' )    // direct address jump
	{
		src = c->bytes + c->start;
		_httoi ( addr + 1, &val );
		dst = val;
		dword |= (((dst - (c->start + c->bytes + 8) ) /4 ) & TYPE32_B_O_MASK);
		ok = 1;
	}
	else if ( addr[0] == '_' )    // just use import name which is absolute
	{
		src = c->bytes + c->start;
		dst = get_import ( c, addr + 1 );
		if ( dst != C_ERR )
		{
			dword |= (((dst - (c->start + c->bytes + 8) ) /4 ) & TYPE32_B_O_MASK);
			ok = 1;
		}
	}

	if ( !ok )
	{
		tmp = malloc ( strlen ( addr ) + 200 );
		sprintf ( tmp, "The symbol '%s' could not be resolved\r\n", addr );
		add_error ( c, tmp, c->current_line );
		free ( tmp );
		return -E_WRONG_PARM;
	}
	
	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );
	c->bytes += 4;

	return 4;
}


int   comp32_bx      ( struct code_data *c, char *t, int cmd )
{
	char *p1 = NULL;
	unsigned int   r1  = 0;
	unsigned int dword =  TYPE32_BX | COND(c);

	ALIGN_4;

	p1 = strdup(getparm ( t, 1 ));
	if ( pt32 ( p1 ) != PARM_REG32  )
    {
        free ( p1 );
        return -E_WRONG_PARM;
    }

	r1  = reg32 ( p1 );
	free ( p1 );


	dword |= r1;

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}

int   comp32_mov      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_MOV, 1 );
}


int   comp32_mvn      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_MVN, 1 );
}

int   comp32_cmp      ( struct code_data *c, char *t, int cmd )
{
	c->arm32_flag_s = 1;
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_CMP, 2 );
}

int   comp32_cmn      ( struct code_data *c, char *t, int cmd )
{
	c->arm32_flag_s = 1;
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_CMN, 2 );
}

int   comp32_teq      ( struct code_data *c, char *t, int cmd )
{
	c->arm32_flag_s = 1;
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_TEQ, 2 );
}

int   comp32_tst      ( struct code_data *c, char *t, int cmd )
{
	c->arm32_flag_s = 1;
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_TST, 2 );
}

int   comp32_and      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_AND, 3 );
}

int   comp32_eor      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_EOR, 3 );
}

int   comp32_sub      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_SUB, 3 );
}

int   comp32_rsb      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_RSB, 3 );
}

int   comp32_add      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_ADD, 3 );
}

int   comp32_adc      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_ADC, 3 );
}

int   comp32_sbc      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_SBC, 3 );
}

int   comp32_rsc      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_RSC, 3 );
}

int   comp32_orr      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_ORR, 3 );
}

int   comp32_bic      ( struct code_data *c, char *t, int cmd )
{
	return comp32_dp ( c, t, cmd, TYPE32_DP_OP_BIC, 3 );
}

int   comp32_dp      ( struct code_data *c, char *t, int cmd, int opcode, int operand_type )
{
	char *parm = NULL;
	unsigned int   rd  = 0;
	unsigned int   rn  = 0;
	unsigned int dword = TYPE32_DP | (opcode << TYPE32_DP_OP_OFF) | COND(c) | (c->arm32_flag_s << TYPE32_DP_S_OFF);
	int operand = 0;
	int opnum = 1;

	ALIGN_4;

	// operand type 1: MOV,MVN
	// operand type 3: AND,EOR,SUB,RSB,ADD,ADC,SBC,RSC,ORR,BIC
	if ( operand_type == 1 || operand_type == 3 )
	{
		parm = strdup(getparm ( t, opnum ));
		if ( pt32 ( parm ) != PARM_REG32  )
		{
			free ( parm );
			return -E_WRONG_PARM;
		}

		rd  = reg32 ( parm );
		free ( parm );

		opnum++;
	}

	// operand type 2: CMP,CMN,TEQ,TST
	// operand type 3: AND,EOR,SUB,RSB,ADD,ADC,SBC,RSC,ORR,BIC
	if ( operand_type == 2 || operand_type == 3 )
	{
		parm = strdup(getparm ( t, opnum ));
		if ( pt32 ( parm ) != PARM_REG32  )
		{
			free ( parm );
			return -E_WRONG_PARM;
		}

		rn  = reg32 ( parm );
		free ( parm );

		opnum++;
	}

	operand = get_operand ( t, opnum );
	if ( operand < 0 )
		return operand;

	if ( cmd == CMD_COMPILE )
	{
		if ( operand & TYPE32_OP_IMM )
			dword |= TYPE32_DP_I;

		dword |= (rd << TYPE32_DP_RD_OFF) | (rn << TYPE32_DP_RN_OFF) | (operand & TYPE32_OP_MASK);

		c->data = realloc ( c->data, c->bytes + 200 );
		c->data[c->bytes] = (dword & 0xFF000000) >> 24;
		c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
		c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
		c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

		set_endianess ( c, 4 );

		c->bytes += 4;
	}

	return 4;
}


int   comp32_mrs      ( struct code_data *c, char *t, int cmd )
{
	char *parm = NULL;
	unsigned int   rd  = 0;
	unsigned int dword = TYPE32_MRS | COND(c);

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rd = reg32 ( parm );
	free ( parm );

	parm = strdup(getparm ( t, 2 ));
	if ( pt32 ( parm ) == PARM_REG_SPSR  )
		dword |= TYPE32_MRS_PS;
	else if ( pt32 ( parm ) != PARM_REG_CPSR  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	free ( parm );


	dword |= (rd << TYPE32_MRS_RD_OFF);

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}


// MSR SPSR_flg, 0xd0   --  buggy in IDA?

int   comp32_msr      ( struct code_data *c, char *t, int cmd )
{
	char *parm = NULL;
	int flag_only = 0;
	int operand = 0;
	unsigned int   rm  = 0;
	unsigned int dword = TYPE32_MSR | COND(c);

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) == PARM_REG_CPSR )
	{
		dword |= TYPE32_MSR_DIRECT;
		flag_only = 0;
	}
	else if ( pt32 ( parm ) == PARM_REG_SPSR )
	{
		dword |= TYPE32_MSR_PD | TYPE32_MSR_DIRECT;
		flag_only = 0;
	}
	else if ( pt32 ( parm ) == PARM_REG_CPSRF )
	{
		flag_only = 1;
	}
	else if ( pt32 ( parm ) == PARM_REG_SPSRF )
	{
		dword |= TYPE32_MSR_PD;
		flag_only = 1;
	}
	else
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	parm = strdup(getparm ( t, 2 ));
	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );

		if ( flag_only )
		{
			operand = get_operand ( t, 2 );
			if ( operand < 0 )
				return operand;

			if ( operand & TYPE32_OP_IMM )
				dword |= TYPE32_MSR_I;

			dword |= (operand & TYPE32_OP_MASK);
		}
		else
			return -E_WRONG_PARM;
	}
	else
	{
		rm = reg32 ( parm );
		free ( parm );

		dword |= (rm << TYPE32_MSR_RM_OFF);
	}


	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}

int   comp32_mul     ( struct code_data *c, char *t, int cmd )
{
	return comp32_mul_mla ( c, t, cmd, 0 );
}

int   comp32_mla     ( struct code_data *c, char *t, int cmd )
{
	return comp32_mul_mla ( c, t, cmd, 1 );
}


int   comp32_mul_mla     ( struct code_data *c, char *t, int cmd, int add_rn )
{
	char *parm = NULL;
	unsigned int   rd  = 0;
	unsigned int   rm  = 0;
	unsigned int   rs  = 0;
	unsigned int   rn  = 0;
	unsigned int dword = TYPE32_MUL | COND(c) | (c->arm32_flag_s << TYPE32_MUL_S_OFF);

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rd = reg32 ( parm );
	free ( parm );


	parm = strdup(getparm ( t, 2 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rm = reg32 ( parm );
	free ( parm );

	
	parm = strdup(getparm ( t, 3 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rs = reg32 ( parm );
	free ( parm );

	
	if ( add_rn )
	{
		dword |= TYPE32_MUL_A;
		parm = strdup(getparm ( t, 4 ));
		if ( pt32 ( parm ) != PARM_REG32  )
		{
			free ( parm );
			return -E_WRONG_PARM;
		}

		rn = reg32 ( parm );
		free ( parm );
	}


	dword |= (rd << TYPE32_MUL_RD_OFF) | (rm << TYPE32_MUL_RM_OFF) | (rs << TYPE32_MUL_RS_OFF) | (rn << TYPE32_MUL_RN_OFF);

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}


int   comp32_umull      ( struct code_data *c, char *t, int cmd )
{
	return comp32_mull_mlal ( c, t, cmd, 0, 0 );
}

int   comp32_umlal      ( struct code_data *c, char *t, int cmd )
{
	return comp32_mull_mlal ( c, t, cmd, 1, 0 );
}

int   comp32_smull      ( struct code_data *c, char *t, int cmd )
{
	return comp32_mull_mlal ( c, t, cmd, 0, 1 );
}

int   comp32_smlal      ( struct code_data *c, char *t, int cmd )
{
	return comp32_mull_mlal ( c, t, cmd, 1, 1 );
}


int   comp32_mull_mlal     ( struct code_data *c, char *t, int cmd, int add_rd, int is_signed )
{
	char *parm = NULL;
	unsigned int   rh  = 0;
	unsigned int   rl  = 0;
	unsigned int   rm  = 0;
	unsigned int   rs  = 0;
	unsigned int dword = TYPE32_MULL | COND(c) | (c->arm32_flag_s << TYPE32_MULL_S_OFF) | (is_signed << TYPE32_MULL_U_OFF);

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rl = reg32 ( parm );
	free ( parm );


	parm = strdup(getparm ( t, 2 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rh = reg32 ( parm );
	free ( parm );

	
	parm = strdup(getparm ( t, 3 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rm = reg32 ( parm );
	free ( parm );

	
	if ( add_rd )
		dword |= TYPE32_MULL_A;

	parm = strdup(getparm ( t, 4 ));
	if ( pt32 ( parm ) != PARM_REG32  )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rs = reg32 ( parm );
	free ( parm );

	dword |= (rl << TYPE32_MULL_RL_OFF) | (rh << TYPE32_MULL_RH_OFF) | (rm << TYPE32_MULL_RM_OFF) | (rs << TYPE32_MULL_RS_OFF);

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}


int   comp32_ldr      ( struct code_data *c, char *t, int cmd )
{
	return comp32_ldr_str ( c, t, cmd, 1 );
}
int   comp32_str      ( struct code_data *c, char *t, int cmd )
{
	return comp32_ldr_str ( c, t, cmd, 0 );
}


int get_ldr_str_shift_code ( unsigned char *parms, int parm_num )
{
	unsigned int shift_code = 0;
	unsigned int shift_type = 0;
	char *p1 = NULL;
	char *p2 = NULL;

	p1 = strdup(getparm ( parms, parm_num ));
	shift_type = get_shift_type ( p1 );
	free ( p1 );
	switch ( shift_type )
	{
		case 0:
		case 1:
		case 2:
		case 3:
			shift_code = shift_type << 1;
			p2 = strdup(getparm ( parms, parm_num + 1 ));

			if ( pt32 ( p2 ) == PARM_VAL )
			{
				if ( val ( p2 ) > 31 )
					return -E_WRONG_PARM;

				shift_code |= val ( p2 ) << 3;
			}
			else
			{
				free ( p2 );
				return -E_WRONG_PARM;
			}

			free ( p2 );

			break;

		case 0xFE:
			shift_code = 0;
			break;

		default:
			return -E_WRONG_PARM;
	}

	return shift_code;
}

// flags:
//        I P U _ W _
// offset:
//
// returns rn

int get_ldr_str_address ( unsigned char *string, unsigned int *flags, unsigned int *offset )
{
	unsigned char *rn_str = NULL;
	unsigned char *rm_str = NULL;
	unsigned char *shift_str = NULL;
	unsigned int rn = 0;
	unsigned int rm = 0;
	int pre_indexed = 0;
	int offset_up = 0;
	int pos = 0;
	int str_pos = 0;
	int shift_code = 0;
	int write_back = 0;
	int register_offset = 0;

	while ( *string == ' ' )
		string++;
	if ( *string != '[' )
		return -E_WRONG_PARM;

	string++;

	rn_str = strdup ( string );

	while ( rn_str[pos] && rn_str[pos] != ',' && rn_str[pos] != ']' )
		pos++;

	rn_str[pos] = '\000';
	rn = reg32 ( rn_str );
	free ( rn_str );

	if ( rn == REG32_UNK )
		return -E_WRONG_PARM;

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	// [Rn]...
	if ( string[pos] == ']' )
	{
		pre_indexed = 0;
		pos++;

		// skip spaces
		while ( string[pos] == ' ' )
			pos++;

		// [Rn]
		if ( string[pos] == '\000' )
		{
			(*flags) |= 0x18; // to have same behaviour as if there were a [Rn,#0]
			(*offset) = 0;
			return rn;
		}
	}
	else
		pre_indexed = 1;


	// [Rn,   or   [Rn],
	if ( string[pos] != ',' )
		return -E_WRONG_PARM;

	pos++;

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	if ( string[pos] == '+' )
	{
		offset_up = 1;
		pos++;
	}
	else if ( string[pos] == '-' )
	{
		offset_up = 0;
		pos++;
	}
	else
		offset_up = 1;

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	// now parse expression or Rm
	rm_str = strdup ( &string[pos] );

	str_pos = 0;
	while ( rm_str[str_pos] && rm_str[str_pos] != ',' && rm_str[str_pos] != ']' )
		str_pos++;
	pos += str_pos;
	rm_str[str_pos] = '\000';

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	// [Rn,Rm,LSR 10]
	if ( string[pos] == ',' )
	{
		// rm must be a shifted register
		if ( pt32 ( rm_str ) != PARM_REG32 )
			return -E_INV_VAL;
		rm = reg32 ( rm_str );
		free ( rm_str );

		pos++;
		// skip spaces
		while ( string[pos] == ' ' )
			pos++;

		shift_str = strdup ( &string[pos] );
		str_pos = 0;
		while ( shift_str[str_pos] && shift_str[str_pos] != ']' )
			str_pos++;
		pos += str_pos;
		pos++;

		shift_str[str_pos] = '\000';

		shift_code = get_ldr_str_shift_code ( shift_str, 1 );

		*offset = (shift_code << 4 ) | rm;
		register_offset = 1;
	}
	else
	{
		// skip ']'
		if ( pre_indexed )
		{
			if ( string[pos] != ']' )
				return -E_INV_VAL;
			pos++;
		}

		// may be both, but unshifted
		if ( pt32 ( rm_str ) == PARM_REG32 )
		{
			rm = reg32 ( rm_str );
			free ( rm_str );
			*offset = rm;
			register_offset = 1;
		}
		else if ( pt32 ( rm_str ) == PARM_VAL )
		{
			*offset = val ( rm_str );
			free ( rm_str );
			register_offset = 0;
		}
		else
		{
			free ( rm_str );
			return -E_INV_VAL;
		}
	}

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	if ( pre_indexed )
	{
		if ( string[pos] == '!' )
			write_back = 1;
	}
	else
	{
		if ( string[pos] == '!' )
			return -E_UNK_OPCODE;
	}

	if ( register_offset )
		(*flags) |= 0x20;

	if ( pre_indexed )
		(*flags) |= 0x10;

	if ( offset_up )
		(*flags) |= 0x08;

	if ( write_back )
		(*flags) |= 0x02;

	return rn;
}

int   comp32_ldr_str      ( struct code_data *c, char *t, int cmd, int load )
{
	unsigned char *parm = NULL;
	unsigned char *offset_string = NULL;
	unsigned int   rd   = 0;
	unsigned int   rn   = 0;
	unsigned int   rm   = 0;
	unsigned int   pos  = 0;
	unsigned int dword  = TYPE32_LDR_STR | COND(c);
	unsigned int flags  = 0;
	unsigned int offset = 0;
	int ret = 0;


	ALIGN_4;

	if ( cmd == CMD_PRECOMPILE )
		return 4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rd = reg32 ( parm );

	offset_string = strdup ( t + strlen ( parm ) + 1 );
	free ( parm );

	ret = get_ldr_str_address ( offset_string, &flags, &offset );

	if ( ret < 0 )
	{
		unsigned char *label = offset_string;
		int relative = 0;

		while ( (*label) != '\000' && (*label) == ' ' )
			label++;

		pos = get_label ( c, label );

		if ( pos == C_ERR )
		{
			pos = get_import ( c, label + 1 );
			if ( pos == C_ERR )
			{
				free ( offset_string );
				return -E_WRONG_PARM;
			}
			pos -= c->start;
		}

		relative = pos - c->bytes;
		relative -= 8;

		if ( relative > 0 )
		{
			flags |= 0x18;
			offset = relative;
		}
		else
		{
			flags |= 0x10;
			offset = -relative;
		}

		ret = REG32_R15;
	}

	free ( offset_string );


	if ( ret < 0 )
		return ret;

	rn = ret;

	if ( flags & 0x20 )
		dword |= TYPE32_LDR_STR_I;
	if ( flags & 0x10 )
		dword |= TYPE32_LDR_STR_P;
	if ( flags & 0x08 )
		dword |= TYPE32_LDR_STR_U;
	if ( flags & 0x02 )
		dword |= TYPE32_LDR_STR_W;

	if ( c->arm32_flag_b )
		dword |= TYPE32_LDR_STR_B;
	if ( c->arm32_flag_t )
		dword |= TYPE32_LDR_STR_W;
	if ( load )
		dword |= TYPE32_LDR_STR_L;

	dword |= (rn << TYPE32_LDR_STR_RN_OFF) | (rd << TYPE32_LDR_STR_RD_OFF) | offset;

	if ( cmd == CMD_COMPILE )
	{
		c->data = realloc ( c->data, c->bytes + 200 );
		c->data[c->bytes] = (dword & 0xFF000000) >> 24;
		c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
		c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
		c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

		set_endianess ( c, 4 );

		c->bytes += 4;
	}

	return 4;
}


// flags:
//        P U I W _
// offset:
//
// returns rn

int get_ldrh_strh_address ( unsigned char *string, unsigned int *flags, unsigned int *offset )
{
	unsigned char *rn_str = NULL;
	unsigned char *rm_str = NULL;
	unsigned char *shift_str = NULL;
	unsigned int rn = 0;
	unsigned int rm = 0;
	int pre_indexed = 0;
	int offset_up = 0;
	int pos = 0;
	int str_pos = 0;
	int shift_code = 0;
	int write_back = 0;
	int register_offset = 0;

	while ( *string == ' ' )
		string++;

	if ( *string != '[' )
		return -E_WRONG_PARM;

	string++;

	rn_str = strdup ( string );

	while ( rn_str[pos] && rn_str[pos] != ',' && rn_str[pos] != ']' )
		pos++;

	rn_str[pos] = '\000';
	rn = reg32 ( rn_str );
	free ( rn_str );

	if ( rn == REG32_UNK )
		return -E_WRONG_PARM;

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	// [Rn]...
	if ( string[pos] == ']' )
	{
		pre_indexed = 0;
		pos++;

		// skip spaces
		while ( string[pos] == ' ' )
			pos++;

		// [Rn]
		if ( string[pos] == '\000' )
		{
			(*flags) |= 0x18; // to have same behaviour as if there were a [Rn,#0]
			(*offset) = 0;
			return rn;
		}
	}
	else
		pre_indexed = 1;


	// [Rn,   or   [Rn],
	if ( string[pos] != ',' )
		return -E_WRONG_PARM;

	pos++;

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	if ( string[pos] == '+' )
	{
		offset_up = 1;
		pos++;
	}
	else if ( string[pos] == '-' )
	{
		offset_up = 0;
		pos++;
	}
	else
		offset_up = 1;

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	// now parse expression or Rm
	rm_str = strdup ( &string[pos] );

	str_pos = 0;
	while ( rm_str[str_pos] && rm_str[str_pos] != ',' && rm_str[str_pos] != ']' )
		str_pos++;
	pos += str_pos;
	rm_str[str_pos] = '\000';

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	// [Rn,Rm,LSR 10]
	if ( string[pos] == ',' )
	{
		// shifted registers not supported by this opcode
		return -E_INV_VAL;
	}
	else
	{
		// skip ']'
		if ( pre_indexed )
		{
			if ( string[pos] != ']' )
				return -E_INV_VAL;
			pos++;
		}

		// may be both, but unshifted
		if ( pt32 ( rm_str ) == PARM_REG32 )
		{
			rm = reg32 ( rm_str );
			free ( rm_str );
			*offset = rm;
			register_offset = 1;
		}
		else if ( pt32 ( rm_str ) == PARM_VAL )
		{
			*offset = val ( rm_str );
			free ( rm_str );
			register_offset = 0;
		}
		else
		{
			free ( rm_str );
			return -E_INV_VAL;
		}
	}

	// skip spaces
	while ( string[pos] == ' ' )
		pos++;

	if ( pre_indexed )
	{
		if ( string[pos] == '!' )
			write_back = 1;
	}
	else
	{
		if ( string[pos] == '!' )
			return -E_UNK_OPCODE;
	}

	if ( pre_indexed )
		(*flags) |= 0x10;

	if ( offset_up )
		(*flags) |= 0x08;

	if ( !register_offset )
		(*flags) |= 0x04;

	if ( write_back )
		(*flags) |= 0x02;

	return rn;
}


int   comp32_ldrh     ( struct code_data *c, char *t, int cmd )
{
	return comp32_ldrh_strh ( c, t, cmd, 1 );
}

int   comp32_strh     ( struct code_data *c, char *t, int cmd )
{
	return comp32_ldrh_strh ( c, t, cmd, 0 );
}

int   comp32_ldrh_strh      ( struct code_data *c, char *t, int cmd, int load )
{
	unsigned char *parm = NULL;
	unsigned char *offset_string = NULL;
	unsigned int   rd  = 0;
	unsigned int   rn  = 0;
	unsigned int   rm  = 0;
	unsigned int  pos  = 0;
	unsigned int dword = TYPE32_LDRH_STRH | COND(c);
	unsigned int flags = 0;
	unsigned int offset = 0;
	int ret = 0;


	ALIGN_4;

	if ( cmd == CMD_PRECOMPILE )
		return 4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}

	rd = reg32 ( parm );

	offset_string = strdup ( t + strlen ( parm ) + 1 );
	free ( parm );

	ret = get_ldrh_strh_address ( offset_string, &flags, &offset );

	if ( ret < 0 )
	{
		unsigned char *label = offset_string;
		int relative = 0;

		while ( (*label) != '\000' && (*label) == ' ' )
			label++;

		pos = get_label ( c, label );

		if ( pos == C_ERR )
		{
			pos = get_import ( c, label + 1 );
			if ( pos == C_ERR )
			{
				free ( offset_string );
				return -E_WRONG_PARM;
			}
			pos -= c->start;
		}

		relative = pos - c->bytes;
		relative -= 8;

		if ( relative > 0 )
		{
			flags |= 0x18;
			offset = relative;
		}
		else
		{
			flags |= 0x10;
			offset = -relative;
		}

		ret = REG32_R15;
	}

	free ( offset_string );

	if ( ret < 0 )
		return ret;

	rn = ret;

	if ( flags & 0x10 )
		dword |= TYPE32_LDRH_STRH_P;
	if ( flags & 0x08 )
		dword |= TYPE32_LDRH_STRH_U;
	if ( flags & 0x04 )
		dword |= TYPE32_LDRH_STRH_I;
	if ( flags & 0x02 )
		dword |= TYPE32_LDRH_STRH_W;

	if ( c->arm32_flag_h )
		dword |= TYPE32_LDRH_STRH_H;

	if ( c->arm32_flag_sh )
	{
		dword |= TYPE32_LDRH_STRH_H;
		dword |= TYPE32_LDRH_STRH_S;
	}
	if ( c->arm32_flag_sb )
		dword |= TYPE32_LDRH_STRH_S;

	if ( load )
		dword |= TYPE32_LDRH_STRH_L;

	dword |= (rn << TYPE32_LDRH_STRH_RN_OFF) | (rd << TYPE32_LDRH_STRH_RD_OFF) | ((offset&0x0F)<< TYPE32_LDRH_STRH_OL_OFF)| (((offset&0xF0)>>4) << TYPE32_LDRH_STRH_OH_OFF);

	if ( cmd == CMD_COMPILE )
	{
		c->data = realloc ( c->data, c->bytes + 200 );
		c->data[c->bytes] = (dword & 0xFF000000) >> 24;
		c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
		c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
		c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

		set_endianess ( c, 4 );

		c->bytes += 4;
	}

	return 4;
}

int   comp32_ldm_stm      ( struct code_data *c, char *t, int cmd, int load )
{
	unsigned char *parm = NULL;
	unsigned int   rn  = 0;
	unsigned int dword = TYPE32_LDM_STM | COND(c);
	int i = 2;

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( parm[strlen(parm)-1] == '!' )
	{
		dword |= TYPE32_LDM_STM_W;
		parm[strlen(parm)-1] = '\000';
	}

	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	rn = reg32 ( parm );

	parm = strdup(getparm ( t, 2 ));
	if ( parm[0] == '!' )
	{
		dword |= TYPE32_LDM_STM_W;
		i++;
	}
	free ( parm );


	do 
	{
		parm = getparm ( t, i );
		if ( pt_true ( parm, PARM_REG32 )  )
			dword |= (1 << reg32 ( parm ));

		i++;
	} while ( reg ( parm ) != REG_UNK );

	if ( parm && parm[0] == '^' )
		dword |= TYPE32_LDM_STM_S;
	else if ( parm && strlen ( parm ) > 0 )
		return -E_UNK_REG;

	if ( load )
		dword |= TYPE32_LDM_STM_L;

	if ( c->arm32_flag_pu & 0x02 )
		dword |= TYPE32_LDM_STM_P;

	if ( c->arm32_flag_pu & 0x01 )
		dword |= TYPE32_LDM_STM_U;



	dword |= (rn << TYPE32_LDM_STM_RN_OFF);

	if ( cmd == CMD_COMPILE )
	{
		c->data = realloc ( c->data, c->bytes + 200 );
		c->data[c->bytes] = (dword & 0xFF000000) >> 24;
		c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
		c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
		c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

		set_endianess ( c, 4 );

		c->bytes += 4;
	}

	return 4;
}

int   comp32_ldm      ( struct code_data *c, char *t, int cmd )
{
	return comp32_ldm_stm ( c, t, cmd, 1 );
}

int   comp32_stm      ( struct code_data *c, char *t, int cmd )
{
	return comp32_ldm_stm ( c, t, cmd, 0 );
}


int   comp32_swp      ( struct code_data *c, char *t, int cmd )
{
	unsigned char *parm = NULL;
	unsigned int   rn  = 0;
	unsigned int   rd  = 0;
	unsigned int   rm  = 0;
	unsigned int dword = TYPE32_SWP | COND(c);

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	rd = reg32 ( parm );

	parm = strdup(getparm ( t, 2 ));
	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	rm = reg32 ( parm );

	parm = strdup(getparm ( t, 3 ));
	if ( pt32 ( parm ) != PARM_REG32 )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	rn = reg32 ( parm );

	if ( c->arm32_flag_b )
		dword |= TYPE32_SWP_B;

	dword |= (rd << TYPE32_SWP_RD_OFF) | (rm << TYPE32_SWP_RM_OFF) | (rn << TYPE32_SWP_RN_OFF);

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}


int   comp32_swi      ( struct code_data *c, char *t, int cmd )
{
	unsigned char *parm = NULL;
	unsigned int   cf  = 0;
	unsigned int dword = TYPE32_SWI | COND(c);

	ALIGN_4;


	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_VAL )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	cf = val ( parm );

	dword |= (cf & 0x00FFFFFF);

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}

int   comp32_cdp      ( struct code_data *c, char *t, int cmd )
{
	unsigned char *parm = NULL;
	unsigned int   pn  = 0;
	unsigned int   cpo  = 0;
	unsigned int   cd  = 0;
	unsigned int   cn  = 0;
	unsigned int   cm  = 0;
	unsigned int   cp  = 0;
	unsigned int dword = TYPE32_CDP | COND(c);

	ALIGN_4;

	parm = strdup(getparm ( t, 1 ));
	if ( pt32 ( parm ) != PARM_CPID )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	pn = reg32 ( parm );

	parm = strdup(getparm ( t, 2 ));
	if ( pt32 ( parm ) != PARM_VAL )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	cpo = val ( parm );

	parm = strdup(getparm ( t, 3 ));
	if ( pt32 ( parm ) != PARM_CREG )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	cd = reg32 ( parm );


	parm = strdup(getparm ( t, 4 ));
	if ( pt32 ( parm ) != PARM_CREG )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	cn = reg32 ( parm );

	parm = strdup(getparm ( t, 5 ));
	if ( pt32 ( parm ) != PARM_CREG )
	{
		free ( parm );
		return -E_WRONG_PARM;
	}
	cm = reg32 ( parm );

	parm = strdup(getparm ( t, 6 ));
	if ( pt32 ( parm ) == PARM_VAL )
		cp = val ( parm );


	dword |= (pn << TYPE32_CDP_CPN_OFF) | (cpo << TYPE32_CDP_CPO_OFF) | (cd << TYPE32_CDP_CRD_OFF) | (cn << TYPE32_CDP_CRN_OFF) | (cm << TYPE32_CDP_CRM_OFF) | (cp << TYPE32_CDP_CP_OFF);

	c->data = realloc ( c->data, c->bytes + 200 );
	c->data[c->bytes] = (dword & 0xFF000000) >> 24;
	c->data[c->bytes+1] = (dword & 0x00FF0000) >> 16;
	c->data[c->bytes+2] = (dword & 0x0000FF00) >> 8;
	c->data[c->bytes+3] = (dword & 0x000000FF) >> 0;

	set_endianess ( c, 4 );

	c->bytes += 4;

	return 4;
}