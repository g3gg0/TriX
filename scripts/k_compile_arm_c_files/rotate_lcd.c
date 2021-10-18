
#define MAX (128*128-2)
#define PIXEL(num) address[(num)/2*3]

void trans ( unsigned char *pos1, unsigned char *pos2  );

void rotate_lcd( unsigned char *address, int length )
{
	int pos = 0;
	unsigned char buf[3];


	while ( pos < MAX/2 )
	{
		trans ( &PIXEL(pos), &PIXEL(MAX-pos) );
		pos += 2;
	}

	spi_send_dma ( address, length );
	while ( !spi_check_bit ( 2 ) );

	pos = 0;
	while ( pos < MAX/2 )
	{
		trans ( &PIXEL(pos), &PIXEL(MAX-pos) );
		pos += 2;
	}
}


void trans ( unsigned char *pos1, unsigned char *pos2  )
{
	unsigned char buf[3];
	unsigned char *x = pos1+0;
	unsigned char *y = pos1+1;
	unsigned char *z = pos1+2;
	unsigned char *a = pos2+0;
	unsigned char *b = pos2+1;
	unsigned char *c = pos2+2;

	buf[0] = (*y)<<4|(*z)>>4;
	buf[1] = (*z)<<4|(*x)>>4;
	buf[2] = (*x)<<4|(*y)>>4;

	*x = (*b)<<4|(*c)>>4;
	*y = (*c)<<4|(*a)>>4;
	*z = (*a)<<4|(*b)>>4;

	*a = buf[0];
	*b = buf[1];
	*c = buf[2];

}
