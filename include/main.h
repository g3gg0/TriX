#ifndef __TRIX_MAIN_H__
#define __TRIX_MAIN_H__

#include "defines.h"

unsigned int main_setup ( void );
unsigned int main_cleanup ( void );

unsigned int trix_run ( char *script, char *flash );

#endif
