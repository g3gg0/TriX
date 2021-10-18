#ifndef __TRIX_E32IMAGE_DEFLATE_H__
#define __TRIX_E32IMAGE_DEFLATE_H__

#define MAX_FIXED_LENGTH 27
#define LITERALS_COUNT 285
#define DISTANCES_COUNT 44
#define END_OF_STREAM 0x11C

unsigned char *e32image_deflate_decompress (unsigned char *data, unsigned int data_length, unsigned int unpacked_length );

#endif
